#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

// ==============================================================================
// Colour palette
// ==============================================================================
namespace Colours
{
    const juce::Colour bgBlack          { 0xFF0A0A0F };  // Deep space black
    const juce::Colour bgDark           { 0xFF111118 };  // Slightly lighter
    const juce::Colour bgPanel          { 0xFF1A1A24 };  // Panel background
    const juce::Colour accentViolet     { 0xFF7C3AED };  // Primary violet
    const juce::Colour accentVioletGlow { 0xFFA78BFA };  // Glowing violet
    const juce::Colour accentVioletDim  { 0xFF4C1D95 };  // Dim violet
    const juce::Colour textPrimary      { 0xFFE2E8F0 };  // Light text
    const juce::Colour textSecondary    { 0xFF9CA3AF };  // Muted text
    const juce::Colour textMuted        { 0xFF6B7280 };  // Very muted
    const juce::Colour meterGreen       { 0xFF10B981 };  // Green for low reduction
    const juce::Colour meterYellow      { 0xFFF59E0B };  // Yellow for medium
    const juce::Colour meterRed         { 0xFFEF4444 };  // Red for heavy
    const juce::Colour meterBg          { 0xFF1F2937 };  // Meter background
    const juce::Colour ringInactive     { 0xFF1E1B4B };  // Inactive ring
    const juce::Colour noiseFloorColour { 0xFF6366F1 };  // Indigo for noise floor
}

// ==============================================================================
// BlackHoleLookAndFeel Implementation
// ==============================================================================

VoidAudioProcessorEditor::BlackHoleLookAndFeel::BlackHoleLookAndFeel() {}

