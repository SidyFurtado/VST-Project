#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

// ==============================================================================
// LuminarAudioProcessor — Implementation
// Phase 18.2: parallel Air exciter + functional Output Gain with smoothing.
// ==============================================================================

LuminarAudioProcessor::LuminarAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "LUMINAR_PARAMS", createParameterLayout())
{
}

LuminarAudioProcessor::~LuminarAudioProcessor()
{
}

// ==============================================================================
// Plugin metadata
// ==============================================================================

const juce::String LuminarAudioProcessor::getName() const
{
    return "LUMINAR";
}

bool LuminarAudioProcessor::acceptsMidi()  const { return false; }
bool LuminarAudioProcessor::producesMidi() const { return false; }
bool LuminarAudioProcessor::isMidiEffect() const { return false; }

double LuminarAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

// ==============================================================================
// Program (not used — no preset system in Phase 18.1)
// ==============================================================================

int  LuminarAudioProcessor::getNumPrograms()                               { return 1; }
int  LuminarAudioProcessor::getCurrentProgram()                            { return 0; }
void LuminarAudioProcessor::setCurrentProgram (int)                        {}
const juce::String LuminarAudioProcessor::getProgramName (int)             { return {}; }
void LuminarAudioProcessor::changeProgramName (int, const juce::String&)   {}

// ==============================================================================
// Audio lifecycle
// ==============================================================================

void LuminarAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    airProcessor.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    // Prepare output gain smoother: 10 ms linear ramp.
    // initialise at 0 dB (linear 1.0) to avoid any gain click on load.
    outputGainSmoother.reset (sampleRate, 0.010);
    outputGainSmoother.setCurrentAndTargetValue (1.0f);
}

void LuminarAudioProcessor::releaseResources()
{
    airProcessor.reset();
}

bool LuminarAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Accept mono or stereo; input and output must match.
    if (layouts.getMainInputChannelSet()  != juce::AudioChannelSet::mono()
     && layouts.getMainInputChannelSet()  != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    return true;
}

// ==============================================================================
// processBlock
// ==============================================================================

void LuminarAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that contain no input data.
    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    // Read parameter values (atomic load, safe on audio thread).
    const float presenceAmount = apvts.getRawParameterValue (LUMINAR::Params::presenceID())->load();
    const float airAmount = apvts.getRawParameterValue (LUMINAR::Params::airID())->load();
    airProcessor.process (buffer, presenceAmount, airAmount);

    // ------------------------------------------------------------------
    // Output Gain — applied after the Air Stage.
    // ------------------------------------------------------------------

    const float targetGainDb     = apvts.getRawParameterValue (LUMINAR::Params::outputGainID())->load();
    const float targetGainLinear = juce::Decibels::decibelsToGain (targetGainDb);
    outputGainSmoother.setTargetValue (targetGainLinear);

    // Apply smoothed output gain sample-by-sample (stereo-linked).
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float gain = outputGainSmoother.getNextValue();

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.setSample (ch, sample, buffer.getSample (ch, sample) * gain);
    }
}

// ==============================================================================
// Editor
// ==============================================================================

bool LuminarAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* LuminarAudioProcessor::createEditor()
{
    return new LuminarAudioProcessorEditor (*this);
}

// ==============================================================================
// State persistence (APVTS XML)
// ==============================================================================

void LuminarAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void LuminarAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// ==============================================================================
// Parameter layout
// ==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
LuminarAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // --- Presence (0.0 – 1.0, default 0.40) ------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (LUMINAR::Params::presenceID(), 1),
        "Presence",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.40f));

    // --- Air (0.0 – 1.0, default 0.30) -----------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (LUMINAR::Params::airID(), 1),
        "Air",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.30f));


    // --- Output Gain (-12.0 – +6.0 dB, default 0.0 dB) -------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (LUMINAR::Params::outputGainID(), 1),
        "Output",
        juce::NormalisableRange<float> (-12.0f, 6.0f, 0.1f),
        0.0f,
        "dB"));

    return { params.begin(), params.end() };
}

// ==============================================================================
// Plugin entry point
// ==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LuminarAudioProcessor();
}
