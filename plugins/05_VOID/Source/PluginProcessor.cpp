#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

// ==============================================================================
// VoidAudioProcessor — Implementation
// ==============================================================================

VoidAudioProcessor::VoidAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "VOID_PARAMS", createParameterLayout()),
      inferenceThread (inferenceCore, inputFifo, outputFifo, signalEvent),
      forwardFFT (FFT_ORDER),
      inverseFFT (FFT_ORDER),
      window (FFT_SIZE, juce::dsp::WindowingFunction<float>::hann, false)
{
    vacuumIntensityParam = apvts.getRawParameterValue (VoidParams::vacuumIntensity());
    bypassParam = apvts.getRawParameterValue (VoidParams::bypass());
}

VoidAudioProcessor::~VoidAudioProcessor()
{
}

// ==============================================================================
// Plugin metadata
// ==============================================================================

const juce::String VoidAudioProcessor::getName() const
{
    return "VOID";
}

bool VoidAudioProcessor::acceptsMidi()  const { return false; }
bool VoidAudioProcessor::producesMidi() const { return false; }
bool VoidAudioProcessor::isMidiEffect() const { return false; }

double VoidAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VoidAudioProcessor::getNumPrograms()
{
    return 1;
}

int VoidAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VoidAudioProcessor::setCurrentProgram (int)
{
}

const juce::String VoidAudioProcessor::getProgramName (int)
{
    return {};
}

void VoidAudioProcessor::changeProgramName (int, const juce::String&)
{
}

// ==============================================================================
// Audio lifecycle
// ==============================================================================

void VoidAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate);
    const int windowSize = InferenceCore::MODEL_FRAME_SIZE;
    
    // Allocate FIFOs with at least 8x the capacity to buffer processing jitter
    const int fifoCapacity = std::max (4096, windowSize * 8);
    
    inputFifo.prepare (1, fifoCapacity);
    outputFifo.prepare (1, fifoCapacity);
    dryFifo.prepare (1, fifoCapacity);
    stftOutputFifo.prepare (1, fifoCapacity);
    
    inputFifo.reset();
    outputFifo.reset();
    dryFifo.reset();
    stftOutputFifo.reset();

    // Pre-fill both dryFifo and outputFifo with exactly MODEL_FRAME_SIZE samples
    // of silence. This aligns the latency in samples (PDC) and prevents phase cancellation.
    juce::AudioBuffer<float> silence (1, windowSize);
    silence.clear();
    dryFifo.write (silence, 0, windowSize);
    outputFifo.write (silence, 0, windowSize);
    
    // Pre-fill stftOutputFifo with the combined latency: 480 (ONNX) + 768 (OLA) = 1248 samples.
    const int totalLatency = windowSize + (FFT_SIZE - HOP_SIZE);
    juce::AudioBuffer<float> stftSilence (1, totalLatency);
    stftSilence.clear();
    stftOutputFifo.write (stftSilence, 0, totalLatency);
    
    // Pre-allocate audio thread buffers
    monoInputBuffer.setSize (1, samplesPerBlock);
    monoOutputBuffer.setSize (1, samplesPerBlock);
    monoDryBuffer.setSize (1, samplesPerBlock);
    
    monoInputBuffer.clear();
    monoOutputBuffer.clear();
    monoDryBuffer.clear();

    stftHopDryBuffer.setSize (1, HOP_SIZE);
    stftHopWetBuffer.setSize (1, HOP_SIZE);
    stftHopDryBuffer.clear();
    stftHopWetBuffer.clear();

    // Reset FFT/OLA working arrays
    dryTimeBuffer.fill (0.0f);
    wetTimeBuffer.fill (0.0f);
    fftDryComplex.fill (0.0f);
    fftWetComplex.fill (0.0f);
    olaOutputBuffer.fill (0.0f);

    fftDryInput.fill (0.0f);
    fftWetInput.fill (0.0f);
    ifftOutput.fill (0.0f);
    
    // Prepare and start the inference background thread
    inferenceThread.prepare();
    
    // Report the total latency to the DAW/Host
    setLatencySamples (totalLatency);
}

void VoidAudioProcessor::releaseResources()
{
    inferenceThread.stopThread (2000);
    inputFifo.reset();
    outputFifo.reset();
    dryFifo.reset();
    stftOutputFifo.reset();
}

bool VoidAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Support Mono and Stereo configurations; inputs and outputs must match
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    return true;
}

// ==============================================================================
// processBlock
// ==============================================================================

void VoidAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples             = buffer.getNumSamples();

    // Clear output channels that contain no input data
    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    // Read parameter values atomically
    const bool isBypassed = bypassParam->load() > 0.5f;
    const float alpha = vacuumIntensityParam->load() / 100.0f; // Crossfade: 0.0 (Dry) to 1.0 (Wet)
    const float activeAlpha = isBypassed ? 0.0f : alpha;

    // 1. Downmix original input channels to Mono (L+R / 2) into monoInputBuffer
    monoInputBuffer.clear();
    if (totalNumInputChannels > 0)
    {
        if (totalNumInputChannels == 1)
        {
            monoInputBuffer.copyFrom (0, 0, buffer.getReadPointer (0), numSamples);
        }
        else
        {
            auto* writePtr = monoInputBuffer.getWritePointer (0);
            auto* leftPtr  = buffer.getReadPointer (0);
            auto* rightPtr = buffer.getReadPointer (1);
            
            for (int i = 0; i < numSamples; ++i)
                writePtr[i] = (leftPtr[i] + rightPtr[i]) * 0.5f;
        }
    }

    // 2. Write mono samples to both input FIFO and dry FIFO
    if (inputFifo.getFreeSpace() >= numSamples && dryFifo.getFreeSpace() >= numSamples)
    {
        inputFifo.write (monoInputBuffer, 0, numSamples);
        dryFifo.write (monoInputBuffer, 0, numSamples);
    }
    
    // 3. Wake up the inference thread
    signalEvent.signal();

    // 4. Run STFT Overlap-Add loop whenever we have at least HOP_SIZE (256) samples in both channels
    while (dryFifo.getNumReady() >= HOP_SIZE)
    {
        const bool wetAvailable = (outputFifo.getNumReady() >= HOP_SIZE);
        
        stftHopDryBuffer.clear();
        dryFifo.read (stftHopDryBuffer, 0, HOP_SIZE);
        
        stftHopWetBuffer.clear();
        if (wetAvailable)
        {
            outputFifo.read (stftHopWetBuffer, 0, HOP_SIZE);
        }

        // --- STFT OLA 75% Engine ---
        // a) Shift historical buffers left by HOP_SIZE
        std::copy (dryTimeBuffer.begin() + HOP_SIZE, dryTimeBuffer.end(), dryTimeBuffer.begin());
        std::copy (wetTimeBuffer.begin() + HOP_SIZE, wetTimeBuffer.end(), wetTimeBuffer.begin());
        
        // b) Append new samples to the end of the history
        auto* newDryPtr = stftHopDryBuffer.getReadPointer (0);
        auto* newWetPtr = stftHopWetBuffer.getReadPointer (0);
        std::copy (newDryPtr, newDryPtr + HOP_SIZE, dryTimeBuffer.begin() + (FFT_SIZE - HOP_SIZE));
        std::copy (newWetPtr, newWetPtr + HOP_SIZE, wetTimeBuffer.begin() + (FFT_SIZE - HOP_SIZE));

        // c) Apply Hann windowing to copies of the time domain signals
        std::copy (dryTimeBuffer.begin(), dryTimeBuffer.end(), fftDryInput.begin());
        std::copy (wetTimeBuffer.begin(), wetTimeBuffer.end(), fftWetInput.begin());
        
        window.multiplyWithWindowingTable (fftDryInput.data(), FFT_SIZE);
        window.multiplyWithWindowingTable (fftWetInput.data(), FFT_SIZE);

        // d) Zero-pad complex structures for real-to-complex FFT
        std::fill (fftDryComplex.begin(), fftDryComplex.end(), 0.0f);
        std::fill (fftWetComplex.begin(), fftWetComplex.end(), 0.0f);
        std::copy (fftDryInput.begin(), fftDryInput.end(), fftDryComplex.begin());
        std::copy (fftWetInput.begin(), fftWetInput.end(), fftWetComplex.begin());

        // e) Forward FFT
        forwardFFT.performRealOnlyForwardTransform (fftDryComplex.data());
        forwardFFT.performRealOnlyForwardTransform (fftWetComplex.data());

        // f) Magnitude Masking (Phase Preservation)
        // Itere over all FFT bins: real part is at index 2*k, imaginary is at 2*k+1
        for (int k = 0; k < FFT_SIZE; ++k)
        {
            float rD = fftDryComplex[2 * k];
            float iD = fftDryComplex[2 * k + 1];
            
            float rW = fftWetComplex[2 * k];
            float iW = fftWetComplex[2 * k + 1];
            
            float magD = std::sqrt (rD * rD + iD * iD);
            float magW = std::sqrt (rW * rW + iW * iW);
            
            // Mask ratio
            float mask = juce::jlimit (0.0f, 1.0f, magW / (magD + 1e-6f));
            float finalMask = (1.0f - activeAlpha) + activeAlpha * mask;
            
            // Modify magnitude, leaving Phase completely intact
            fftDryComplex[2 * k]     = rD * finalMask;
            fftDryComplex[2 * k + 1] = iD * finalMask;
        }

        // g) Inverse FFT
        inverseFFT.performRealOnlyInverseTransform (fftDryComplex.data());
        std::copy (fftDryComplex.begin(), fftDryComplex.begin() + FFT_SIZE, ifftOutput.begin());

        // h) Synthesis Windowing (to satisfy COLA reconstruction)
        window.multiplyWithWindowingTable (ifftOutput.data(), FFT_SIZE);

        // i) Accumulate (Overlap-Add) into olaOutputBuffer
        for (int i = 0; i < FFT_SIZE; ++i)
        {
            olaOutputBuffer[i] += ifftOutput[i];
        }

        // j) Write HOP_SIZE samples to stftOutputFifo (wrapping as stack-allocated AudioBuffer)
        auto* olaPtr = olaOutputBuffer.data();
        juce::AudioBuffer<float> tempStftBuffer (&olaPtr, 1, HOP_SIZE);
        stftOutputFifo.write (tempStftBuffer, 0, HOP_SIZE);

        // k) Shift olaOutputBuffer left by HOP_SIZE
        std::copy (olaOutputBuffer.begin() + HOP_SIZE, olaOutputBuffer.end(), olaOutputBuffer.begin());
        std::fill (olaOutputBuffer.begin() + (FFT_SIZE - HOP_SIZE), olaOutputBuffer.end(), 0.0f);
    }

    // 5. Read OLA output samples to output buffer
    monoOutputBuffer.clear();
    bool stftReady = false;
    
    if (stftOutputFifo.getNumReady() >= numSamples)
    {
        stftOutputFifo.read (monoOutputBuffer, 0, numSamples);
        stftReady = true;
    }
    
    auto* outPtr = monoOutputBuffer.getReadPointer (0);

    // 6. Output and duplicate mono output to the stereo buffers (Upmix)
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer (ch);
        
        for (int i = 0; i < numSamples; ++i)
        {
            channelData[i] = stftReady ? outPtr[i] : 0.0f;
        }
    }
}

