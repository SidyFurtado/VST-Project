#include "PluginEditor.h"
#include "ParameterIDs.h"
#include <algorithm>
#include <cmath>

// ==============================================================================
// Layout constants
// ==============================================================================

namespace
{
    constexpr int kEditorW       = 800;
    constexpr int kEditorH       = 500;

    constexpr int kTopBarH       = 54;
    constexpr int kDisplayY      = kTopBarH;
    constexpr int kDisplayH      = 92;
    constexpr int kControlPanelY = kDisplayY + kDisplayH;
    constexpr int kControlPanelH = 286;
    constexpr int kFooterY       = kControlPanelY + kControlPanelH;
    constexpr int kFooterH       = kEditorH - kFooterY;

    constexpr int kPanelMarginX  = 14;
    constexpr int kColumnGap     = 8;
    constexpr int kColumnW       = (kEditorW - 2 * kPanelMarginX - 2 * kColumnGap) / 3;
    constexpr int kLeftColumnX   = kPanelMarginX;
    constexpr int kCenterColumnX = kLeftColumnX + kColumnW + kColumnGap;
    constexpr int kRightColumnX  = kCenterColumnX + kColumnW + kColumnGap;

    constexpr int kLabelH        = 18;
    constexpr int kValueH        = 22;
    constexpr int kSliderW       = 46;
    constexpr int kSliderH       = 178;
    constexpr int kSliderY       = kControlPanelY + 58;
    constexpr int kMeterW        = 18;
    constexpr int kMeterH        = kSliderH;
    constexpr int kMeterGap      = 12;

    constexpr int kCurveX        = kCenterColumnX + 18;
    constexpr int kCurveY        = kControlPanelY + 22;
    constexpr int kCurveW        = kColumnW - 36;
    constexpr int kCurveH        = 110;

    constexpr int kRatioSliderH  = 100;
    constexpr int kRatioSliderY  = kCurveY + kCurveH + 16;

    constexpr int kFooterControlY = kFooterY + 10;
    constexpr int kFooterLabelY   = kFooterY + 4;
    constexpr int kSmallSliderW   = 160;
    constexpr int kSmallSliderH   = 24;
    constexpr int kButtonW        = 78;
    constexpr int kButtonH        = 26;

    inline int columnCenterX (int columnX) noexcept
    {
        return columnX + kColumnW / 2;
    }

    inline juce::String utf8 (const char* text)
    {
        return juce::String (juce::CharPointer_UTF8 (text));
    }

    constexpr int kMenuDefaultFlat = 1;
    constexpr int kMenuSavePreset  = 2;
    constexpr int kMenuFactoryBase = 1000;
    constexpr int kMenuUserBase    = 5000;

    struct FactoryPreset
    {
        int menuID;
        const char* presetID;
        const char* name;
        float thresholdDb;
        float ratio;
        float attackMs;
        float releaseMs;
        float gainDb;
        int mode;
        bool arc;
    };

    const FactoryPreset kFactoryPresets[] =
    {
        { kMenuFactoryBase +  1, "forro_vocal",              "Vocal",         -22.0f,  3.0f,  8.0f, 180.0f,  2.5f, 1, true  },
        { kMenuFactoryBase +  2, "forro_sanfona",            "Sanfona",       -18.5f,  2.4f, 18.0f, 220.0f,  1.0f, 1, true  },
        { kMenuFactoryBase +  3, "forro_zabumba",            "Zabumba",       -16.0f,  5.2f,  3.5f, 120.0f,  1.5f, 0, false },
        { kMenuFactoryBase +  4, "forro_baixo",              "Baixo",         -20.0f,  4.2f, 12.0f, 180.0f,  1.2f, 1, true  },
        { kMenuFactoryBase +  5, "forro_mix_bus",            "Mix Bus",       -10.0f,  2.0f, 28.0f, 320.0f,  0.8f, 1, true  },

        { kMenuFactoryBase +  6, "trap_808_punch",           "808 Punch",     -14.0f,  6.5f,  1.5f,  90.0f,  2.0f, 0, false },
        { kMenuFactoryBase +  7, "trap_snare_snap",          "Snare Snap",    -12.5f,  4.8f,  0.8f,  70.0f,  1.8f, 0, false },
        { kMenuFactoryBase +  8, "trap_vocal",               "Vocal",         -24.0f,  3.6f,  5.0f, 140.0f,  3.0f, 1, true  },

        { kMenuFactoryBase +  9, "hip_hop",                  "Hip-Hop",       -18.0f,  3.8f,  8.0f, 180.0f,  1.5f, 0, true  },

        { kMenuFactoryBase + 10, "funk_br_tamborzao",        "Tamborzão",     -15.0f,  5.8f,  2.5f, 105.0f,  1.8f, 0, false },
        { kMenuFactoryBase + 11, "funk_br_mandelao",         "Mandelão",      -13.5f,  7.2f,  1.2f,  80.0f,  2.2f, 0, false },

        { kMenuFactoryBase + 12, "rock",                     "Rock",          -17.0f,  4.0f,  6.0f, 160.0f,  1.4f, 0, true  },

        { kMenuFactoryBase + 13, "sertanejo_violao",         "Violão",        -20.0f,  2.8f, 16.0f, 240.0f,  1.0f, 1, true  },
        { kMenuFactoryBase + 14, "sertanejo_acordeon",       "Acordeon",      -19.0f,  2.6f, 20.0f, 260.0f,  0.8f, 1, true  },
        { kMenuFactoryBase + 15, "sertanejo_voz_principal",  "Voz Principal", -23.0f,  3.2f,  7.0f, 170.0f,  2.6f, 1, true  },

        { kMenuFactoryBase + 16, "pop",                      "Pop",           -20.0f,  3.4f,  4.0f, 130.0f,  2.0f, 1, true  },
        { kMenuFactoryBase + 17, "gospel",                   "Gospel",        -21.0f,  2.7f, 12.0f, 260.0f,  1.6f, 1, true  },
        { kMenuFactoryBase + 18, "electronic",               "Electronic",    -13.0f,  4.5f,  2.0f, 110.0f,  1.5f, 0, false },

        { kMenuFactoryBase + 19, "source_bateria",           "Bateria",       -16.0f,  4.6f,  3.0f, 120.0f,  1.2f, 0, false },
        { kMenuFactoryBase + 20, "source_baixo",             "Baixo",         -19.0f,  4.0f, 11.0f, 190.0f,  1.0f, 1, true  },
        { kMenuFactoryBase + 21, "source_voz",               "Voz",           -23.0f,  3.1f,  6.0f, 160.0f,  2.5f, 1, true  },
        { kMenuFactoryBase + 22, "source_violao",            "Violão",        -21.0f,  2.5f, 15.0f, 230.0f,  0.8f, 1, true  },
        { kMenuFactoryBase + 23, "source_master",            "Master",        -8.5f,   1.7f, 35.0f, 420.0f,  0.4f, 1, true  },
    };
}

