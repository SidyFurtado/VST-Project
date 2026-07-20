#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

// ==============================================================================
// VoidAudioProcessorEditor — Implementation
// ==============================================================================

VoidAudioProcessorEditor::VoidAudioProcessorEditor (VoidAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Configure mix slider
    mixSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    mixSlider.setRange (0.0, 1.0, 0.01);
    addAndMakeVisible (mixSlider);

    // Configure output gain slider
    outputSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    outputSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    outputSlider.setRange (-12.0, 12.0, 0.1);
    addAndMakeVisible (outputSlider);

    // Configure labels
    mixLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    mixLabel.setJustificationType (juce::Justification::centred);
    mixLabel.setColour (juce::Label::textColourId, juce::Colour (0xFF9CA3AF)); // Gray
    addAndMakeVisible (mixLabel);

    outputLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    outputLabel.setJustificationType (juce::Justification::centred);
    outputLabel.setColour (juce::Label::textColourId, juce::Colour (0xFF9CA3AF)); // Gray
    addAndMakeVisible (outputLabel);

    // Model path display
    modelPathLabel.setFont (juce::Font (11.0f, juce::Font::plain));
    modelPathLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (modelPathLabel);

    // Bypass toggle
    addAndMakeVisible (bypassToggle);

    // Buttons
    loadButton.addListener (this);
    unloadButton.addListener (this);
    addAndMakeVisible (loadButton);
    addAndMakeVisible (unloadButton);

    // Attachments
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::mix(), mixSlider);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, VoidParams::outputGain(), outputSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, VoidParams::bypass(), bypassToggle);

    // Update path label
    updateModelPathDisplay();

    // Style buttons/sliders
    auto emerald = juce::Colour (0xFF10B981);
    auto darkBg  = juce::Colour (0xFF1E293B);
    
    // Sliders coloring
    mixSlider.setColour (juce::Slider::rotarySliderFillColourId, emerald);
    mixSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    outputSlider.setColour (juce::Slider::rotarySliderFillColourId, emerald);
    outputSlider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    
    // Toggle coloring
    bypassToggle.setColour (juce::ToggleButton::tickColourId, emerald);

    // Buttons coloring
    loadButton.setColour (juce::TextButton::buttonColourId, darkBg);
    loadButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    unloadButton.setColour (juce::TextButton::buttonColourId, darkBg.darker());
    unloadButton.setColour (juce::TextButton::textColourOffId, juce::Colours::lightgrey);

    setSize (500, 300);
}

VoidAudioProcessorEditor::~VoidAudioProcessorEditor()
{
    loadButton.removeListener (this);
    unloadButton.removeListener (this);
}

void VoidAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Draw deep premium gradient background
    auto fillGrad = juce::ColourGradient (
        juce::Colour (0xFF0F172A), 0.0f, 0.0f,
        juce::Colour (0xFF020617), 0.0f, 300.0f, false);
    g.setGradientFill (fillGrad);
    g.fillAll();

    // Draw glassmorphic display box in the upper section
    g.setColour (juce::Colour (0x10FFFFFF)); // Semi-transparent white
    g.fillRoundedRectangle (20.0f, 60.0f, 460.0f, 80.0f, 8.0f);
    
    g.setColour (juce::Colour (0x20FFFFFF));
    g.drawRoundedRectangle (20.0f, 60.0f, 460.0f, 80.0f, 8.0f, 1.0f);

    // Title / Brand
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (18.0f, juce::Font::bold));
    g.drawText ("VOID", 20, 20, 200, 30, juce::Justification::left);
    
    g.setColour (juce::Colour (0xFF10B981)); // Emerald
    g.setFont (juce::Font (10.0f, juce::Font::bold));
    g.drawText ("ASTRA AUDIO SUITE // REAL-TIME AI", 220, 20, 260, 30, juce::Justification::right);
}

void VoidAudioProcessorEditor::resized()
{
    // Layout display box contents
    loadButton.setBounds (35, 80, 110, 40);
    unloadButton.setBounds (155, 80, 70, 40);
    modelPathLabel.setBounds (235, 75, 230, 50);

    // Layout bottom controls
    bypassToggle.setBounds (40, 200, 80, 30);
    
    // Sliders
    mixSlider.setBounds (170, 160, 100, 100);
    mixLabel.setBounds (170, 265, 100, 20);

    outputSlider.setBounds (310, 160, 100, 100);
    outputLabel.setBounds (310, 265, 100, 20);
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
        modelPathLabel.setText ("Model: " + file.getFileName(), juce::dontSendNotification);
        modelPathLabel.setColour (juce::Label::textColourId, juce::Colour (0xFF34D399)); // Emerald Green
    }
    else
    {
        modelPathLabel.setText ("No Model Loaded\n(Bypassed)", juce::dontSendNotification);
        modelPathLabel.setColour (juce::Label::textColourId, juce::Colour (0xFF9CA3AF)); // Gray
    }
}
