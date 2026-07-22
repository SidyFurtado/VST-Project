#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "Theme.h"
#include "EQGraphView.h"
#include "AnalyzerProcessor.h"
#include "Localization.h"

class AureqLookAndFeel : public juce::LookAndFeel_V4
{
public:
    AureqLookAndFeel();
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, 
                           float sliderPosProportionDelta, float rotaryStartAngle, float rotaryEndAngle, 
                           juce::Slider& slider) override;
                           
    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, 
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
                               
    void drawButtonText (juce::Graphics& g, juce::TextButton& button, 
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
                               
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, 
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class FloatingBandPanel : public juce::Component
{
public:
    FloatingBandPanel (AureqAudioProcessor& processor, juce::LookAndFeel& lookAndFeel);
    ~FloatingBandPanel() override;

    void updateSelection (int bandIndex);
    void updateThemeColors (const AUREQ::ThemeColors& colors);
    void updateRealtimeMeter (const AUREQ::ThemeColors& colors);
    
    void paint (juce::Graphics& g) override;
    void resized() override;

    std::function<void()> onDeleteClicked;

    // Sliders / Knobs
    juce::Slider freqSlider;
    juce::Slider gainSlider;
    juce::Slider qSlider;
    juce::Slider dynRangeSlider;
    juce::ToggleButton bandBypassBtn;
    juce::TextButton removeBandBtn { "Delete" };
    juce::Label dynGainMeterLabel;

private:
    AureqAudioProcessor& audioProcessor;
    AUREQ::ThemeColors themeColors;
    int selectedBandIndex = -1;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dynRangeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bandBypassAttachment;

    juce::DropShadowEffect shadowEffect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloatingBandPanel)
};

class AureqAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   private juce::AudioProcessorValueTreeState::Listener,
                                   private juce::AsyncUpdater,
                                   private juce::Timer
{
public:
    AureqAudioProcessorEditor (AureqAudioProcessor&);
    ~AureqAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress& key) override;
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;

private:
    // UI Helpers for premium liquid glass simulation
    void drawGlassPanel (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour baseColor, float cornerRadius);
    void drawSoftShadow (juce::Graphics& g, juce::Rectangle<int> area, int shadowSize, float maxAlpha, juce::Colour shadowColour, float cornerRadius);
    void drawPill (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour fillColors, juce::Colour borderColors, const juce::String& text, juce::Colour textColor, bool isBold);

    // APVTS Listener callback (thread-safe, calls triggerAsyncUpdate)
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    // AsyncUpdater callback (runs on message thread)
    void handleAsyncUpdate() override;

    // Read parameter value and update internal ThemeColors
    void updateTheme();

    // Timer callback (runs on message thread)
    void timerCallback() override;

    // Helper to dynamically re-bind selected band controls
    void updateSelectedBandControls();
    void captureSelectedBandGestureCheckpoint();
    void captureWheelQGestureCheckpoint (int bandIndex);
    bool performUndoFromUI();
    bool performRedoFromUI();
    void applyLocalizedTexts();
    void setLanguage (AUREQ::AureqLanguage language);
    AUREQ::AureqLanguage loadLanguagePreference() const;
    void saveLanguagePreference (AUREQ::AureqLanguage language) const;
    juce::String text (AUREQ::TextKey key) const;

    // Factory preset browser helpers
    void showPresetBrowserMenu();
    void showABContextMenu();
    void showSettingsMenu();
    void loadPresetAndRefresh (int presetIndex);
    void loadUserPresetAndRefresh (int userPresetIndex);
    void showSaveUserPresetDialog();
    void confirmDeleteUserPreset (int userPresetIndex);

    AureqAudioProcessor& audioProcessor;

    AUREQ::EQGraphView graphView;
    AUREQ::ThemeColors currentColors;
    AUREQ::ThemeMode currentMode { AUREQ::ThemeMode::Dark };
    AUREQ::AureqLanguage currentLanguage { AUREQ::AureqLanguage::PortugueseBR };

    AureqLookAndFeel aureqLookAndFeel;
    FloatingBandPanel floatingPanel;

    // Sliders / Knobs
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;

    // Buttons
    juce::ToggleButton globalBypassButton;
    juce::ToggleButton themeToggleButton;

    // Header buttons (placeholders / toggle)
    juce::TextButton prevPresetBtn { "<" };
    juce::TextButton nextPresetBtn { ">" };
    juce::TextButton abBtn { "A/B" };
    juce::TextButton settingsBtn { "Settings" };
    juce::TextButton themeHeaderBtn { "Theme" };
    juce::TextButton bypassHeaderBtn { "Bypass" };
    juce::TextButton resetBtn { "Reset" };

    // Preset Label
    juce::Label presetLabel;
    std::unique_ptr<juce::AlertWindow> saveUserPresetAlert;

    int lastWheelQCheckpointBand = -1;
    juce::uint32 lastWheelQCheckpointTimeMs = 0;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> globalBypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> themeToggleAttachment;

    // Spectrum analyzer processor (UI thread only, driven by timerCallback)
    AnalyzerProcessor analyzerProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AureqAudioProcessorEditor)
};
