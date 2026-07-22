#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

// ==============================================================================
// GravityAudioProcessor — Implementation
// ASTRA Audio Suite — GRAVITY (Dynamic Compressor)
// ==============================================================================

GravityAudioProcessor::GravityAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "GRAVITY_PARAMS", createParameterLayout())
{
}

GravityAudioProcessor::~GravityAudioProcessor()
{
}

// ==============================================================================
// Plugin metadata
// ==============================================================================

const juce::String GravityAudioProcessor::getName() const { return "GRAVITY"; }
bool GravityAudioProcessor::acceptsMidi()  const { return false; }
bool GravityAudioProcessor::producesMidi() const { return false; }
bool GravityAudioProcessor::isMidiEffect() const { return false; }

double GravityAudioProcessor::getTailLengthSeconds() const
{
    // Compressor has a finite release tail. Expose worst-case release (3 s).
    return 3.0;
}

// ==============================================================================
// Program (no preset system in skeleton phase)
// ==============================================================================

int  GravityAudioProcessor::getNumPrograms()                               { return 1; }
int  GravityAudioProcessor::getCurrentProgram()                            { return 0; }
void GravityAudioProcessor::setCurrentProgram (int)                        {}
const juce::String GravityAudioProcessor::getProgramName (int)             { return {}; }
void GravityAudioProcessor::changeProgramName (int, const juce::String&)   {}

// ==============================================================================
// Audio lifecycle
// ==============================================================================

void GravityAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gravityProcessor.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void GravityAudioProcessor::releaseResources()
{
    gravityProcessor.reset();
}

bool GravityAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GravityAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear output channels that carry no input data.
    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    // ---- Read parameters (atomic loads, safe on the audio thread) -----------

    const float thresholdDb = apvts.getRawParameterValue (GRAVITY::Params::threshID())  ->load();
    const float ratio       = apvts.getRawParameterValue (GRAVITY::Params::ratioID())   ->load();
    const float attackMs    = apvts.getRawParameterValue (GRAVITY::Params::attackID())  ->load();
    const float releaseMs   = apvts.getRawParameterValue (GRAVITY::Params::releaseID()) ->load();
    const float makeupGainDb= apvts.getRawParameterValue (GRAVITY::Params::gainID())    ->load();

    const float modeRaw     = apvts.getRawParameterValue (GRAVITY::Params::modeID())    ->load();
    const float arcRaw      = apvts.getRawParameterValue (GRAVITY::Params::arcID())     ->load();

    const auto mode = (modeRaw >= 0.5f)
                    ? GRAVITY::DSP::DetectorMode::RMS   // Opto
                    : GRAVITY::DSP::DetectorMode::Peak; // Electro

    const bool arcEnabled = (arcRaw >= 0.5f);

    // ---- Forward to DSP engine -----------------------------------------------

    gravityProcessor.process (buffer,
                               thresholdDb,
                               ratio,
                               attackMs,
                               releaseMs,
                               makeupGainDb,
                               mode,
                               arcEnabled);
}

// ==============================================================================
// Editor
// ==============================================================================

bool GravityAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* GravityAudioProcessor::createEditor()
{
    return new GravityAudioProcessorEditor (*this);
}

// ==============================================================================
// State persistence (APVTS XML)
// ==============================================================================

void GravityAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GravityAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// ==============================================================================
// Presets
// ==============================================================================

void GravityAudioProcessor::applyPresetValues (float thresholdDb,
                                               float ratio,
                                               float attackMs,
                                               float releaseMs,
                                               float makeupGainDb,
                                               int mode,
                                               bool arc)
{
    auto setParameterValue = [this] (const juce::String& parameterID, float value)
    {
        if (auto* parameter = apvts.getParameter (parameterID))
        {
            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost (parameter->convertTo0to1 (value));
            parameter->endChangeGesture();
        }
    };

    setParameterValue (GRAVITY::Params::threshID(),  thresholdDb);
    setParameterValue (GRAVITY::Params::ratioID(),   ratio);
    setParameterValue (GRAVITY::Params::attackID(),  attackMs);
    setParameterValue (GRAVITY::Params::releaseID(), releaseMs);
    setParameterValue (GRAVITY::Params::gainID(),    makeupGainDb);
    setParameterValue (GRAVITY::Params::modeID(),    static_cast<float> (mode));
    setParameterValue (GRAVITY::Params::arcID(),     arc ? 1.0f : 0.0f);
}

bool GravityAudioProcessor::savePresetToFile (const juce::File& file)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    if (xml == nullptr)
        return false;

    return xml->writeTo (file);
}

bool GravityAudioProcessor::loadPresetFromFile (const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xmlState (juce::XmlDocument::parse (file));

    if (xmlState == nullptr || ! xmlState->hasTagName (apvts.state.getType()))
        return false;

    apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
    return true;
}

// ==============================================================================
// Metering passthrough
// ==============================================================================

float GravityAudioProcessor::getGainReductionDb() const noexcept
{
    return gravityProcessor.getGainReductionDb();
}

float GravityAudioProcessor::getInputLevelDb() const noexcept
{
    return gravityProcessor.getInputLevelDb();
}

float GravityAudioProcessor::getOutputLevelDb() const noexcept
{
    return gravityProcessor.getOutputLevelDb();
}

// ==============================================================================
// Parameter layout — APVTS
// ==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
GravityAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // --- Threshold: -60.0 – 0.0 dB, default -18.0 dB -------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (GRAVITY::Params::threshID(), 1),
        "Threshold",
        juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f),
        -18.0f,
        "dB"));

    // --- Ratio: 1.0 – 50.0, default 4.0 -------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (GRAVITY::Params::ratioID(), 1),
        "Ratio",
        juce::NormalisableRange<float> (1.0f, 50.0f, 0.1f, 0.35f), // skew for log feel
        4.0f));

    // --- Attack: 0.1 – 500.0 ms, default 10.0 ms ----------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (GRAVITY::Params::attackID(), 1),
        "Attack",
        juce::NormalisableRange<float> (0.1f, 500.0f, 0.1f, 0.30f), // log skew
        10.0f,
        "ms"));

    // --- Release: 5.0 – 3000.0 ms, default 150.0 ms -------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (GRAVITY::Params::releaseID(), 1),
        "Release",
        juce::NormalisableRange<float> (5.0f, 3000.0f, 1.0f, 0.30f), // log skew
        150.0f,
        "ms"));

    // --- Makeup Gain: 0.0 – 30.0 dB, default 0.0 dB -------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (GRAVITY::Params::gainID(), 1),
        "Gain",
        juce::NormalisableRange<float> (0.0f, 30.0f, 0.1f),
        0.0f,
        "dB"));

    // --- Mode: 0 = Electro (Peak), 1 = Opto (RMS) ---------------------------
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID (GRAVITY::Params::modeID(), 1),
        "Mode",
        juce::StringArray { "Electro", "Opto" },
        0));

    // --- ARC: 0 = Off, 1 = On ------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (GRAVITY::Params::arcID(), 1),
        "ARC",
        false));

    return { params.begin(), params.end() };
}

// ==============================================================================
// Plugin entry point
// ==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GravityAudioProcessor();
}
