#include "PluginEditor.h"
#include "ParameterIDs.h"
#include <cmath>

// ==============================================================================
// Layout constants
// ==============================================================================

namespace
{
    constexpr int kEditorW       = 800;
    constexpr int kEditorH       = 420;

    constexpr int kTopBarH       = 56;
    constexpr int kAuroraH       = 160;
    constexpr int kKnobPanelY    = kTopBarH + kAuroraH;  // 216
    constexpr int kKnobPanelH    = 172;
    constexpr int kFooterY       = kKnobPanelY + kKnobPanelH; // 388
    constexpr int kFooterH       = kEditorH - kFooterY;        // 32

    // Glass panel within knob panel area
    constexpr float kPanelMarginX = 16.0f;
    constexpr float kPanelY       = static_cast<float> (kKnobPanelY) + 4.0f;
    constexpr float kPanelW       = static_cast<float> (kEditorW) - 2.0f * kPanelMarginX;
    constexpr float kPanelH       = static_cast<float> (kKnobPanelH) - 8.0f;
    constexpr float kPanelRadius  = 12.0f;

    // Knob layout within the panel
    constexpr int kKnobSize  = 80;
    constexpr int kKnobGap   = 90;
    // Total row: 3*80 + 2*90 = 420 px; centred in 800 px → start at 190
    constexpr int kKnobStartX = (kEditorW - (3 * kKnobSize + 2 * kKnobGap)) / 2; // 190

    constexpr int kSliderY = kKnobPanelY + 26; // 242
    constexpr int kLabelY  = kSliderY + kKnobSize + 6; // 328
    constexpr int kLabelH  = 18;

    inline int knobX (int index) noexcept
    {
        return kKnobStartX + index * (kKnobSize + kKnobGap);
    }
}

// ==============================================================================
// LuminarLookAndFeel
// ==============================================================================

LuminarLookAndFeel::LuminarLookAndFeel()
{
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (30,  46,  72));
    setColour (juce::Slider::rotarySliderFillColourId,    juce::Colour (246, 201,  14)); // Amber
    setColour (juce::Slider::thumbColourId,               juce::Colour ( 14,  22,  36));
    setColour (juce::Label::textColourId,                 juce::Colour (248, 250, 252));
}

void LuminarLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                           int x, int y, int width, int height,
                                           float sliderPosProportion,
                                           float rotaryStartAngle,
                                           float rotaryEndAngle,
                                           juce::Slider& slider)
{
    const juce::Colour accentAmber (246, 201, 14);
    const juce::Colour capBase     ( 14,  22, 36);
    const juce::Colour trackBase   ( 30,  46, 72);

    // Reserve bottom strip for the value text label
    constexpr float kTextH = 13.0f;

    auto bounds    = juce::Rectangle<int> (x, y, width, height).toFloat();
    auto knobArea  = bounds.reduced (2.0f);
    knobArea.removeFromBottom (kTextH);

    const float size    = std::min (knobArea.getWidth(), knobArea.getHeight());
    const float cx      = knobArea.getCentreX();
    const float cy      = knobArea.getCentreY();
    const float radius  = size * 0.5f - 2.5f;
    const float capR    = radius - 5.5f;

    g.saveState();

    // 1 — Drop shadow under knob body
    g.setColour (juce::Colours::black.withAlpha (0.28f));
    g.fillEllipse (cx - radius - 1.0f, cy - radius + 2.0f,
                   (radius + 1.0f) * 2.0f, (radius + 1.0f) * 2.0f);

    // 2 — Outer glass bezel
    g.setColour (juce::Colours::white.withAlpha (0.05f));
    g.drawEllipse (cx - radius - 1.0f, cy - radius - 1.0f,
                   (radius + 1.0f) * 2.0f, (radius + 1.0f) * 2.0f, 0.75f);

    // 3 — Inactive background arc
    {
        juce::Path bgArc;
        bgArc.addCentredArc (cx, cy, radius - 3.5f, radius - 3.5f,
                             0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (trackBase.withAlpha (0.22f));
        g.strokePath (bgArc, juce::PathStrokeType (2.0f,
                      juce::PathStrokeType::curved,
                      juce::PathStrokeType::rounded));
    }

    // 4 — Active amber arc
    if (sliderPosProportion > 0.001f)
    {
        const float activeAngle = rotaryStartAngle
                                  + sliderPosProportion * (rotaryEndAngle - rotaryStartAngle);
        juce::Path activeArc;
        activeArc.addCentredArc (cx, cy, radius - 3.5f, radius - 3.5f,
                                 0.0f, rotaryStartAngle, activeAngle, true);

        // Outer glow
        g.setColour (accentAmber.withAlpha (0.18f));
        g.strokePath (activeArc, juce::PathStrokeType (4.5f,
                      juce::PathStrokeType::curved,
                      juce::PathStrokeType::rounded));

        // Solid core
        g.setColour (accentAmber);
        g.strokePath (activeArc, juce::PathStrokeType (2.0f,
                      juce::PathStrokeType::curved,
                      juce::PathStrokeType::rounded));
    }

    // 5 — Cap drop shadow
    g.setColour (juce::Colours::black.withAlpha (0.50f));
    g.fillEllipse (cx - capR, cy - capR + 2.0f, capR * 2.0f, capR * 2.0f);

    // 6 — Cap body with vertical gradient
    {
        juce::ColourGradient capGrad (capBase.brighter (0.06f), cx, cy - capR,
                                      capBase.darker   (0.18f), cx, cy + capR,
                                      false);
        g.setGradientFill (capGrad);
        g.fillEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f);
    }

    // 7 — Cap glass border
    g.setColour (juce::Colours::white.withAlpha (0.07f));
    g.drawEllipse (cx - capR + 0.5f, cy - capR + 0.5f,
                   capR * 2.0f - 1.0f, capR * 2.0f - 1.0f, 0.75f);

    // 8 — Specular highlight on cap (top-left quadrant)
    g.setColour (juce::Colours::white.withAlpha (0.10f));
    g.fillEllipse (cx - capR * 0.42f, cy - capR * 0.54f,
                   capR * 0.42f, capR * 0.36f);

    // 9 — Pointer line
    {
        const float angle = rotaryStartAngle
                            + sliderPosProportion * (rotaryEndAngle - rotaryStartAngle);
        const float pStartR = capR * 0.28f;
        const float pEndR   = capR - 1.0f;

        juce::Path pointer;
        pointer.startNewSubPath (cx + pStartR * std::sin (angle),
                                 cy - pStartR * std::cos (angle));
        pointer.lineTo           (cx + pEndR   * std::sin (angle),
                                  cy - pEndR   * std::cos (angle));

        g.setColour (accentAmber.withAlpha (0.30f));
        g.strokePath (pointer, juce::PathStrokeType (3.0f,
                      juce::PathStrokeType::curved,
                      juce::PathStrokeType::rounded));

        g.setColour (accentAmber);
        g.strokePath (pointer, juce::PathStrokeType (1.5f,
                      juce::PathStrokeType::curved,
                      juce::PathStrokeType::rounded));
    }

    g.restoreState();

    // 10 — Value text (drawn in slider local coords, bottom strip)
    g.setColour (slider.findColour (juce::Label::textColourId).withAlpha (0.65f));
    g.setFont (juce::Font (juce::FontOptions ("Roboto Mono", 8.5f, juce::Font::plain)));
    const juce::String valText = slider.getTextFromValue (slider.getValue());
    g.drawText (valText,
                0, juce::roundToInt (height - kTextH - 1),
                width, juce::roundToInt (kTextH),
                juce::Justification::centred, true);
}

// ==============================================================================
// LuminarAudioProcessorEditor — Constructor / Destructor
// ==============================================================================

