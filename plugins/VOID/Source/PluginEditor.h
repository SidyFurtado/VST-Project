#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// ==============================================================================
// VoidAudioProcessorEditor — ASTRA Audio Suite / VOID
// Black Hole Aesthetic: Dark matter, accretion disk, gravitational lensing
// ==============================================================================

class VoidAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 private juce::Button::Listener,
                                 private juce::ComboBox::Listener,
                                 private juce::Timer
{
public:
    VoidAudioProcessorEditor (VoidAudioProcessor&);
    ~VoidAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* button) override;
    void comboBoxChanged (juce::ComboBox* combo) override;
    void timerCallback() override;

private:
    // ==============================================================================
    // Custom LookAndFeel: Black Hole Knob with reactive glow
    // ==============================================================================
    class BlackHoleLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        BlackHoleLookAndFeel();
        void setCoreGlow (float glow) noexcept { currentCoreGlow = glow; }
        void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider& slider) override;
    private:
        float currentCoreGlow = 0.0f;
    };

    BlackHoleLookAndFeel blackHoleLF;

    // ==============================================================================
    // Custom LookAndFeel: Mini knobs for DSP controls
    // ==============================================================================
    class MiniKnobLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        MiniKnobLookAndFeel();
        void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider& slider) override;
    };

    MiniKnobLookAndFeel miniKnobLF;

    // ==============================================================================
    // Controls
    // ==============================================================================
    // Main vacuum intensity knob
    juce::Slider vacuumSlider;
    juce::Label  vacuumLabel;

    // DSP knobs
    juce::Slider thresholdSlider;
    juce::Slider floorSlider;
    juce::Slider amountSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider smoothSlider;

    juce::Label thresholdLabel;
    juce::Label floorLabel;
    juce::Label amountLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label smoothLabel;

    // Buttons
    juce::ToggleButton bypassToggle;
    juce::TextButton   loadButton;
    juce::TextButton   unloadButton;
    juce::TextButton   learnButton;

    // Preset selector
    juce::ComboBox presetCombo;
    juce::Label    presetLabel;

    // User preset save/load
    juce::TextButton savePresetButton { "SAVE" };
    juce::TextButton loadPresetButton { "LOAD" };

    // Mode selector
    juce::ComboBox modeCombo;

    // Status / Model path display
    juce::Label modelPathLabel;
    juce::Label modeLabel;

    // ==============================================================================
    // Custom painted components
    // ==============================================================================
    class ReductionMeter : public juce::Component
    {
    public:
        ReductionMeter (VoidAudioProcessor& processor)
            : audioProcessor (processor) {}

        void paint (juce::Graphics& g) override;

    private:
        VoidAudioProcessor& audioProcessor;
        float ballisticValue = 0.0f;
    };

    class NoiseFloorMeter : public juce::Component
    {
    public:
        NoiseFloorMeter (VoidAudioProcessor& processor)
            : audioProcessor (processor) {}

        void paint (juce::Graphics& g) override;

    private:
        VoidAudioProcessor& audioProcessor;
        float ballisticValue = 0.0f;
    };

    class VUMeter : public juce::Component
    {
    public:
        VUMeter (VoidAudioProcessor& processor, bool isInput)
            : audioProcessor (processor), input (isInput) {}

        void paint (juce::Graphics& g) override;

    private:
        VoidAudioProcessor& audioProcessor;
        bool input;
        float ballisticValue = -90.0f;
    };

    std::unique_ptr<ReductionMeter>  reductionMeter;
    std::unique_ptr<NoiseFloorMeter> noiseFloorMeter;
    std::unique_ptr<VUMeter>        inputVUMeter;
    std::unique_ptr<VUMeter>        outputVUMeter;

    // ==============================================================================
    // APVTS Attachments
    // ==============================================================================
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  vacuumAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  floorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  amountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>  smoothAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>  bypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>  learnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;

    // ==============================================================================
    // Helpers
    // ==============================================================================
    void updateModelPathDisplay();
    void updateModeVisibility();

    // ---- Reactive vacuum core glow (low-pass smoothed) ----
    float vacuumCoreGlow = 0.0f;

    // Transient error message timer (ticks at 30 Hz)
    int modelPathErrorTimer = 0;
    static constexpr int ERROR_DISPLAY_TICKS = 120;  // ~4 seconds at 30 Hz

    VoidAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoidAudioProcessorEditor)
};
