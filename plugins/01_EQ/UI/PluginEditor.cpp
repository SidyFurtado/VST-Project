#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"
#include <cmath>
#include <map>

namespace
{
    constexpr auto resetComponentId = "aureq-reset-button";
    constexpr auto bypassComponentId = "aureq-bypass-button";
    constexpr auto themeComponentId = "aureq-theme-button";

    juce::PropertiesFile::Options createLocalizationPreferenceOptions()
    {
        juce::PropertiesFile::Options options;
        options.applicationName = "AUREQ";
        options.filenameSuffix = ".settings";
        options.folderName = "AUREQ";
        options.osxLibrarySubFolder = "Application Support";
        options.storageFormat = juce::PropertiesFile::storeAsXML;
        return options;
    }
}

AureqLookAndFeel::AureqLookAndFeel()
{
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (34, 48, 64));
    setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (34, 211, 238));
    setColour (juce::Slider::thumbColourId, juce::Colour (14, 22, 32));
    setColour (juce::Label::textColourId, juce::Colour (248, 250, 252));
}

void AureqLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, 
                                       float sliderPosProportionDelta, float rotaryStartAngle, float rotaryEndAngle, 
                                       juce::Slider& slider)
{
    if (slider.getName() == "DynamicRange")
    {
        auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
        
        bool isEnabled = slider.isEnabled();
        if (!isEnabled)
            return;
            
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
        float textHeight = 12.0f;
        auto knobBounds = bounds.reduced (2.0f);
        knobBounds.removeFromBottom (textHeight);
        
        auto size = std::min (knobBounds.getWidth(), knobBounds.getHeight());
        auto cx = knobBounds.getCentreX();
        auto cy = knobBounds.getCentreY();
        auto radius = size * 0.5f - 2.0f;
        
        g.saveState();
        
        // 1. Draw background thin ring track
        juce::Path backgroundTrack;
        backgroundTrack.addCentredArc (cx, cy, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (outline.withAlpha (0.05f));
        g.strokePath (backgroundTrack, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // 2. Draw active track (bipolar: center is 0.5)
        if (std::abs (sliderPosProportionDelta - 0.5f) > 0.001f)
        {
            float startAngle = rotaryStartAngle + 0.5f * (rotaryEndAngle - rotaryStartAngle);
            float endAngle = rotaryStartAngle + sliderPosProportionDelta * (rotaryEndAngle - rotaryStartAngle);
            
            juce::Path activeTrack;
            activeTrack.addCentredArc (cx, cy, radius, radius, 0.0f, startAngle, endAngle, true);
            
            // Choose color: Expansion (positive, delta > 0.5) -> Cyan, Compression (negative, delta < 0.5) -> Orange
            juce::Colour ringColor = (sliderPosProportionDelta > 0.5f) 
                ? juce::Colour (34, 211, 238) // Cyan
                : juce::Colour (251, 146, 60); // Orange
                
            // Draw glow
            g.setColour (ringColor.withAlpha (0.15f));
            g.strokePath (activeTrack, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            
            // Draw solid line
            g.setColour (ringColor);
            g.strokePath (activeTrack, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
        
        g.restoreState();
        return;
    }

    auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (juce::Slider::rotarySliderFillColourId);
    auto thumb   = slider.findColour (juce::Slider::thumbColourId);
    
    bool isEnabled = slider.isEnabled();
    if (!isEnabled)
    {
        outline = outline.withAlpha (0.03f);
        fill = fill.withAlpha (0.10f);
        thumb = thumb.withAlpha (0.15f);
    }
    
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
    
    float textHeight = 12.0f;
    auto knobBounds = bounds.reduced (2.0f);
    knobBounds.removeFromBottom (textHeight);
    
    auto size = std::min (knobBounds.getWidth(), knobBounds.getHeight());
    auto cx = knobBounds.getCentreX();
    auto cy = knobBounds.getCentreY();
    auto activeArea = juce::Rectangle<float> (cx - size * 0.5f, cy - size * 0.5f, size, size);
    
    auto radius = size * 0.5f - 2.5f;
    
    g.saveState();
    
    // 1. Draw outer glass housing bezel/shadow ring
    g.setColour (juce::Colours::black.withAlpha (isEnabled ? 0.22f : 0.08f));
    g.fillEllipse (cx - radius - 1.0f, cy - radius, (radius + 1.0f) * 2.0f, (radius + 1.0f) * 2.0f);
    
    g.setColour (juce::Colours::white.withAlpha (isEnabled ? 0.06f : 0.02f));
    g.drawEllipse (cx - radius - 1.0f, cy - radius - 1.0f, (radius + 1.0f) * 2.0f, (radius + 1.0f) * 2.0f, 1.0f);
    
    // 2. Draw background track (very thin and discrete)
    juce::Path backgroundTrack;
    backgroundTrack.addCentredArc (cx, cy, radius - 3.5f, radius - 3.5f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (outline.withAlpha (isEnabled ? 0.08f : 0.03f));
    g.strokePath (backgroundTrack, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // 3. Draw active track (fine glowing cyan/mint line)
    if (sliderPosProportionDelta > 0.0f)
    {
        auto activeAngle = rotaryStartAngle + sliderPosProportionDelta * (rotaryEndAngle - rotaryStartAngle);
        juce::Path activeTrack;
        activeTrack.addCentredArc (cx, cy, radius - 3.5f, radius - 3.5f, 0.0f, rotaryStartAngle, activeAngle, true);
        
        // Active track glow
        g.setColour (fill.withAlpha (isEnabled ? 0.15f : 0.05f));
        g.strokePath (activeTrack, juce::PathStrokeType (4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // Solid core active track
        g.setColour (fill);
        g.strokePath (activeTrack, juce::PathStrokeType (2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    
    // 4. Central Cap (luminous glass button cap)
    auto capRadius = radius - 5.5f;
    auto capBounds = juce::Rectangle<float> (cx - capRadius, cy - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    
    // Soft drop shadow under cap
    g.setColour (juce::Colours::black.withAlpha (isEnabled ? 0.40f : 0.15f));
    g.fillEllipse (cx - capRadius, cy - capRadius + 2.0f, capRadius * 2.0f, capRadius * 2.0f);
    
    // Cap vertical glass gradient
    juce::ColourGradient capGrad (thumb.brighter (0.05f), cx, cy - capRadius,
                                  thumb.darker (0.16f), cx, cy + capRadius, false);
    g.setGradientFill (capGrad);
    g.fillEllipse (capBounds);
    
    // Double-bezel inner cap reflection
    g.setColour (juce::Colours::white.withAlpha (isEnabled ? 0.08f : 0.03f));
    g.drawEllipse (capBounds.reduced (0.5f), 1.0f);
    
    // Specular highlight: white crescent glow on the top half of the cap
    g.setColour (juce::Colours::white.withAlpha (isEnabled ? 0.12f : 0.04f));
    g.fillEllipse (cx - capRadius * 0.4f, cy - capRadius * 0.5f, capRadius * 0.4f, capRadius * 0.4f);
    
    // 5. Draw active indicator pointer line
    auto angle = rotaryStartAngle + sliderPosProportionDelta * (rotaryEndAngle - rotaryStartAngle);
    juce::Path p;
    // Offset the line start slightly from the absolute center to make it look clean
    p.startNewSubPath (cx + (capRadius * 0.25f) * std::sin (angle), cy - (capRadius * 0.25f) * std::cos (angle));
    p.lineTo (cx + (capRadius - 1.0f) * std::sin (angle), cy - (capRadius - 1.0f) * std::cos (angle));
    
    // Draw pointer glow
    g.setColour (fill.withAlpha (isEnabled ? 0.25f : 0.08f));
    g.strokePath (p, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    // Pointer solid core
    g.setColour (fill.withAlpha (isEnabled ? 1.0f : 0.3f));
    g.strokePath (p, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    g.restoreState();
    
    // 6. Value text display below the knob
    g.setColour (slider.findColour (juce::Label::textColourId).withAlpha (isEnabled ? 0.70f : 0.28f));
    g.setFont (juce::Font (juce::FontOptions ("Roboto Mono", 8.5f, juce::Font::plain)));
    
    juce::String valText = slider.getTextFromValue (slider.getValue());
    g.drawText (valText, 0, juce::roundToInt (height - textHeight - 2), width, juce::roundToInt (textHeight), 
                juce::Justification::centred, true);
}

void AureqLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, 
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused (backgroundColour);
    auto bounds = button.getLocalBounds().toFloat();
    float r = bounds.getHeight() * 0.5f;
    
    g.saveState();
    
    const bool isReset = button.getComponentID() == resetComponentId;
    const bool isDelete = button.getComponentID() == "delete-button";
    bool isToggled = button.getToggleState();
    
    // Glass base color
    juce::Colour baseColor;
    if (isToggled)
    {
        baseColor = juce::Colour (34, 211, 238).withAlpha (shouldDrawButtonAsDown ? 0.24f : (shouldDrawButtonAsHighlighted ? 0.18f : 0.12f));
    }
    else if (isReset)
    {
        baseColor = juce::Colour (251, 191, 36).withAlpha (shouldDrawButtonAsDown ? 0.25f : (shouldDrawButtonAsHighlighted ? 0.16f : 0.08f));
    }
    else if (isDelete)
    {
        baseColor = juce::Colour (248, 113, 113).withAlpha (shouldDrawButtonAsDown ? 0.25f : (shouldDrawButtonAsHighlighted ? 0.16f : 0.08f));
    }
    else
    {
        baseColor = juce::Colour (30, 48, 72).withAlpha (shouldDrawButtonAsDown ? 0.32f : (shouldDrawButtonAsHighlighted ? 0.20f : 0.12f));
    }
    
    juce::ColourGradient grad (baseColor.brighter (0.04f), bounds.getX(), bounds.getY(),
                               baseColor.darker (0.02f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (bounds, r);
    
    // Soft specular border
    juce::Colour borderColors;
    if (isToggled)
    {
        borderColors = juce::Colour (34, 211, 238).withAlpha (shouldDrawButtonAsHighlighted ? 0.50f : 0.30f);
    }
    else if (isReset)
    {
        borderColors = juce::Colour (251, 191, 36).withAlpha (shouldDrawButtonAsHighlighted ? 0.35f : 0.20f);
    }
    else if (isDelete)
    {
        borderColors = juce::Colour (248, 113, 113).withAlpha (shouldDrawButtonAsHighlighted ? 0.35f : 0.20f);
    }
    else
    {
        borderColors = juce::Colours::white.withAlpha (shouldDrawButtonAsHighlighted ? 0.16f : 0.08f);
    }
        
    g.setColour (borderColors);
    g.drawRoundedRectangle (bounds.reduced (0.5f), r, 1.0f);
    
    // Top refraction edge line
    g.setColour (juce::Colours::white.withAlpha (shouldDrawButtonAsHighlighted ? 0.20f : 0.08f));
    juce::Path topHighlight;
    topHighlight.startNewSubPath (bounds.getX() + r, bounds.getY());
    topHighlight.lineTo (bounds.getRight() - r, bounds.getY());
    g.strokePath (topHighlight, juce::PathStrokeType (1.0f));
    
    g.restoreState();
}

void AureqLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, 
                                     bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    
    g.setFont (juce::Font (juce::FontOptions ("Inter", button.getHeight() * 0.40f, juce::Font::bold)));
    
    juce::Colour textColor = juce::Colour (248, 250, 252); // Primary light text
    const bool isReset = button.getComponentID() == resetComponentId;
    const bool isDelete = button.getComponentID() == "delete-button";
    bool isToggled = button.getToggleState();
    
    if (isToggled)
    {
        textColor = juce::Colour (34, 211, 238);
    }
    else if (isReset)
    {
        textColor = juce::Colour (251, 191, 36);
    }
    else if (isDelete)
    {
        textColor = juce::Colour (248, 113, 113);
    }
        
    if (!button.isEnabled())
        textColor = textColor.withAlpha (0.35f);
    else if (shouldDrawButtonAsDown)
        textColor = textColor.brighter (0.15f);
    else if (shouldDrawButtonAsHighlighted)
        textColor = textColor.brighter (0.08f);
        
    g.setColour (textColor);
    g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
}

void AureqLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, 
                                       bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    auto bounds = button.getLocalBounds().toFloat();
    const bool isBypassed = button.getComponentID() == bypassComponentId;
    const bool isTheme = button.getComponentID() == themeComponentId;
    bool isToggled = button.getToggleState();
    
    g.saveState();
    
    float r = bounds.getHeight() * 0.5f;
    
    juce::Colour fillColors;
    juce::Colour borderColors;
    juce::Colour textColors;
    
    if (isBypassed)
    {
        fillColors = isToggled ? juce::Colour (248, 113, 113).withAlpha (0.08f) : juce::Colour (30, 48, 72).withAlpha (0.08f);
        borderColors = isToggled ? juce::Colour (248, 113, 113).withAlpha (0.30f) : juce::Colour (255, 255, 255).withAlpha (0.08f);
        textColors = isToggled ? juce::Colour (248, 113, 113) : juce::Colour (148, 163, 184);
    }
    else if (isTheme)
    {
        fillColors = juce::Colour (34, 211, 238).withAlpha (0.06f);
        borderColors = juce::Colour (34, 211, 238).withAlpha (0.20f);
        textColors = juce::Colour (34, 211, 238);
    }
    else // normal/selected band controls
    {
        fillColors = isToggled ? juce::Colour (34, 211, 238).withAlpha (0.08f) : juce::Colour (30, 48, 72).withAlpha (0.08f);
        borderColors = isToggled ? juce::Colour (34, 211, 238).withAlpha (0.30f) : juce::Colour (255, 255, 255).withAlpha (0.08f);
        textColors = isToggled ? juce::Colour (34, 211, 238) : juce::Colour (148, 163, 184);
    }
    
    juce::ColourGradient fillGrad (fillColors.brighter (0.02f), bounds.getX(), bounds.getY(),
                                   fillColors.darker (0.02f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill (fillGrad);
    g.fillRoundedRectangle (bounds, r);
    
    g.setColour (borderColors);
    g.drawRoundedRectangle (bounds.reduced (0.5f), r, 1.0f);
    
    // Top specular highlight line on toggle pill
    g.setColour (juce::Colours::white.withAlpha (shouldDrawButtonAsHighlighted ? 0.12f : 0.05f));
    juce::Path toggleTopHighlight;
    toggleTopHighlight.startNewSubPath (bounds.getX() + r, bounds.getY());
    toggleTopHighlight.lineTo (bounds.getRight() - r, bounds.getY());
    g.strokePath (toggleTopHighlight, juce::PathStrokeType (0.75f));
    
    float ledSize = 5.0f;
    float ledX = bounds.getX() + 10.0f;
    float ledY = bounds.getY() + (bounds.getHeight() - ledSize) * 0.5f;
    
    g.setColour (textColors.withAlpha (0.35f));
    g.fillEllipse (ledX - 1.5f, ledY - 1.5f, ledSize + 3.0f, ledSize + 3.0f);
    g.setColour (textColors);
    g.fillEllipse (ledX, ledY, ledSize, ledSize);
    
    g.setColour (textColors);
    g.setFont (juce::Font (juce::FontOptions ("Inter", bounds.getHeight() * 0.42f, juce::Font::bold)));
    
    juce::Rectangle<int> textRect (juce::roundToInt (bounds.getX() + 16.0f), juce::roundToInt (bounds.getY()), 
                                   juce::roundToInt (bounds.getWidth() - 22.0f), juce::roundToInt (bounds.getHeight()));
    g.drawText (button.getButtonText(), textRect, juce::Justification::centred, true);
    
    g.restoreState();
}

// =============================================================================
// FloatingBandPanel Implementation
// =============================================================================

FloatingBandPanel::FloatingBandPanel (AureqAudioProcessor& processor, juce::LookAndFeel& lookAndFeel)
    : audioProcessor (processor)
{
    // Apply LookAndFeel
    freqSlider.setLookAndFeel (&lookAndFeel);
    gainSlider.setLookAndFeel (&lookAndFeel);
    qSlider.setLookAndFeel (&lookAndFeel);
    dynRangeSlider.setLookAndFeel (&lookAndFeel);
    bandBypassBtn.setLookAndFeel (&lookAndFeel);
    removeBandBtn.setLookAndFeel (&lookAndFeel);

    // Setup sliders
    freqSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    freqSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (freqSlider);

    gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (gainSlider);

    qSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    qSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (qSlider);

    dynRangeSlider.setName ("DynamicRange");
    dynRangeSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    dynRangeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (dynRangeSlider);

    // Setup buttons
    bandBypassBtn.setButtonText ("Bypass");
    bandBypassBtn.setComponentID ("aureq-bypass-button");
    addAndMakeVisible (bandBypassBtn);

    removeBandBtn.setButtonText ("Delete");
    removeBandBtn.setComponentID ("delete-button");
    addAndMakeVisible (removeBandBtn);

    // Setup realtime meter label
    dynGainMeterLabel.setText ("Δ -", juce::dontSendNotification);
    dynGainMeterLabel.setJustificationType (juce::Justification::centred);
    dynGainMeterLabel.setFont (juce::Font (juce::FontOptions ("Roboto Mono", 10.0f, juce::Font::bold)));
    dynGainMeterLabel.setColour (juce::Label::textColourId, juce::Colour (34, 211, 238));
    addAndMakeVisible (dynGainMeterLabel);

    // Setup slider checkpoint callbacks
    freqSlider.onDragStart = [this]() { audioProcessor.captureUndoCheckpoint(); };
    gainSlider.onDragStart = [this]() { audioProcessor.captureUndoCheckpoint(); };
    qSlider.onDragStart = [this]() { audioProcessor.captureUndoCheckpoint(); };
    dynRangeSlider.onDragStart = [this]() { audioProcessor.captureUndoCheckpoint(); };

    removeBandBtn.onClick = [this]()
    {
        if (onDeleteClicked)
            onDeleteClicked();
    };

    // Soft drop shadow effect
    juce::DropShadow shadow;
    shadow.colour = juce::Colours::black.withAlpha (0.45f);
    shadow.radius = 16;
    shadow.offset = juce::Point<int> (0, 4);
    shadowEffect.setShadowProperties (shadow);
    setComponentEffect (&shadowEffect);
}

FloatingBandPanel::~FloatingBandPanel()
{
    freqSlider.setLookAndFeel (nullptr);
    gainSlider.setLookAndFeel (nullptr);
    qSlider.setLookAndFeel (nullptr);
    dynRangeSlider.setLookAndFeel (nullptr);
    bandBypassBtn.setLookAndFeel (nullptr);
    removeBandBtn.setLookAndFeel (nullptr);
}

void FloatingBandPanel::updateSelection (int bandIndex)
{
    selectedBandIndex = bandIndex;

    freqAttachment.reset();
    gainAttachment.reset();
    qAttachment.reset();
    dynRangeAttachment.reset();
    bandBypassAttachment.reset();

    if (bandIndex >= 0 && bandIndex < AUREQ::Params::numBands)
    {
        freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            audioProcessor.apvts, AUREQ::Params::bandFrequencyID (bandIndex), freqSlider);
        gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            audioProcessor.apvts, AUREQ::Params::bandGainID (bandIndex), gainSlider);
        qAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            audioProcessor.apvts, AUREQ::Params::bandQID (bandIndex), qSlider);
        dynRangeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            audioProcessor.apvts, AUREQ::Params::bandDynamicRangeID (bandIndex), dynRangeSlider);
        bandBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
            audioProcessor.apvts, AUREQ::Params::bandBypassID (bandIndex), bandBypassBtn);

        freqSlider.setEnabled (true);
        gainSlider.setEnabled (true);
        qSlider.setEnabled (true);
        bandBypassBtn.setEnabled (true);
        removeBandBtn.setEnabled (true);

        freqSlider.setAlpha (1.0f);
        bandBypassBtn.setAlpha (1.0f);
        removeBandBtn.setAlpha (1.0f);

        // Apply initial visual dimming for Q/Gain/Dyn based on type
        if (auto* typeParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandTypeID (bandIndex)))
        {
            int activeType = juce::roundToInt (typeParam->load());
            bool usesGain = (activeType == 0 || activeType == 3 || activeType == 4);
            bool usesQ    = (activeType == 0 || activeType == 1 || activeType == 2 || activeType == 5 || activeType == 6);
            gainSlider.setAlpha (usesGain ? 1.0f : 0.35f);
            qSlider.setAlpha (usesQ ? 1.0f : 0.35f);

            bool supportsDyn = (activeType == 0 || activeType == 3 || activeType == 4);
            dynRangeSlider.setEnabled (supportsDyn);
            dynRangeSlider.setAlpha (supportsDyn ? 1.0f : 0.35f);
        }
    }
    else
    {
        freqSlider.setEnabled (false);
        gainSlider.setEnabled (false);
        qSlider.setEnabled (false);
        bandBypassBtn.setEnabled (false);
        removeBandBtn.setEnabled (false);

        freqSlider.setAlpha (0.35f);
        gainSlider.setAlpha (0.35f);
        qSlider.setAlpha (0.35f);
        bandBypassBtn.setAlpha (0.35f);
        removeBandBtn.setAlpha (0.35f);

        dynRangeSlider.setEnabled (false);
        dynRangeSlider.setAlpha (0.35f);
    }
}

void FloatingBandPanel::updateThemeColors (const AUREQ::ThemeColors& colors)
{
    themeColors = colors;

    freqSlider.setColour (juce::Label::textColourId, colors.textPrimary);
    gainSlider.setColour (juce::Label::textColourId, colors.textPrimary);
    qSlider.setColour (juce::Label::textColourId, colors.textPrimary);
    dynRangeSlider.setColour (juce::Label::textColourId, colors.textPrimary);

    freqSlider.setColour (juce::Slider::rotarySliderOutlineColourId, colors.border.withAlpha (0.4f));
    freqSlider.setColour (juce::Slider::rotarySliderFillColourId, colors.accent.withAlpha (0.8f));
    freqSlider.setColour (juce::Slider::thumbColourId, colors.textPrimary);

    gainSlider.setColour (juce::Slider::rotarySliderOutlineColourId, colors.border.withAlpha (0.4f));
    gainSlider.setColour (juce::Slider::rotarySliderFillColourId, colors.accent.withAlpha (0.8f));
    gainSlider.setColour (juce::Slider::thumbColourId, colors.textPrimary);

    qSlider.setColour (juce::Slider::rotarySliderOutlineColourId, colors.border.withAlpha (0.4f));
    qSlider.setColour (juce::Slider::rotarySliderFillColourId, colors.accent.withAlpha (0.8f));
    qSlider.setColour (juce::Slider::thumbColourId, colors.textPrimary);

    dynRangeSlider.setColour (juce::Slider::rotarySliderOutlineColourId, colors.border.withAlpha (0.4f));
    dynRangeSlider.setColour (juce::Slider::rotarySliderFillColourId, colors.accent.withAlpha (0.8f));
    dynRangeSlider.setColour (juce::Slider::thumbColourId, colors.textPrimary);

    repaint();
}

void FloatingBandPanel::updateRealtimeMeter (const AUREQ::ThemeColors& colors)
{
    if (selectedBandIndex < 0 || selectedBandIndex >= AUREQ::Params::numBands)
    {
        dynGainMeterLabel.setText ("", juce::dontSendNotification);
        return;
    }

    int activeType = 0;
    if (auto* typeParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandTypeID (selectedBandIndex)))
        activeType = juce::roundToInt (typeParam->load());

    bool supportsDyn = (activeType == 0 || activeType == 3 || activeType == 4);
    if (supportsDyn)
    {
        auto* pDynRange = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandDynamicRangeID (selectedBandIndex));
        bool isDynEnabled = (pDynRange != nullptr) ? (std::abs (pDynRange->load()) > 1e-4f) : false;

        if (isDynEnabled)
        {
            float dynamicGainDb = audioProcessor.getBandDynamicGainDb (selectedBandIndex);
            if (std::abs (dynamicGainDb) > 0.05f)
            {
                juce::String text = "Δ " + (dynamicGainDb >= 0.0f ? juce::String ("+") : juce::String()) + juce::String (dynamicGainDb, 1) + " dB";
                dynGainMeterLabel.setText (text, juce::dontSendNotification);

                juce::Colour meterColor = (dynamicGainDb > 0.0f) ? colors.accent : colors.accentSoft;
                dynGainMeterLabel.setColour (juce::Label::textColourId, meterColor);
                dynGainMeterLabel.setAlpha (1.0f);
            }
            else
            {
                dynGainMeterLabel.setText ("Δ 0.0 dB", juce::dontSendNotification);
                dynGainMeterLabel.setColour (juce::Label::textColourId, colors.textPrimary);
                dynGainMeterLabel.setAlpha (1.0f);
            }
        }
        else
        {
            dynGainMeterLabel.setText ("Δ -", juce::dontSendNotification);
            dynGainMeterLabel.setColour (juce::Label::textColourId, colors.textSecondary.withAlpha (0.4f));
            dynGainMeterLabel.setAlpha (0.35f);
        }
    }
    else
    {
        dynGainMeterLabel.setText ("", juce::dontSendNotification);
        dynGainMeterLabel.setAlpha (0.0f);
    }
}

void FloatingBandPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Glass panel simulation (semitransparent background)
    g.setColour (themeColors.panelElevated.withAlpha (0.85f));
    g.fillRoundedRectangle (bounds, 12.0f);
    
    // Border
    g.setColour (themeColors.border.withAlpha (0.25f));
    g.drawRoundedRectangle (bounds, 12.0f, 1.0f);
}

void FloatingBandPanel::resized()
{
    int panelW = getWidth();
    int panelH = getHeight();

    // Concentric sliders in the center (center is X = panelW/2, Y = 50)
    int centerX = panelW / 2;
    int centerY = 50;

    // dynRangeSlider: 76x82 (concentric)
    dynRangeSlider.setBounds (centerX - 38, centerY - 41, 76, 82);
    // gainSlider: 60x66 (concentric)
    gainSlider.setBounds (centerX - 30, centerY - 33, 60, 66);
    gainSlider.toFront (false);

    // Lateral knobs (Y = centerY - 27, W = 48, H = 54)
    freqSlider.setBounds (15, centerY - 27, 48, 54);
    qSlider.setBounds (panelW - 15 - 48, centerY - 27, 48, 54);

    // Meter Label right below concentric sliders
    dynGainMeterLabel.setBounds (centerX - 60, centerY + 43, 120, 12);

    // Bypass and Delete buttons at the bottom (Y = panelH - 28, height = 20)
    int btnH = 20;
    int btnW = 76;
    int btnY = panelH - 28;

    bandBypassBtn.setBounds (15, btnY, btnW, btnH);
    removeBandBtn.setBounds (panelW - 15 - btnW, btnY, btnW, btnH);
}

AureqAudioProcessorEditor::AureqAudioProcessorEditor (AureqAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      floatingPanel (p, aureqLookAndFeel)
{
    setWantsKeyboardFocus (true);
    currentLanguage = loadLanguagePreference();

    // Apply LookAndFeel
    inputGainSlider.setLookAndFeel (&aureqLookAndFeel);
    outputGainSlider.setLookAndFeel (&aureqLookAndFeel);
    
    globalBypassButton.setLookAndFeel (&aureqLookAndFeel);
    themeToggleButton.setLookAndFeel (&aureqLookAndFeel);
    
    // Apply LookAndFeel to header buttons
    prevPresetBtn.setLookAndFeel (&aureqLookAndFeel);
    nextPresetBtn.setLookAndFeel (&aureqLookAndFeel);
    abBtn.setLookAndFeel (&aureqLookAndFeel);
    settingsBtn.setLookAndFeel (&aureqLookAndFeel);
    themeHeaderBtn.setLookAndFeel (&aureqLookAndFeel);
    bypassHeaderBtn.setLookAndFeel (&aureqLookAndFeel);
    resetBtn.setLookAndFeel (&aureqLookAndFeel);

    globalBypassButton.setComponentID (bypassComponentId);
    bypassHeaderBtn.setComponentID (bypassComponentId);
    themeToggleButton.setComponentID (themeComponentId);
    themeHeaderBtn.setComponentID (themeComponentId);
    resetBtn.setComponentID (resetComponentId);
    
    // Sliders setup
    inputGainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (inputGainSlider);
    
    outputGainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (outputGainSlider);
    
    // Global Buttons setup
    globalBypassButton.setButtonText ("Bypass");
    addAndMakeVisible (globalBypassButton);
    
    themeToggleButton.setButtonText ("Theme");
    addAndMakeVisible (themeToggleButton);

    // Add floating panel
    addAndMakeVisible (floatingPanel);
    floatingPanel.setVisible (false);

    // Setup Floating Panel Delete Action
    floatingPanel.onDeleteClicked = [this]()
    {
        int bandIdx = graphView.getSelectedBandIndex();
        if (bandIdx >= 0 && bandIdx < AUREQ::Params::numBands)
        {
            audioProcessor.captureUndoCheckpoint();

            if (auto* enabledParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandEnabledID (bandIdx)))
            {
                enabledParam->beginChangeGesture();
                enabledParam->setValueNotifyingHost (0.0f);
                enabledParam->endChangeGesture();
            }
            if (auto* bypassParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandBypassID (bandIdx)))
            {
                bypassParam->beginChangeGesture();
                bypassParam->setValueNotifyingHost (0.0f);
                bypassParam->endChangeGesture();
            }

            int nextSelectedIdx = -1;
            for (int i = 0; i < AUREQ::Params::numBands; ++i)
            {
                if (auto* enabledParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandEnabledID (i)))
                {
                    if (enabledParam->load() > 0.5f)
                    {
                        nextSelectedIdx = i;
                        break;
                    }
                }
            }

            graphView.setSelectedBandIndex (nextSelectedIdx);
            updateSelectedBandControls();
        }
    };
    
    // Header Buttons setup
    prevPresetBtn.setButtonText ("<");
    addAndMakeVisible (prevPresetBtn);
    nextPresetBtn.setButtonText (">");
    addAndMakeVisible (nextPresetBtn);
    
    abBtn.setButtonText (audioProcessor.getActiveABSlotName());
    addAndMakeVisible (abBtn);
    settingsBtn.setButtonText ("Settings");
    addAndMakeVisible (settingsBtn);
    themeHeaderBtn.setButtonText ("Theme");
    addAndMakeVisible (themeHeaderBtn);
    bypassHeaderBtn.setButtonText ("Bypass");
    addAndMakeVisible (bypassHeaderBtn);
    resetBtn.setButtonText ("Reset");
    addAndMakeVisible (resetBtn);
    
    // Preset Label
    presetLabel.setText (audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
    presetLabel.setJustificationType (juce::Justification::centred);
    presetLabel.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible (presetLabel);
    presetLabel.addMouseListener (this, false);
    abBtn.addMouseListener (this, false);
    globalBypassButton.addMouseListener (this, false);
    
    // Setup preset cycling callback
    prevPresetBtn.onClick = [this]()
    {
        if (auto* pm = audioProcessor.getPresetManager())
        {
            int numPresets = pm->getNumPresets();
            if (numPresets > 0)
            {
                int nextIndex = (pm->getCurrentPresetIndex() - 1 + numPresets) % numPresets;
                loadPresetAndRefresh (nextIndex);
            }
        }
    };
    nextPresetBtn.onClick = [this]()
    {
        if (auto* pm = audioProcessor.getPresetManager())
        {
            int numPresets = pm->getNumPresets();
            if (numPresets > 0)
            {
                int nextIndex = (pm->getCurrentPresetIndex() + 1) % numPresets;
                loadPresetAndRefresh (nextIndex);
            }
        }
    };

    abBtn.onClick = [this]()
    {
        if (juce::ModifierKeys::getCurrentModifiers().isPopupMenu())
            return;

        audioProcessor.toggleABSlot();
        abBtn.setButtonText (audioProcessor.getActiveABSlotName());
        presetLabel.setText (audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
        updateSelectedBandControls();
        timerCallback();
        repaint();
    };

    settingsBtn.onClick = [this]()
    {
        showSettingsMenu();
    };
    
    // Setup header Theme / Bypass / Reset callbacks
    themeHeaderBtn.onClick = [this]()
    {
        if (auto* param = audioProcessor.apvts.getParameter (AUREQ::Params::themeModeID()))
            param->setValueNotifyingHost (param->getValue() > 0.5f ? 0.0f : 1.0f);
    };
    
    bypassHeaderBtn.onClick = [this]()
    {
        if (auto* param = audioProcessor.apvts.getParameter (AUREQ::Params::globalBypassID()))
            param->setValueNotifyingHost (param->getValue() > 0.5f ? 0.0f : 1.0f);
    };
    
    resetBtn.onClick = [this]()
    {
        bool hasGainToReset = false;
        for (int i = 0; i < AUREQ::Params::numBands; ++i)
        {
            if (auto* gainValue = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandGainID (i)))
                hasGainToReset = hasGainToReset || std::abs (gainValue->load()) > 0.0001f;
        }

        if (hasGainToReset)
            audioProcessor.captureUndoCheckpoint();

        for (int i = 0; i < AUREQ::Params::numBands; ++i)
        {
            if (auto* gainParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandGainID (i)))
                gainParam->setValueNotifyingHost (gainParam->convertTo0to1 (0.0f));
        }
    };
    
    // Add subcomponents
    addAndMakeVisible (graphView);

    // Register parameter listener for themeMode
    audioProcessor.apvts.addParameterListener (AUREQ::Params::themeModeID(), this);

    inputGainSlider.onDragStart = [this]() { audioProcessor.captureUndoCheckpoint(); };
    outputGainSlider.onDragStart = [this]() { audioProcessor.captureUndoCheckpoint(); };

    // Setup band drag callbacks
    graphView.onDragStart = [this] (int bandIndex)
    {
        audioProcessor.captureUndoCheckpoint();

        if (auto* freqParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandFrequencyID (bandIndex)))
            freqParam->beginChangeGesture();
        if (auto* gainParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandGainID (bandIndex)))
            gainParam->beginChangeGesture();
    };

    graphView.onBandDragged = [this] (int bandIndex, float frequencyHz, float gainDb)
    {
        if (auto* freqParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandFrequencyID (bandIndex)))
            freqParam->setValueNotifyingHost (freqParam->convertTo0to1 (frequencyHz));
        if (auto* gainParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandGainID (bandIndex)))
            gainParam->setValueNotifyingHost (gainParam->convertTo0to1 (gainDb));

        updateSelectedBandControls();
    };

    graphView.onDragEnd = [this] (int bandIndex)
    {
        if (auto* freqParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandFrequencyID (bandIndex)))
            freqParam->endChangeGesture();
        if (auto* gainParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandGainID (bandIndex)))
            gainParam->endChangeGesture();
    };

    graphView.onBandQChanged = [this] (int bandIndex, float qValue)
    {
        if (auto* qParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandQID (bandIndex)))
        {
            captureWheelQGestureCheckpoint (bandIndex);
            qParam->beginChangeGesture();
            qParam->setValueNotifyingHost (qParam->convertTo0to1 (qValue));
            qParam->endChangeGesture();
        }
    };

    // Setup band selection changed callback
    graphView.onBandSelectionChanged = [this] (int newIndex)
    {
        juce::ignoreUnused (newIndex);
        updateSelectedBandControls();
    };

    // Setup band solo status changed callback
    graphView.onBandSoloStatusChanged = [this] (int bandIdx, bool isSolo)
    {
        audioProcessor.soloBandIndex.store (isSolo ? bandIdx : -1);
    };

    // Setup band create requested callback (double-click in EQGraphView)
    graphView.onBandCreateRequested = [this] (float frequencyHz, float gainDb, int filterTypeIndex, int slopeIndex)
    {
        int freeBandIdx = -1;
        for (int i = 0; i < AUREQ::Params::numBands; ++i)
        {
            if (auto* enabledParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandEnabledID (i)))
            {
                if (enabledParam->load() < 0.5f)
                {
                    freeBandIdx = i;
                    break;
                }
            }
        }

        if (freeBandIdx != -1)
        {
            auto* enabledParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandEnabledID (freeBandIdx));
            auto* bypassParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandBypassID (freeBandIdx));
            auto* freqParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandFrequencyID (freeBandIdx));
            auto* gainParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandGainID (freeBandIdx));
            auto* qParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandQID (freeBandIdx));
            auto* typeParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandTypeID (freeBandIdx));
            auto* slopeParam = audioProcessor.apvts.getParameter (AUREQ::Params::bandSlopeID (freeBandIdx));

            if (enabledParam && bypassParam && freqParam && gainParam && qParam && typeParam && slopeParam)
            {
                audioProcessor.captureUndoCheckpoint();

                enabledParam->beginChangeGesture();
                bypassParam->beginChangeGesture();
                freqParam->beginChangeGesture();
                gainParam->beginChangeGesture();
                qParam->beginChangeGesture();
                typeParam->beginChangeGesture();
                slopeParam->beginChangeGesture();

                freqParam->setValueNotifyingHost (freqParam->convertTo0to1 (frequencyHz));
                gainParam->setValueNotifyingHost (gainParam->convertTo0to1 (gainDb));
                qParam->setValueNotifyingHost (qParam->convertTo0to1 (1.0f)); // Pro-Q style default Q
                typeParam->setValueNotifyingHost (typeParam->convertTo0to1 ((float)filterTypeIndex));
                slopeParam->setValueNotifyingHost (slopeParam->convertTo0to1 ((float)slopeIndex));
                bypassParam->setValueNotifyingHost (0.0f); // Not bypassed
                
                // Activate the band (isActive = true)
                enabledParam->setValueNotifyingHost (1.0f);

                enabledParam->endChangeGesture();
                bypassParam->endChangeGesture();
                freqParam->endChangeGesture();
                gainParam->endChangeGesture();
                qParam->endChangeGesture();
                typeParam->endChangeGesture();
                slopeParam->endChangeGesture();
            }

            // Focus and anchor the new band handle immediately
            graphView.setSelectedBandIndex (freeBandIdx);
            updateSelectedBandControls();
        }
        else
        {
            juce::Logger::writeToLog ("AUREQ: Maximum band limit reached (12 bands). Cannot create new band.");
        }
    };

    // Setup band context menu requested callback (right-click on node)
    graphView.onBandContextMenuRequested = [this] (int bandIdx, juce::Point<int> pos)
    {
        if (bandIdx < 0 || bandIdx >= AUREQ::Params::numBands)
            return;

        int activeType = 0;
        if (auto* typeParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandTypeID (bandIdx)))
            activeType = juce::roundToInt (typeParam->load());

        int activeSlope = 0;
        if (auto* slopeParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandSlopeID (bandIdx)))
            activeSlope = juce::roundToInt (slopeParam->load());

        int activeMode = 0;
        if (auto* modeParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandChannelModeID (bandIdx)))
            activeMode = juce::roundToInt (modeParam->load());

        bool isBypassed = false;
        if (auto* bypassParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandBypassID (bandIdx)))
            isBypassed = (bypassParam->load() > 0.5f);

        juce::PopupMenu menu;

        // Filter Type Submenu
        juce::PopupMenu typeMenu;
        juce::StringArray shortNames = { "Bell", "Low Cut", "High Cut", "Low Shelf", "High Shelf", "Notch", "Band Pass" };
        for (int i = 0; i < 7; ++i)
        {
            typeMenu.addItem (1 + i, shortNames[i], true, i == activeType);
        }
        menu.addSubMenu ("Filter Type", typeMenu);

        // Slope Submenu (only for Cut and Shelf filters)
        bool supportsSlope = (activeType == 1 || activeType == 2 || activeType == 3 || activeType == 4);
        juce::PopupMenu slopeMenu;
        slopeMenu.addItem (10, "6 dB/oct", supportsSlope, activeSlope == 0);
        slopeMenu.addItem (11, "12 dB/oct", supportsSlope, activeSlope == 1);
        slopeMenu.addItem (12, "18 dB/oct", supportsSlope, activeSlope == 2);
        slopeMenu.addItem (13, "24 dB/oct", supportsSlope, activeSlope == 3);
        menu.addSubMenu ("Slope", slopeMenu, supportsSlope);

        // Stereo Placement Submenu
        juce::PopupMenu modeMenu;
        juce::StringArray chModeNames = { "Stereo", "Mid", "Side", "Left", "Right" };
        for (int i = 0; i < 5; ++i)
        {
            modeMenu.addItem (20 + i, chModeNames[i], true, i == activeMode);
        }
        menu.addSubMenu ("Stereo Placement", modeMenu);

        menu.addSeparator();
        menu.addItem (30, "Bypass Band", true, isBypassed);
        menu.addItem (31, "Delete Band");

        menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&graphView).withTargetScreenArea (juce::Rectangle<int> (pos.x, pos.y, 1, 1)),
            [this, bandIdx](int result)
            {
                if (result == 0)
                    return;

                audioProcessor.captureUndoCheckpoint();

                // 1. Filter Type selected
                if (result >= 1 && result <= 7)
                {
                    int typeIndex = result - 1;
                    if (auto* p = audioProcessor.apvts.getParameter (AUREQ::Params::bandTypeID (bandIdx)))
                    {
                        p->beginChangeGesture();
                        p->setValueNotifyingHost (p->convertTo0to1 (static_cast<float>(typeIndex)));
                        p->endChangeGesture();
                    }
                }
                // 2. Slope selected
                else if (result >= 10 && result <= 13)
                {
                    int slopeIndex = result - 10;
                    if (auto* p = audioProcessor.apvts.getParameter (AUREQ::Params::bandSlopeID (bandIdx)))
                    {
                        p->beginChangeGesture();
                        p->setValueNotifyingHost (p->convertTo0to1 (static_cast<float>(slopeIndex)));
                        p->endChangeGesture();
                    }
                }
                // 3. Stereo Placement selected
                else if (result >= 20 && result <= 24)
                {
                    int modeIndex = result - 20;
                    if (auto* p = audioProcessor.apvts.getParameter (AUREQ::Params::bandChannelModeID (bandIdx)))
                    {
                        p->beginChangeGesture();
                        p->setValueNotifyingHost (p->convertTo0to1 (static_cast<float>(modeIndex)));
                        p->endChangeGesture();
                    }
                }
                // 4. Bypass Toggle
                else if (result == 30)
                {
                    if (auto* p = audioProcessor.apvts.getParameter (AUREQ::Params::bandBypassID (bandIdx)))
                    {
                        bool currentVal = p->getValue() > 0.5f;
                        p->beginChangeGesture();
                        p->setValueNotifyingHost (currentVal ? 0.0f : 1.0f);
                        p->endChangeGesture();
                    }
                }
                // 5. Delete Band
                else if (result == 31)
                {
                    if (auto* p = audioProcessor.apvts.getParameter (AUREQ::Params::bandEnabledID (bandIdx)))
                    {
                        p->beginChangeGesture();
                        p->setValueNotifyingHost (0.0f);
                        p->endChangeGesture();
                    }
                    
                    // Select another active band if possible
                    int nextSelectedIdx = -1;
                    for (int i = 0; i < AUREQ::Params::numBands; ++i)
                    {
                        if (auto* nextEnabledParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandEnabledID (i)))
                        {
                            if (nextEnabledParam->load() > 0.5f)
                            {
                                nextSelectedIdx = i;
                                break;
                            }
                        }
                    }
                    graphView.setSelectedBandIndex (nextSelectedIdx);
                }

                updateSelectedBandControls();
            });
    };

    // Setup static attachments
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, AUREQ::Params::inputGainID(), inputGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, AUREQ::Params::outputGainID(), outputGainSlider);
    globalBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, AUREQ::Params::globalBypassID(), globalBypassButton);
    themeToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, AUREQ::Params::themeModeID(), themeToggleButton);

    // Initial theme setup
    updateTheme();

    // Initial bands setup
    timerCallback();

    // Select Band 1 by default on launch
    graphView.setSelectedBandIndex (0);
    updateSelectedBandControls();
    applyLocalizedTexts();

    // Start UI update timer at 30 Hz
    startTimerHz (30);

    // Set window size (900 x 560)
    setSize (900, 560);
    grabKeyboardFocus();
}

