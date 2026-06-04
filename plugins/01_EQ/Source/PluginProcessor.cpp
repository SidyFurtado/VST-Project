#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"


AureqAudioProcessor::AureqAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    presetManager = std::make_unique<AUREQ::PresetManager> (apvts);
}

AureqAudioProcessor::~AureqAudioProcessor()
{
}

const juce::String AureqAudioProcessor::getName() const
{
    return "AUREQ";
}

bool AureqAudioProcessor::acceptsMidi() const
{
    return false;
}

bool AureqAudioProcessor::producesMidi() const
{
    return false;
}

bool AureqAudioProcessor::isMidiEffect() const
{
    return false;
}

double AureqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AureqAudioProcessor::getNumPrograms()
{
    return 1; // Deve retornar pelo menos 1
}

int AureqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AureqAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AureqAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AureqAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void AureqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    eqCore.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void AureqAudioProcessor::releaseResources()
{
    eqCore.reset();
}

bool AureqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Suportar Mono e Estéreo
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Entrada e saída devem coincidir
    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    return true;
}

void AureqAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Limpar canais excedentes
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Atualizar e processar no motor de EQ
    eqCore.updateFromAPVTS (apvts);
    eqCore.processBlock (buffer);

    // Push post-EQ samples into the lock-free analyzer FIFO.
    // Only channel 0 is used (Left channel / mono).
    // This is entirely allocation-free and lock-free — safe for the audio thread.
    if (buffer.getNumChannels() > 0)
        analyzerFifo.push (buffer.getReadPointer (0), buffer.getNumSamples());
}

bool AureqAudioProcessor::hasEditor() const
{
    return true;
}

float AureqAudioProcessor::getBandDynamicGainDb (int bandIndex) const
{
    return eqCore.getBandDynamicGainDb (bandIndex);
}

juce::AudioProcessorEditor* AureqAudioProcessor::createEditor()
{
    return new AureqAudioProcessorEditor (*this);
}

void AureqAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // APVTS XML State Persistence Verification (Phase 5.20)
    // All 108 parameters registered in createParameterLayout (including input/output gain,
    // global bypass, theme mode, and the 13 parameters for each of the 8 bands)
    // are automatically captured here via apvts.copyState().
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AureqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // APVTS XML State Persistence Restoration Verification (Phase 5.20)
    // All 108 parameters are automatically restored here by replacing the ValueTree state.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
            clearUndoRedoHistory();
        }
    }
}

void AureqAudioProcessor::loadPreset (int index)
{
    if (presetManager != nullptr && index >= 0 && index < presetManager->getNumPresets())
    {
        captureUndoCheckpoint();
        presetManager->loadPreset (index);
    }
}

bool AureqAudioProcessor::loadUserPreset (int index)
{
    if (presetManager == nullptr || index < 0 || index >= presetManager->getNumUserPresets())
        return false;

    captureUndoCheckpoint();
    return presetManager->loadUserPreset (index);
}

bool AureqAudioProcessor::saveCurrentUserPreset (const juce::String& name)
{
    if (presetManager == nullptr)
        return false;

    return presetManager->saveCurrentUserPreset (name);
}

void AureqAudioProcessor::refreshUserPresets()
{
    if (presetManager != nullptr)
        presetManager->refreshUserPresets();
}

bool AureqAudioProcessor::revealUserPresetDirectory() const
{
    if (presetManager == nullptr)
        return false;

    return presetManager->revealUserPresetDirectory();
}

bool AureqAudioProcessor::deleteUserPreset (int index)
{
    if (presetManager == nullptr)
        return false;

    return presetManager->deleteUserPreset (index);
}

juce::String AureqAudioProcessor::getCurrentPresetName() const
{
    if (presetManager != nullptr)
        return presetManager->getCurrentPresetName();
    return {};
}

void AureqAudioProcessor::toggleABSlot()
{
    ensureABSlotsInitialized();
    captureUndoCheckpoint();
    saveCurrentABSlot();

    activeABSlotIsA = ! activeABSlotIsA;
    restoreActiveABSlot();
}

juce::String AureqAudioProcessor::getActiveABSlotName() const
{
    return activeABSlotIsA ? "A" : "B";
}