void VoidAudioProcessorEditor::BlackHoleLookAndFeel::drawRotarySlider (
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (8.0f);
    auto radius = std::min (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto cx     = bounds.getCentreX();
    auto cy     = bounds.getCentreY();

    auto arcRadius = radius - 6.0f;

    // Background ring
    juce::Path bgTrack;
    bgTrack.addCentredArc (cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (Colours::ringInactive);
    g.strokePath (bgTrack, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active fill arc
    if (sliderPosProportional > 0.0f)
    {
        auto activeAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Glow behind arc
        juce::Path glowTrack;
        glowTrack.addCentredArc (cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, activeAngle, true);
        g.setColour (Colours::accentVioletGlow.withAlpha (0.25f * sliderPosProportional));
        g.strokePath (glowTrack, juce::PathStrokeType (8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Solid arc
        juce::Path valueTrack;
        valueTrack.addCentredArc (cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, activeAngle, true);
        g.setColour (Colours::accentViolet.interpolatedWith (juce::Colours::white, 0.15f * sliderPosProportional));
        g.strokePath (valueTrack, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // ---- Reactive vacuum core (animated by reduction dB) ----
    auto holeRadius = arcRadius - 8.0f;
    auto holeBounds = juce::Rectangle<float> (cx - holeRadius, cy - holeRadius, holeRadius * 2.0f, holeRadius * 2.0f);

    // Core glow intensity — pulses with noise reduction
    float glowIntensity = juce::jlimit (0.0f, 1.0f, currentCoreGlow);

    // Event horizon glow (brighter/more violet when reducing noise)
    auto horizonColour = Colours::accentVioletDim.interpolatedWith (Colours::accentVioletGlow, glowIntensity);
    float horizonAlpha = 0.2f + 0.6f * glowIntensity;
    juce::ColourGradient holeGradient (
        juce::Colours::black, cx, cy,
        horizonColour.withAlpha (horizonAlpha), cx + holeRadius, cy + holeRadius, true);
    holeGradient.addColour (0.5f, juce::Colours::black.withAlpha (0.3f + 0.5f * glowIntensity));
    g.setGradientFill (holeGradient);
    g.fillEllipse (holeBounds);

    // Inner black (pulses slightly with reduction)
    float innerPulse = 1.0f - 0.15f * glowIntensity;
    g.setColour (juce::Colours::black.withAlpha (0.85f + 0.15f * (1.0f - glowIntensity)));
    g.fillEllipse (holeBounds.reduced (4.0f * innerPulse));

    // Accretion disk ring (thickens and brightens with reduction)
    float reactiveBoost = glowIntensity * sliderPosProportional;
    auto lensingThickness = 1.5f + 2.5f * sliderPosProportional + 3.0f * reactiveBoost;
    auto lensingColour = Colours::accentVioletGlow.interpolatedWith (juce::Colours::white, 0.3f * sliderPosProportional + 0.4f * reactiveBoost);
    float lensingAlpha = 0.15f + 0.6f * sliderPosProportional + 0.25f * reactiveBoost;
    g.setColour (lensingColour.withAlpha (lensingAlpha));
    g.drawEllipse (holeBounds.expanded (lensingThickness * 0.5f + 2.0f), std::max (1.0f, lensingThickness));
}

// ==============================================================================
// MiniKnobLookAndFeel Implementation
// ==============================================================================

VoidAudioProcessorEditor::MiniKnobLookAndFeel::MiniKnobLookAndFeel() {}

void VoidAudioProcessorEditor::MiniKnobLookAndFeel::drawRotarySlider (
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (2.0f);
    auto radius = std::min (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto cx     = bounds.getCentreX();
    auto cy     = bounds.getCentreY();

    // Background circle
    g.setColour (Colours::bgPanel);
    g.fillEllipse (bounds);

    // Outer ring
    g.setColour (Colours::ringInactive);
    g.drawEllipse (bounds, 2.0f);

    // Active arc
    auto arcRadius = radius - 3.0f;
    auto activeAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    juce::Path valueArc;
    valueArc.addCentredArc (cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, activeAngle, true);
    g.setColour (Colours::accentViolet);
    g.strokePath (valueArc, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Center dot
    float dotAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    float dotX = cx + arcRadius * 0.65f * std::cos (dotAngle - juce::MathConstants<float>::halfPi);
    float dotY = cy + arcRadius * 0.65f * std::sin (dotAngle - juce::MathConstants<float>::halfPi);

    g.setColour (Colours::accentVioletGlow);
    g.fillEllipse (dotX - 3.0f, dotY - 3.0f, 6.0f, 6.0f);
}

// ==============================================================================
// VoidAudioProcessorEditor Implementation
// ==============================================================================

VoidAudioProcessorEditor::VoidAudioProcessorEditor (VoidAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // ---- Preset combo ---
    const auto& presets = VoidAudioProcessor::getFactoryPresets();
    for (const auto& p : presets)
        presetCombo.addItem (p.name, (int) (&p - presets.data() + 1));
    presetCombo.setSelectedId (1);  // Default: Custom
    presetCombo.addListener (this);

    // ---- User preset save/load buttons ---
    savePresetButton.addListener (this);
    savePresetButton.setColour (juce::TextButton::buttonColourId, Colours::accentVioletDim);
    savePresetButton.setColour (juce::TextButton::textColourOffId, Colours::accentVioletGlow);
    addAndMakeVisible (savePresetButton);

    loadPresetButton.addListener (this);
    loadPresetButton.setColour (juce::TextButton::buttonColourId, Colours::bgPanel);
    loadPresetButton.setColour (juce::TextButton::textColourOffId, Colours::textSecondary);
    addAndMakeVisible (loadPresetButton);
    presetCombo.setColour (juce::ComboBox::backgroundColourId, Colours::bgPanel);
    presetCombo.setColour (juce::ComboBox::textColourId, Colours::textPrimary);
    presetCombo.setColour (juce::ComboBox::outlineColourId, Colours::ringInactive);
    presetCombo.setColour (juce::ComboBox::arrowColourId, Colours::accentViolet);
    addAndMakeVisible (presetCombo);

    presetLabel.setFont (juce::FontOptions (8.0f).withStyle ("Bold"));
    presetLabel.setJustificationType (juce::Justification::centred);
    presetLabel.setColour (juce::Label::textColourId, Colours::textMuted);
    presetLabel.setText ("PRESET", juce::dontSendNotification);
    addAndMakeVisible (presetLabel);

    // ---- Main vacuum knob ---
    vacuumSlider.setLookAndFeel (&blackHoleLF);
    vacuumSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    vacuumSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    vacuumSlider.setRange (0.0, 100.0, 0.1);
    addAndMakeVisible (vacuumSlider);

    vacuumLabel.setFont (juce::FontOptions (11.0f).withStyle ("Bold"));
    vacuumLabel.setJustificationType (juce::Justification::centred);
    vacuumLabel.setColour (juce::Label::textColourId, Colours::textSecondary);
    vacuumLabel.setText ("VACUUM", juce::dontSendNotification);
    addAndMakeVisible (vacuumLabel);

    // ---- DSP mini knobs ---
    auto setupMiniKnob = [&] (juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        slider.setLookAndFeel (&miniKnobLF);
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        slider.setRange (0.0, 100.0, 0.1);
        addAndMakeVisible (slider);

        label.setFont (juce::FontOptions (8.0f).withStyle ("Bold"));
        label.setJustificationType (juce::Justification::centred);
        label.setColour (juce::Label::textColourId, Colours::textMuted);
        label.setText (text, juce::dontSendNotification);
        addAndMakeVisible (label);
    };

    setupMiniKnob (thresholdSlider, thresholdLabel, "THRESH");
    setupMiniKnob (floorSlider,     floorLabel,     "FLOOR");
    setupMiniKnob (amountSlider,    amountLabel,    "AMOUNT");
    setupMiniKnob (attackSlider,    attackLabel,    "ATTACK");
    setupMiniKnob (releaseSlider,   releaseLabel,   "RELEASE");
    setupMiniKnob (smoothSlider,    smoothLabel,    "SMOOTH");

    thresholdSlider.setRange (-80.0, 0.0, 0.5);
    floorSlider.setRange (-100.0, -20.0, 0.5);
    amountSlider.setRange (0.0, 100.0, 0.5);
    attackSlider.setRange (1.0, 200.0, 1.0);
    releaseSlider.setRange (10.0, 2000.0, 10.0);
    smoothSlider.setRange (1.0, 9.0, 2.0);

    // ---- Buttons ---
    loadButton.setButtonText ("LOAD ONNX");
    loadButton.addListener (this);
    loadButton.setColour (juce::TextButton::buttonColourId, Colours::bgPanel);
    loadButton.setColour (juce::TextButton::textColourOffId, Colours::textPrimary);
    addAndMakeVisible (loadButton);

    unloadButton.setButtonText ("UNLOAD");
    unloadButton.addListener (this);
    unloadButton.setColour (juce::TextButton::buttonColourId, Colours::bgPanel);
    unloadButton.setColour (juce::TextButton::textColourOffId, Colours::textSecondary);
    addAndMakeVisible (unloadButton);

    learnButton.setButtonText ("LEARN FLOOR");
    learnButton.setClickingTogglesState (true);
    learnButton.setColour (juce::TextButton::buttonColourId, Colours::bgPanel);
    learnButton.setColour (juce::TextButton::textColourOffId, Colours::textSecondary);
    learnButton.setColour (juce::TextButton::buttonOnColourId, Colours::accentVioletDim);
    learnButton.setColour (juce::TextButton::textColourOnId, Colours::accentVioletGlow);
    addAndMakeVisible (learnButton);

    bypassToggle.setButtonText ("BYPASS");
    bypassToggle.setColour (juce::ToggleButton::tickColourId, Colours::accentViolet);
    bypassToggle.setColour (juce::ToggleButton::textColourId, Colours::textPrimary);
    addAndMakeVisible (bypassToggle);

    // ---- Mode combo ---
    modeCombo.addItemList ({ "AI", "DSP", "HYBRID" }, 1);
    modeCombo.setSelectedId (2); // Default: DSP
    modeCombo.setColour (juce::ComboBox::backgroundColourId, Colours::bgPanel);
    modeCombo.setColour (juce::ComboBox::textColourId, Colours::textPrimary);
    modeCombo.setColour (juce::ComboBox::outlineColourId, Colours::ringInactive);
    modeCombo.setColour (juce::ComboBox::arrowColourId, Colours::accentViolet);
    addAndMakeVisible (modeCombo);

    modeLabel.setFont (juce::FontOptions (9.0f).withStyle ("Bold"));
    modeLabel.setJustificationType (juce::Justification::centred);
    modeLabel.setColour (juce::Label::textColourId, Colours::textMuted);
    modeLabel.setText ("MODE", juce::dontSendNotification);
    addAndMakeVisible (modeLabel);

    // ---- Model path / status label ---
    modelPathLabel.setFont (juce::FontOptions (9.0f).withStyle ("Bold"));
    modelPathLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (modelPathLabel);

    // ---- Custom meters (ballistic smoothing initialized) ---
    reductionMeter  = std::make_unique<ReductionMeter>  (audioProcessor);
    noiseFloorMeter = std::make_unique<NoiseFloorMeter> (audioProcessor);
    inputVUMeter    = std::make_unique<VUMeter>         (audioProcessor, true);
    outputVUMeter   = std::make_unique<VUMeter>         (audioProcessor, false);
    addAndMakeVisible (*reductionMeter);
    addAndMakeVisible (*noiseFloorMeter);
    addAndMakeVisible (*inputVUMeter);
    addAndMakeVisible (*outputVUMeter);

    // ---- APVTS Attachments ---
    vacuumAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::vacuumIntensity(), vacuumSlider);
    bypassAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, VoidParams::bypass(), bypassToggle);
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::noiseThreshold(), thresholdSlider);
    floorAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::noiseFloor(), floorSlider);
    amountAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::reductionAmount(), amountSlider);
    attackAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::attackTime(), attackSlider);
    releaseAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::releaseTime(), releaseSlider);
    smoothAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::spectralSmooth(), smoothSlider);
    learnAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, VoidParams::learnNoise(), learnButton);
    modeAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.apvts, VoidParams::reductionMode(), modeCombo);

    updateModelPathDisplay();
    updateModeVisibility();

    // ---- Enable window resizing with aspect ratio constraint ----
    // Maintains 4:3 ratio to preserve layout proportions
    setResizable (true, false);
    setResizeLimits (480, 360, 1280, 960);
    getConstrainer()->setFixedAspectRatio (640.0 / 480.0);
    setSize (640, 480);

    // Start timer at 30 Hz for smooth meter updates & reactive animation
    startTimerHz (30);
}