// ==============================================================================
// GravityLookAndFeel
// ==============================================================================

GravityLookAndFeel::GravityLookAndFeel()
{
    // Vertical slider colours
    setColour (juce::Slider::trackColourId,          juce::Colour ( 26,  38,  66));
    setColour (juce::Slider::thumbColourId,           juce::Colour ( 12,  18,  34));
    setColour (juce::Slider::backgroundColourId,      juce::Colour (  8,  13,  24));
    setColour (juce::Label::textColourId,             juce::Colour (245, 248, 255));
}

void GravityLookAndFeel::drawLinearSlider (juce::Graphics& g,
                                           int x, int y, int width, int height,
                                           float sliderPos,
                                           float /*minSliderPos*/,
                                           float /*maxSliderPos*/,
                                           juce::Slider::SliderStyle style,
                                           juce::Slider& /*slider*/)
{
    const juce::Colour accent     (232,  64,  64);  // Crimson
    const juce::Colour trackBase  ( 26,  38,  66);
    const juce::Colour thumbBase  ( 14,  20,  38);

    if (style == juce::Slider::LinearHorizontal)
    {
        const float trackH = 4.0f;
        const float trackY = y + (height - trackH) * 0.5f;
        const float startX = static_cast<float> (x);
        const float endX   = static_cast<float> (x + width);
        const float posX   = std::clamp (sliderPos, startX, endX);

        g.setColour (trackBase.withAlpha (0.55f));
        g.fillRoundedRectangle (startX, trackY, static_cast<float> (width), trackH, trackH * 0.5f);

        if (posX > startX)
        {
            juce::ColourGradient fillGrad (accent.withAlpha (0.40f), startX, trackY,
                                           accent, posX, trackY,
                                           false);
            g.setGradientFill (fillGrad);
            g.fillRoundedRectangle (startX, trackY, posX - startX, trackH, trackH * 0.5f);
        }

        const float thumbR  = 7.0f;
        const float thumbCY = y + height * 0.5f;

        g.setColour (juce::Colours::black.withAlpha (0.42f));
        g.fillEllipse (posX - thumbR, thumbCY - thumbR + 2.0f, thumbR * 2.0f, thumbR * 2.0f);

        juce::ColourGradient thumbGrad (thumbBase.brighter (0.08f), posX, thumbCY - thumbR,
                                        thumbBase.darker   (0.20f), posX, thumbCY + thumbR,
                                        false);
        g.setGradientFill (thumbGrad);
        g.fillEllipse (posX - thumbR, thumbCY - thumbR, thumbR * 2.0f, thumbR * 2.0f);

        g.setColour (accent.withAlpha (0.85f));
        g.fillEllipse (posX - 2.2f, thumbCY - 2.2f, 4.4f, 4.4f);
        return;
    }

    const float trackW  = 4.0f;
    const float trackX  = x + (width - trackW) * 0.5f;

    // --- Background track ---------------------------------------------------
    g.setColour (trackBase.withAlpha (0.55f));
    g.fillRoundedRectangle (trackX, static_cast<float> (y),
                             trackW, static_cast<float> (height), trackW * 0.5f);

    // --- Active fill (bottom to thumb, crimson) --------------------------------
    const float activeTop    = sliderPos;
    const float activeBottom = static_cast<float> (y + height);
    if (activeTop < activeBottom)
    {
        juce::ColourGradient fillGrad (accent,              trackX, activeBottom,
                                       accent.withAlpha (0.40f), trackX, activeTop,
                                       false);
        g.setGradientFill (fillGrad);
        g.fillRoundedRectangle (trackX, activeTop, trackW, activeBottom - activeTop, trackW * 0.5f);
    }

    // --- Thumb --------------------------------------------------------------
    const float thumbR = 8.0f;
    const float thumbCX = x + width  * 0.5f;
    const float thumbCY = sliderPos;

    // Drop shadow
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillEllipse (thumbCX - thumbR, thumbCY - thumbR + 2.0f, thumbR * 2.0f, thumbR * 2.0f);

    // Body gradient
    juce::ColourGradient thumbGrad (thumbBase.brighter (0.08f), thumbCX, thumbCY - thumbR,
                                    thumbBase.darker   (0.20f), thumbCX, thumbCY + thumbR,
                                    false);
    g.setGradientFill (thumbGrad);
    g.fillEllipse (thumbCX - thumbR, thumbCY - thumbR, thumbR * 2.0f, thumbR * 2.0f);

    // Thumb border (glass)
    g.setColour (juce::Colours::white.withAlpha (0.10f));
    g.drawEllipse (thumbCX - thumbR + 0.5f, thumbCY - thumbR + 0.5f,
                   thumbR * 2.0f - 1.0f, thumbR * 2.0f - 1.0f, 0.75f);

    // Crimson glow dot on top
    g.setColour (accent.withAlpha (0.85f));
    g.fillEllipse (thumbCX - 2.5f, thumbCY - 2.5f, 5.0f, 5.0f);
}