AureqAudioProcessorEditor::~AureqAudioProcessorEditor()
{
    stopTimer();
    presetLabel.removeMouseListener (this);
    abBtn.removeMouseListener (this);
    globalBypassButton.removeMouseListener (this);
    audioProcessor.apvts.removeParameterListener (AUREQ::Params::themeModeID(), this);

    if (saveUserPresetAlert != nullptr)
    {
        saveUserPresetAlert->setLookAndFeel (nullptr);
        saveUserPresetAlert.reset();
    }

    // Clean up LookAndFeels
    inputGainSlider.setLookAndFeel (nullptr);
    outputGainSlider.setLookAndFeel (nullptr);
    
    globalBypassButton.setLookAndFeel (nullptr);
    themeToggleButton.setLookAndFeel (nullptr);
    
    // Clean up header buttons LookAndFeel
    prevPresetBtn.setLookAndFeel (nullptr);
    nextPresetBtn.setLookAndFeel (nullptr);
    abBtn.setLookAndFeel (nullptr);
    settingsBtn.setLookAndFeel (nullptr);
    themeHeaderBtn.setLookAndFeel (nullptr);
    bypassHeaderBtn.setLookAndFeel (nullptr);
    resetBtn.setLookAndFeel (nullptr);
}

bool AureqAudioProcessorEditor::keyPressed (const juce::KeyPress& key)
{
    const auto modifiers = key.getModifiers();
    if (! modifiers.isCommandDown())
        return false;

    const auto keyCode = key.getKeyCode();
    const bool wantsRedo = (keyCode == 'z' || keyCode == 'Z') && modifiers.isShiftDown();
    const bool wantsUndo = (keyCode == 'z' || keyCode == 'Z') && ! modifiers.isShiftDown();
    const bool wantsRedoAlt = keyCode == 'y' || keyCode == 'Y';

    if (wantsRedo || wantsRedoAlt)
        return performRedoFromUI();

    if (wantsUndo)
        return performUndoFromUI();

    return false;
}