// ==============================================================================
// State persistence
// ==============================================================================

void VoidAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VoidAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// ==============================================================================
// Model loading
// ==============================================================================

bool VoidAudioProcessor::loadModel (const juce::String& modelPath)
{
    bool success = inferenceCore.loadModel (modelPath);
    
    if (success)
    {
        // Reset and reconfigure thread and queues
        inferenceThread.prepare();
        inputFifo.reset();
        dryFifo.reset();
        outputFifo.reset();
        stftOutputFifo.reset();
        
        juce::AudioBuffer<float> silence (1, InferenceCore::MODEL_FRAME_SIZE);
        silence.clear();
        dryFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
        outputFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
        
        const int totalLatency = InferenceCore::MODEL_FRAME_SIZE + (FFT_SIZE - HOP_SIZE);
        juce::AudioBuffer<float> stftSilence (1, totalLatency);
        stftSilence.clear();
        stftOutputFifo.write (stftSilence, 0, totalLatency);

        // Reset FFT/OLA working arrays
        dryTimeBuffer.fill (0.0f);
        wetTimeBuffer.fill (0.0f);
        fftDryComplex.fill (0.0f);
        fftWetComplex.fill (0.0f);
        olaOutputBuffer.fill (0.0f);

        fftDryInput.fill (0.0f);
        fftWetInput.fill (0.0f);
        ifftOutput.fill (0.0f);
        
        // Update latency reported to host
        setLatencySamples (totalLatency);
    }
    
    return success;
}

void VoidAudioProcessor::unloadModel()
{
    inferenceCore.unloadModel();
    
    inferenceThread.prepare();
    inputFifo.reset();
    dryFifo.reset();
    outputFifo.reset();
    stftOutputFifo.reset();
    
    juce::AudioBuffer<float> silence (1, InferenceCore::MODEL_FRAME_SIZE);
    silence.clear();
    dryFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
    outputFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
    
    const int totalLatency = InferenceCore::MODEL_FRAME_SIZE + (FFT_SIZE - HOP_SIZE);
    juce::AudioBuffer<float> stftSilence (1, totalLatency);
    stftSilence.clear();
    stftOutputFifo.write (stftSilence, 0, totalLatency);

    // Reset FFT/OLA working arrays
    dryTimeBuffer.fill (0.0f);
    wetTimeBuffer.fill (0.0f);
    fftDryComplex.fill (0.0f);
    fftWetComplex.fill (0.0f);
    olaOutputBuffer.fill (0.0f);

    fftDryInput.fill (0.0f);
    fftWetInput.fill (0.0f);
    ifftOutput.fill (0.0f);

    setLatencySamples (totalLatency);
}

bool VoidAudioProcessor::isModelLoaded() const noexcept
{
    return inferenceCore.isModelLoaded();
}

juce::String VoidAudioProcessor::getLoadedModelPath() const noexcept
{
    return inferenceCore.getLoadedModelPath();
}

// ==============================================================================
// Parameter layout
// ==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout VoidAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (VoidParams::bypass(), 1),
        "Bypass",
        false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (VoidParams::vacuumIntensity(), 1),
        "Vacuum",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        100.0f));

    return { params.begin(), params.end() };
}

// ==============================================================================
// Editor creation
// ==============================================================================

bool VoidAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* VoidAudioProcessor::createEditor()
{
    return new VoidAudioProcessorEditor (*this);
}

// ==============================================================================
// Plugin Filter instantiation
// ==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VoidAudioProcessor();
}