VoidAudioProcessorEditor::~VoidAudioProcessorEditor()
{
    stopTimer();
    vacuumSlider.setLookAndFeel (nullptr);
    thresholdSlider.setLookAndFeel (nullptr);
    floorSlider.setLookAndFeel (nullptr);
    amountSlider.setLookAndFeel (nullptr);
    attackSlider.setLookAndFeel (nullptr);
    releaseSlider.setLookAndFeel (nullptr);
    smoothSlider.setLookAndFeel (nullptr);
    presetCombo.removeListener (this);
    savePresetButton.removeListener (this);
    loadPresetButton.removeListener (this);
    loadButton.removeListener (this);
    unloadButton.removeListener (this);
}

// ==============================================================================
// Paint — Deep space black hole aesthetic
// ==============================================================================

void VoidAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Deep space background
    g.fillAll (Colours::bgBlack);

    auto w = getWidth();
    auto h = getHeight();

    // Subtle radial gradient (event horizon glow behind knob area)
    juce::ColourGradient bgGradient (
        Colours::bgDark, w * 0.5f, h * 0.3f,
        Colours::bgBlack, w * 0.5f, h, true);
    bgGradient.addColour (0.6, Colours::bgDark);
    g.setGradientFill (bgGradient);
    g.fillRect (0, 0, w, h);

    // Title
    g.setColour (Colours::accentVioletGlow);
    g.setFont (juce::FontOptions (28.0f).withStyle ("Bold"));
    g.drawText ("V  O  I  D", 0, 16, w, 40, juce::Justification::centred);

    // Subtitle
    g.setColour (Colours::textMuted);
    g.setFont (juce::FontOptions (8.0f).withStyle ("Bold"));
    g.drawText ("AI NOISE VACUUM", 0, 44, w, 16, juce::Justification::centred);

    // Thin accent line under title
    g.setColour (Colours::accentVioletDim.withAlpha (0.5f));
    g.drawHorizontalLine (62, 60.0f, w - 60.0f);

    // Section labels
    g.setColour (Colours::textMuted);
    g.setFont (juce::FontOptions (9.0f).withStyle ("Bold"));

    g.drawText ("REDUCTION",  85,  254, 80, 14, juce::Justification::centred);
    g.drawText ("NOISE FLOOR", 165, 254, 80, 14, juce::Justification::centred);
    g.drawText ("IN",  260, 254, 40, 14, juce::Justification::centred);
    g.drawText ("OUT", 310, 254, 40, 14, juce::Justification::centred);

    // Bottom separator
    g.setColour (Colours::ringInactive.withAlpha (0.3f));
    g.drawHorizontalLine (324, 20, w - 20);

    // DSP section label
    g.setColour (Colours::textMuted);
    g.setFont (juce::FontOptions (8.0f).withStyle ("Bold"));
    g.drawText ("SPECTRAL ENGINE", 0, 330, w, 14, juce::Justification::centred);
}