void AureqAudioProcessorEditor::mouseDown (const juce::MouseEvent& event)
{
    if (event.eventComponent == &globalBypassButton && globalBypassButton.isEnabled())
        audioProcessor.captureUndoCheckpoint();
    else if (event.eventComponent == &floatingPanel.bandBypassBtn
             && floatingPanel.bandBypassBtn.isEnabled()
             && graphView.getSelectedBandIndex() >= 0
             && graphView.getSelectedBandIndex() < AUREQ::Params::numBands)
        audioProcessor.captureUndoCheckpoint();
}

void AureqAudioProcessorEditor::mouseUp (const juce::MouseEvent& event)
{
    if (event.eventComponent == &presetLabel)
    {
        showPresetBrowserMenu();
    }
    else if (event.eventComponent == &abBtn)
    {
        if (event.mods.isPopupMenu())
        {
            showABContextMenu();
        }
    }
}

juce::String AureqAudioProcessorEditor::text (AUREQ::TextKey key) const
{
    return AUREQ::getText (key, currentLanguage);
}

AUREQ::AureqLanguage AureqAudioProcessorEditor::loadLanguagePreference() const
{
    juce::PropertiesFile properties (createLocalizationPreferenceOptions());
    return AUREQ::languageFromPreferenceValue (properties.getValue ("language", "pt-BR"));
}