void GravityLookAndFeel::drawToggleButton (juce::Graphics& g,
                                           juce::ToggleButton& button,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool /*shouldDrawButtonAsDown*/)
{
    const juce::Colour accent  (232,  64,  64);
    const juce::Colour offBg   ( 20,  28,  50);
    const juce::Colour onBg    = accent.withAlpha (0.18f);
    const bool isOn            = button.getToggleState();

    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    const float radius = 5.0f;

    // Background
    g.setColour (isOn ? onBg : offBg.withAlpha (0.70f));
    g.fillRoundedRectangle (bounds, radius);

    // Border
    g.setColour (isOn ? accent.withAlpha (0.60f)
                      : juce::Colours::white.withAlpha (0.10f));
    g.drawRoundedRectangle (bounds, radius, 1.0f);

    // Highlight on hover
    if (shouldDrawButtonAsHighlighted)
    {
        g.setColour (juce::Colours::white.withAlpha (0.04f));
        g.fillRoundedRectangle (bounds, radius);
    }

    // Text
    g.setColour (isOn ? accent : juce::Colour (140, 155, 180));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 10.0f, juce::Font::bold)));
    g.drawText (button.getButtonText(), bounds.toNearestInt(),
                juce::Justification::centred, false);
}

// ==============================================================================
// GravityMeterComponent
// ==============================================================================

GravityMeterComponent::GravityMeterComponent (MeterType type, GRAVITY::GravityColors col)
    : meterType (type), colors (col)
{
    if (meterType == MeterType::GainReduction)
        peakDb = 0.0f;
}

void GravityMeterComponent::setLevelDb (float db)
{
    currentDb = std::clamp (db, kMinDb - 6.0f, kMaxDb + 3.0f);

    if (meterType == MeterType::GainReduction)
    {
        if (currentDb < peakDb)
        {
            peakDb     = currentDb;
            peakHoldMs = 0;
        }
        else
        {
            ++peakHoldMs;
            if (peakHoldMs > 120)
            {
                peakDb += 0.3f;
                if (peakDb > 0.0f)
                    peakDb = 0.0f;
            }
        }

        repaint();
        return;
    }

    if (currentDb > peakDb)
    {
        peakDb     = currentDb;
        peakHoldMs = 0;
    }
    else
    {
        ++peakHoldMs;
        if (peakHoldMs > 120)  // ~2 s at 60 fps
        {
            peakDb -= 0.3f;
            if (peakDb < kMinDb)
                peakDb = kMinDb;
        }
    }

    repaint();
}

float GravityMeterComponent::dbToY (float db) const noexcept
{
    const float h  = static_cast<float> (getHeight());
    const float norm = (db - kMinDb) / (kMaxDb - kMinDb);
    return h - std::clamp (norm, 0.0f, 1.0f) * h;
}

void GravityMeterComponent::paint (juce::Graphics& g)
{
    const float w = static_cast<float> (getWidth());
    const float h = static_cast<float> (getHeight());
    const float radius = 2.5f;

    // Background
    g.setColour (juce::Colour (10, 14, 26));
    g.fillRoundedRectangle (0.0f, 0.0f, w, h, radius);

    if (meterType == MeterType::GainReduction)
    {
        const float grAbs = std::clamp (-currentDb, 0.0f, 30.0f);
        const float barH  = juce::jmap (grAbs, 0.0f, 30.0f, 0.0f, h);

        if (barH > 0.5f)
        {
            const juce::Colour amber (246, 201, 14);
            juce::ColourGradient barGrad (amber.withAlpha (0.95f), 0.0f, 0.0f,
                                          amber.withAlpha (0.32f), 0.0f, barH,
                                          false);
            g.setGradientFill (barGrad);
            g.fillRoundedRectangle (1.0f, 1.0f, w - 2.0f, barH - 1.0f, radius);
        }

        if (grAbs > 0.5f)
        {
            const float peakAbs = std::clamp (-peakDb, 0.0f, 30.0f);
            const float peakY   = juce::jmap (peakAbs, 0.0f, 30.0f, 0.0f, h);
            g.setColour (juce::Colour (246, 201, 14).withAlpha (0.80f));
            g.fillRect (1.0f, peakY, w - 2.0f, 1.5f);
        }

        g.setColour (colors.border);
        g.drawRoundedRectangle (0.5f, 0.5f, w - 1.0f, h - 1.0f, radius, 0.75f);
        return;
    }

    // Active bar
    const float levelY = dbToY (currentDb);
    const float barH   = h - levelY;

    if (barH > 0.5f)
    {
        juce::Colour barColor;

        // Input / Output: green -> amber -> red near 0 dBFS
        if (currentDb < -18.0f)
            barColor = colors.meterGreen;
        else if (currentDb < -6.0f)
            barColor = colors.meterAmber;
        else
            barColor = colors.meterRed;

        juce::ColourGradient barGrad (barColor.withAlpha (0.90f), 0.0f, levelY,
                                      barColor.withAlpha (0.35f), 0.0f, h,
                                      false);
        g.setGradientFill (barGrad);
        g.fillRoundedRectangle (1.0f, levelY, w - 2.0f, barH, radius);
    }

    // Peak hold line
    if (peakDb > kMinDb)
    {
        const float peakY = dbToY (peakDb);
        g.setColour (colors.accent.withAlpha (0.75f));
        g.fillRect (1.0f, peakY, w - 2.0f, 1.5f);
    }

    // Border
    g.setColour (colors.border);
    g.drawRoundedRectangle (0.5f, 0.5f, w - 1.0f, h - 1.0f, radius, 0.75f);
}

void GravityMeterComponent::resized() {}

// ==============================================================================
// GravityTransferCurveComponent
// ==============================================================================

GravityTransferCurveComponent::GravityTransferCurveComponent (GRAVITY::GravityColors col)
    : colors (col)
{
}

void GravityTransferCurveComponent::setParameterSources (std::atomic<float>* thresholdDb, std::atomic<float>* ratio)
{
    thresholdParam = thresholdDb;
    ratioParam     = ratio;
}

float GravityTransferCurveComponent::compressDb (float inputDb, float thresholdDb, float ratio) const noexcept
{
    constexpr float kneeDb = 6.0f;
    const float safeRatio = std::max (1.0f, ratio);
    const float kneeStart = thresholdDb - kneeDb * 0.5f;
    const float kneeEnd   = thresholdDb + kneeDb * 0.5f;

    if (inputDb < kneeStart)
        return inputDb;

    if (inputDb > kneeEnd)
        return thresholdDb + (inputDb - thresholdDb) / safeRatio;

    const float kneePosition = inputDb - kneeStart;
    return inputDb + ((1.0f / safeRatio) - 1.0f) * kneePosition * kneePosition / (2.0f * kneeDb);
}

