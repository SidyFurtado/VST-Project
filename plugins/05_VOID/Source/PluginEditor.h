#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// ==============================================================================
// VoidAudioProcessorEditor — ASTRA Audio Suite / VOID
// ==============================================================================

class VoidAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 private juce::Button::Listener,
                                 private juce::Timer
{
public:
    VoidAudioProcessorEditor (VoidAudioProcessor&);
    ~VoidAudioProcessorEditor() override;

    // ---- Component lifecycle ----------------------------------------------------
    void paint (juce::Graphics&) override;
    void resized() override;

    // ---- Button Listener --------------------------------------------------------
    void buttonClicked (juce::Button* button) override;

    // ---- Timer callback ---------------------------------------------------------
    void timerCallback() override;

private:
    VoidAudioProcessor& audioProcessor;

    // Custom LookAndFeel for the Black Hole Knob
    class BlackHoleLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        BlackHoleLookAndFeel();
        void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider& slider) override;
    };

    BlackHoleLookAndFeel blackHoleLF;

    // UI Controls
    juce::Slider vacuumSlider;
    juce::ToggleButton bypassToggle { "BYPASS" };
    
    juce::TextButton loadButton { "LOAD MODEL" };
    juce::TextButton unloadButton { "UNLOAD" };
    
    juce::Label vacuumLabel { "vacuumLabel", "VACUUM INTENSITY" };
    juce::Label modelPathLabel;

    // APVTS Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vacuumAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    // Helper to update the loaded model path label
    void updateModelPathDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoidAudioProcessorEditor)
};