void AureqAudioProcessorEditor::saveLanguagePreference (AUREQ::AureqLanguage language) const
{
    juce::PropertiesFile properties (createLocalizationPreferenceOptions());
    properties.setValue ("language", AUREQ::languageToPreferenceValue (language));
    properties.saveIfNeeded();
}

void AureqAudioProcessorEditor::setLanguage (AUREQ::AureqLanguage language)
{
    if (currentLanguage == language)
        return;

    currentLanguage = language;
    saveLanguagePreference (currentLanguage);
    applyLocalizedTexts();
}

void AureqAudioProcessorEditor::applyLocalizedTexts()
{
    settingsBtn.setButtonText (text (AUREQ::TextKey::Settings));
    themeHeaderBtn.setButtonText (text (AUREQ::TextKey::Theme));
    themeToggleButton.setButtonText (text (AUREQ::TextKey::Theme));
    bypassHeaderBtn.setButtonText (text (AUREQ::TextKey::Bypass));
    globalBypassButton.setButtonText (text (AUREQ::TextKey::Bypass));
    floatingPanel.bandBypassBtn.setButtonText (text (AUREQ::TextKey::Bypass));
    resetBtn.setButtonText (text (AUREQ::TextKey::Reset));
    floatingPanel.removeBandBtn.setButtonText (text (AUREQ::TextKey::Remove));

    graphView.setLanguage (currentLanguage);
    repaint();
}