void AureqAudioProcessor::copyABSlotAToB()
{
    ensureABSlotsInitialized();
    abSlotBState = abSlotAState.createCopy();
}

void AureqAudioProcessor::copyABSlotBToA()
{
    ensureABSlotsInitialized();
    abSlotAState = abSlotBState.createCopy();
}

void AureqAudioProcessor::copyCurrentABSlotToOther()
{
    ensureABSlotsInitialized();
    auto currentState = apvts.copyState().createCopy();
    abSlotAState = currentState.createCopy();
    abSlotBState = currentState.createCopy();
}

void AureqAudioProcessor::resetABSlotBFromCurrent()
{
    ensureABSlotsInitialized();
    abSlotBState = apvts.copyState().createCopy();
}

void AureqAudioProcessor::ensureABSlotsInitialized()
{
    auto currentState = apvts.copyState().createCopy();

    if (! abSlotAState.isValid())
        abSlotAState = currentState.createCopy();

    if (! abSlotBState.isValid())
        abSlotBState = currentState.createCopy();
}

void AureqAudioProcessor::saveCurrentABSlot()
{
    auto currentState = apvts.copyState().createCopy();

    if (activeABSlotIsA)
        abSlotAState = currentState;
    else
        abSlotBState = currentState;
}

void AureqAudioProcessor::restoreActiveABSlot()
{
    auto& targetState = activeABSlotIsA ? abSlotAState : abSlotBState;

    if (! targetState.isValid())
        targetState = apvts.copyState().createCopy();

    apvts.replaceState (targetState.createCopy());
}

void AureqAudioProcessor::captureUndoCheckpoint()
{
    if (isRestoringUndoRedoState)
        return;

    auto currentState = apvts.copyState().createCopy();

    if (! currentState.isValid())
        return;

    if (! undoStack.empty() && areStatesEquivalent (undoStack.back(), currentState))
        return;

    undoStack.push_back (currentState);

    while (undoStack.size() > maxUndoHistorySize)
        undoStack.pop_front();

    redoStack.clear();
}

bool AureqAudioProcessor::undoAPVTSState()
{
    if (undoStack.empty())
        return false;

    auto targetState = undoStack.back().createCopy();
    undoStack.pop_back();

    auto currentState = apvts.copyState().createCopy();
    if (currentState.isValid())
        redoStack.push_back (currentState);

    isRestoringUndoRedoState = true;
    apvts.replaceState (targetState.createCopy());
    isRestoringUndoRedoState = false;

    return true;
}

bool AureqAudioProcessor::redoAPVTSState()
{
    if (redoStack.empty())
        return false;

    auto targetState = redoStack.back().createCopy();
    redoStack.pop_back();

    auto currentState = apvts.copyState().createCopy();
    if (currentState.isValid())
    {
        undoStack.push_back (currentState);

        while (undoStack.size() > maxUndoHistorySize)
            undoStack.pop_front();
    }

    isRestoringUndoRedoState = true;
    apvts.replaceState (targetState.createCopy());
    isRestoringUndoRedoState = false;

    return true;
}

bool AureqAudioProcessor::canUndo() const
{
    return ! undoStack.empty();
}

bool AureqAudioProcessor::canRedo() const
{
    return ! redoStack.empty();
}

void AureqAudioProcessor::clearUndoRedoHistory()
{
    undoStack.clear();
    redoStack.clear();
}

bool AureqAudioProcessor::areStatesEquivalent (const juce::ValueTree& first, const juce::ValueTree& second)
{
    if (! first.isValid() || ! second.isValid())
        return first.isValid() == second.isValid();

    return first.isEquivalentTo (second);
}