// ==============================================================================
// Resized
// ==============================================================================

void VoidAudioProcessorEditor::resized()
{
    auto w = getWidth();
    auto h = getHeight();

    // Title area: preset combo + save/load buttons
    presetLabel.setBounds (40, 70, 80, 12);
    presetCombo.setBounds (40, 84, 120, 20);
    savePresetButton.setBounds (165, 84, 42, 20);
    loadPresetButton.setBounds (209, 84, 42, 20);
    modelPathLabel.setBounds (265, 68, w - 280, 18);

    // Main vacuum knob (centered at top)
    vacuumSlider.setBounds (w / 2 - 85, 110, 170, 170);
    vacuumLabel.setBounds (w / 2 - 85, 282, 170, 16);

    // Meters row
    reductionMeter->setBounds  (80,  300, 60, 8);
    noiseFloorMeter->setBounds (160, 300, 60, 8);
    inputVUMeter->setBounds    (253, 300, 46, 8);
    outputVUMeter->setBounds   (306, 300, 46, 8);

    // DSP knobs row (6 knobs)
    int knobY = 358;
    int knobW = 62;
    int labelH = 12;
    int spacing = (w - 40 - knobW * 6) / 5;

    auto placeKnob = [&] (juce::Slider& slider, juce::Label& label, int index)
    {
        int x = 20 + index * (knobW + spacing);
        slider.setBounds (x, knobY, knobW, knobW);
        label.setBounds (x, knobY + knobW - 2, knobW, labelH);
    };

    placeKnob (thresholdSlider, thresholdLabel, 0);
    placeKnob (floorSlider,     floorLabel,     1);
    placeKnob (amountSlider,    amountLabel,     2);
    placeKnob (attackSlider,    attackLabel,     3);
    placeKnob (releaseSlider,   releaseLabel,    4);
    placeKnob (smoothSlider,    smoothLabel,     5);

    // Bottom row: buttons and mode selector
    int bottomY = h - 50;

    bypassToggle.setBounds (15,  bottomY, 70, 24);
    learnButton.setBounds  (90,  bottomY, 90, 24);
    loadButton.setBounds   (190, bottomY, 100, 24);
    unloadButton.setBounds (295, bottomY, 80, 24);

    modeLabel.setBounds  (410, bottomY, 40, 14);
    modeCombo.setBounds  (410, bottomY + 14, 80, 24);
}