juce::Point<float> GravityTransferCurveComponent::dbToPoint (float inputDb,
                                                             float outputDb,
                                                             juce::Rectangle<float> plot) const noexcept
{
    constexpr float minDb = -60.0f;
    constexpr float maxDb =   0.0f;

    const float xNorm = (inputDb  - minDb) / (maxDb - minDb);
    const float yNorm = (outputDb - minDb) / (maxDb - minDb);

    return { plot.getX() + std::clamp (xNorm, 0.0f, 1.0f) * plot.getWidth(),
             plot.getBottom() - std::clamp (yNorm, 0.0f, 1.0f) * plot.getHeight() };
}

void GravityTransferCurveComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (1.0f);
    auto plot   = bounds.reduced (10.0f, 9.0f);

    g.setColour (juce::Colour (8, 13, 24).withAlpha (0.96f));
    g.fillRoundedRectangle (bounds, 7.0f);

    g.setColour (juce::Colours::white.withAlpha (0.07f));
    g.drawRoundedRectangle (bounds, 7.0f, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.045f));
    for (int i = 1; i < 4; ++i)
    {
        const float x = plot.getX() + plot.getWidth() * static_cast<float> (i) / 4.0f;
        const float y = plot.getY() + plot.getHeight() * static_cast<float> (i) / 4.0f;
        g.drawVerticalLine (static_cast<int> (std::round (x)), plot.getY(), plot.getBottom());
        g.drawHorizontalLine (static_cast<int> (std::round (y)), plot.getX(), plot.getRight());
    }

    const auto dryStart = dbToPoint (-60.0f, -60.0f, plot);
    const auto dryEnd   = dbToPoint (0.0f, 0.0f, plot);

    g.setColour (colors.textMuted.withAlpha (0.30f));
    g.drawLine ({ dryStart, dryEnd }, 1.0f);

    const float thresholdDb = thresholdParam != nullptr ? thresholdParam->load() : -18.0f;
    const float ratio       = ratioParam     != nullptr ? ratioParam->load()     : 4.0f;

    juce::Path curve;
    constexpr int steps = 72;

    for (int i = 0; i <= steps; ++i)
    {
        const float inputDb  = juce::jmap (static_cast<float> (i), 0.0f, static_cast<float> (steps), -60.0f, 0.0f);
        const float outputDb = compressDb (inputDb, thresholdDb, ratio);
        const auto p = dbToPoint (inputDb, outputDb, plot);

        if (i == 0)
            curve.startNewSubPath (p);
        else
            curve.lineTo (p);
    }

    const auto thresholdTop    = dbToPoint (thresholdDb, 0.0f, plot);
    const auto thresholdBottom = dbToPoint (thresholdDb, -60.0f, plot);

    g.setColour (colors.accent.withAlpha (0.20f));
    g.drawLine ({ thresholdTop, thresholdBottom }, 1.0f);

    g.setColour (colors.accent.withAlpha (0.18f));
    g.strokePath (curve, juce::PathStrokeType (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    g.setColour (colors.accent);
    g.strokePath (curve, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    g.setColour (colors.textMuted);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 7.5f, juce::Font::plain)));
    g.drawText ("TRANSFER", bounds.removeFromTop (13.0f), juce::Justification::centred, false);
}

// ==============================================================================
// GravityAudioProcessorEditor — Constructor / Destructor
// ==============================================================================

