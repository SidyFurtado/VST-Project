#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "AirProcessor.h"

// ==============================================================================
// LuminarAudioProcessor — ASTRA Audio Suite
//
// Phase 18.2: parallel Air exciter DSP + functional Output Gain with smoothing.
// Presence / Smooth remain APVTS/UI controls only until the Smooth Guard phase.
// ==============================================================================

class LuminarAudioProcessor : public juce::AudioProcessor
{
public:
    LuminarAudioProcessor();
    ~LuminarAudioProcessor() override;

    // ---- Audio lifecycle --------------------------------------------------------
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // ---- Editor -----------------------------------------------------------------
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // ---- Plugin metadata --------------------------------------------------------
    const juce::String getName() const override;
    bool acceptsMidi()  const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    // ---- Program (unused — no preset system in Phase 18.1) ---------------------
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // ---- State persistence ------------------------------------------------------
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // ---- APVTS (public so the editor can attach sliders) -------------------------
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

private:
    // Parallel Air Stage. The dry path stays intact; this processor only adds a
    // conservative high-frequency exciter layer controlled by luminar_air.
    LUMINAR::DSP::AirProcessor airProcessor;

    // Output gain smoother — prevents zipper noise when Output knob is moved.
    // Linear ramp over 10 ms.
    juce::SmoothedValue<float> outputGainSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LuminarAudioProcessor)
};