// ==============================================================================
// Button clicked
// ==============================================================================

void VoidAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &loadButton)
    {
        auto chooserFlags = juce::FileBrowserComponent::openMode
                          | juce::FileBrowserComponent::canSelectFiles;

        auto chooser = std::make_shared<juce::FileChooser> (
            "Select ONNX Model...",
            juce::File{}, "*.onnx");

        chooser->launchAsync (chooserFlags, [this, chooser] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                if (audioProcessor.loadModel (file.getFullPathName()))
                {
                    updateModelPathDisplay();
                }
                else
                {
                    // Show transient error feedback for invalid ONNX file
                    modelPathLabel.setText ("ERROR: Failed to load model — check file format", juce::dontSendNotification);
                    modelPathLabel.setColour (juce::Label::textColourId, Colours::meterRed);
                    modelPathErrorTimer = ERROR_DISPLAY_TICKS;  // Show for ~4 seconds
                }
            }
        });
    }
    else if (button == &unloadButton)
    {
        audioProcessor.unloadModel();
        updateModelPathDisplay();
    }
    else if (button == &savePresetButton)
    {
        // Save current state as a user preset XML file
        auto presetDir = VoidAudioProcessor::getUserPresetDirectory();
        auto chooser = std::make_shared<juce::FileChooser> (
            "Save VOID Preset",
            presetDir.getChildFile ("My Preset.voidpreset"),
            "*.voidpreset");

        chooser->launchAsync (juce::FileBrowserComponent::saveMode
                              | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File{})
            {
                // Ensure .voidpreset extension
                auto finalFile = file.withFileExtension (".voidpreset");
                if (audioProcessor.savePresetToFile (finalFile))
                {
                    modelPathLabel.setText ("Preset saved: " + finalFile.getFileNameWithoutExtension(),
                                            juce::dontSendNotification);
                    modelPathLabel.setColour (juce::Label::textColourId, Colours::accentViolet);
                }
                else
                {
                    modelPathLabel.setText ("ERROR: Failed to save preset", juce::dontSendNotification);
                    modelPathLabel.setColour (juce::Label::textColourId, Colours::meterRed);
                }
                modelPathErrorTimer = ERROR_DISPLAY_TICKS;
            }
        });
    }
    else if (button == &loadPresetButton)
    {
        // Load a user preset from an XML file
        auto presetDir = VoidAudioProcessor::getUserPresetDirectory();
        auto chooser = std::make_shared<juce::FileChooser> (
            "Load VOID Preset",
            presetDir,
            "*.voidpreset");

        chooser->launchAsync (juce::FileBrowserComponent::openMode
                              | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                if (audioProcessor.loadPresetFromFile (file))
                {
                    modelPathLabel.setText ("Preset loaded: " + file.getFileNameWithoutExtension(),
                                            juce::dontSendNotification);
                    modelPathLabel.setColour (juce::Label::textColourId, Colours::accentVioletGlow);
                    // Reset preset combo to Custom since we loaded a user preset
                    presetCombo.setSelectedId (1);
                }
                else
                {
                    modelPathLabel.setText ("ERROR: Invalid preset file", juce::dontSendNotification);
                    modelPathLabel.setColour (juce::Label::textColourId, Colours::meterRed);
                }
                modelPathErrorTimer = ERROR_DISPLAY_TICKS;
            }
        });
    }
}