void AureqAudioProcessorEditor::showPresetBrowserMenu()
{
    static constexpr int saveCurrentPresetMenuId = 10000;
    static constexpr int userPresetMenuIdBase = 20000;
    static constexpr int refreshUserPresetsMenuId = 29000;
    static constexpr int revealUserPresetFolderMenuId = 29001;
    static constexpr int deleteUserPresetMenuIdBase = 30000;

    audioProcessor.refreshUserPresets();

    auto* pm = audioProcessor.getPresetManager();
    if (pm == nullptr)
        return;

    const int numPresets = pm->getNumPresets();
    if (numPresets <= 0)
        return;

    juce::PopupMenu menu;
    menu.setLookAndFeel (&aureqLookAndFeel);

    const int currentIndex = pm->getCurrentPresetIndex();
    const int currentUserPresetIndex = pm->getCurrentUserPresetIndex();
    const auto menuLanguage = currentLanguage;
    int nextFactoryMenuId = 1;
    int currentFactoryMenuId = 0;
    std::map<int, int> factoryMenuIdToPresetIndex;

    auto addPresetItem = [pm, currentIndex, currentUserPresetIndex, &nextFactoryMenuId, &currentFactoryMenuId, &factoryMenuIdToPresetIndex] (juce::PopupMenu& target, int presetIndex)
    {
        const int itemId = nextFactoryMenuId++;
        factoryMenuIdToPresetIndex[itemId] = presetIndex;

        if (currentUserPresetIndex < 0 && presetIndex == currentIndex)
            currentFactoryMenuId = itemId;

        target.addItem (itemId,
                        pm->getPresetName (presetIndex),
                        true,
                        currentUserPresetIndex < 0 && presetIndex == currentIndex);
    };

    auto getGenrePresetSource = [pm] (int presetIndex)
    {
        auto tags = pm->getPresetTags (presetIndex);
        for (const auto& tag : tags)
        {
            if (tag.startsWithIgnoreCase ("source:"))
                return tag.fromFirstOccurrenceOf ("source:", false, true).trim();
        }

        return juce::String();
    };

    menu.addItem (saveCurrentPresetMenuId, text (AUREQ::TextKey::SaveCurrentPreset));
    menu.addSeparator();

    // Keep Default Flat as the first visible command before the categorized library.
    addPresetItem (menu, 0);
    menu.addSeparator();

    auto addCategoryMenu = [pm, numPresets, menuLanguage, &addPresetItem] (juce::PopupMenu& parentMenu, const juce::String& category)
    {
        juce::PopupMenu categoryMenu;

        for (int i = 1; i < numPresets; ++i)
        {
            if (pm->getPresetCategory (i) == category)
                addPresetItem (categoryMenu, i);
        }

        if (categoryMenu.getNumItems() > 0)
            parentMenu.addSubMenu (AUREQ::getCategoryText (category, menuLanguage), categoryMenu);
    };

    const juce::StringArray genreNames { "Forro", "Trap", "Hip-Hop", "Funk BR", "Rock", "Sertanejo", "Pop", "Gospel", "Electronic" };
    const juce::StringArray genreSourceOrder {
        "vocal", "rap vocal", "lead vocal", "backing vocal", "choir", "adlibs",
        "accordion", "zabumba", "808", "bass / 808", "bass", "kick", "snare", "snare / clap", "hi-hat",
        "percussion", "beat", "beat bus", "drum bus", "drums", "sample", "electric guitar", "acoustic guitar", "guitar", "viola",
        "piano / keys", "synth", "lead", "lead / synth", "pad", "mix bus", "master"
    };

    auto getSourceMenuLabel = [menuLanguage] (const juce::String& sourceName)
    {
        return AUREQ::getSourceText (sourceName, menuLanguage);
    };

    juce::PopupMenu genresMenu;

    for (const auto& genreName : genreNames)
    {
        juce::PopupMenu genreSubMenu;
        for (const auto& sourceName : genreSourceOrder)
        {
            juce::PopupMenu sourceSubMenu;

            for (int i = 1; i < numPresets; ++i)
            {
                if (pm->getPresetCategory (i) == "Genres"
                    && pm->getPresetSubcategory (i) == genreName
                    && getGenrePresetSource (i).equalsIgnoreCase (sourceName))
                {
                    addPresetItem (sourceSubMenu, i);
                }
            }

            if (sourceSubMenu.getNumItems() > 0)
                genreSubMenu.addSubMenu (getSourceMenuLabel (sourceName), sourceSubMenu);
        }

        if (genreSubMenu.getNumItems() > 0)
            genresMenu.addSubMenu (genreName, genreSubMenu);
    }

    if (genresMenu.getNumItems() > 0)
        menu.addSubMenu (text (AUREQ::TextKey::Genres), genresMenu);

    juce::PopupMenu bySourceMenu;
    addCategoryMenu (bySourceMenu, "Vocals");
    addCategoryMenu (bySourceMenu, "Drums");
    addCategoryMenu (bySourceMenu, "Bass / 808");
    addCategoryMenu (bySourceMenu, "Instruments");
    addCategoryMenu (bySourceMenu, "Mix Bus / Master");
    addCategoryMenu (bySourceMenu, "Dynamic EQ");
    addCategoryMenu (bySourceMenu, "Utility / Cleanup");

    if (bySourceMenu.getNumItems() > 0)
        menu.addSubMenu (text (AUREQ::TextKey::BySource), bySourceMenu);

    juce::PopupMenu userPresetMenu;
    const int numUserPresets = pm->getNumUserPresets();
    if (numUserPresets > 0)
    {
        for (int i = 0; i < numUserPresets; ++i)
        {
            userPresetMenu.addItem (userPresetMenuIdBase + i,
                                    pm->getUserPresetName (i),
                                    true,
                                    i == currentUserPresetIndex);
        }
    }
    else
    {
        userPresetMenu.addItem (userPresetMenuIdBase, text (AUREQ::TextKey::NoUserPresets), false, false);
    }

    userPresetMenu.addSeparator();
    userPresetMenu.addItem (refreshUserPresetsMenuId, text (AUREQ::TextKey::RefreshUserPresets));
    userPresetMenu.addItem (revealUserPresetFolderMenuId, text (AUREQ::TextKey::RevealPresetsFolder));

    juce::PopupMenu deleteUserPresetMenu;
    if (numUserPresets > 0)
    {
        for (int i = 0; i < numUserPresets; ++i)
            deleteUserPresetMenu.addItem (deleteUserPresetMenuIdBase + i, pm->getUserPresetName (i));
    }
    else
    {
        deleteUserPresetMenu.addItem (deleteUserPresetMenuIdBase, text (AUREQ::TextKey::NoUserPresets), false, false);
    }

    userPresetMenu.addSubMenu (text (AUREQ::TextKey::DeleteUserPreset), deleteUserPresetMenu, numUserPresets > 0);

    menu.addSeparator();
    menu.addSubMenu (text (AUREQ::TextKey::UserPresets), userPresetMenu);

    auto options = juce::PopupMenu::Options()
        .withTargetComponent (presetLabel)
        .withMinimumWidth (presetLabel.getWidth())
        .withStandardItemHeight (24)
        .withInitiallySelectedItem (currentFactoryMenuId)
        .withDeletionCheck (*this);

    menu.showMenuAsync (options, [safeThis = juce::Component::SafePointer<AureqAudioProcessorEditor> (this),
                                  factoryMenuIdToPresetIndex] (int selectedId)
    {
        if (safeThis == nullptr || selectedId <= 0)
            return;

        if (selectedId == saveCurrentPresetMenuId)
        {
            safeThis->showSaveUserPresetDialog();
        }
        else if (selectedId == refreshUserPresetsMenuId)
        {
            safeThis->audioProcessor.refreshUserPresets();
        }
        else if (selectedId == revealUserPresetFolderMenuId)
        {
            if (! safeThis->audioProcessor.revealUserPresetDirectory())
            {
                juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                        safeThis->text (AUREQ::TextKey::RevealPresetsFolderFailed),
                                                        safeThis->text (AUREQ::TextKey::RevealPresetsFolderFailedMessage),
                                                        safeThis->text (AUREQ::TextKey::Ok),
                                                        safeThis);
            }
        }
        else if (selectedId >= deleteUserPresetMenuIdBase)
        {
            safeThis->confirmDeleteUserPreset (selectedId - deleteUserPresetMenuIdBase);
        }
        else if (selectedId >= userPresetMenuIdBase)
        {
            safeThis->loadUserPresetAndRefresh (selectedId - userPresetMenuIdBase);
        }
        else
        {
            auto foundPreset = factoryMenuIdToPresetIndex.find (selectedId);
            if (foundPreset != factoryMenuIdToPresetIndex.end())
                safeThis->loadPresetAndRefresh (foundPreset->second);
        }
    });
}

