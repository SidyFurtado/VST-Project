#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include "PluginProcessor.h"
#include "GravityTheme.h"

// ==============================================================================
// GravityLookAndFeel — vertical slider with crimson accent
// ==============================================================================

class GravityLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GravityLookAndFeel();

    /** Custom vertical linear slider thumb + track. */
    void drawLinearSlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           juce::Slider::SliderStyle style,
                           juce::Slider& slider) override;

    /** Custom toggle button style (for Electro/Opto and ARC). */
    void drawToggleButton (juce::Graphics& g,
                           juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};

// ==============================================================================
// GravityMeterComponent — VU-style bar meter (Input / Output / GR)
// ==============================================================================

class GravityMeterComponent : public juce::Component
{
public:
    enum class MeterType { Input, Output, GainReduction };

    explicit GravityMeterComponent (MeterType type, GRAVITY::GravityColors colors);

    /** Called by the editor's timer with the latest dB value. */
    void setLevelDb (float db);

    void paint    (juce::Graphics& g) override;
    void resized  () override;

private:
    MeterType              meterType;
    GRAVITY::GravityColors colors;
    float                  currentDb  = -96.0f;
    float                  peakDb     = -96.0f;  // Peak hold
    int                    peakHoldMs = 0;

    static constexpr float kMinDb = -60.0f;
    static constexpr float kMaxDb =   0.0f;

    float dbToY (float db) const noexcept;
};

// ==============================================================================
// GravityTransferCurveComponent — threshold / ratio transfer display
// ==============================================================================

class GravityTransferCurveComponent : public juce::Component
{
public:
    explicit GravityTransferCurveComponent (GRAVITY::GravityColors colors);

    void setParameterSources (std::atomic<float>* thresholdDb, std::atomic<float>* ratio);
    void paint (juce::Graphics& g) override;

private:
    GRAVITY::GravityColors colors;
    std::atomic<float>* thresholdParam = nullptr;
    std::atomic<float>* ratioParam     = nullptr;

    float compressDb (float inputDb, float thresholdDb, float ratio) const noexcept;
    juce::Point<float> dbToPoint (float inputDb, float outputDb, juce::Rectangle<float> plot) const noexcept;
};

// ==============================================================================
// GravityAudioProcessorEditor
// ==============================================================================

class GravityAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::Timer
{
public:
    explicit GravityAudioProcessorEditor (GravityAudioProcessor&);
    ~GravityAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized ()                override;

private:
    // ---- Timer for metering -------------------------------------------------
    void timerCallback() override;
    void updateDynamicLabels();
    void showPresetMenu();
    juce::PopupMenu buildPresetMenu();
    juce::PopupMenu buildFactoryGenresMenu();
    juce::PopupMenu buildSourceMenu();
    juce::PopupMenu buildUserPresetsMenu();
    void applyFactoryPreset (const juce::String& presetID);
    void saveCurrentPresetAs();
    void loadUserPresetFile (const juce::File& file);
    juce::File getUserPresetDirectory() const;

    // ---- Paint helpers ------------------------------------------------------
    void paintTopBar        (juce::Graphics& g);
    void paintDisplayZone   (juce::Graphics& g);
    void paintControlPanel  (juce::Graphics& g);
    void paintFooter        (juce::Graphics& g);

    // ---- Data ---------------------------------------------------------------
    GravityAudioProcessor&   audioProcessor;
    GravityLookAndFeel       gravityLookAndFeel;
    GRAVITY::GravityColors   colors;

    // ---- Main vertical sliders (Threshold, Ratio, Gain) --------------------
    juce::Slider threshSlider;
    juce::Slider ratioSlider;
    juce::Slider gainSlider;

    juce::Label threshLabel;
    juce::Label ratioLabel;
    juce::Label gainLabel;

    juce::Label threshValueLabel;
    juce::Label ratioValueLabel;
    juce::Label gainValueLabel;

    // ---- Secondary sliders (Attack, Release) --------------------------------
    juce::Slider attackSlider;
    juce::Slider releaseSlider;

    juce::Label attackLabel;
    juce::Label releaseLabel;

    juce::Label attackValueLabel;
    juce::Label releaseValueLabel;

    // ---- Mode / ARC toggle buttons ------------------------------------------
    juce::TextButton presetsButton;
    juce::ToggleButton modeButton;   ///< Electro / Opto
    juce::ToggleButton arcButton;    ///< ARC On / Off

    // ---- Meters -------------------------------------------------------------
    GravityMeterComponent inputMeter;
    GravityMeterComponent outputMeter;
    GravityMeterComponent grMeter;
    GravityTransferCurveComponent transferCurve;

    juce::Label inputMeterLabel;
    juce::Label outputMeterLabel;
    juce::Label grMeterLabel;

    // ---- APVTS attachments --------------------------------------------------
    using SliderAtt  = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAtt  = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAtt> threshAtt;
    std::unique_ptr<SliderAtt> ratioAtt;
    std::unique_ptr<SliderAtt> attackAtt;
    std::unique_ptr<SliderAtt> releaseAtt;
    std::unique_ptr<SliderAtt> gainAtt;
    std::unique_ptr<ButtonAtt> modeAtt;
    std::unique_ptr<ButtonAtt> arcAtt;
    std::unique_ptr<juce::FileChooser> presetFileChooser;
    juce::Array<juce::File> userPresetFiles;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GravityAudioProcessorEditor)
};