LuminarAudioProcessorEditor::LuminarAudioProcessorEditor (LuminarAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    colors = LUMINAR::LuminarTheme::getDarkColors();

    // ---- Setup helper lambdas -------------------------------------------------

    auto setupSlider = [this] (juce::Slider& sl)
    {
        sl.setSliderStyle    (juce::Slider::RotaryHorizontalVerticalDrag);
        sl.setTextBoxStyle   (juce::Slider::NoTextBox, false, 0, 0);
        sl.setLookAndFeel    (&luminarLookAndFeel);
        addAndMakeVisible    (sl);
    };

    auto setupLabel = [this] (juce::Label& lbl, const juce::String& text)
    {
        lbl.setText               (text, juce::dontSendNotification);
        lbl.setFont               (juce::Font (juce::FontOptions ("Inter", 10.0f, juce::Font::plain)));
        lbl.setColour             (juce::Label::textColourId, colors.textSecondary);
        lbl.setJustificationType  (juce::Justification::centred);
        addAndMakeVisible         (lbl);
    };

    // ---- Sliders --------------------------------------------------------------
    setupSlider (presenceSlider);
    setupSlider (airSlider);
    setupSlider (outputSlider);

    // Text-from-value functions for display in the knob LookAndFeel
    presenceSlider.textFromValueFunction = [] (double v)
    {
        return juce::String (juce::roundToInt (v * 100.0)) + "%";
    };
    airSlider.textFromValueFunction = [] (double v)
    {
        return juce::String (juce::roundToInt (v * 100.0)) + "%";
    };
    outputSlider.textFromValueFunction = [] (double v)
    {
        return (v > 0.0 ? "+" : "") + juce::String (v, 1) + " dB";
    };

    // ---- Labels ---------------------------------------------------------------
    setupLabel (presenceLabel, "Presence");
    setupLabel (airLabel,      "Air");
    setupLabel (outputLabel,   "Output");

    presenceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, LUMINAR::Params::presenceID(), presenceSlider);

    airAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, LUMINAR::Params::airID(), airSlider);

    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, LUMINAR::Params::outputGainID(), outputSlider);

    // ---- Window size ----------------------------------------------------------
    setSize (kEditorW, kEditorH);
}

LuminarAudioProcessorEditor::~LuminarAudioProcessorEditor()
{
    // Detach LookAndFeel before destruction to avoid dangling pointer.
    presenceSlider.setLookAndFeel (nullptr);
    airSlider.setLookAndFeel      (nullptr);
    outputSlider.setLookAndFeel   (nullptr);
}

// ==============================================================================
// paint — delegates to section helpers
// ==============================================================================

void LuminarAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Overall background gradient: deep space navy
    {
        juce::ColourGradient bg (colors.backgroundTop,
                                 0.0f, 0.0f,
                                 colors.background,
                                 0.0f, static_cast<float> (getHeight()),
                                 false);
        g.setGradientFill (bg);
        g.fillAll();
    }

    paintAuroraDisplay (g);
    paintTopBar        (g);  // Drawn over aurora so it overlaps cleanly
    paintKnobPanel     (g);
    paintFooter        (g);
}

// ==============================================================================
// paintTopBar
// ==============================================================================