void AureqAudioProcessorEditor::loadPresetAndRefresh (int presetIndex)
{
    auto* pm = audioProcessor.getPresetManager();
    if (pm == nullptr || presetIndex < 0 || presetIndex >= pm->getNumPresets())
        return;

    audioProcessor.loadPreset (presetIndex);
    presetLabel.setText (audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
    updateSelectedBandControls();
    repaint();
}

void AureqAudioProcessorEditor::loadUserPresetAndRefresh (int userPresetIndex)
{
    if (! audioProcessor.loadUserPreset (userPresetIndex))
        return;

    presetLabel.setText (audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
    updateSelectedBandControls();
    timerCallback();
    repaint();
}

void AureqAudioProcessorEditor::showSaveUserPresetDialog()
{
    if (saveUserPresetAlert != nullptr)
    {
        saveUserPresetAlert->toFront (true);
        return;
    }

    saveUserPresetAlert = std::make_unique<juce::AlertWindow> (text (AUREQ::TextKey::SaveCurrentPresetTitle),
                                                               text (AUREQ::TextKey::SaveCurrentPresetMessage),
                                                               juce::AlertWindow::NoIcon,
                                                               this);
    saveUserPresetAlert->setLookAndFeel (&aureqLookAndFeel);
    saveUserPresetAlert->addTextEditor ("presetName", audioProcessor.getCurrentPresetName(), text (AUREQ::TextKey::Name));
    saveUserPresetAlert->addButton (text (AUREQ::TextKey::Save), 1, juce::KeyPress (juce::KeyPress::returnKey));
    saveUserPresetAlert->addButton (text (AUREQ::TextKey::Cancel), 0, juce::KeyPress (juce::KeyPress::escapeKey));

    saveUserPresetAlert->enterModalState (true,
        juce::ModalCallbackFunction::create ([safeThis = juce::Component::SafePointer<AureqAudioProcessorEditor> (this)] (int result)
        {
            if (safeThis == nullptr || safeThis->saveUserPresetAlert == nullptr)
                return;

            auto presetName = safeThis->saveUserPresetAlert->getTextEditorContents ("presetName").trim();
            safeThis->saveUserPresetAlert->setLookAndFeel (nullptr);
            safeThis->saveUserPresetAlert.reset();

            if (result != 1)
                return;

            if (presetName.isEmpty() || ! safeThis->audioProcessor.saveCurrentUserPreset (presetName))
            {
                juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                        safeThis->text (AUREQ::TextKey::SavePresetFailed),
                                                        safeThis->text (AUREQ::TextKey::SavePresetFailedMessage),
                                                        safeThis->text (AUREQ::TextKey::Ok),
                                                        safeThis);
                return;
            }

            safeThis->audioProcessor.refreshUserPresets();
            safeThis->presetLabel.setText (safeThis->audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
        }),
        false);
}

void AureqAudioProcessorEditor::confirmDeleteUserPreset (int userPresetIndex)
{
    auto* pm = audioProcessor.getPresetManager();
    if (pm == nullptr || userPresetIndex < 0 || userPresetIndex >= pm->getNumUserPresets())
        return;

    const auto presetName = pm->getUserPresetName (userPresetIndex);

    juce::AlertWindow::showOkCancelBox (juce::AlertWindow::WarningIcon,
                                        text (AUREQ::TextKey::DeleteUserPresetTitle),
                                        text (AUREQ::TextKey::DeleteUserPresetMessagePrefix) + presetName + text (AUREQ::TextKey::DeleteUserPresetMessageSuffix),
                                        text (AUREQ::TextKey::Delete),
                                        text (AUREQ::TextKey::Cancel),
                                        this,
                                        juce::ModalCallbackFunction::create (
                                            [safeThis = juce::Component::SafePointer<AureqAudioProcessorEditor> (this),
                                             userPresetIndex] (int result)
                                            {
                                                if (safeThis == nullptr || result != 1)
                                                    return;

                                                if (! safeThis->audioProcessor.deleteUserPreset (userPresetIndex))
                                                {
                                                    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                                                            safeThis->text (AUREQ::TextKey::DeletePresetFailed),
                                                                                            safeThis->text (AUREQ::TextKey::DeletePresetFailedMessage),
                                                                                            safeThis->text (AUREQ::TextKey::Ok),
                                                                                            safeThis);
                                                    return;
                                                }

                                                safeThis->audioProcessor.refreshUserPresets();
                                            }));
}

void AureqAudioProcessorEditor::captureSelectedBandGestureCheckpoint()
{
    const int bandIndex = graphView.getSelectedBandIndex();
    if (bandIndex >= 0 && bandIndex < AUREQ::Params::numBands)
        audioProcessor.captureUndoCheckpoint();
}

void AureqAudioProcessorEditor::captureWheelQGestureCheckpoint (int bandIndex)
{
    if (bandIndex < 0 || bandIndex >= AUREQ::Params::numBands)
        return;

    const juce::uint32 now = juce::Time::getMillisecondCounter();
    constexpr juce::uint32 wheelGestureWindowMs = 500;

    if (lastWheelQCheckpointBand != bandIndex
        || lastWheelQCheckpointTimeMs == 0
        || now - lastWheelQCheckpointTimeMs > wheelGestureWindowMs)
    {
        audioProcessor.captureUndoCheckpoint();
        lastWheelQCheckpointBand = bandIndex;
    }

    lastWheelQCheckpointTimeMs = now;
}

bool AureqAudioProcessorEditor::performUndoFromUI()
{
    if (! audioProcessor.undoAPVTSState())
        return false;

    abBtn.setButtonText (audioProcessor.getActiveABSlotName());
    presetLabel.setText (audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
    updateSelectedBandControls();
    timerCallback();
    repaint();
    return true;
}

bool AureqAudioProcessorEditor::performRedoFromUI()
{
    if (! audioProcessor.redoAPVTSState())
        return false;

    abBtn.setButtonText (audioProcessor.getActiveABSlotName());
    presetLabel.setText (audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
    updateSelectedBandControls();
    timerCallback();
    repaint();
    return true;
}

void AureqAudioProcessorEditor::showABContextMenu()
{
    juce::PopupMenu menu;
    menu.setLookAndFeel (&aureqLookAndFeel);

    menu.addItem (1, text (AUREQ::TextKey::CopyAToB));
    menu.addItem (2, text (AUREQ::TextKey::CopyBToA));
    menu.addItem (3, text (AUREQ::TextKey::CopyCurrentToOther));
    menu.addItem (4, text (AUREQ::TextKey::ResetBToCurrent));

    auto options = juce::PopupMenu::Options()
        .withTargetComponent (abBtn)
        .withMinimumWidth (120)
        .withStandardItemHeight (24)
        .withDeletionCheck (*this);

    menu.showMenuAsync (options, [safeThis = juce::Component::SafePointer<AureqAudioProcessorEditor> (this)] (int selectedId)
    {
        if (safeThis == nullptr || selectedId <= 0)
            return;

        if (selectedId == 1)
            safeThis->audioProcessor.copyABSlotAToB();
        else if (selectedId == 2)
            safeThis->audioProcessor.copyABSlotBToA();
        else if (selectedId == 3)
            safeThis->audioProcessor.copyCurrentABSlotToOther();
        else if (selectedId == 4)
            safeThis->audioProcessor.resetABSlotBFromCurrent();

        // Refresh UI state
        safeThis->abBtn.setButtonText (safeThis->audioProcessor.getActiveABSlotName());
        safeThis->presetLabel.setText (safeThis->audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
        safeThis->updateSelectedBandControls();
        safeThis->timerCallback();
        safeThis->repaint();
    });
}

void AureqAudioProcessorEditor::showSettingsMenu()
{
    static constexpr int undoMenuId = 1;
    static constexpr int redoMenuId = 2;
    static constexpr int portugueseLanguageMenuId = 10;
    static constexpr int englishLanguageMenuId = 11;

    juce::PopupMenu menu;
    menu.setLookAndFeel (&aureqLookAndFeel);

    menu.addItem (undoMenuId, text (AUREQ::TextKey::Undo), audioProcessor.canUndo());
    menu.addItem (redoMenuId, text (AUREQ::TextKey::Redo), audioProcessor.canRedo());
    menu.addSeparator();

    juce::PopupMenu languageMenu;
    languageMenu.addItem (portugueseLanguageMenuId,
                          text (AUREQ::TextKey::PortugueseBR),
                          true,
                          currentLanguage == AUREQ::AureqLanguage::PortugueseBR);
    languageMenu.addItem (englishLanguageMenuId,
                          text (AUREQ::TextKey::English),
                          true,
                          currentLanguage == AUREQ::AureqLanguage::English);
    menu.addSubMenu (text (AUREQ::TextKey::Language), languageMenu);

    auto options = juce::PopupMenu::Options()
        .withTargetComponent (settingsBtn)
        .withMinimumWidth (120)
        .withStandardItemHeight (24)
        .withDeletionCheck (*this);

    menu.showMenuAsync (options, [safeThis = juce::Component::SafePointer<AureqAudioProcessorEditor> (this)] (int selectedId)
    {
        if (safeThis == nullptr || selectedId <= 0)
            return;

        if (selectedId == undoMenuId)
            safeThis->performUndoFromUI();
        else if (selectedId == redoMenuId)
            safeThis->performRedoFromUI();
        else if (selectedId == portugueseLanguageMenuId)
            safeThis->setLanguage (AUREQ::AureqLanguage::PortugueseBR);
        else if (selectedId == englishLanguageMenuId)
            safeThis->setLanguage (AUREQ::AureqLanguage::English);
    });
}

void AureqAudioProcessorEditor::drawGlassPanel (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour baseColor, float cornerRadius)
{
    g.saveState();
    
    // 1. Frosted glass background vertical gradient (subtle and high-end)
    juce::ColourGradient grad (baseColor.brighter (0.02f), area.getX(), area.getY(),
                               baseColor.darker (0.04f), area.getX(), area.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (area, cornerRadius);
    
    // 2. Highlight bezel gradient for liquid glass edge
    juce::Colour highlightColor = (currentMode == AUREQ::ThemeMode::Dark) ? juce::Colours::white.withAlpha (0.16f) : juce::Colours::white.withAlpha (0.60f);
    juce::Colour shadowColor = (currentMode == AUREQ::ThemeMode::Dark) ? juce::Colours::black.withAlpha (0.35f) : juce::Colours::black.withAlpha (0.10f);
    
    juce::ColourGradient borderGrad (highlightColor, area.getX(), area.getY(),
                                     shadowColor, area.getRight(), area.getBottom(), false);
    g.setGradientFill (borderGrad);
    g.drawRoundedRectangle (area.reduced (0.5f), cornerRadius, 1.0f);
    
    // 3. Specular highlight line on the top edge (liquid glass catches overhead light)
    g.setColour (highlightColor.withAlpha (currentMode == AUREQ::ThemeMode::Dark ? 0.22f : 0.70f));
    juce::Path topHighlight;
    float r = cornerRadius;
    topHighlight.startNewSubPath (area.getX(), area.getY() + r);
    topHighlight.addCentredArc (area.getX() + r, area.getY() + r, r, r, 0.0f, -juce::MathConstants<float>::halfPi, 0.0f, true);
    topHighlight.lineTo (area.getRight() - r, area.getY());
    topHighlight.addCentredArc (area.getRight() - r, area.getY() + r, r, r, 0.0f, 0.0f, juce::MathConstants<float>::halfPi, true);
    g.strokePath (topHighlight, juce::PathStrokeType (1.0f));
    
    // 4. Double-bezel inner highlight to enhance refraction depth
    g.setColour (juce::Colours::white.withAlpha (currentMode == AUREQ::ThemeMode::Dark ? 0.04f : 0.25f));
    g.drawRoundedRectangle (area.reduced (1.5f), cornerRadius - 1.0f, 1.0f);
    
    g.restoreState();
}

void AureqAudioProcessorEditor::drawSoftShadow (juce::Graphics& g, juce::Rectangle<int> area, int shadowSize, float maxAlpha, juce::Colour shadowColour, float cornerRadius)
{
    g.saveState();
    // Offset the shadow downwards to simulate a top-down light source (depth/elevation)
    auto shadowArea = area.toFloat().translated (0.0f, 3.0f);
    
    for (int i = 1; i <= shadowSize; ++i)
    {
        float progress = (float)i / (float)shadowSize;
        // Quadratic decay for beautiful soft diffusion
        float alpha = maxAlpha * std::pow (1.0f - progress, 2.5f);
        g.setColour (shadowColour.withAlpha (alpha));
        
        // Expand the shadow rectangle as it diffuses outward
        float expansion = (float)i * 1.2f;
        auto shadowRect = shadowArea.expanded (expansion);
        float radius = cornerRadius + expansion;
        
        // Use thicker lines as progress increases to simulate blur diffusion
        float thickness = 1.0f + progress * 1.5f;
        g.drawRoundedRectangle (shadowRect, radius, thickness);
    }
    g.restoreState();
}

void AureqAudioProcessorEditor::drawPill (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour fillColors, juce::Colour borderColors, const juce::String& text, juce::Colour textColor, bool isBold)
{
    g.saveState();
    
    // Draw fill with small gradient
    juce::ColourGradient fillGrad (fillColors.brighter (0.02f), area.getX(), area.getY(),
                                   fillColors.darker (0.02f), area.getX(), area.getBottom(), false);
    g.setGradientFill (fillGrad);
    float r = area.getHeight() * 0.5f;
    g.fillRoundedRectangle (area, r);
    
    // Draw border
    g.setColour (borderColors);
    g.drawRoundedRectangle (area.reduced (0.5f), r, 1.0f);
    
    // Draw status LED (glowing dot) on the left
    float ledSize = 5.0f;
    float ledX = area.getX() + 10.0f;
    float ledY = area.getY() + (area.getHeight() - ledSize) * 0.5f;
    
    // LED Glow aura
    g.setColour (textColor.withAlpha (0.35f));
    g.fillEllipse (ledX - 1.5f, ledY - 1.5f, ledSize + 3.0f, ledSize + 3.0f);
    
    // LED Center
    g.setColour (textColor);
    g.fillEllipse (ledX, ledY, ledSize, ledSize);
    
    // Draw text with Inter font shifted to the right to accommodate the LED
    g.setColour (textColor);
    g.setFont (juce::Font (juce::FontOptions ("Inter", area.getHeight() * 0.42f, isBold ? juce::Font::bold : juce::Font::plain)));
    
    juce::Rectangle<int> textRect (juce::roundToInt (area.getX() + 16.0f), juce::roundToInt (area.getY()), 
                                   juce::roundToInt (area.getWidth() - 22.0f), juce::roundToInt (area.getHeight()));
    g.drawText (text, textRect, juce::Justification::centred, true);
    
    g.restoreState();
}

void AureqAudioProcessorEditor::paint (juce::Graphics& g)
{
    // 1. Draw main window background space gradient using 3 points
    juce::ColourGradient bgGrad (currentColors.backgroundTop, 0.0f, 0.0f,
                                 currentColors.backgroundBase, 0.0f, (float)getHeight(), false);
    bgGrad.addColour (0.5, currentColors.backgroundMiddle);
    g.setGradientFill (bgGrad);
    g.fillAll();
    
    // 2. Draw space aurora/nebula glows in the background to simulate depth
    if (currentMode == AUREQ::ThemeMode::Dark)
    {
        // Cosmic Nebula Left (Cyan glow)
        juce::ColourGradient glowGradLeft (currentColors.accent.withAlpha (0.05f), (float)getWidth() * 0.25f, 50.0f,
                                           currentColors.accent.withAlpha (0.0f), (float)getWidth() * 0.25f, 300.0f, true);
        g.setGradientFill (glowGradLeft);
        g.fillAll();

        // Cosmic Nebula Right (Mint/Teal glow)
        juce::ColourGradient glowGradRight (currentColors.accentSoft.withAlpha (0.04f), (float)getWidth() * 0.80f, 100.0f,
                                            currentColors.accentSoft.withAlpha (0.0f), (float)getWidth() * 0.80f, 350.0f, true);
        g.setGradientFill (glowGradRight);
        g.fillAll();

        // Atmospheric nebula centered directly behind the graph card
        juce::ColourGradient behindGraphGlow (currentColors.accent.withAlpha (0.09f), (float)getWidth() * 0.5f, 231.0f,
                                              currentColors.accent.withAlpha (0.0f), (float)getWidth() * 0.5f + 300.0f, 231.0f, true);
        behindGraphGlow.addColour (0.4, currentColors.accentSoft.withAlpha (0.05f));
        g.setGradientFill (behindGraphGlow);
        g.fillRect (24, 76, getWidth() - 48, 310);
    }
    else
    {
        // Faint soft cyan/teal atmospheric reflection for light mode
        juce::ColourGradient glowGradLeft (currentColors.accent.withAlpha (0.03f), (float)getWidth() * 0.35f, 50.0f,
                                           currentColors.accent.withAlpha (0.0f), (float)getWidth() * 0.35f, 350.0f, true);
        g.setGradientFill (glowGradLeft);
        g.fillAll();

        // Light mode glow behind graph card
        juce::ColourGradient behindGraphGlow (currentColors.accent.withAlpha (0.04f), (float)getWidth() * 0.5f, 231.0f,
                                              currentColors.accent.withAlpha (0.0f), (float)getWidth() * 0.5f + 250.0f, 231.0f, true);
        g.setGradientFill (behindGraphGlow);
        g.fillRect (24, 76, getWidth() - 48, 310);
    }

    // 3. Draw Floating Top Bar (Header) - margin 24px, height 44px
    juce::Rectangle<int> topBarArea (24, 16, getWidth() - 48, 44);
    drawSoftShadow (g, topBarArea, 8, 0.15f, currentColors.shadow, 12.0f);
    drawGlassPanel (g, topBarArea.toFloat(), currentColors.topBar, 12.0f);
    
    // Draw Logo/Title "A U R E Q"
    g.setColour (currentColors.textPrimary);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 18.0f, juce::Font::bold)));
    g.drawText ("A U R E Q", topBarArea.getX() + 18, topBarArea.getY(), 110, topBarArea.getHeight(), juce::Justification::centredLeft, true);
    
    // Elegant separator line
    float separatorX = (float)topBarArea.getX() + 115.0f;
    float separatorY = (float)topBarArea.getY() + ((float)topBarArea.getHeight() - 14.0f) * 0.5f;
    g.setColour (currentColors.border.withAlpha (currentMode == AUREQ::ThemeMode::Dark ? 0.12f : 0.40f));
    g.drawVerticalLine (juce::roundToInt (separatorX), separatorY, separatorY + 14.0f);
    
    // Subtitle "ASTRA Audio Suite"
    g.setColour (currentColors.textSecondary.withAlpha (0.65f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 9.0f, juce::Font::plain)));
    g.drawText ("ASTRA Audio Suite", topBarArea.getX() + 130, topBarArea.getY(), 120, topBarArea.getHeight(), juce::Justification::centredLeft, true);
    
    // 4. Draw central graph card shadow (wider, softer, with 16px corner radius)
    juce::Rectangle<int> graphArea (24, 76, getWidth() - 48, 310);
    drawSoftShadow (g, graphArea, 14, 0.24f, currentColors.shadow, 16.0f);
    
    // 5. Draw Bottom Panel (116px height glass card)
    juce::Rectangle<int> bottomBarArea (24, 402, getWidth() - 48, 116);
    drawSoftShadow (g, bottomBarArea, 10, 0.20f, currentColors.shadow, 14.0f);
    drawGlassPanel (g, bottomBarArea.toFloat(), currentColors.topBar, 14.0f);
    // Draw labels above bottom knobs
    g.setColour (currentColors.textSecondary.withAlpha (0.75f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 8.0f, juce::Font::bold)));
    
    // Global Knob Titles
    g.drawText (text (AUREQ::TextKey::InputGain), 48, 414, 64, 12, juce::Justification::centred, true);
    g.drawText (text (AUREQ::TextKey::OutputGain), 788, 414, 64, 12, juce::Justification::centred, true);

    // 6. Draw Footer
    juce::Rectangle<int> footerArea (24, getHeight() - 28, getWidth() - 48, 20);
    g.setColour (currentColors.textSecondary.withAlpha (0.50f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 8.5f, juce::Font::plain)));
    g.drawText (text (AUREQ::TextKey::FooterProduct), footerArea.getX() + 10, footerArea.getY(), 180, footerArea.getHeight(), juce::Justification::centredLeft, true);
    g.drawText (juce::String (juce::CharPointer_UTF8 ("ASTRA Audio Suite · v0.1.0")), footerArea.getRight() - 210, footerArea.getY(), 200, footerArea.getHeight(), juce::Justification::centredRight, true);
}