juce::AudioProcessorValueTreeState::ParameterLayout AureqAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Parâmetros Globais
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (AUREQ::Params::inputGainID(), 1),
        "Input Gain",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
        0.0f,
        "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (AUREQ::Params::outputGainID(), 1),
        "Output Gain",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
        0.0f,
        "dB"));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (AUREQ::Params::globalBypassID(), 1),
        "Global Bypass",
        false));

    juce::StringArray themeChoices { "Light", "Dark" };
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID (AUREQ::Params::themeModeID(), 1),
        "Theme Mode",
        themeChoices,
        1)); // Default: Dark (index 1)

    // Parâmetros por Banda (0 a 7)
    juce::StringArray filterTypeChoices {
        "Bell",
        "Low Cut",
        "High Cut",
        "Low Shelf",
        "High Shelf",
        "Notch",
        "Band Pass"
    };

    for (int bandIndex = 0; bandIndex < AUREQ::Params::numBands; ++bandIndex)
    {
        // 1. Enabled (Band 1 default true, others default false)
        bool isDefaultEnabled = (bandIndex == 0);
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID (AUREQ::Params::bandEnabledID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Enabled",
            isDefaultEnabled));

        // 2. Type
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID (AUREQ::Params::bandTypeID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Type",
            filterTypeChoices,
            0)); // Default: Bell (index 0)

        // 3. Frequency
        juce::NormalisableRange<float> freqRange (20.0f, 20000.0f, 0.1f);
        freqRange.setSkewForCentre (1000.0f);
        
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID (AUREQ::Params::bandFrequencyID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Frequency",
            freqRange,
            AUREQ::Params::defaultBandFrequency (bandIndex),
            "Hz"));

        // 4. Gain
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID (AUREQ::Params::bandGainID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Gain",
            juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
            0.0f,
            "dB"));

        // 5. Q
        juce::NormalisableRange<float> qRange (0.1f, 18.0f, 0.01f);
        qRange.setSkewForCentre (1.0f);
        
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID (AUREQ::Params::bandQID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Q",
            qRange,
            1.0f));

        // 6. Bypass
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID (AUREQ::Params::bandBypassID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Bypass",
            false));

        // Phase 5.19 — Advanced parameters

        // 7. Slope (6/12/18/24 dB/oct). Default: 12 dB/oct (index 1).
        juce::StringArray slopeChoices { "6", "12", "18", "24" };
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID (AUREQ::Params::bandSlopeID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Slope",
            slopeChoices,
            1)); // Default: 12 dB/oct

        // 8. Dynamic Enabled (DSP reserved — Phase 5.20+)
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID (AUREQ::Params::bandDynamicEnabledID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Dynamic Enabled",
            false));

        // 9. Dynamic Range in dB (DSP reserved) - default corrected to 0.0f (Phase 5.20)
        // Phase 8.0.1: allow negative ranges for dynamic attenuation presets.
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID (AUREQ::Params::bandDynamicRangeID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Dynamic Range",
            juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
            0.0f,
            "dB"));

        // 10. Dynamic Threshold in dBFS (DSP reserved)
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID (AUREQ::Params::bandDynamicThresholdID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Dynamic Threshold",
            juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f),
            -24.0f,
            "dBFS"));

        // 11. Dynamic Attack in ms (DSP reserved)
        {
            juce::NormalisableRange<float> attackRange (0.1f, 200.0f, 0.1f);
            attackRange.setSkewForCentre (20.0f);
            params.push_back (std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID (AUREQ::Params::bandDynamicAttackID (bandIndex), 1),
                AUREQ::Params::bandDisplayName (bandIndex) + " Dynamic Attack",
                attackRange,
                10.0f,
                "ms"));
        }

        // 12. Dynamic Release in ms (DSP reserved) - default corrected to 120.0f (Phase 5.20)
        {
            juce::NormalisableRange<float> releaseRange (10.0f, 2000.0f, 1.0f);
            releaseRange.setSkewForCentre (200.0f);
            params.push_back (std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID (AUREQ::Params::bandDynamicReleaseID (bandIndex), 1),
                AUREQ::Params::bandDisplayName (bandIndex) + " Dynamic Release",
                releaseRange,
                120.0f,
                "ms"));
        }

        // 13. Channel Mode (DSP reserved — 0=Stereo, 1=Mid, 2=Side, 3=Left, 4=Right)
        juce::StringArray channelModeChoices { "Stereo", "Mid", "Side", "Left", "Right" };
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID (AUREQ::Params::bandChannelModeID (bandIndex), 1),
            AUREQ::Params::bandDisplayName (bandIndex) + " Channel Mode",
            channelModeChoices,
            0)); // Default: Stereo
    }

    return { params.begin(), params.end() };
}

// Criar ponto de entrada do plugin para o framework JUCE
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AureqAudioProcessor();
}
