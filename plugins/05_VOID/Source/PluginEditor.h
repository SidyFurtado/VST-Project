#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// ==============================================================================
// VoidAudioProcessorEditor — ASTRA Audio Suite / VOID
// ==============================================================================

class VoidAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 private juce::Button::Listener
{
public:
    VoidAudioProcessorEditor (VoidAudioProcessor&);
    ~VoidAudioProcessorEditor() override;

    // ---- Component lifecycle ----------------------------------------------------
    void paint (juce::Graphics&) override;
    void resized() override;

    // ---- Button Listener --------------------------------------------------------
    void buttonClicked (juce::Button* button) override;

private:
    VoidAudioProcessor& audioProcessor;

    // UI Controls
    juce::TextButton loadButton { "LOAD MODEL" };
    juce::TextButton unloadButton { "UNLOAD" };
    
    juce::Slider mixSlider;
    juce::Slider outputSlider;
    juce::ToggleButton bypassToggle { "BYPASS" };

    juce::Label mixLabel { "mixLabel", "MIX" };
    juce::Label outputLabel { "outputLabel", "OUTPUT" };
    juce::Label modelPathLabel;

    // APVTS Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    // Helper to update the loaded model path label
    void updateModelPathDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoidAudioProcessorEditor)
};
