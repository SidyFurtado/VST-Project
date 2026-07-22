#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "LuminarTheme.h"

// ==============================================================================
// LuminarLookAndFeel — custom rotary knob with amber/gold accent
// ==============================================================================

class LuminarLookAndFeel : public juce::LookAndFeel_V4
{
public:
    LuminarLookAndFeel();

    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider& slider) override;
};

// ==============================================================================
// LuminarAudioProcessorEditor
// ==============================================================================

class LuminarAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit LuminarAudioProcessorEditor (LuminarAudioProcessor&);
    ~LuminarAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized ()                override;

private:
    // ---- Paint helpers --------------------------------------------------------
    void paintTopBar      (juce::Graphics& g);
    void paintAuroraDisplay (juce::Graphics& g);
    void paintKnobPanel   (juce::Graphics& g);
    void paintFooter      (juce::Graphics& g);

    // ---- Data -----------------------------------------------------------------
    LuminarAudioProcessor&   audioProcessor;
    LuminarLookAndFeel       luminarLookAndFeel;
    LUMINAR::LuminarColors   colors;

    // ---- Sliders (knobs) ------------------------------------------------------
    juce::Slider presenceSlider;
    juce::Slider airSlider;
    juce::Slider outputSlider;

    // ---- Labels ---------------------------------------------------------------
    juce::Label presenceLabel;
    juce::Label airLabel;
    juce::Label outputLabel;

    // ---- APVTS attachments ----------------------------------------------------
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> presenceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> airAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LuminarAudioProcessorEditor)
};
