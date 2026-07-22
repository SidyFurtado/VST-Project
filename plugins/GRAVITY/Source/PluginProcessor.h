#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "GravityProcessor.h"

// ==============================================================================
// GravityAudioProcessor — ASTRA Audio Suite / GRAVITY
//
// Dynamic compressor plugin with Peak/RMS detection, logarithmic soft-knee
// transfer curve, and ARC (Auto Release Control).
// ==============================================================================

class GravityAudioProcessor : public juce::AudioProcessor
{
public:
    GravityAudioProcessor();
    ~GravityAudioProcessor() override;

    // ---- Audio lifecycle --------------------------------------------------------
    void prepareToPlay  (double sampleRate, int samplesPerBlock) override;
    void releaseResources()                                       override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // ---- Editor -----------------------------------------------------------------
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // ---- Plugin metadata --------------------------------------------------------
    const juce::String getName()        const override;
    bool  acceptsMidi()                 const override;
    bool  producesMidi()                const override;
    bool  isMidiEffect()                const override;
    double getTailLengthSeconds()       const override;

    // ---- Program (no preset system in skeleton phase) ---------------------------
    int  getNumPrograms()                                           override;
    int  getCurrentProgram()                                        override;
    void setCurrentProgram (int index)                              override;
    const juce::String getProgramName (int index)                   override;
    void changeProgramName (int index, const juce::String& newName) override;

    // ---- State persistence (APVTS XML) ------------------------------------------
    void getStateInformation (juce::MemoryBlock& destData)              override;
    void setStateInformation (const void* data, int sizeInBytes)        override;

    // ---- Presets ---------------------------------------------------------------
    void applyPresetValues (float thresholdDb,
                            float ratio,
                            float attackMs,
                            float releaseMs,
                            float makeupGainDb,
                            int mode,
                            bool arc);
    bool savePresetToFile (const juce::File& file);
    bool loadPresetFromFile (const juce::File& file);

    // ---- APVTS (public so the editor can attach sliders) -------------------------
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    // ---- Metering (called by editor timer on the message thread) ----------------
    float getGainReductionDb() const noexcept;
    float getInputLevelDb()    const noexcept;
    float getOutputLevelDb()   const noexcept;

private:
    GRAVITY::DSP::GravityProcessor gravityProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GravityAudioProcessor)
};