void LuminarAudioProcessorEditor::paintTopBar (juce::Graphics& g)
{
    const auto bounds = juce::Rectangle<float> (0.0f, 0.0f,
                                                static_cast<float> (getWidth()),
                                                static_cast<float> (kTopBarH));

    // Glass background
    {
        juce::ColourGradient topGrad (colors.topBarBg.withAlpha (0.82f),
                                      bounds.getX(), bounds.getY(),
                                      colors.topBarBg.withAlpha (0.70f),
                                      bounds.getX(), bounds.getBottom(),
                                      false);
        g.setGradientFill (topGrad);
        g.fillRect (bounds);
    }

    // Bottom accent line — amber glow
    g.setColour (colors.accent.withAlpha (0.25f));
    g.drawHorizontalLine (kTopBarH - 1, 0.0f, static_cast<float> (getWidth()));

    // Very subtle top edge refraction
    g.setColour (juce::Colours::white.withAlpha (0.06f));
    g.drawHorizontalLine (0, 0.0f, static_cast<float> (getWidth()));

    // Company name — left, small
    g.setColour (colors.textSecondary.withAlpha (0.70f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 9.0f, juce::Font::plain)));
    g.drawText ("ASTRA AUDIO",
                juce::Rectangle<float> (20.0f, 0.0f, 130.0f, static_cast<float> (kTopBarH)),
                juce::Justification::centredLeft, false);

    // Plugin name — centred, bold, letter-spaced
    g.setColour (colors.textPrimary);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 21.0f, juce::Font::bold)));
    g.drawText ("L U M I N A R",
                juce::Rectangle<float> (0.0f, 0.0f,
                                        static_cast<float> (getWidth()),
                                        static_cast<float> (kTopBarH)),
                juce::Justification::centred, false);

    // Version badge — right side, muted
    g.setColour (colors.textMuted);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 8.5f, juce::Font::plain)));
    g.drawText ("v0.1.0",
                juce::Rectangle<float> (static_cast<float> (getWidth()) - 60.0f,
                                        0.0f, 50.0f,
                                        static_cast<float> (kTopBarH)),
                juce::Justification::centredRight, false);
}

// ==============================================================================
// paintAuroraDisplay
// ==============================================================================

void LuminarAudioProcessorEditor::paintAuroraDisplay (juce::Graphics& g)
{
    const float x = 0.0f;
    const float y = static_cast<float> (kTopBarH);
    const float w = static_cast<float> (getWidth());
    const float h = static_cast<float> (kAuroraH);
    const float cx = w * 0.5f;
    const float bottom = y + h;

    // Background gradient for the aurora display area
    {
        juce::ColourGradient bg (colors.backgroundTop,
                                 cx, y,
                                 colors.backgroundAurora,
                                 cx, bottom,
                                 false);
        g.setGradientFill (bg);
        g.fillRect (x, y, w, h);
    }

    // Outer amber radial glow — very subtle, centred at bottom of display
    {
        const float glowCY = bottom - 10.0f;
        juce::ColourGradient outerGlow (
            colors.accent.withAlpha (0.07f), cx, glowCY,
            colors.accent.withAlpha (0.0f),  cx, y + 10.0f,
            true);
        g.setGradientFill (outerGlow);
        g.fillEllipse (cx - 310.0f, y, 620.0f, h + 10.0f);
    }

    // Inner brighter core glow
    {
        const float glowCY = bottom;
        juce::ColourGradient innerGlow (
            colors.accent.withAlpha (0.12f), cx, glowCY,
            colors.accent.withAlpha (0.0f),  cx, y + 40.0f,
            true);
        g.setGradientFill (innerGlow);
        g.fillEllipse (cx - 140.0f, bottom - 120.0f, 280.0f, 180.0f);
    }

    // Horizontal light rays (3 subtle lines)
    for (int i = 0; i < 3; ++i)
    {
        const float rayY     = y + h * (0.38f + static_cast<float>(i) * 0.16f);
        const float rayAlpha = 0.030f - static_cast<float>(i) * 0.008f;

        juce::ColourGradient rayGrad (
            colors.accent.withAlpha (0.0f),    x,  rayY,
            colors.accent.withAlpha (rayAlpha), cx, rayY,
            false);
        rayGrad.addColour (1.0, colors.accent.withAlpha (0.0f));
        g.setGradientFill (rayGrad);
        g.fillRect (x, rayY - 0.5f, w, 1.5f);
    }

    // Subtitle text
    g.setColour (colors.textSecondary.withAlpha (0.28f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 9.5f, juce::Font::plain)));
    g.drawText ("HIGH-FREQUENCY ENHANCER",
                juce::Rectangle<float> (x + 20.0f, y + h * 0.5f - 9.0f, w - 40.0f, 18.0f),
                juce::Justification::centred, false);

    // Faint bottom divider
    g.setColour (colors.accent.withAlpha (0.12f));
    g.drawHorizontalLine (static_cast<int> (bottom) - 1, x, x + w);
}