void AureqAudioProcessorEditor::resized()
{
    // A. Top Header Area (Preset Browser and Header Buttons)
    const int topBarX = 24;
    const int topBarW = getWidth() - 48;
    const int topY = 25;
    const int controlH = 26;
    const int gap = 8;

    const int navBtnW = 26;
    const int abW = 44;
    const int resetW = 54;
    const int settingsW = 72;
    const int themeW = 62;
    const int bypassW = 70;

    // Right-aligned header buttons. Lay them out from right to left so
    // spacing remains explicit and stable across hosts/languages.
    int rightX = topBarX + topBarW - 18;

    rightX -= bypassW;
    bypassHeaderBtn.setBounds (rightX, topY, bypassW, controlH);
    rightX -= gap;

    rightX -= themeW;
    themeHeaderBtn.setBounds (rightX, topY, themeW, controlH);
    rightX -= gap;

    rightX -= settingsW;
    settingsBtn.setBounds (rightX, topY, settingsW, controlH);
    rightX -= gap;

    rightX -= resetW;
    resetBtn.setBounds (rightX, topY, resetW, controlH);
    rightX -= gap;

    rightX -= abW;
    abBtn.setBounds (rightX, topY, abW, controlH);

    const int rightGroupLeft = rightX;

    // Preset browser gets the safe remaining space between the left branding
    // area and the right button cluster. If space is tight, the label shrinks
    // first; the buttons keep their hit targets and never collide.
    const int leftBrandSafeRight = topBarX + 255;
    const int presetAreaX = leftBrandSafeRight + gap;
    const int presetAreaRight = rightGroupLeft - gap;
    const int presetAreaW = std::max (0, presetAreaRight - presetAreaX);

    const int preferredPresetLabelW = 190;
    const int minPresetLabelW = 96;
    const int maxPresetLabelW = std::max (0, presetAreaW - (2 * navBtnW) - (2 * gap));
    const int presetLabelW = (maxPresetLabelW <= 0)
                                ? 0
                                : juce::jlimit (std::min (minPresetLabelW, maxPresetLabelW),
                                                maxPresetLabelW,
                                                preferredPresetLabelW);
    const int presetBrowserW = (2 * navBtnW) + (2 * gap) + presetLabelW;
    const int presetBrowserX = presetAreaX + std::max (0, (presetAreaW - presetBrowserW) / 2);

    prevPresetBtn.setBounds (presetBrowserX, topY, navBtnW, controlH);
    presetLabel.setBounds (presetBrowserX + navBtnW + gap, topY, presetLabelW, controlH);
    nextPresetBtn.setBounds (presetBrowserX + navBtnW + gap + presetLabelW + gap, topY, navBtnW, controlH);
    
    // Preset Label styling matching preset browser
    presetLabel.setColour (juce::Label::backgroundColourId, currentColors.panelElevated.withAlpha (0.25f));
    presetLabel.setColour (juce::Label::outlineColourId, currentColors.border.withAlpha (0.20f));
    presetLabel.setColour (juce::Label::textColourId, currentColors.textPrimary);

        // B. Graph Section (EQGraphView)
    graphView.setBounds (24, 76, getWidth() - 48, 310);
    
    // C. Bottom Control Panel controls
    int bottomY = 402;
    
    // Sliders (Aligned at Y = bottomY + 30, height = 72)
    inputGainSlider.setBounds (48, bottomY + 30, 64, 72);
    outputGainSlider.setBounds (816, bottomY + 30, 52, 72);
    
    globalBypassButton.setBounds (750, bottomY + 54, 60, 24);
    
    // Toggles/Buttons (Vertically aligned at Y = bottomY + 54, height = 24)
    themeToggleButton.setBounds (126, bottomY + 54, 80, 24);
}