GravityAudioProcessorEditor::GravityAudioProcessorEditor (GravityAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      inputMeter  (GravityMeterComponent::MeterType::Input,         GRAVITY::GravityTheme::getDarkColors()),
      outputMeter (GravityMeterComponent::MeterType::Output,        GRAVITY::GravityTheme::getDarkColors()),
      grMeter     (GravityMeterComponent::MeterType::GainReduction, GRAVITY::GravityTheme::getDarkColors()),
      transferCurve (GRAVITY::GravityTheme::getDarkColors())
{
    colors = GRAVITY::GravityTheme::getDarkColors();

    // ---- Slider setup helper -------------------------------------------------

    auto setupVertSlider = [this] (juce::Slider& sl)
    {
        sl.setSliderStyle  (juce::Slider::LinearVertical);
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        sl.setLookAndFeel  (&gravityLookAndFeel);
        addAndMakeVisible  (sl);
    };

    auto setupLabel = [this] (juce::Label& lbl, const juce::String& text)
    {
        lbl.setText              (text, juce::dontSendNotification);
        lbl.setFont              (juce::Font (juce::FontOptions ("Inter", 10.0f, juce::Font::plain)));
        lbl.setColour            (juce::Label::textColourId, colors.textSecondary);
        lbl.setJustificationType (juce::Justification::centred);
        addAndMakeVisible        (lbl);
    };

    auto setupValueLabel = [this] (juce::Label& lbl)
    {
        lbl.setFont              (juce::Font (juce::FontOptions ("Inter", 12.0f, juce::Font::bold)));
        lbl.setColour            (juce::Label::textColourId, colors.textPrimary);
        lbl.setJustificationType (juce::Justification::centred);
        addAndMakeVisible        (lbl);
    };

    auto setupMeterLabel = [this] (juce::Label& lbl, const juce::String& text)
    {
        lbl.setText              (text, juce::dontSendNotification);
        lbl.setFont              (juce::Font (juce::FontOptions ("Inter", 8.5f, juce::Font::plain)));
        lbl.setColour            (juce::Label::textColourId, colors.textMuted);
        lbl.setJustificationType (juce::Justification::centred);
        addAndMakeVisible        (lbl);
    };

    // ---- Primary sliders (vertical) ------------------------------------------
    setupVertSlider (threshSlider);
    setupVertSlider (ratioSlider);
    setupVertSlider (gainSlider);

    // Text-from-value lambdas
    threshSlider.textFromValueFunction = [] (double v)
    {
        return (v >= 0.0 ? "+" : "") + juce::String (v, 1) + " dB";
    };
    ratioSlider.textFromValueFunction = [] (double v)
    {
        return juce::String (v, 1) + ":1";
    };
    gainSlider.textFromValueFunction = [] (double v)
    {
        return "+" + juce::String (v, 1) + " dB";
    };

    setupLabel (threshLabel,  "THRESH");
    setupLabel (ratioLabel,   "RATIO");
    setupLabel (gainLabel,    "GAIN");

    setupValueLabel (threshValueLabel);
    setupValueLabel (ratioValueLabel);
    setupValueLabel (gainValueLabel);

    // ---- Secondary sliders (Attack / Release) --------------------------------
    setupVertSlider (attackSlider);
    setupVertSlider (releaseSlider);
    attackSlider.setSliderStyle  (juce::Slider::LinearHorizontal);
    releaseSlider.setSliderStyle (juce::Slider::LinearHorizontal);

    attackSlider.textFromValueFunction = [] (double v)
    {
        return juce::String (v, 1) + " ms";
    };
    releaseSlider.textFromValueFunction = [] (double v)
    {
        if (v >= 1000.0)
            return juce::String (v / 1000.0, 2) + " s";
        return juce::String (v, 0) + " ms";
    };

    setupLabel (attackLabel,  "ATK");
    setupLabel (releaseLabel, "REL");

    setupValueLabel (attackValueLabel);
    setupValueLabel (releaseValueLabel);

    // ---- Presets button -------------------------------------------------------
    presetsButton.setButtonText ("Presets");
    presetsButton.setColour (juce::TextButton::buttonColourId, colors.panel.withAlpha (0.20f));
    presetsButton.setColour (juce::TextButton::buttonOnColourId, colors.accent.withAlpha (0.18f));
    presetsButton.setColour (juce::TextButton::textColourOffId, colors.textSecondary);
    presetsButton.setColour (juce::TextButton::textColourOnId, colors.accent);
    presetsButton.onClick = [this] { showPresetMenu(); };
    addAndMakeVisible (presetsButton);

    // ---- Toggle buttons ------------------------------------------------------
    modeButton.setButtonText ("OPTO");
    modeButton.setClickingTogglesState (true);
    modeButton.setLookAndFeel (&gravityLookAndFeel);
    addAndMakeVisible (modeButton);

    arcButton.setButtonText ("ARC");
    arcButton.setClickingTogglesState (true);
    arcButton.setLookAndFeel (&gravityLookAndFeel);
    addAndMakeVisible (arcButton);

    // ---- Meters --------------------------------------------------------------
    addAndMakeVisible (inputMeter);
    addAndMakeVisible (outputMeter);
    addAndMakeVisible (grMeter);

    setupMeterLabel (inputMeterLabel,  "IN");
    setupMeterLabel (outputMeterLabel, "OUT");
    setupMeterLabel (grMeterLabel,     "GR");

    // ---- Transfer curve -------------------------------------------------------
    transferCurve.setParameterSources (p.apvts.getRawParameterValue (GRAVITY::Params::threshID()),
                                       p.apvts.getRawParameterValue (GRAVITY::Params::ratioID()));
    addAndMakeVisible (transferCurve);

    // ---- APVTS attachments ---------------------------------------------------
    threshAtt  = std::make_unique<SliderAtt> (p.apvts, GRAVITY::Params::threshID(),  threshSlider);
    ratioAtt   = std::make_unique<SliderAtt> (p.apvts, GRAVITY::Params::ratioID(),   ratioSlider);
    attackAtt  = std::make_unique<SliderAtt> (p.apvts, GRAVITY::Params::attackID(),  attackSlider);
    releaseAtt = std::make_unique<SliderAtt> (p.apvts, GRAVITY::Params::releaseID(), releaseSlider);
    gainAtt    = std::make_unique<SliderAtt> (p.apvts, GRAVITY::Params::gainID(),    gainSlider);
    modeAtt    = std::make_unique<ButtonAtt> (p.apvts, GRAVITY::Params::modeID(),    modeButton);
    arcAtt     = std::make_unique<ButtonAtt> (p.apvts, GRAVITY::Params::arcID(),     arcButton);

    updateDynamicLabels();

    // ---- Start metering timer (30 fps) ---------------------------------------
    startTimerHz (30);

    setSize (kEditorW, kEditorH);
}

GravityAudioProcessorEditor::~GravityAudioProcessorEditor()
{
    stopTimer();

    // Detach LookAndFeel before destruction to avoid dangling pointers.
    threshSlider.setLookAndFeel  (nullptr);
    ratioSlider.setLookAndFeel   (nullptr);
    gainSlider.setLookAndFeel    (nullptr);
    attackSlider.setLookAndFeel  (nullptr);
    releaseSlider.setLookAndFeel (nullptr);
    modeButton.setLookAndFeel    (nullptr);
    arcButton.setLookAndFeel     (nullptr);
}

// ==============================================================================
// Timer — metering update
// ==============================================================================

void GravityAudioProcessorEditor::timerCallback()
{
    inputMeter.setLevelDb  (audioProcessor.getInputLevelDb());
    outputMeter.setLevelDb (audioProcessor.getOutputLevelDb());
    grMeter.setLevelDb     (audioProcessor.getGainReductionDb());
    transferCurve.repaint();
    updateDynamicLabels();
}

void GravityAudioProcessorEditor::updateDynamicLabels()
{
    threshValueLabel.setText  (threshSlider.getTextFromValue (threshSlider.getValue()),   juce::dontSendNotification);
    ratioValueLabel.setText   (ratioSlider.getTextFromValue  (ratioSlider.getValue()),    juce::dontSendNotification);
    gainValueLabel.setText    (gainSlider.getTextFromValue   (gainSlider.getValue()),     juce::dontSendNotification);
    attackValueLabel.setText  (attackSlider.getTextFromValue (attackSlider.getValue()),   juce::dontSendNotification);
    releaseValueLabel.setText (releaseSlider.getTextFromValue (releaseSlider.getValue()), juce::dontSendNotification);

    if (auto* mode = audioProcessor.apvts.getRawParameterValue (GRAVITY::Params::modeID()))
        modeButton.setButtonText (mode->load() >= 0.5f ? "OPTO" : "ELECTRO");

    if (auto* arc = audioProcessor.apvts.getRawParameterValue (GRAVITY::Params::arcID()))
        arcButton.setButtonText (arc->load() >= 0.5f ? "ARC ON" : "ARC");
}

void GravityAudioProcessorEditor::showPresetMenu()
{
    auto menu = buildPresetMenu();
    juce::Component::SafePointer<GravityAudioProcessorEditor> safeThis (this);

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&presetsButton),
                        [safeThis] (int selectedID)
                        {
                            if (safeThis == nullptr || selectedID == 0)
                                return;

                            if (selectedID == kMenuDefaultFlat)
                            {
                                safeThis->audioProcessor.applyPresetValues (-18.0f, 4.0f, 10.0f, 150.0f, 0.0f, 0, false);
                                safeThis->updateDynamicLabels();
                                safeThis->transferCurve.repaint();
                                return;
                            }

                            if (selectedID == kMenuSavePreset)
                            {
                                safeThis->saveCurrentPresetAs();
                                return;
                            }

                            if (selectedID >= kMenuFactoryBase && selectedID < kMenuUserBase)
                            {
                                for (const auto& preset : kFactoryPresets)
                                {
                                    if (preset.menuID == selectedID)
                                    {
                                        safeThis->applyFactoryPreset (preset.presetID);
                                        return;
                                    }
                                }
                            }

                            if (selectedID >= kMenuUserBase)
                            {
                                const int fileIndex = selectedID - kMenuUserBase;
                                if (juce::isPositiveAndBelow (fileIndex, safeThis->userPresetFiles.size()))
                                    safeThis->loadUserPresetFile (safeThis->userPresetFiles.getReference (fileIndex));
                            }
                        });
}