// ==============================================================================
// ComboBox changed (preset selector)
// ==============================================================================

void VoidAudioProcessorEditor::comboBoxChanged (juce::ComboBox* combo)
{
    if (combo == &presetCombo)
    {
        int selectedId = presetCombo.getSelectedId();
        if (selectedId > 1)  // Index 0 = Custom (do nothing)
            audioProcessor.loadPreset (selectedId - 1);  // Convert 1-based combo ID to 0-based index
    }
}

// ==============================================================================
// Timer — 30 Hz: reactive core glow, meter ballistics, UI updates
// ==============================================================================

void VoidAudioProcessorEditor::timerCallback()
{
    // ---- Reactive vacuum core glow (low-pass smoothed) ----
    {
        float targetGlow = juce::jlimit (0.0f, 1.0f, audioProcessor.getDspReductionDb() / 40.0f);
        vacuumCoreGlow += (targetGlow - vacuumCoreGlow) * 0.12f;
        blackHoleLF.setCoreGlow (vacuumCoreGlow);
        vacuumSlider.repaint();
    }

    // ---- Transient error message countdown ----
    if (modelPathErrorTimer > 0)
    {
        --modelPathErrorTimer;
        if (modelPathErrorTimer == 0)
            updateModelPathDisplay();
    }
    else
    {
        updateModelPathDisplay();
    }

    updateModeVisibility();

    // Repaint meters every tick (ballistic smoothing applied inside paint)
    reductionMeter->repaint();
    noiseFloorMeter->repaint();
    inputVUMeter->repaint();
    outputVUMeter->repaint();
}

