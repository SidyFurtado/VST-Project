#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

// ==============================================================================
// BlackHoleLookAndFeel — Custom LookAndFeel for VOID
// ==============================================================================

VoidAudioProcessorEditor::BlackHoleLookAndFeel::BlackHoleLookAndFeel()
{
}

void VoidAudioProcessorEditor::BlackHoleLookAndFeel::drawRotarySlider (
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    auto outline = juce::Colour (0xFF1E1B4B); // Very dark indigo
    auto fill    = juce::Colour (0xFF7C3AED); // Vibrant Violet
    auto glow    = juce::Colour (0xFFA78BFA); // Soft Violet

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10.0f);
    auto radius = std::min (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toX    = bounds.getCentreX();
    auto toY    = bounds.getCentreY();

    auto arcRadius = radius - 8.0f;

    // Draw background track ring
    juce::Path backgroundTrack;
    backgroundTrack.addCentredArc (toX, toY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (outline);
    g.strokePath (backgroundTrack, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Draw active fill arc
    if (sliderPosProportional > 0.0f)
    {
        auto activeAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        juce::Path valueTrack;
        valueTrack.addCentredArc (toX, toY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, activeAngle, true);

        // Underglow effect
        g.setColour (glow.withAlpha (0.4f * sliderPosProportional));
        g.strokePath (valueTrack, juce::PathStrokeType (10.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Solid fill
        g.setColour (fill.interpolatedWith (juce::Colours::white, 0.15f * sliderPosProportional));
        g.strokePath (valueTrack, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Draw the Black Hole Core
    auto holeRadius = arcRadius - 10.0f;
    auto holeBounds = juce::Rectangle<float> (toX - holeRadius, toY - holeRadius, holeRadius * 2.0f, holeRadius * 2.0f);

    // Inner shadow of the black hole
    g.setColour (juce::Colours::black);
    g.fillEllipse (holeBounds);

    // Gravitational lensing (Accretion Disk glow ring)
    auto lensingThickness = 1.0f + 3.0f * sliderPosProportional;
    auto lensingGlow = fill.interpolatedWith (juce::Colours::white, 0.3f * sliderPosProportional);
    
    g.setColour (lensingGlow.withAlpha (0.2f + 0.7f * sliderPosProportional));
    g.drawEllipse (holeBounds.expanded (lensingThickness * 0.5f), lensingThickness);
}

// ==============================================================================
// VoidAudioProcessorEditor — Implementation
// ==============================================================================

VoidAudioProcessorEditor::VoidAudioProcessorEditor (VoidAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Apply custom LookAndFeel to the massive knob
    vacuumSlider.setLookAndFeel (&blackHoleLF);
    vacuumSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    vacuumSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); // No textbox
    vacuumSlider.setRange (0.0, 100.0, 0.1);
    addAndMakeVisible (vacuumSlider);

    // Vacuum Label
    vacuumLabel.setFont (juce::FontOptions (13.0f).withStyle ("Regular"));
    vacuumLabel.setJustificationType (juce::Justification::centred);
    vacuumLabel.setColour (juce::Label::textColourId, juce::Colour (0xFF9CA3AF)); // Muted text
    addAndMakeVisible (vacuumLabel);

    // Model path display
    modelPathLabel.setFont (juce::FontOptions (10.0f).withStyle ("Light"));
    modelPathLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (modelPathLabel);

    // Bypass Toggle
    bypassToggle.setButtonText ("BYPASS");
    bypassToggle.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xFF7C3AED));
    bypassToggle.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (bypassToggle);

    // Load/Unload buttons
    loadButton.addListener (this);
    unloadButton.addListener (this);
    addAndMakeVisible (loadButton);
    addAndMakeVisible (unloadButton);

    auto darkBg = juce::Colour (0xFF1E1B4B); // Deep indigo
    loadButton.setColour (juce::TextButton::buttonColourId, darkBg);
    loadButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    unloadButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xFF111827)); // Charcoal
    unloadButton.setColour (juce::TextButton::textColourOffId, juce::Colours::lightgrey);

    // APVTS Attachments
    vacuumAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::vacuumIntensity(), vacuumSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, VoidParams::bypass(), bypassToggle);

    updateModelPathDisplay();

    setSize (400, 450);
}

VoidAudioProcessorEditor::~VoidAudioProcessorEditor()
{
    vacuumSlider.setLookAndFeel (nullptr);
    loadButton.removeListener (this);
    unloadButton.removeListener (this);
}

void VoidAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Draw solid black background
    g.fillAll (juce::Colours::black);

    // Draw Title (VOID) with spread tracking spacing
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (24.0f).withStyle ("Bold"));
    g.drawText ("V  O  I  D", 0, 30, getWidth(), 40, juce::Justification::centred);

    // Under-title line indicator
    g.setColour (juce::Colour (0x1CFFFFFF));
    g.drawHorizontalLine (70, 40.0f, getWidth() - 40.0f);

    // ACCRETION DISK visual ring in background of the knob
    auto centerKnob = vacuumSlider.getBounds().toFloat();
    auto radius = std::min (centerKnob.getWidth(), centerKnob.getHeight()) / 2.0f - 10.0f;
    g.setColour (juce::Colour (0x0A7C3AED)); // Extremely faint violet glow
    g.fillEllipse (centerKnob.getCentreX() - radius, centerKnob.getCentreY() - radius, radius * 2.0f, radius * 2.0f);
}

void VoidAudioProcessorEditor::resized()
{
    // Model Path Display
    modelPathLabel.setBounds (40, 80, 320, 30);

    // Center Massive Knob
    vacuumSlider.setBounds (100, 130, 200, 200);
    vacuumLabel.setBounds (100, 340, 200, 20);

    // Footer controls
    loadButton.setBounds (40, 390, 100, 30);
    unloadButton.setBounds (150, 390, 70, 30);
    bypassToggle.setBounds (280, 390, 80, 30);
}

void VoidAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &loadButton)
    {
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        
        auto chooser = std::make_shared<juce::FileChooser> (
            "Select ONNX Model File...",
            juce::File{},
            "*.onnx");

        chooser->launchAsync (chooserFlags, [this, chooser] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                if (audioProcessor.loadModel (file.getFullPathName()))
                    updateModelPathDisplay();
            }
        });
    }
    else if (button == &unloadButton)
    {
        audioProcessor.unloadModel();
        updateModelPathDisplay();
    }
}

void VoidAudioProcessorEditor::updateModelPathDisplay()
{
    if (audioProcessor.isModelLoaded())
    {
        auto file = juce::File (audioProcessor.getLoadedModelPath());
        modelPathLabel.setText ("VACUUM LAYER ACTIVE: " + file.getFileName().toUpperCase(), juce::dontSendNotification);
        modelPathLabel.setColour (juce::Label::textColourId, juce::Colour (0xFFA78BFA)); // Soft Violet
    }
    else
    {
        modelPathLabel.setText ("VACUUM LAYER IDLE (CLEAN BYPASS)", juce::dontSendNotification);
        modelPathLabel.setColour (juce::Label::textColourId, juce::Colour (0xFF6B7280)); // Medium Gray
    }
}