juce::PopupMenu GravityAudioProcessorEditor::buildPresetMenu()
{
    juce::PopupMenu menu;

    menu.addItem (kMenuDefaultFlat, "Default Flat");
    menu.addItem (kMenuSavePreset,  "Salvar Preset Atual...");
    menu.addSeparator();
    menu.addSubMenu (utf8 ("Gêneros"), buildFactoryGenresMenu());
    menu.addSubMenu ("Por Fonte", buildSourceMenu());
    menu.addSubMenu (utf8 ("Presets do Usuário"), buildUserPresetsMenu());

    return menu;
}

juce::PopupMenu GravityAudioProcessorEditor::buildFactoryGenresMenu()
{
    auto addPreset = [] (juce::PopupMenu& menu, const char* presetID)
    {
        for (const auto& preset : kFactoryPresets)
        {
            if (juce::String (preset.presetID) == presetID)
            {
                menu.addItem (preset.menuID, utf8 (preset.name));
                return;
            }
        }
    };

    juce::PopupMenu forro;
    addPreset (forro, "forro_vocal");
    addPreset (forro, "forro_sanfona");
    addPreset (forro, "forro_zabumba");
    addPreset (forro, "forro_baixo");
    addPreset (forro, "forro_mix_bus");

    juce::PopupMenu trap;
    addPreset (trap, "trap_808_punch");
    addPreset (trap, "trap_snare_snap");
    addPreset (trap, "trap_vocal");

    juce::PopupMenu funkBR;
    addPreset (funkBR, "funk_br_tamborzao");
    addPreset (funkBR, "funk_br_mandelao");

    juce::PopupMenu sertanejo;
    addPreset (sertanejo, "sertanejo_violao");
    addPreset (sertanejo, "sertanejo_acordeon");
    addPreset (sertanejo, "sertanejo_voz_principal");

    juce::PopupMenu menu;
    menu.addSubMenu (utf8 ("Forró"), forro);
    menu.addSubMenu ("Trap", trap);
    addPreset (menu, "hip_hop");
    menu.addSubMenu ("Funk BR", funkBR);
    addPreset (menu, "rock");
    menu.addSubMenu ("Sertanejo", sertanejo);
    addPreset (menu, "pop");
    addPreset (menu, "gospel");
    addPreset (menu, "electronic");

    return menu;
}

juce::PopupMenu GravityAudioProcessorEditor::buildSourceMenu()
{
    auto addPreset = [] (juce::PopupMenu& menu, const char* presetID)
    {
        for (const auto& preset : kFactoryPresets)
        {
            if (juce::String (preset.presetID) == presetID)
            {
                menu.addItem (preset.menuID, utf8 (preset.name));
                return;
            }
        }
    };

    juce::PopupMenu menu;
    addPreset (menu, "source_bateria");
    addPreset (menu, "source_baixo");
    addPreset (menu, "source_voz");
    addPreset (menu, "source_violao");
    addPreset (menu, "source_master");

    return menu;
}

juce::PopupMenu GravityAudioProcessorEditor::buildUserPresetsMenu()
{
    juce::PopupMenu menu;
    userPresetFiles.clear();

    const auto presetDirectory = getUserPresetDirectory();
    presetDirectory.createDirectory();
    presetDirectory.findChildFiles (userPresetFiles, juce::File::findFiles, false, "*.xml");
    presetDirectory.findChildFiles (userPresetFiles, juce::File::findFiles, false, "*.preset");

    if (userPresetFiles.isEmpty())
    {
        menu.addItem (kMenuUserBase, "Nenhum preset salvo", false, false);
        return menu;
    }

    for (int i = 0; i < userPresetFiles.size(); ++i)
        menu.addItem (kMenuUserBase + i, userPresetFiles[i].getFileNameWithoutExtension());

    return menu;
}

void GravityAudioProcessorEditor::applyFactoryPreset (const juce::String& presetID)
{
    for (const auto& preset : kFactoryPresets)
    {
        if (presetID == preset.presetID)
        {
            audioProcessor.applyPresetValues (preset.thresholdDb,
                                              preset.ratio,
                                              preset.attackMs,
                                              preset.releaseMs,
                                              preset.gainDb,
                                              preset.mode,
                                              preset.arc);
            updateDynamicLabels();
            transferCurve.repaint();
            return;
        }
    }
}

void GravityAudioProcessorEditor::saveCurrentPresetAs()
{
    const auto presetDirectory = getUserPresetDirectory();
    presetDirectory.createDirectory();

    presetFileChooser = std::make_unique<juce::FileChooser> ("Salvar Preset Atual",
                                                             presetDirectory.getChildFile ("GRAVITY Preset.preset"),
                                                             "*.preset;*.xml");

    juce::Component::SafePointer<GravityAudioProcessorEditor> safeThis (this);
    presetFileChooser->launchAsync (juce::FileBrowserComponent::saveMode
                                      | juce::FileBrowserComponent::canSelectFiles
                                      | juce::FileBrowserComponent::warnAboutOverwriting,
                                    [safeThis] (const juce::FileChooser& chooser)
                                    {
                                        if (safeThis == nullptr)
                                            return;

                                        auto file = chooser.getResult();
                                        if (file == juce::File())
                                            return;

                                        const auto extension = file.getFileExtension();
                                        if (! extension.equalsIgnoreCase (".preset")
                                         && ! extension.equalsIgnoreCase (".xml"))
                                            file = file.withFileExtension (".preset");

                                        safeThis->audioProcessor.savePresetToFile (file);
                                    });
}