// ==============================================================================
// Model path display
// ==============================================================================

void VoidAudioProcessorEditor::updateModelPathDisplay()
{
    const bool isLoaded    = audioProcessor.isModelLoaded();
    const bool isBypassed  = audioProcessor.apvts.getRawParameterValue (VoidParams::bypass())->load() > 0.5f;
    const bool isReducing  = audioProcessor.isCurrentlyReducing();

    if (isLoaded)
    {
        if (isBypassed)
        {
            modelPathLabel.setText ("BYPASSED — Model loaded but inactive", juce::dontSendNotification);
            modelPathLabel.setColour (juce::Label::textColourId, Colours::textMuted);
        }
        else if (isReducing)
        {
            modelPathLabel.setText ("VACUUM ACTIVE — Reducing noise", juce::dontSendNotification);
            modelPathLabel.setColour (juce::Label::textColourId, Colours::accentVioletGlow);
        }
        else
        {
            modelPathLabel.setText ("MODEL LOADED — Processing audio", juce::dontSendNotification);
            modelPathLabel.setColour (juce::Label::textColourId, Colours::accentViolet);
        }
    }
    else
    {
        auto mode = modeCombo.getSelectedId();
        if (mode == 2 || mode == 3) // DSP or Hybrid
        {
            modelPathLabel.setText ("DSP ENGINE ACTIVE — No AI model needed", juce::dontSendNotification);
            modelPathLabel.setColour (juce::Label::textColourId, Colours::accentViolet);
        }
        else
        {
            modelPathLabel.setText ("NO MODEL LOADED — Load an ONNX file or switch to DSP mode", juce::dontSendNotification);
            modelPathLabel.setColour (juce::Label::textColourId, Colours::meterRed);
        }
    }
}

void VoidAudioProcessorEditor::updateModeVisibility()
{
    int mode = modeCombo.getSelectedId();

    // Show/hide DSP controls based on mode
    bool showDSP = (mode == 2 || mode == 3); // DSP or Hybrid

    thresholdSlider.setVisible (showDSP);
    floorSlider.setVisible (showDSP);
    amountSlider.setVisible (showDSP);
    attackSlider.setVisible (showDSP);
    releaseSlider.setVisible (showDSP);
    smoothSlider.setVisible (showDSP);
    thresholdLabel.setVisible (showDSP);
    floorLabel.setVisible (showDSP);
    amountLabel.setVisible (showDSP);
    attackLabel.setVisible (showDSP);
    releaseLabel.setVisible (showDSP);
    smoothLabel.setVisible (showDSP);
    learnButton.setVisible (showDSP);
}

// ==============================================================================
// Meter Component drawing
// ==============================================================================

