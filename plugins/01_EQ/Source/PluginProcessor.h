#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <deque>
#include "EQProcessorCore.h"
#include "AnalyzerFIFO.h"
#include "PresetManager.h"


class AureqAudioProcessor  : public juce::AudioProcessor
{
public:
    AureqAudioProcessor();
    ~AureqAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    float getBandDynamicGainDb (int bandIndex) const;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;
    std::atomic<int> soloBandIndex { -1 };

    // Lock-free FIFO for audio → UI spectrum analyzer data transport.
    // Public so the PluginEditor timer can drain it without indirection.
    AnalyzerFIFO analyzerFifo;

    AUREQ::PresetManager* getPresetManager() const { return presetManager.get(); }
    void loadPreset (int index);
    bool loadUserPreset (int index);
    bool saveCurrentUserPreset (const juce::String& name);
    void refreshUserPresets();
    bool revealUserPresetDirectory() const;
    bool deleteUserPreset (int index);
    juce::String getCurrentPresetName() const;

    void toggleABSlot();
    bool isABSlotAActive() const { return activeABSlotIsA; }
    juce::String getActiveABSlotName() const;

    void copyABSlotAToB();
    void copyABSlotBToA();
    void copyCurrentABSlotToOther();
    void resetABSlotBFromCurrent();

    void captureUndoCheckpoint();
    bool undoAPVTSState();
    bool redoAPVTSState();
    bool canUndo() const;
    bool canRedo() const;

private:
    void ensureABSlotsInitialized();
    void saveCurrentABSlot();
    void restoreActiveABSlot();
    void clearUndoRedoHistory();
    static bool areStatesEquivalent (const juce::ValueTree& first, const juce::ValueTree& second);

    EQProcessorCore eqCore;
    std::unique_ptr<AUREQ::PresetManager> presetManager;
    juce::ValueTree abSlotAState;
    juce::ValueTree abSlotBState;
    bool activeABSlotIsA = true;
    std::deque<juce::ValueTree> undoStack;
    std::deque<juce::ValueTree> redoStack;
    bool isRestoringUndoRedoState = false;
    static constexpr size_t maxUndoHistorySize = 32;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AureqAudioProcessor)
};