void GravityAudioProcessorEditor::loadUserPresetFile (const juce::File& file)
{
    if (audioProcessor.loadPresetFromFile (file))
    {
        updateDynamicLabels();
        transferCurve.repaint();
    }
}

juce::File GravityAudioProcessorEditor::getUserPresetDirectory() const
{
    return juce::File::getSpecialLocation (juce::File::userMusicDirectory)
        .getChildFile ("ASTRA Audio")
        .getChildFile ("GRAVITY")
        .getChildFile ("Presets");
}

// ==============================================================================
// paint
// ==============================================================================

void GravityAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Overall background: deep space
    {
        juce::ColourGradient bg (colors.backgroundTop,
                                 0.0f, 0.0f,
                                 colors.background,
                                 0.0f, static_cast<float> (getHeight()),
                                 false);
        g.setGradientFill (bg);
        g.fillAll();
    }

    paintDisplayZone  (g);
    paintTopBar       (g);   // Drawn over display so it overlaps cleanly
    paintControlPanel (g);
    paintFooter       (g);
}

// ==============================================================================
// paintTopBar
// ==============================================================================

void GravityAudioProcessorEditor::paintTopBar (juce::Graphics& g)
{
    const auto bounds = juce::Rectangle<float> (0.0f, 0.0f,
                                                static_cast<float> (getWidth()),
                                                static_cast<float> (kTopBarH));

    // Glass background
    {
        juce::ColourGradient topGrad (colors.topBarBg.withAlpha (0.88f),
                                      0.0f, 0.0f,
                                      colors.topBarBg.withAlpha (0.72f),
                                      0.0f, static_cast<float> (kTopBarH),
                                      false);
        g.setGradientFill (topGrad);
        g.fillRect (bounds);
    }

    // Bottom accent line — crimson pulse
    g.setColour (colors.accent.withAlpha (0.30f));
    g.drawHorizontalLine (kTopBarH - 1, 0.0f, static_cast<float> (getWidth()));

    // Very subtle top refraction
    g.setColour (juce::Colours::white.withAlpha (0.05f));
    g.drawHorizontalLine (0, 0.0f, static_cast<float> (getWidth()));

    // Company name — left
    g.setColour (colors.textSecondary.withAlpha (0.65f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 9.0f, juce::Font::plain)));
    g.drawText ("ASTRA AUDIO",
                juce::Rectangle<float> (20.0f, 0.0f, 130.0f, static_cast<float> (kTopBarH)),
                juce::Justification::centredLeft, false);

    // Plugin name — centred, bold
    g.setColour (colors.textPrimary);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 22.0f, juce::Font::bold)));
    g.drawText ("G R A V I T Y",
                juce::Rectangle<float> (0.0f, 0.0f,
                                        static_cast<float> (getWidth()),
                                        static_cast<float> (kTopBarH)),
                juce::Justification::centred, false);

    // Version badge — right
    g.setColour (colors.textMuted);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 8.5f, juce::Font::plain)));
    g.drawText ("v0.1.0",
                juce::Rectangle<float> (static_cast<float> (getWidth()) - 64.0f,
                                        0.0f, 54.0f,
                                        static_cast<float> (kTopBarH)),
                juce::Justification::centredRight, false);
}

// ==============================================================================
// paintDisplayZone — meter area with crimson pulse glow
// ==============================================================================

void GravityAudioProcessorEditor::paintDisplayZone (juce::Graphics& g)
{
    const float x  = 0.0f;
    const float y  = static_cast<float> (kTopBarH);
    const float w  = static_cast<float> (getWidth());
    const float h  = static_cast<float> (kDisplayH);
    const float cx = w * 0.5f;
    const float bottom = y + h;

    // Display background gradient
    {
        juce::ColourGradient bg (colors.backgroundTop,  cx, y,
                                 colors.backgroundDisplay, cx, bottom,
                                 false);
        g.setGradientFill (bg);
        g.fillRect (x, y, w, h);
    }

    // Central crimson radial glow
    {
        juce::ColourGradient glow (colors.accent.withAlpha (0.09f), cx, bottom,
                                   colors.accent.withAlpha (0.0f),  cx, y,
                                   true);
        g.setGradientFill (glow);
        g.fillEllipse (cx - 220.0f, y, 440.0f, h + 10.0f);
    }

    // Subtitle
    g.setColour (colors.textSecondary.withAlpha (0.22f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 9.0f, juce::Font::plain)));
    g.drawText ("DYNAMIC COMPRESSOR",
                juce::Rectangle<float> (x + 60.0f, y + h * 0.5f - 8.0f, w - 120.0f, 16.0f),
                juce::Justification::centred, false);

    // Bottom divider
    g.setColour (colors.accent.withAlpha (0.15f));
    g.drawHorizontalLine (static_cast<int> (bottom) - 1, x, x + w);

    g.setColour (colors.textMuted.withAlpha (0.45f));
    g.setFont (juce::Font (juce::FontOptions ("Inter", 8.0f, juce::Font::plain)));
    g.drawText ("WORKHORSE COMPRESSION VIEW",
                juce::Rectangle<float> (x + 60.0f, bottom - 24.0f, w - 120.0f, 14.0f),
                juce::Justification::centred, false);
}

// ==============================================================================
// paintControlPanel — glass background behind the controls
// ==============================================================================