void VoidAudioProcessorEditor::ReductionMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (1.0f);

    // Background
    g.setColour (Colours::meterBg);
    g.fillRoundedRectangle (bounds, 2.0f);

    // Target value from DSP engine
    float targetDb = audioProcessor.getDspReductionDb();

    // Professional ballistic smoothing:
    //   Attack: instant (follow upward peaks immediately)
    //   Release: exponential decay at ~20 dB/sec (coefficient for 30 Hz update rate)
    constexpr float releaseCoeff = 0.967f;  // ~20 dB/sec decay at 30 Hz
    if (targetDb > ballisticValue)
        ballisticValue = targetDb;           // Instant attack
    else
        ballisticValue += (targetDb - ballisticValue) * (1.0f - releaseCoeff);  // Smooth decay

    float frac = juce::jlimit (0.0f, 1.0f, ballisticValue / 40.0f);

    // Colour gradient based on intensity
    juce::Colour meterColour;
    if (frac < 0.3f)
        meterColour = Colours::meterGreen.interpolatedWith (Colours::meterYellow, frac / 0.3f);
    else if (frac < 0.7f)
        meterColour = Colours::meterYellow.interpolatedWith (Colours::meterRed, (frac - 0.3f) / 0.4f);
    else
        meterColour = Colours::meterRed;

    if (frac > 0.01f)
    {
        auto fillBounds = bounds.withWidth (bounds.getWidth() * frac);
        g.setColour (meterColour.withAlpha (0.8f));
        g.fillRoundedRectangle (fillBounds, 2.0f);
    }

    // Numeric label
    g.setColour (Colours::textSecondary);
    g.setFont (juce::FontOptions (7.0f).withStyle ("Bold"));
    g.drawText (juce::String (ballisticValue, 1) + " dB", bounds, juce::Justification::centred);
}

void VoidAudioProcessorEditor::NoiseFloorMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (1.0f);

    // Background
    g.setColour (Colours::meterBg);
    g.fillRoundedRectangle (bounds, 2.0f);

    // Noise floor level (slow-moving, gentle smoothing)
    float targetFloor = audioProcessor.getEstimatedNoiseFloorDb();
    constexpr float smoothCoeff = 0.15f;
    ballisticValue += (targetFloor - ballisticValue) * smoothCoeff;

    float frac = juce::jlimit (0.0f, 1.0f, (ballisticValue + 100.0f) / 80.0f);

    if (frac > 0.01f)
    {
        auto fillBounds = bounds.withWidth (bounds.getWidth() * frac);
        g.setColour (Colours::noiseFloorColour.withAlpha (0.7f));
        g.fillRoundedRectangle (fillBounds, 2.0f);
    }

    g.setColour (Colours::textSecondary);
    g.setFont (juce::FontOptions (7.0f).withStyle ("Bold"));
    g.drawText (juce::String (ballisticValue, 1) + " dB", bounds, juce::Justification::centred);
}

void VoidAudioProcessorEditor::VUMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (1.0f);
    auto barBounds = bounds.reduced (0.0f, 2.0f);

    // Background
    g.setColour (Colours::meterBg);
    g.fillRoundedRectangle (barBounds, 2.0f);

    // Get raw level and apply ballistic smoothing
    float rawDb = input ? audioProcessor.getInputLevelDb() : audioProcessor.getOutputLevelDb();
    // Ballistic: instant attack, ~20 dB/sec release
    constexpr float releaseCoeff = 0.967f;
    if (rawDb > ballisticValue)
        ballisticValue = rawDb;           // Instant attack
    else
        ballisticValue += (rawDb - ballisticValue) * (1.0f - releaseCoeff);  // Smooth decay

    float frac = juce::jlimit (0.0f, 1.0f, (ballisticValue + 90.0f) / 90.0f);

    if (frac > 0.01f)
    {
        auto fillBounds = barBounds.withWidth (barBounds.getWidth() * frac);
        g.setColour (Colours::accentViolet.withAlpha (0.7f));
        g.fillRoundedRectangle (fillBounds, 2.0f);
    }

    g.setColour (Colours::textMuted);
    g.setFont (juce::FontOptions (6.0f).withStyle ("Bold"));
    g.drawText (input ? "IN" : "OUT", bounds, juce::Justification::centred);
}