// ==============================================================================
// paintKnobPanel
// ==============================================================================

void LuminarAudioProcessorEditor::paintKnobPanel (juce::Graphics& g)
{
    const juce::Rectangle<float> panel (kPanelMarginX, kPanelY, kPanelW, kPanelH);

    // Soft drop shadow behind panel
    g.setColour (juce::Colours::black.withAlpha (0.18f));
    g.fillRoundedRectangle (panel.translated (0.0f, 3.0f), kPanelRadius);

    // Glass panel fill
    {
        juce::ColourGradient fill (
            juce::Colour (22, 34, 56).withAlpha (0.38f), panel.getX(), panel.getY(),
            juce::Colour (14, 22, 40).withAlpha (0.22f), panel.getX(), panel.getBottom(),
            false);
        g.setGradientFill (fill);
        g.fillRoundedRectangle (panel, kPanelRadius);
    }

    // Top refraction line (specular)
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawHorizontalLine (static_cast<int> (kPanelY),
                          panel.getX() + kPanelRadius,
                          panel.getRight() - kPanelRadius);

    // Glass border
    g.setColour (colors.border);
    g.drawRoundedRectangle (panel, kPanelRadius, 1.0f);

    // Very subtle amber glow at the bottom of the panel
    {
        juce::ColourGradient bottomGlow (
            colors.accent.withAlpha (0.0f),   panel.getCentreX(), panel.getCentreY(),
            colors.accent.withAlpha (0.04f),  panel.getCentreX(), panel.getBottom(),
            false);
        g.setGradientFill (bottomGlow);
        g.fillRoundedRectangle (panel, kPanelRadius);
    }
}

// ==============================================================================
// paintFooter
// ==============================================================================

void LuminarAudioProcessorEditor::paintFooter (juce::Graphics& g)
{
    const juce::Rectangle<float> footer (
        0.0f, static_cast<float> (kFooterY),
        static_cast<float> (getWidth()),
        static_cast<float> (kFooterH));

    // Footer background: slightly darker than main bg
    g.setColour (juce::Colour (6, 9, 18).withAlpha (0.95f));
    g.fillRect (footer);

    // Top border — very subtle
    g.setColour (juce::Colours::white.withAlpha (0.05f));
    g.drawHorizontalLine (kFooterY, 0.0f, static_cast<float> (getWidth()));

    // Footer text
    g.setColour (colors.textMuted);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 9.0f, juce::Font::plain)));
    g.drawText ("ASTRA Audio Suite  \u2022  LUMINAR v0.1.0",
                footer, juce::Justification::centred, false);
}

// ==============================================================================
// resized
// ==============================================================================

void LuminarAudioProcessorEditor::resized()
{
    // Knob sliders
    presenceSlider.setBounds (knobX (0), kSliderY, kKnobSize, kKnobSize);
    airSlider.setBounds      (knobX (1), kSliderY, kKnobSize, kKnobSize);
    outputSlider.setBounds   (knobX (2), kSliderY, kKnobSize, kKnobSize);

    // Labels (slightly wider than the knob to allow text to breathe)
    constexpr int kLabelW = kKnobSize + 20;
    constexpr int kLabelOffX = -10; // shift left by half the extra width

    presenceLabel.setBounds (knobX (0) + kLabelOffX, kLabelY, kLabelW, kLabelH);
    airLabel.setBounds      (knobX (1) + kLabelOffX, kLabelY, kLabelW, kLabelH);
    outputLabel.setBounds   (knobX (2) + kLabelOffX, kLabelY, kLabelW, kLabelH);
}
