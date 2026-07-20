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
    
    // Allocate FIFOs with at least 4x the window size to buffer processing jitter
    const int fifoCapacity = std::max (4096, windowSize * 4);
    
    inputFifo.prepare (1, fifoCapacity);
    outputFifo.prepare (1, fifoCapacity);
    
    inputFifo.reset();
    outputFifo.reset();
    
    // Pre-allocate audio thread buffers
    monoInputBuffer.setSize (1, samplesPerBlock);
    monoOutputBuffer.setSize (1, samplesPerBlock);
    monoInputBuffer.clear();
    monoOutputBuffer.clear();
    
    // Prepare and start the inference background thread
    inferenceThread.prepare();
    
    // Report the IA window size as the plugin's latency to the DAW/Host
    setLatencySamples (windowSize);
}

void VoidAudioProcessor::releaseResources()
{
    inferenceThread.stopThread (2000);
    inputFifo.reset();
    outputFifo.reset();
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
    const bool isBypassed = apvts.getRawParameterValue (VoidParams::bypass())->load() > 0.5f;
    const float mixAmount = apvts.getRawParameterValue (VoidParams::mix())->load();
    const float outputGainDb = apvts.getRawParameterValue (VoidParams::outputGain())->load();
    const float outputGain = juce::Decibels::decibelsToGain (outputGainDb);

    if (isBypassed)
    {
        // In bypass, keep input signal unmodified.
        return;
    }

    // 1. Downmix input channels to Mono (L+R / 2) into monoInputBuffer
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

    // 2. Write mono samples to input FIFO (only if there is space to avoid overflow)
    if (inputFifo.getFreeSpace() >= numSamples)
    {
        inputFifo.write (monoInputBuffer, 0, numSamples);
    }
    
    // 3. Wake up the inference thread
    signalEvent.signal();

    // 4. Try reading processed samples from output FIFO
    monoOutputBuffer.clear();
    bool outputReady = false;
    
    if (outputFifo.getNumReady() >= numSamples)
    {
        outputFifo.read (monoOutputBuffer, 0, numSamples);
        outputReady = true;
    }
    else
    {
        // Safety: If output FIFO is empty, output silence for the wet portion (no blocking)
        monoOutputBuffer.clear();
    }

    // 5. Apply wet/dry mix, output gain, and duplicate mono output to the stereo buffers
    auto* processedPtr = monoOutputBuffer.getReadPointer (0);
    
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer (ch);
        auto* originalReadPtr = buffer.getReadPointer (ch < totalNumInputChannels ? ch : 0);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float dry = originalReadPtr[i];
            float wet = outputReady ? processedPtr[i] : 0.0f;
            
            float mixed = (wet * mixAmount) + (dry * (1.0f - mixAmount));
            channelData[i] = mixed * outputGain;
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
        outputFifo.reset();
        
        // Update latency reported to host
        setLatencySamples (InferenceCore::MODEL_FRAME_SIZE);
    }
    
    return success;
}

void VoidAudioProcessor::unloadModel()
{
    inferenceCore.unloadModel();
    
    setLatencySamples (InferenceCore::MODEL_FRAME_SIZE);
    inferenceThread.prepare();
    inputFifo.reset();
    outputFifo.reset();
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
        juce::ParameterID (VoidParams::mix(), 1),
        "Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (VoidParams::outputGain(), 1),
        "Output Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f,
        "dB"));

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