void AureqAudioProcessorEditor::parameterChanged (const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused (newValue);

    if (parameterID == AUREQ::Params::themeModeID())
    {
        // Defer theme update to the message thread via AsyncUpdater
        triggerAsyncUpdate();
    }
}

void AureqAudioProcessorEditor::handleAsyncUpdate()
{
    updateTheme();
}

void AureqAudioProcessorEditor::updateTheme()
{
    auto* themeParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::themeModeID());
    
    // Default to Dark Mode (1.0f) if parameter is not found
    float themeValue = (themeParam != nullptr) ? themeParam->load() : 1.0f;
    
    currentMode = (themeValue < 0.5f) ? AUREQ::ThemeMode::Light : AUREQ::ThemeMode::Dark;
    currentColors = AUREQ::Theme::getColors (currentMode);
    
    // Propagate colors to the graph view and redraw the editor
    graphView.setThemeColors (currentColors);

    aureqLookAndFeel.setColour (juce::PopupMenu::backgroundColourId, currentColors.panelElevated.withAlpha (0.96f));
    aureqLookAndFeel.setColour (juce::PopupMenu::textColourId, currentColors.textPrimary);
    aureqLookAndFeel.setColour (juce::PopupMenu::highlightedBackgroundColourId, currentColors.accent.withAlpha (0.24f));
    aureqLookAndFeel.setColour (juce::PopupMenu::highlightedTextColourId, currentColors.textPrimary);
    
    // Propagate theme colors to controls
    inputGainSlider.setColour (juce::Label::textColourId, currentColors.textPrimary);
    outputGainSlider.setColour (juce::Label::textColourId, currentColors.textPrimary);
    
    inputGainSlider.setColour (juce::Slider::rotarySliderFillColourId, currentColors.accent);
    outputGainSlider.setColour (juce::Slider::rotarySliderFillColourId, currentColors.accent);
    
    inputGainSlider.setColour (juce::Slider::rotarySliderOutlineColourId, currentColors.gridLine);
    outputGainSlider.setColour (juce::Slider::rotarySliderOutlineColourId, currentColors.gridLine);
    
    inputGainSlider.setColour (juce::Slider::thumbColourId, currentColors.panelElevated.withAlpha(1.0f));
    outputGainSlider.setColour (juce::Slider::thumbColourId, currentColors.panelElevated.withAlpha(1.0f));
    
    floatingPanel.updateThemeColors (currentColors);
    repaint();
}

void AureqAudioProcessorEditor::timerCallback()
{
    std::array<AUREQ::VisualEQBand, AUREQ::Params::numBands> visualBands;
    
    for (int i = 0; i < AUREQ::Params::numBands; ++i)
    {
        auto* enabledParam   = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandEnabledID (i));
        auto* bypassedParam  = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandBypassID (i));
        auto* typeParam      = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandTypeID (i));
        auto* freqParam      = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandFrequencyID (i));
        auto* gainParam      = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandGainID (i));
        auto* qParam         = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandQID (i));
        
        visualBands[i].enabled     = (enabledParam != nullptr) ? (enabledParam->load() > 0.5f) : false;
        visualBands[i].bypassed    = (bypassedParam != nullptr) ? (bypassedParam->load() > 0.5f) : false;
        visualBands[i].type        = (typeParam != nullptr) ? juce::roundToInt (typeParam->load()) : 0;
        visualBands[i].frequencyHz = (freqParam != nullptr) ? freqParam->load() : 1000.0f;
        visualBands[i].gainDb      = (gainParam != nullptr) ? gainParam->load() : 0.0f;
        visualBands[i].q           = (qParam != nullptr) ? qParam->load() : 1.0f;
        visualBands[i].slope       = [&]() -> int {
            auto* pSlope = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandSlopeID (i));
            if (pSlope == nullptr) return 12;
            int idx = juce::roundToInt (pSlope->load());
            if (idx == 0) return 6;
            if (idx == 1) return 12;
            if (idx == 2) return 18;
            if (idx == 3) return 24;
            return 12;
        }();
        
        auto* dynEnabledParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandDynamicEnabledID (i));
        auto* dynRangeParam   = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandDynamicRangeID (i));
        auto* dynThresholdParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandDynamicThresholdID (i));
        visualBands[i].dynamicEnabled = (dynEnabledParam != nullptr) ? (dynEnabledParam->load() > 0.5f) : false;
        visualBands[i].dynamicRange   = (dynRangeParam != nullptr) ? dynRangeParam->load() : 0.0f;
        visualBands[i].dynamicThreshold = (dynThresholdParam != nullptr) ? dynThresholdParam->load() : -24.0f;
        visualBands[i].dynamicGainDb  = audioProcessor.getBandDynamicGainDb (i);
        
        visualBands[i].channelMode = [&]() -> int {
            auto* pChannelMode = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandChannelModeID (i));
            if (pChannelMode == nullptr) return 0;
            return juce::roundToInt (pChannelMode->load());
        }();
    }
    
    graphView.setBands (visualBands);
 
    // Sync if the selected band is disabled externally (e.g., host automation)
    int bandIndex = graphView.getSelectedBandIndex();
    if (bandIndex >= 0 && bandIndex < AUREQ::Params::numBands)
    {
        if (auto* enabledParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandEnabledID (bandIndex)))
        {
            if (enabledParam->load() < 0.5f)
            {
                int nextSelectedIdx = -1;
                for (int i = 0; i < AUREQ::Params::numBands; ++i)
                {
                    if (auto* nextEnabledParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandEnabledID (i)))
                    {
                        if (nextEnabledParam->load() > 0.5f)
                        {
                            nextSelectedIdx = i;
                            break;
                        }
                    }
                }
                graphView.setSelectedBandIndex (nextSelectedIdx);
                updateSelectedBandControls();
                bandIndex = nextSelectedIdx;
            }
        }
    }
 
    if (floatingPanel.isVisible())
    {
        floatingPanel.updateRealtimeMeter (currentColors);
    }
 
    // --- Spectrum Analyzer Update ---
    float tempBuf[512];
    int available;
    while ((available = audioProcessor.analyzerFifo.pop (tempBuf, 512)) > 0)
        analyzerProcessor.pushSamples (tempBuf, available);
 
    graphView.setSpectrumData (analyzerProcessor.getBins(), analyzerProcessor.getNumBins());
    
    // Update preset label text periodically
    presetLabel.setText (audioProcessor.getCurrentPresetName(), juce::dontSendNotification);
}

void AureqAudioProcessorEditor::updateSelectedBandControls()
{
    int bandIndex = graphView.getSelectedBandIndex();
    
    if (bandIndex >= 0 && bandIndex < AUREQ::Params::numBands)
    {
        floatingPanel.updateSelection (bandIndex);
        floatingPanel.setVisible (true);
        
        // Calculate coordinates and set bounds with collision checking
        float freq = 1000.0f;
        float gain = 0.0f;
        if (auto* freqParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandFrequencyID (bandIndex)))
            freq = freqParam->load();
        if (auto* gainParam = audioProcessor.apvts.getRawParameterValue (AUREQ::Params::bandGainID (bandIndex)))
            gain = gainParam->load();

        float graphX = graphView.frequencyToX (freq);
        float graphY = graphView.gainToY (gain);

        // Convert point to editor coordinate system
        juce::Point<int> editorPos = getLocalPoint (&graphView, juce::Point<float>(graphX, graphY)).roundToInt();
        
        int panelW = 240; // width of floating panel
        int panelH = 140; // height of floating panel
        
        int nodeX = editorPos.getX();
        int nodeY = editorPos.getY();

        int targetX = nodeX - panelW / 2;
        int targetY = nodeY + 30; // 30px offset below node

        // Check vertical collision - if it goes off bottom, show above node instead
        if (targetY + panelH > 480) 
        {
            targetY = nodeY - panelH - 30; // show 30px above node
        }

        // Horizontal and vertical boundaries check
        targetX = juce::jlimit (10, getWidth() - panelW - 10, targetX);
        targetY = juce::jlimit (10, getHeight() - panelH - 10, targetY);

        floatingPanel.setBounds (targetX, targetY, panelW, panelH);
        floatingPanel.toFront (false);
    }
    else
    {
        floatingPanel.updateSelection (-1);
        floatingPanel.setVisible (false);
    }
    repaint();
}
