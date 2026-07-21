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
      inferenceThread (inferenceCore, inputFifo, outputFifo, signalEvent)
{
    vacuumIntensityParam = apvts.getRawParameterValue (VoidParams::vacuumIntensity());
    bypassParam = apvts.getRawParameterValue (VoidParams::bypass());
    resampledOutputStage.fill (0.0f);
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
    const int windowSize = InferenceCore::MODEL_FRAME_SIZE; // 480 samples
    
    // Allocate FIFOs with at least 8x the capacity to buffer processing jitter
    const int fifoCapacity = std::max (4096, windowSize * 8);
    
    inputFifo.prepare (1, fifoCapacity);
    outputFifo.prepare (1, fifoCapacity);
    dryFifo.prepare (1, fifoCapacity);
    
    inputFifo.reset();
    outputFifo.reset();
    dryFifo.reset();

    // Pre-fill both dryFifo and outputFifo with exactly MODEL_FRAME_SIZE samples
    // of silence. This aligns the latency in samples (PDC) and prevents phase cancellation.
    juce::AudioBuffer<float> silence (1, windowSize);
    silence.clear();
    dryFifo.write (silence, 0, windowSize);
    outputFifo.write (silence, 0, windowSize);
    
    // Pre-allocate audio thread buffers
    monoInputBuffer.setSize (1, samplesPerBlock);
    monoOutputBuffer.setSize (1, samplesPerBlock);
    monoDryBuffer.setSize (1, samplesPerBlock);
    
    monoInputBuffer.clear();
    monoOutputBuffer.clear();
    monoDryBuffer.clear();

    // Reset resamplers for the new session rate
    inferenceCore.resetResamplers (sampleRate);
    resampledOutputStage.fill (0.0f);

    // Prepare and start the inference background thread
    inferenceThread.prepare();
    
    // Report the latency of 480 samples to the DAW/Host
    setLatencySamples (windowSize);
}

void VoidAudioProcessor::releaseResources()
{
    inferenceThread.stopThread (2000);
    inputFifo.reset();
    outputFifo.reset();
    dryFifo.reset();
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

    // Early return: clean bypass if bypass is active OR if no model is loaded.
    // The original audio buffer passes through 100% untouched.
    if (bypassParam->load() > 0.5f || !isModelLoaded())
        return;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples             = buffer.getNumSamples();

    // Clear output channels that contain no input data
    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    // Read parameter values atomically
    const float alpha = juce::jlimit (0.0f, 1.0f, vacuumIntensityParam->load() / 100.0f);

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

    // 2. Write mono samples to both input FIFO (for AI) and dry FIFO (for phase-aligned crossfade)
    if (inputFifo.getFreeSpace() >= numSamples && dryFifo.getFreeSpace() >= numSamples)
    {
        inputFifo.write (monoInputBuffer, 0, numSamples);
        dryFifo.write (monoInputBuffer, 0, numSamples);
    }
    
    // 3. Wake up the inference thread whenever enough samples are available for an ONNX frame
    signalEvent.signal();

    // 4. Read aligned samples from dryFifo and outputFifo with Fallback Underrun Protection
    monoDryBuffer.clear();
    monoOutputBuffer.clear();

    if (dryFifo.getNumReady() >= numSamples)
        dryFifo.read (monoDryBuffer, 0, numSamples);

    // Read available wet samples (partial pop if queue is warming up or thread is slightly late)
    const int wetAvailable = outputFifo.getNumReady();
    const int wetToRead = std::min (numSamples, wetAvailable);
    if (wetToRead > 0)
    {
        outputFifo.read (monoOutputBuffer, 0, wetToRead);
    }

    auto* dryPtr = monoDryBuffer.getReadPointer (0);
    auto* wetPtr = monoOutputBuffer.getReadPointer (0);

    // 5. Sample-accurate crossfade & upmix with per-sample underrun fallback
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer (ch);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float dryVal = dryPtr[i];
            // Use ready AI wet sample if available; seamlessly fall back to dryVal if underrun occurs
            float wetVal = (i < wetToRead) ? wetPtr[i] : dryVal;

            // Pure linear time-domain crossfade (0% knob = 100% Dry, 100% knob = 100% Wet)
            float outVal = (1.0f - alpha) * dryVal + alpha * wetVal;

            channelData[i] = juce::jlimit (-1.0f, 1.0f, outVal);
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
    bool success = inferenceCore.loadModel (modelPath, getSampleRate());
    
    if (success)
    {
        // Reset and reconfigure thread and queues
        inferenceThread.prepare();
        inputFifo.reset();
        dryFifo.reset();
        outputFifo.reset();
        
        juce::AudioBuffer<float> silence (1, InferenceCore::MODEL_FRAME_SIZE);
        silence.clear();
        dryFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
        outputFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
        
        setLatencySamples (InferenceCore::MODEL_FRAME_SIZE);
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
    
    juce::AudioBuffer<float> silence (1, InferenceCore::MODEL_FRAME_SIZE);
    silence.clear();
    dryFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
    outputFifo.write (silence, 0, InferenceCore::MODEL_FRAME_SIZE);
    
    setLatencySamples (InferenceCore::MODEL_FRAME_SIZE);
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