void GravityAudioProcessorEditor::paintControlPanel (juce::Graphics& g)
{
    const float panelMarginX = 14.0f;
    const float panelY       = static_cast<float> (kControlPanelY) + 4.0f;
    const float panelW       = static_cast<float> (kEditorW) - 2.0f * panelMarginX;
    const float panelH       = static_cast<float> (kControlPanelH) - 8.0f;
    const float radius       = 12.0f;

    const juce::Rectangle<float> panel (panelMarginX, panelY, panelW, panelH);

    // Drop shadow
    g.setColour (juce::Colours::black.withAlpha (0.20f));
    g.fillRoundedRectangle (panel.translated (0.0f, 3.0f), radius);

    // Glass fill
    {
        juce::ColourGradient fill (
            juce::Colour (20, 30, 54).withAlpha (0.42f), panelMarginX, panelY,
            juce::Colour (12, 20, 40).withAlpha (0.24f), panelMarginX, panelY + panelH,
            false);
        g.setGradientFill (fill);
        g.fillRoundedRectangle (panel, radius);
    }

    // Top specular line
    g.setColour (juce::Colours::white.withAlpha (0.07f));
    g.drawHorizontalLine (static_cast<int> (panelY),
                          panelMarginX + radius, panelMarginX + panelW - radius);

    // Glass border
    g.setColour (colors.border);
    g.drawRoundedRectangle (panel, radius, 1.0f);

    // Subtle crimson bottom glow
    {
        juce::ColourGradient bottomGlow (
            colors.accent.withAlpha (0.0f),  panel.getCentreX(), panel.getCentreY(),
            colors.accent.withAlpha (0.05f), panel.getCentreX(), panel.getBottom(),
            false);
        g.setGradientFill (bottomGlow);
        g.fillRoundedRectangle (panel, radius);
    }
}

// ==============================================================================
// paintFooter
// ==============================================================================

void GravityAudioProcessorEditor::paintFooter (juce::Graphics& g)
{
    const juce::Rectangle<float> footer (
        0.0f, static_cast<float> (kFooterY),
        static_cast<float> (getWidth()),
        static_cast<float> (kFooterH));

    g.setColour (juce::Colour (5, 8, 16).withAlpha (0.97f));
    g.fillRect (footer);

    g.setColour (juce::Colours::white.withAlpha (0.04f));
    g.drawHorizontalLine (kFooterY, 0.0f, static_cast<float> (getWidth()));

    g.setColour (colors.textMuted);
    g.setFont (juce::Font (juce::FontOptions ("Inter", 9.0f, juce::Font::plain)));
    auto footerText = footer.reduced (0.0f, 6.0f);
    g.drawText ("ASTRA Audio Suite  \u2022  GRAVITY v0.1.0  \u2022  Dynamic Compressor",
                footerText.removeFromBottom (16.0f),
                juce::Justification::centred, false);
}

// ==============================================================================
// resized
// ==============================================================================

void GravityAudioProcessorEditor::resized()
{
    const int leftCenter   = columnCenterX (kLeftColumnX);
    const int centerCenter = columnCenterX (kCenterColumnX);
    const int rightCenter  = columnCenterX (kRightColumnX);

    const int threshX = leftCenter - kSliderW / 2;
    const int ratioX  = centerCenter - kSliderW / 2;
    const int gainX   = rightCenter - kSliderW / 2;

    const int labelY = kSliderY - kLabelH - 8;
    const int valueY = kSliderY + kSliderH + 6;
    const int labelW = 94;

    // ---- Column 1: Threshold with Input + GR meters --------------------------
    threshSlider.setBounds     (threshX, kSliderY, kSliderW, kSliderH);
    inputMeter.setBounds       (threshX - kMeterGap - kMeterW, kSliderY, kMeterW, kMeterH);
    grMeter.setBounds          (threshX + kSliderW + kMeterGap, kSliderY, kMeterW, kMeterH);
    threshLabel.setBounds      (leftCenter - labelW / 2, labelY, labelW, kLabelH);
    threshValueLabel.setBounds (leftCenter - labelW / 2, valueY, labelW, kValueH);

    // ---- Column 2: Transfer Curve + Ratio ------------------------------------
    transferCurve.setBounds  (kCurveX, kCurveY, kCurveW, kCurveH);
    ratioSlider.setBounds    (ratioX, kRatioSliderY, kSliderW, kRatioSliderH);
    ratioLabel.setBounds     (centerCenter - labelW / 2, kCurveY - kLabelH - 4, labelW, kLabelH);
    ratioValueLabel.setBounds (centerCenter - labelW / 2,
                               kRatioSliderY + kRatioSliderH + 5,
                               labelW,
                               kValueH);

    // ---- Column 3: Makeup Gain with Output meter -----------------------------
    gainSlider.setBounds     (gainX, kSliderY, kSliderW, kSliderH);
    outputMeter.setBounds    (gainX + kSliderW + kMeterGap, kSliderY, kMeterW, kMeterH);
    gainLabel.setBounds      (rightCenter - labelW / 2, labelY, labelW, kLabelH);
    gainValueLabel.setBounds (rightCenter - labelW / 2, valueY, labelW, kValueH);

    // ---- Footer: compact timing + mode controls ------------------------------
    attackLabel.setBounds       (24,  kFooterLabelY, 52, kLabelH);
    attackSlider.setBounds      (68,  kFooterControlY, kSmallSliderW, kSmallSliderH);
    attackValueLabel.setBounds  (230, kFooterLabelY, 74, kValueH);

    releaseLabel.setBounds      (320, kFooterLabelY, 52, kLabelH);
    releaseSlider.setBounds     (368, kFooterControlY, kSmallSliderW, kSmallSliderH);
    releaseValueLabel.setBounds (530, kFooterLabelY, 74, kValueH);

    modeButton.setBounds (616, kFooterControlY - 1, kButtonW, kButtonH);
    arcButton.setBounds  (704, kFooterControlY - 1, kButtonW, kButtonH);

    presetsButton.setBounds (632, 15, 86, 24);

    // ---- Meter labels ---------------------------------------------------------
    constexpr int kMeterLabelH = 14;
    const int meterLabelY = kSliderY - kMeterLabelH - 24;

    inputMeterLabel.setBounds  (threshX - kMeterGap - kMeterW - 2, meterLabelY, kMeterW + 4, kMeterLabelH);
    grMeterLabel.setBounds     (threshX + kSliderW + kMeterGap - 2, meterLabelY, kMeterW + 4, kMeterLabelH);
    outputMeterLabel.setBounds (gainX + kSliderW + kMeterGap - 2, meterLabelY, kMeterW + 4, kMeterLabelH);
}
