#include "EQGraphView.h"
#include <cmath>
#include <vector>

namespace AUREQ
{
    EQGraphView::EQGraphView()
    {
        // Set default theme colors (Dark mode)
        themeColors = Theme::getColors(ThemeMode::Dark);
    }

    EQGraphView::~EQGraphView()
    {
    }

    void EQGraphView::setThemeColors(const ThemeColors& colors)
    {
        themeColors = colors;
        repaint();
    }

    void EQGraphView::setLanguage (AureqLanguage language)
    {
        if (currentLanguage == language)
            return;

        currentLanguage = language;
        repaint();
    }

    void EQGraphView::setBands(const std::array<VisualEQBand, AUREQ::Params::numBands>& newBands)
    {
        bands = newBands;
        repaint();
    }

    void EQGraphView::setSpectrumData (const float* bins, int numBins)
    {
        if (numBins <= 0)
            return;

        // Reserve on first call to avoid repeated allocations.
        if ((int) spectrumBins.size() != numBins)
            spectrumBins.resize ((size_t) numBins, -90.0f);

        std::memcpy (spectrumBins.data(), bins, (size_t) numBins * sizeof (float));
        spectrumNumBins = numBins;
        repaint();
    }

    void EQGraphView::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds();
        
        // 1. Draw the main panel background with a subtle vertical gradient
        juce::ColourGradient bgGrad (themeColors.panel.brighter (0.02f), 0.0f, 0.0f,
                                     themeColors.panel.darker (0.04f), 0.0f, (float)bounds.getHeight(), false);
        g.setGradientFill (bgGrad);
        g.fillRoundedRectangle (bounds.toFloat(), 16.0f);
        
        // 2. Draw a subtle aurora/nebula glow inside the graph background
        bool isDark = (themeColors.backgroundTop.getBrightness() < 0.5f);
        juce::Colour glowColor = themeColors.accent.withAlpha (isDark ? 0.08f : 0.03f);
        juce::ColourGradient auroraGrad (glowColor, (float)bounds.getCentreX(), (float)bounds.getCentreY(),
                                         themeColors.accent.withAlpha (0.0f), (float)bounds.getCentreX(), (float)bounds.getBottom(), true);
        g.setGradientFill (auroraGrad);
        g.fillRoundedRectangle (bounds.toFloat(), 16.0f);

        // 3. Draw simulated inner shadow for recessed depth
        g.setColour (juce::Colours::black.withAlpha (0.08f));
        g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f), 16.0f, 1.0f);
        
        juce::ColourGradient innerShadow (juce::Colours::black.withAlpha (0.08f), 0.0f, 0.0f,
                                          juce::Colours::black.withAlpha (0.0f), 0.0f, 12.0f, false);
        g.setGradientFill (innerShadow);
        g.fillRect (bounds.getX(), bounds.getY(), bounds.getWidth(), 12);
        
        // Double-bezel inner highlight for liquid glass refraction
        g.setColour (juce::Colours::white.withAlpha (isDark ? 0.05f : 0.35f));
        g.drawRoundedRectangle (bounds.toFloat().reduced (1.5f), 15.0f, 1.0f);
        
        // 4. Draw highlight bezel border (translucent glass edge with top-down lighting)
        juce::ColourGradient borderGrad (juce::Colours::white.withAlpha (isDark ? 0.18f : 0.55f), 0.0f, 0.0f,
                                         juce::Colours::white.withAlpha (isDark ? 0.04f : 0.12f), 0.0f, (float)bounds.getHeight(), false);
        g.setGradientFill (borderGrad);
        g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f), 16.0f, 1.0f);

        // 4b. Draw top specular highlight path along the top edge (liquid glass reflection)
        g.setColour (juce::Colours::white.withAlpha (isDark ? 0.22f : 0.65f));
        juce::Path topHighlight;
        float r = 16.0f;
        topHighlight.startNewSubPath (0.0f, r);
        topHighlight.addCentredArc (r, r, r, r, 0.0f, -juce::MathConstants<float>::halfPi, 0.0f, true);
        topHighlight.lineTo ((float)bounds.getWidth() - r, 0.0f);
        topHighlight.addCentredArc ((float)bounds.getWidth() - r, r, r, r, 0.0f, 0.0f, juce::MathConstants<float>::halfPi, true);
        g.strokePath (topHighlight, juce::PathStrokeType (1.0f));

        // Define plotting area inside the component margins
        auto plotBounds = bounds.reduced(35, 20);

        // 5. Draw Frequency Grid Lines and Labels (Vertical)
        struct FreqLabel
        {
            float hz;
            juce::String text;
        };
        
        std::vector<FreqLabel> freqs = {
            { 20.0f, "20" },
            { 50.0f, "50" },
            { 100.0f, "100" },
            { 200.0f, "200" },
            { 500.0f, "500" },
            { 1000.0f, "1k" },
            { 2000.0f, "2k" },
            { 5000.0f, "5k" },
            { 10000.0f, "10k" },
            { 20000.0f, "20k" }
        };

        g.setFont (juce::Font (juce::FontOptions ("Inter", 8.5f, juce::Font::plain)));

        for (const auto& f : freqs)
        {
            float x = getNormalizedX(f.hz);
            
            // Draw vertical grid line (extremely thin and elegant)
            g.setColour(themeColors.gridLine.withAlpha(0.04f));
            g.drawLine(x, (float)plotBounds.getY(), x, (float)plotBounds.getBottom(), 0.50f);

            // Draw frequency label text centered under the grid line
            g.setColour(themeColors.textSecondary.withAlpha(0.55f));
            juce::Rectangle<int> textRect(juce::roundToInt(x) - 20, plotBounds.getBottom() + 4, 40, 12);
            g.drawText(f.text, textRect, juce::Justification::centred, true);
        }

        // 6. Draw Gain Grid Lines and Labels (Horizontal)
        struct GainLabel
        {
            float db;
            juce::String text;
        };

        std::vector<GainLabel> gains = {
            { 24.0f, "+24" },
            { 12.0f, "+12" },
            { 0.0f, "0" },
            { -12.0f, "-12" },
            { -24.0f, "-24" }
        };

        for (const auto& gain : gains)
        {
            float y = getNormalizedY(gain.db);

            // Draw horizontal grid line
            if (gain.db == 0.0f)
            {
                // Muted 0 dB line (single elegant accent line instead of harsh laser)
                g.setColour(themeColors.accent.withAlpha(0.24f));
                g.drawLine((float)plotBounds.getX(), y, (float)plotBounds.getRight(), y, 1.0f);
            }
            else
            {
                g.setColour(themeColors.gridLine.withAlpha(0.04f));
                g.drawLine((float)plotBounds.getX(), y, (float)plotBounds.getRight(), y, 0.50f);
            }

            // Draw gain text on left and right borders of the plot area
            g.setColour(themeColors.textSecondary.withAlpha(0.55f));
            
            // Left side dB label
            juce::Rectangle<int> leftTextRect(2, juce::roundToInt(y) - 6, plotBounds.getX() - 4, 12);
            g.drawText(gain.text, leftTextRect, juce::Justification::centredRight, true);

            // Right side dB label
            juce::Rectangle<int> rightTextRect(plotBounds.getRight() + 2, juce::roundToInt(y) - 6, bounds.getWidth() - plotBounds.getRight() - 4, 12);
            g.drawText(gain.text, rightTextRect, juce::Justification::centredLeft, true);
        }

        // 7. Draw real-time spectrum analyzer BEFORE the EQ curve
        // so the curve stays on top and remains legible.
        if (spectrumNumBins > 1 && plotBounds.getWidth() > 0)
        {
            // The analyzer provides bins 0..NUM_BINS-1 corresponding to
            // frequencies 0 Hz (DC) to sampleRate/2 (Nyquist).
            // We use a fixed reference sample rate of 44100 Hz for bin-to-frequency
            // mapping (the visual difference at 48kHz is negligible for display).
            constexpr float REF_SAMPLE_RATE = 44100.0f;
            float nyquistHz = REF_SAMPLE_RATE * 0.5f;
            int numBins = spectrumNumBins;

            // dB range for the analyzer display (maps to the same gain axis as the EQ curve)
            constexpr float SPEC_MIN_DB = -90.0f;  // silence floor (maps to bottom)
            constexpr float SPEC_MAX_DB =   0.0f;  // 0 dBFS (maps to top of plot area)

            juce::Path spectrumPath;
            juce::Path spectrumTopPath;
            bool specFirstPoint = true;
            float specFloorY = (float) plotBounds.getBottom();

            for (int px = plotBounds.getX(); px <= plotBounds.getRight(); ++px)
            {
                // Convert pixel to frequency (logarithmic)
                float freq = getFrequencyForX ((float) px);

                // Map frequency to a fractional bin index
                float binF = freq / nyquistHz * (float) (numBins - 1);
                int   binL = juce::jlimit (0, numBins - 2, (int) binF);
                int   binR = binL + 1;
                float t    = binF - (float) binL;

                // Linear interpolation between adjacent bins
                float dbVal = spectrumBins[binL] * (1.0f - t) + spectrumBins[binR] * t;
                dbVal = juce::jlimit (SPEC_MIN_DB, SPEC_MAX_DB, dbVal);

                // Map dB value to pixel Y (SPEC_MAX_DB → top, SPEC_MIN_DB → bottom)
                float normY = (dbVal - SPEC_MAX_DB) / (SPEC_MIN_DB - SPEC_MAX_DB);
                float specY = (float) plotBounds.getY() + normY * (float) plotBounds.getHeight();

                if (specFirstPoint)
                {
                    spectrumPath.startNewSubPath ((float) px, specY);
                    spectrumTopPath.startNewSubPath ((float) px, specY);
                    specFirstPoint = false;
                }
                else
                {
                    spectrumPath.lineTo ((float) px, specY);
                    spectrumTopPath.lineTo ((float) px, specY);
                }
            }

            // Close the path at the bottom to create a filled shape
            spectrumPath.lineTo ((float) plotBounds.getRight(), specFloorY);
            spectrumPath.lineTo ((float) plotBounds.getX(),     specFloorY);
            spectrumPath.closeSubPath();

            // Fill with a vertical cyan/mint glass gradient: visible enough to
            // read in DAWs, but still behind the EQ curve and handles.
            const auto mint = juce::Colour (45, 212, 191);
            juce::Colour specTopColor    = themeColors.accent.interpolatedWith (mint, 0.35f)
                                               .withAlpha (isDark ? 0.26f : 0.14f);
            juce::Colour specBottomColor = themeColors.accent.withAlpha (0.0f);
            juce::ColourGradient specGrad (specTopColor,    (float) plotBounds.getX(), (float) plotBounds.getY(),
                                           specBottomColor, (float) plotBounds.getX(), (float) plotBounds.getBottom(),
                                           false);
            g.setGradientFill (specGrad);
            g.fillPath (spectrumPath);

            // Draw only the top edge. This keeps the spectrum readable without
            // outlining the floor of the filled shape.
            g.setColour (themeColors.accent.withAlpha (isDark ? 0.16f : 0.08f));
            g.strokePath (spectrumTopPath, juce::PathStrokeType (3.0f,
                                                                  juce::PathStrokeType::curved,
                                                                  juce::PathStrokeType::rounded));

            g.setColour (themeColors.accent.interpolatedWith (mint, 0.25f)
                              .withAlpha (isDark ? 0.58f : 0.32f));
            g.strokePath (spectrumTopPath, juce::PathStrokeType (1.05f,
                                                                  juce::PathStrokeType::curved,
                                                                  juce::PathStrokeType::rounded));
        }

        // 7b. Draw combined theoretical EQ Curve (on top of spectrum)
        if (plotBounds.getWidth() > 0)
        {
            juce::Path curvePath;
            bool firstPoint = true;

            for (int x = plotBounds.getX(); x <= plotBounds.getRight(); ++x)
            {
                float freq = getFrequencyForX((float)x);
                float totalDb = 0.0f;

                for (const auto& band : bands)
                {
                    if (!band.enabled || band.bypassed)
                        continue;

                    const float f0     = std::max(20.0f, band.frequencyHz);
                    const float gainDb = band.gainDb;
                    const float q      = std::max(0.01f, band.q);

                    switch (band.type)
                    {
                        case 0: // Bell
                        {
                            float distOctaves = std::log2(freq / f0);
                            float shape = 1.0f / (1.0f + (distOctaves * q) * (distOctaves * q));
                            totalDb += gainDb * shape;
                            break;
                        }
                        case 1: // Low Cut (High-Pass)
                        {
                            float ratio = freq / f0;
                            float magSq = 1.0f;
                            
                            if (band.slope == 6)
                            {
                                magSq = (ratio * ratio) / (1.0f + ratio * ratio);
                            }
                            else if (band.slope == 12)
                            {
                                float denom = ratio * ratio * ratio * ratio + (1.0f / (q * q) - 2.0f) * ratio * ratio + 1.0f;
                                magSq = (ratio * ratio * ratio * ratio) / std::max(1e-12f, denom);
                            }
                            else if (band.slope == 18)
                            {
                                float magSq1 = (ratio * ratio) / (1.0f + ratio * ratio);
                                float denom2 = ratio * ratio * ratio * ratio + (1.0f / (q * q) - 2.0f) * ratio * ratio + 1.0f;
                                float magSq2 = (ratio * ratio * ratio * ratio) / std::max(1e-12f, denom2);
                                magSq = magSq1 * magSq2;
                            }
                            else // 24
                            {
                                float denom = ratio * ratio * ratio * ratio + (1.0f / (q * q) - 2.0f) * ratio * ratio + 1.0f;
                                float singleMagSq = (ratio * ratio * ratio * ratio) / std::max(1e-12f, denom);
                                magSq = singleMagSq * singleMagSq;
                            }
                            
                            float attenuationDb = 10.0f * std::log10(std::max(1e-12f, magSq));
                            totalDb += juce::jlimit(-48.0f, 0.0f, attenuationDb);
                            break;
                        }
                        case 2: // High Cut (Low-Pass)
                        {
                            float ratio = f0 / freq;
                            float magSq = 1.0f;
                            
                            if (band.slope == 6)
                            {
                                magSq = 1.0f / (1.0f + ratio * ratio);
                            }
                            else if (band.slope == 12)
                            {
                                float denom = ratio * ratio * ratio * ratio + (1.0f / (q * q) - 2.0f) * ratio * ratio + 1.0f;
                                magSq = 1.0f / std::max(1e-12f, denom);
                            }
                            else if (band.slope == 18)
                            {
                                float magSq1 = 1.0f / (1.0f + ratio * ratio);
                                float denom2 = ratio * ratio * ratio * ratio + (1.0f / (q * q) - 2.0f) * ratio * ratio + 1.0f;
                                float magSq2 = 1.0f / std::max(1e-12f, denom2);
                                magSq = magSq1 * magSq2;
                            }
                            else // 24
                            {
                                float denom = ratio * ratio * ratio * ratio + (1.0f / (q * q) - 2.0f) * ratio * ratio + 1.0f;
                                float singleMagSq = 1.0f / std::max(1e-12f, denom);
                                magSq = singleMagSq * singleMagSq;
                            }
                            
                            float attenuationDb = 10.0f * std::log10(std::max(1e-12f, magSq));
                            totalDb += juce::jlimit(-48.0f, 0.0f, attenuationDb);
                            break;
                        }
                        case 3: // Low Shelf
                        {
                            float steepness = 2.50f;
                            if (band.slope == 6) steepness = 1.25f;
                            else if (band.slope == 18) steepness = 3.75f;
                            else if (band.slope == 24) steepness = 5.00f;
                            
                            float t = 1.0f / (1.0f + std::exp(steepness * std::log2(freq / f0)));
                            totalDb += gainDb * t;
                            break;
                        }
                        case 4: // High Shelf
                        {
                            float steepness = 2.50f;
                            if (band.slope == 6) steepness = 1.25f;
                            else if (band.slope == 18) steepness = 3.75f;
                            else if (band.slope == 24) steepness = 5.00f;
                            
                            float t = 1.0f / (1.0f + std::exp(-steepness * std::log2(freq / f0)));
                            totalDb += gainDb * t;
                            break;
                        }
                        case 5: // Notch — narrow dip centred at f0
                        {
                            float distOctaves = std::log2(freq / f0);
                            float shape = 1.0f / (1.0f + (distOctaves * q) * (distOctaves * q));
                            // A notch is an inverted Bell with fixed -24 dB depth
                            totalDb += -24.0f * shape;
                            break;
                        }
                        case 6: // Band Pass — narrow peak centred at f0
                        {
                            float distOctaves = std::log2(freq / f0);
                            float shape = 1.0f / (1.0f + (distOctaves * q) * (distOctaves * q));
                            // Visualise as a positive Bell of moderate height
                            totalDb += 12.0f * shape;
                            break;
                        }
                        default:
                            break;
                    }
                }

                totalDb = juce::jlimit(-24.0f, 24.0f, totalDb);
                float y = getNormalizedY(totalDb);

                if (firstPoint)
                {
                    curvePath.startNewSubPath((float)x, y);
                    firstPoint = false;
                }
                else
                {
                    curvePath.lineTo((float)x, y);
                }
            }

            // Draw premium optical glow for the curve (triple layer for soft bloom)
            g.setColour(themeColors.accent.withAlpha(0.02f));
            g.strokePath(curvePath, juce::PathStrokeType(24.0f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));

            g.setColour(themeColors.accent.withAlpha(0.06f));
            g.strokePath(curvePath, juce::PathStrokeType(10.0f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));

            g.setColour(themeColors.accent.withAlpha(0.15f));
            g.strokePath(curvePath, juce::PathStrokeType(4.0f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));

            // Draw the main curve line
            g.setColour(themeColors.accent.brighter(0.05f)); // Luminous core
            g.strokePath(curvePath, juce::PathStrokeType(1.8f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));
        }

        // 8. Draw active band handles
        for (size_t i = 0; i < bands.size(); ++i)
        {
            const auto& band = bands[i];
            
            if (!band.enabled)
                continue;
            
            float x = getNormalizedX(band.frequencyHz);
            float y = getNormalizedY(band.gainDb);
            
            float outerRingRadius = 11.0f;
            float innerCircleRadius = 8.0f;
            float glowRadius = 15.0f;
            
            bool isBeingDragged = (activeBandDragIndex == static_cast<int>(i));
            if (isBeingDragged)
            {
                outerRingRadius = 13.0f;
                innerCircleRadius = 9.0f;
                glowRadius = 19.0f;
            }
            
            // Draw dynamic EQ vertical range indicator if supported and active
            bool supportsDyn = (band.type == 0 || band.type == 3 || band.type == 4);
            if (band.dynamicEnabled && supportsDyn && !band.bypassed)
            {
                if (std::abs (band.dynamicRange) > 0.05f)
                {
                    float yDest = getNormalizedY (band.gainDb + band.dynamicRange);
                    g.setColour (themeColors.accent.withAlpha (isDark ? 0.22f : 0.12f));
                    
                    juce::Path rangeLine;
                    rangeLine.startNewSubPath (x, y);
                    rangeLine.lineTo (x, yDest);
                    
                    juce::Path dashedRangeLine;
                    float dashedLengths[] = { 3.0f, 3.0f };
                    juce::PathStrokeType (1.0f).createDashedStroke (dashedRangeLine, rangeLine, dashedLengths, 2);
                    g.fillPath (dashedRangeLine);
                    
                    // Draw range limit dot
                    g.setColour (themeColors.accent.withAlpha (0.4f));
                    g.fillEllipse (x - 2.0f, yDest - 2.0f, 4.0f, 4.0f);
                    
                    // Draw current dynamic gain modulation on top of the dashed line
                    float currentMod = band.dynamicGainDb;
                    if (std::abs (currentMod) > 0.02f)
                    {
                        float yValMod = getNormalizedY (band.gainDb + currentMod);
                        juce::Colour meterColor = (currentMod > 0.0f) ? themeColors.accent : themeColors.accentSoft; // Cyan for boost, Mint/Teal for cut
                        g.setColour (meterColor.withAlpha (isDark ? 0.85f : 0.65f));
                        g.drawLine (x, y, x, yValMod, 2.0f);
                        
                        // Small glowing dot for current gain value
                        g.setColour (meterColor);
                        g.fillEllipse (x - 3.0f, yValMod - 3.0f, 6.0f, 6.0f);
                    }
                }
            }

            // Draw visual focus ring and bandwidth aura for the selected band
            bool isSelected = (selectedBandIndex == static_cast<int>(i));
            if (isSelected)
            {
                bool usesQ = (band.type != 3 && band.type != 4);
                if (!band.bypassed && usesQ)
                {
                    float bandwidthRadius = juce::jlimit(12.0f, 100.0f, 35.0f / std::sqrt(band.q));
                    
                    // Draw filled Q-aura (extremely transparent)
                    juce::Colour auraColor = themeColors.accent.withAlpha(isDark ? 0.03f : 0.015f);
                    g.setColour(auraColor);
                    g.fillEllipse(x - bandwidthRadius, y - bandwidthRadius, bandwidthRadius * 2.0f, bandwidthRadius * 2.0f);
                    
                    // Draw dotted border for Q-aura
                    g.setColour(themeColors.accent.withAlpha(isDark ? 0.10f : 0.05f));
                    juce::Path borderPath;
                    borderPath.addCentredArc(x, y, bandwidthRadius, bandwidthRadius, 0.0f, 0.0f, 2.0f * 3.14159265f, true);
                    
                    juce::Path dashedPath;
                    float dashedLengths[] = { 4.0f, 4.0f };
                    juce::PathStrokeType(0.75f).createDashedStroke(dashedPath, borderPath, dashedLengths, 2);
                    g.fillPath(dashedPath);
                }
                
                // Draw glowing halo around selected handle
                g.setColour(themeColors.accent.withAlpha(isDark ? 0.25f : 0.12f));
                float focusRingRadius = outerRingRadius + 4.0f;
                g.drawEllipse(x - focusRingRadius, y - focusRingRadius, focusRingRadius * 2.0f, focusRingRadius * 2.0f, 1.0f);
            }
            
            // Draw soft drop shadow under the handle
            g.setColour(juce::Colours::black.withAlpha(isDark ? (isBeingDragged ? 0.40f : 0.30f) : 0.08f));
            g.fillEllipse(x - innerCircleRadius, y - innerCircleRadius + 2.0f, innerCircleRadius * 2.0f, innerCircleRadius * 2.0f);
            
            if (!band.bypassed)
            {
                // Glow active
                float glowAlpha = isDark ? (isBeingDragged ? 0.35f : 0.18f) : (isBeingDragged ? 0.12f : 0.06f);
                juce::Colour glowColour = themeColors.accent.withAlpha(glowAlpha);
                g.setColour(glowColour);
                g.fillEllipse(x - glowRadius, y - glowRadius, glowRadius * 2.0f, glowRadius * 2.0f);
                
                // Translucent outer ring (aura)
                float outerRingAlpha = isDark ? (isBeingDragged ? 0.18f : 0.09f) : (isBeingDragged ? 0.08f : 0.04f);
                juce::Colour outerRingColour = themeColors.accent.withAlpha(outerRingAlpha);
                g.setColour(outerRingColour);
                g.fillEllipse(x - outerRingRadius, y - outerRingRadius, outerRingRadius * 2.0f, outerRingRadius * 2.0f);
                
                // Subtle border for outer ring
                g.setColour(themeColors.accent.withAlpha(isDark ? (isBeingDragged ? 0.40f : 0.25f) : 0.10f));
                g.drawEllipse(x - outerRingRadius, y - outerRingRadius, outerRingRadius * 2.0f, outerRingRadius * 2.0f, 0.75f);
                
                // Solid inner circle with vertical gradient (gives a 3D acrylic look)
                juce::ColourGradient handleGrad (themeColors.accent.brighter(0.04f), x, y - innerCircleRadius,
                                                 themeColors.accent.darker(0.12f), x, y + innerCircleRadius, false);
                g.setGradientFill (handleGrad);
                g.fillEllipse(x - innerCircleRadius, y - innerCircleRadius, innerCircleRadius * 2.0f, innerCircleRadius * 2.0f);
                
                // Specular highlight: tiny white reflection dot at top-left
                g.setColour (juce::Colours::white.withAlpha (0.50f));
                g.fillEllipse (x - 3.5f, y - 3.5f, 2.5f, 2.5f);
                
                // Top crescent highlight line for 3D glassy edge
                g.setColour(juce::Colours::white.withAlpha(isDark ? 0.35f : 0.55f));
                g.drawEllipse(x - innerCircleRadius, y - innerCircleRadius, innerCircleRadius * 2.0f, innerCircleRadius * 2.0f, 0.75f);
                
                // Band number text (centered, elegant)
                juce::String numText = juce::String(i + 1);
                g.setFont(juce::Font(juce::FontOptions("Inter", isBeingDragged ? 9.5f : 9.0f, juce::Font::plain)));
                
                juce::Colour textColour = isDark ? juce::Colour(6, 10, 15) : juce::Colours::white;
                g.setColour(textColour);
                
                juce::Rectangle<float> textBounds(x - innerCircleRadius, y - innerCircleRadius - 0.5f, innerCircleRadius * 2.0f, innerCircleRadius * 2.0f);
                g.drawText(numText, textBounds.toNearestInt(), juce::Justification::centred, false);
                
                // Draw "DYN" badge above handle if active, supported and enabled
                if (band.dynamicEnabled && supportsDyn)
                {
                    float pillW = 20.0f;
                    float pillH = 10.0f;
                    juce::Rectangle<float> dynBadge (x - pillW * 0.5f, y - outerRingRadius - 12.0f, pillW, pillH);
                    
                    g.setColour (themeColors.accent.withAlpha (isDark ? 0.15f : 0.08f));
                    g.fillRoundedRectangle (dynBadge, 3.0f);
                    
                    g.setColour (themeColors.accent.withAlpha (isDark ? 0.40f : 0.25f));
                    g.drawRoundedRectangle (dynBadge, 3.0f, 0.5f);
                    
                    g.setFont (juce::Font (juce::FontOptions ("Inter", 6.5f, juce::Font::bold)));
                    g.setColour (themeColors.accent);
                    g.drawText ("DYN", dynBadge.toNearestInt(), juce::Justification::centred, true);
                }

                // Draw micro badge for non-ST channel modes
                if (band.channelMode != 0) // 1 = Mid, 2 = Side, 3 = Left, 4 = Right
                {
                    float badgeRadius = 4.5f;
                    float badgeX = x + 7.5f;
                    float badgeY = y + 7.5f;
                    
                    // Draw shadow
                    g.setColour (juce::Colours::black.withAlpha (0.4f));
                    g.fillEllipse (badgeX - badgeRadius, badgeY - badgeRadius + 1.0f, badgeRadius * 2.0f, badgeRadius * 2.0f);
                    
                    // Draw badge background
                    g.setColour (themeColors.accent);
                    g.fillEllipse (badgeX - badgeRadius, badgeY - badgeRadius, badgeRadius * 2.0f, badgeRadius * 2.0f);
                    
                    // Draw outline
                    g.setColour (juce::Colours::white.withAlpha (0.4f));
                    g.drawEllipse (badgeX - badgeRadius, badgeY - badgeRadius, badgeRadius * 2.0f, badgeRadius * 2.0f, 0.5f);
                    
                    // Draw letter
                    juce::String badgeText;
                    if (band.channelMode == 1) badgeText = "M";
                    else if (band.channelMode == 2) badgeText = "S";
                    else if (band.channelMode == 3) badgeText = "L";
                    else if (band.channelMode == 4) badgeText = "R";
                    
                    g.setColour (juce::Colour (6, 10, 15));
                    g.setFont (juce::Font (juce::FontOptions ("Inter", 6.5f, juce::Font::bold)));
                    juce::Rectangle<float> badgeBounds (badgeX - badgeRadius, badgeY - badgeRadius - 0.5f, badgeRadius * 2.0f, badgeRadius * 2.0f);
                    g.drawText (badgeText, badgeBounds.toNearestInt(), juce::Justification::centred, false);
                }
            }
            else
            {
                // Bypassed Band State (low opacity, no glow, secondary/muted colors)
                g.setColour(themeColors.textSecondary.withAlpha(0.04f));
                g.fillEllipse(x - outerRingRadius, y - outerRingRadius, outerRingRadius * 2.0f, outerRingRadius * 2.0f);
                g.setColour(themeColors.textSecondary.withAlpha(0.08f));
                g.drawEllipse(x - outerRingRadius, y - outerRingRadius, outerRingRadius * 2.0f, outerRingRadius * 2.0f, 0.75f);
                
                g.setColour(themeColors.accent.withAlpha(0.15f));
                g.fillEllipse(x - innerCircleRadius, y - innerCircleRadius, innerCircleRadius * 2.0f, innerCircleRadius * 2.0f);
                
                g.setColour(themeColors.textSecondary.withAlpha(0.12f));
                g.drawEllipse(x - innerCircleRadius, y - innerCircleRadius, innerCircleRadius * 2.0f, innerCircleRadius * 2.0f, 0.75f);
                
                juce::String numText = juce::String(i + 1);
                g.setFont(juce::Font(juce::FontOptions("Inter", 9.0f, juce::Font::plain)));
                g.setColour(themeColors.textSecondary.withAlpha(0.40f));
                
                juce::Rectangle<float> textBounds(x - innerCircleRadius, y - innerCircleRadius - 0.5f, innerCircleRadius * 2.0f, innerCircleRadius * 2.0f);
                g.drawText(numText, textBounds.toNearestInt(), juce::Justification::centred, false);
            }
        }

        // Draw visual floating band panel for the selected band
        if (selectedBandIndex != -1)
        {
            const auto& band = bands[selectedBandIndex];
            
            if (band.enabled)
            {
                float handleX = getNormalizedX(band.frequencyHz);
                float handleY = getNormalizedY(band.gainDb);
                
                // Larger HUD panel for breathing room
                float panelW = 160.0f;
                float panelH = 92.0f;
                
                float panelX = handleX - panelW * 0.5f;
                float panelY = handleY - panelH - 24.0f;
                
                auto bounds = getLocalBounds();
                float margin = 12.0f;
                
                if (panelY < margin)
                {
                    panelY = handleY + 24.0f;
                }
                
                panelX = juce::jlimit(margin, (float)bounds.getWidth() - panelW - margin, panelX);
                panelY = juce::jlimit(margin, (float)bounds.getHeight() - panelH - margin, panelY);
                
                juce::Rectangle<float> panelRect(panelX, panelY, panelW, panelH);
                
                // Draw soft drop shadow under HUD panel (translated downward)
                g.saveState();
                auto hudShadowRect = panelRect.translated (0.0f, 2.5f);
                for (int i = 1; i <= 6; ++i)
                {
                    float progress = (float)i / 6.0f;
                    float alpha = (isDark ? 0.35f : 0.08f) * std::pow (1.0f - progress, 2.0f);
                    g.setColour (juce::Colours::black.withAlpha (alpha));
                    float expansion = (float)i * 0.8f;
                    g.drawRoundedRectangle (hudShadowRect.expanded (expansion), 10.0f + expansion, 1.0f);
                }
                g.restoreState();
                
                // Draw Frosted Glass background
                juce::Colour baseColor = themeColors.panelElevated;
                juce::ColourGradient bgGrad (baseColor.brighter (0.02f), panelRect.getX(), panelRect.getY(),
                                             baseColor.darker (0.03f), panelRect.getX(), panelRect.getBottom(), false);
                g.setGradientFill (bgGrad);
                g.fillRoundedRectangle(panelRect, 10.0f);
                
                // Glass border (gradient)
                juce::ColourGradient hudBorderGrad (juce::Colours::white.withAlpha (isDark ? 0.18f : 0.50f), panelRect.getX(), panelRect.getY(),
                                                 juce::Colours::white.withAlpha (isDark ? 0.04f : 0.15f), panelRect.getX(), panelRect.getBottom(), false);
                g.setGradientFill (hudBorderGrad);
                g.drawRoundedRectangle(panelRect, 10.0f, 1.0f);
                
                // Top highlight line for HUD panel gloss
                g.setColour (juce::Colours::white.withAlpha (isDark ? 0.20f : 0.60f));
                juce::Path hudTopHighlight;
                float hr = 10.0f;
                hudTopHighlight.startNewSubPath (panelRect.getX(), panelRect.getY() + hr);
                hudTopHighlight.addCentredArc (panelRect.getX() + hr, panelRect.getY() + hr, hr, hr, 0.0f, -juce::MathConstants<float>::halfPi, 0.0f, true);
                hudTopHighlight.lineTo (panelRect.getRight() - hr, panelRect.getY());
                hudTopHighlight.addCentredArc (panelRect.getRight() - hr, panelRect.getY() + hr, hr, hr, 0.0f, 0.0f, juce::MathConstants<float>::halfPi, true);
                g.strokePath (hudTopHighlight, juce::PathStrokeType (1.0f));
                
                // Double-bezel inner highlight
                g.setColour (juce::Colours::white.withAlpha (isDark ? 0.04f : 0.30f));
                g.drawRoundedRectangle (panelRect.reduced(1.0f), 9.0f, 1.0f);
                
                // Content Padding
                float contentPaddingX = 12.0f;
                float contentPaddingY = 10.0f;
                
                // 1. Header: "BAND N", Channel Mode, and Status
                g.setColour(themeColors.textPrimary);
                g.setFont (juce::Font (juce::FontOptions ("Inter", 10.5f, juce::Font::bold)));
                g.drawText(getText (TextKey::Band, currentLanguage) + " " + juce::String(selectedBandIndex + 1), 
                           juce::roundToInt(panelRect.getX() + contentPaddingX), 
                           juce::roundToInt(panelRect.getY() + contentPaddingY), 
                           50, 12, juce::Justification::left, true);

                // Draw channel label and pill in the middle
                g.setColour (themeColors.textSecondary.withAlpha (0.6f));
                g.setFont (juce::Font (juce::FontOptions ("Inter", 8.0f, juce::Font::bold)));
                g.drawText (getText (TextKey::ChannelShort, currentLanguage), 
                            juce::roundToInt (panelRect.getX() + 62.0f), 
                            juce::roundToInt (panelRect.getY() + contentPaddingY), 
                            18, 12, juce::Justification::centredLeft, true);

                juce::String chStr;
                switch (band.channelMode)
                {
                    case 1:  chStr = "M"; break;
                    case 2:  chStr = "S"; break;
                    case 3:  chStr = "L"; break;
                    case 4:  chStr = "R"; break;
                    default: chStr = "ST"; break;
                }
                
                float chPillW = 20.0f;
                float chPillH = 14.0f;
                juce::Rectangle<float> chPill (panelRect.getX() + 81.0f, panelRect.getY() + contentPaddingY - 1.0f, chPillW, chPillH);
                
                g.setColour (themeColors.accent.withAlpha (0.08f));
                g.fillRoundedRectangle (chPill, 7.0f);
                g.setColour (themeColors.accent.withAlpha (0.28f));
                g.drawRoundedRectangle (chPill, 7.0f, 0.75f);
                
                g.setFont (juce::Font (juce::FontOptions ("Inter", 7.0f, juce::Font::bold)));
                g.setColour (themeColors.accent);
                g.drawText (chStr, chPill.toNearestInt(), juce::Justification::centred, true);
                
                // Status Pill / Badge
                juce::String statusText = band.bypassed ? getText (TextKey::Bypass, currentLanguage).toUpperCase()
                                                        : getText (TextKey::Active, currentLanguage);
                juce::Colour statusColor = band.bypassed ? themeColors.textSecondary.withAlpha(0.6f) : themeColors.accent;
                
                // Draw sleek capsule pill for status
                float pillW = 44.0f;
                float pillH = 14.0f;
                juce::Rectangle<float> statusPill (panelRect.getRight() - contentPaddingX - pillW, panelRect.getY() + contentPaddingY - 1.0f, pillW, pillH);
                
                g.setColour (statusColor.withAlpha (0.08f));
                g.fillRoundedRectangle (statusPill, 7.0f);
                g.setColour (statusColor.withAlpha (0.28f));
                g.drawRoundedRectangle (statusPill, 7.0f, 0.75f);
                
                // Glowing dot inside status pill
                float dotS = 4.0f;
                g.setColour (statusColor.withAlpha (0.50f));
                g.fillEllipse (statusPill.getX() + 6.0f - 1.0f, statusPill.getCentreY() - dotS*0.5f - 1.0f, dotS + 2.0f, dotS + 2.0f);
                g.setColour (statusColor);
                g.fillEllipse (statusPill.getX() + 6.0f, statusPill.getCentreY() - dotS*0.5f, dotS, dotS);
                
                // Status pill text
                g.setFont (juce::Font (juce::FontOptions ("Inter", 7.0f, juce::Font::bold)));
                g.drawText (statusText, juce::roundToInt(statusPill.getX() + 10.0f), juce::roundToInt(statusPill.getY()), juce::roundToInt(statusPill.getWidth() - 10.0f), juce::roundToInt(statusPill.getHeight()), juce::Justification::centred, true);
                
                // Divider line
                g.setColour(themeColors.border.withAlpha(isDark ? 0.12f : 0.25f));
                g.drawHorizontalLine(juce::roundToInt(panelRect.getY() + 25.0f), panelRect.getX() + 4.0f, panelRect.getRight() - 4.0f);
                
                // 2. Frequency & Gain Display
                float labelY = panelRect.getY() + 29.0f;
                
                juce::String freqStr;
                if (band.frequencyHz < 1000.0f)
                    freqStr = juce::String(juce::roundToInt(band.frequencyHz)) + " Hz";
                else
                    freqStr = juce::String(band.frequencyHz / 1000.0f, 2) + " kHz";
                
                juce::String gainStr;
                if (band.gainDb >= 0.0f)
                    gainStr = "+" + juce::String(band.gainDb, 1) + " dB";
                else
                    gainStr = juce::String(band.gainDb, 1) + " dB";
                
                juce::String qStr = "Q: " + juce::String(band.q, 2);
                
                // Labels
                g.setColour(themeColors.textSecondary.withAlpha(0.65f));
                g.setFont (juce::Font (juce::FontOptions ("Inter", 7.5f, juce::Font::plain)));
                g.drawText(getText (TextKey::Freq, currentLanguage), juce::roundToInt(panelRect.getX() + contentPaddingX), juce::roundToInt(labelY), 50, 10, juce::Justification::left, true);
                g.drawText(getText (TextKey::Gain, currentLanguage), juce::roundToInt(panelRect.getX() + contentPaddingX + 66.0f), juce::roundToInt(labelY), 50, 10, juce::Justification::left, true);
                
                // Values (clean monospaced layout)
                g.setColour(themeColors.textPrimary);
                g.setFont (juce::Font (juce::FontOptions ("Roboto Mono", 10.0f, juce::Font::plain)));
                g.drawText(freqStr, juce::roundToInt(panelRect.getX() + contentPaddingX), juce::roundToInt(labelY + 11.0f), 60, 12, juce::Justification::left, true);
                g.drawText(gainStr, juce::roundToInt(panelRect.getX() + contentPaddingX + 66.0f), juce::roundToInt(labelY + 11.0f), 60, 12, juce::Justification::left, true);
                
                // 3. Dynamic EQ Display Row
                float dynLabelY = panelRect.getY() + 53.0f;
                bool supportsDyn = (band.type == 0 || band.type == 3 || band.type == 4);
                
                juce::String dynStr = supportsDyn ? (band.dynamicEnabled ? getText (TextKey::On, currentLanguage)
                                                                          : getText (TextKey::Off, currentLanguage))
                                                   : getText (TextKey::NotAvailable, currentLanguage);
                juce::String moveStr = "-";
                if (supportsDyn && band.dynamicEnabled)
                {
                    float currentMod = band.dynamicGainDb;
                    moveStr = (currentMod >= 0.0f ? "+" : "") + juce::String(currentMod, 1) + " dB";
                }
                
                g.setColour(themeColors.textSecondary.withAlpha(0.65f));
                g.setFont (juce::Font (juce::FontOptions ("Inter", 7.5f, juce::Font::plain)));
                g.drawText("DYN", juce::roundToInt(panelRect.getX() + contentPaddingX), juce::roundToInt(dynLabelY), 50, 10, juce::Justification::left, true);
                g.drawText("Δ", juce::roundToInt(panelRect.getX() + contentPaddingX + 66.0f), juce::roundToInt(dynLabelY), 50, 10, juce::Justification::left, true);
                
                g.setColour(supportsDyn ? themeColors.textPrimary : themeColors.textSecondary.withAlpha(0.4f));
                g.setFont (juce::Font (juce::FontOptions ("Roboto Mono", 10.0f, juce::Font::plain)));
                g.drawText(dynStr, juce::roundToInt(panelRect.getX() + contentPaddingX), juce::roundToInt(dynLabelY + 11.0f), 60, 12, juce::Justification::left, true);
                g.drawText(moveStr, juce::roundToInt(panelRect.getX() + contentPaddingX + 66.0f), juce::roundToInt(dynLabelY + 11.0f), 60, 12, juce::Justification::left, true);
                
                // Bottom row details (Type & Q)
                float bottomRowY = panelRect.getY() + 76.0f;
                g.setColour(themeColors.textSecondary.withAlpha(0.80f));
                g.setFont (juce::Font (juce::FontOptions ("Inter", 8.5f, juce::Font::plain)));
                
                juce::String typeStr = getText (TextKey::Type, currentLanguage) + ": " + getFilterTypeText(band.type, currentLanguage);
                if (band.type == 1 || band.type == 2)
                    typeStr += " (" + juce::String(band.slope) + "dB)";
                g.drawText(typeStr, juce::roundToInt(panelRect.getX() + contentPaddingX), juce::roundToInt(bottomRowY), 110, 10, juce::Justification::left, true);
                g.drawText(qStr, juce::roundToInt(panelRect.getRight() - contentPaddingX - 40), juce::roundToInt(bottomRowY), 40, 10, juce::Justification::right, true);
            }
        }
    }

    void EQGraphView::resized()
    {
    }

    float EQGraphView::frequencyToX(float frequencyHz) const
    {
        return getNormalizedX(juce::jlimit(20.0f, 20000.0f, frequencyHz));
    }

    float EQGraphView::xToFrequency(float x) const
    {
        return juce::jlimit(20.0f, 20000.0f, getFrequencyForX(x));
    }

    float EQGraphView::gainToY(float gainDb) const
    {
        return getNormalizedY(juce::jlimit(-24.0f, 24.0f, gainDb));
    }

    float EQGraphView::yToGain(float y) const
    {
        auto plotBounds = getLocalBounds().reduced(35, 20);
        if (plotBounds.getHeight() <= 0)
            return 0.0f;
            
        float minG = -24.0f;
        float maxG = 24.0f;
        
        float val = (y - (float)plotBounds.getY()) / (float)plotBounds.getHeight();
        val = juce::jlimit(0.0f, 1.0f, val);
        
        float gainDb = maxG + val * (minG - maxG);
        return juce::jlimit(-24.0f, 24.0f, gainDb);
    }

    void EQGraphView::mouseDown(const juce::MouseEvent& event)
    {
        activeBandDragIndex = -1;
        float clickRadius = 12.0f; // Safe hit test radius (10-12px)
        
        int clickedIndex = -1;
        for (size_t i = 0; i < bands.size(); ++i)
        {
            const auto& band = bands[i];
            
            // Only interact with enabled bands
            if (band.enabled)
            {
                float x = getNormalizedX(band.frequencyHz);
                float y = getNormalizedY(band.gainDb);
                
                float dx = event.position.x - x;
                float dy = event.position.y - y;
                float dist = std::sqrt (dx * dx + dy * dy);
                if (dist <= clickRadius)
                {
                    clickedIndex = static_cast<int>(i);
                    break;
                }
            }
        }
        
        if (clickedIndex != -1)
        {
            setSelectedBandIndex (clickedIndex);
            
            if (event.mods.isPopupMenu())
            {
                if (onBandContextMenuRequested)
                    onBandContextMenuRequested (clickedIndex, event.getEventRelativeTo (this).position.roundToInt());
                return;
            }
            
            if (event.mods.isAltDown() && onBandSoloStatusChanged)
            {
                onBandSoloStatusChanged (clickedIndex, true);
            }
            
            activeBandDragIndex = clickedIndex;
            if (onDragStart)
                onDragStart(activeBandDragIndex);
        }
        else
        {
            setSelectedBandIndex (-1); // Deselect on clicking empty space
        }
        repaint();
    }

    void EQGraphView::mouseDoubleClick(const juce::MouseEvent& event)
    {
        float clickRadius = 12.0f;
        bool clickedHandle = false;
        
        for (const auto& band : bands)
        {
            if (band.enabled)
            {
                float x = getNormalizedX(band.frequencyHz);
                float y = getNormalizedY(band.gainDb);
                float dx = event.position.x - x;
                float dy = event.position.y - y;
                if (std::sqrt(dx * dx + dy * dy) <= clickRadius)
                {
                    clickedHandle = true;
                    break;
                }
            }
        }

        if (!clickedHandle && onBandCreateRequested)
        {
            float freq = xToFrequency (event.position.x);
            float gain = yToGain (event.position.y);
            
            int filterTypeIndex = 0; // Bell
            int slopeIndex = 1;      // 12 dB/oct
            
            if (freq < 40.0f)
            {
                filterTypeIndex = 1; // Low Cut
                slopeIndex = 3;      // 24 dB/oct
                gain = 0.0f;         // Ignore gain
            }
            else if (freq <= 10000.0f)
            {
                filterTypeIndex = 0; // Bell
                slopeIndex = 1;      // 12 dB/oct
            }
            else
            {
                if (gain >= -3.0f && gain <= 3.0f)
                {
                    filterTypeIndex = 2; // High Cut
                    slopeIndex = 3;      // 24 dB/oct
                    gain = 0.0f;         // Ignore gain
                }
                else
                {
                    filterTypeIndex = 4; // High Shelf
                    slopeIndex = 1;      // 12 dB/oct
                }
            }
            
            onBandCreateRequested (freq, gain, filterTypeIndex, slopeIndex);
        }
    }

    void EQGraphView::mouseDrag(const juce::MouseEvent& event)
    {
        if (activeBandDragIndex != -1)
        {
            float freq = xToFrequency(event.position.x);
            float gain = yToGain(event.position.y);
            
            // Update local band representation for instant rendering response
            // We only update if index is valid
            if (activeBandDragIndex >= 0 && activeBandDragIndex < static_cast<int>(bands.size()))
            {
                bands[activeBandDragIndex].frequencyHz = freq;
                bands[activeBandDragIndex].gainDb = gain;
            }
            
            repaint();
            
            if (event.mods.isAltDown() && onBandSoloStatusChanged)
            {
                onBandSoloStatusChanged (activeBandDragIndex, true);
            }
            else if (onBandSoloStatusChanged)
            {
                onBandSoloStatusChanged (activeBandDragIndex, false);
            }
            
            if (onBandDragged)
                onBandDragged(activeBandDragIndex, freq, gain);
        }
    }

    void EQGraphView::mouseUp(const juce::MouseEvent& event)
    {
        juce::ignoreUnused(event);
        
        if (activeBandDragIndex != -1)
        {
            int finishedIndex = activeBandDragIndex;
            activeBandDragIndex = -1;
            
            repaint();
            
            if (onBandSoloStatusChanged)
                onBandSoloStatusChanged (finishedIndex, false);
            
            if (onDragEnd)
                onDragEnd(finishedIndex);
        }
    }

    void EQGraphView::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
    {
        int targetBandIndex = -1;
        float hoverRadius = 15.0f; // Slightly larger hover target for scroll comfort
        
        // 1. Try to find if the mouse is hovering directly over a band handle
        for (size_t i = 0; i < bands.size(); ++i)
        {
            const auto& band = bands[i];
            if (band.enabled)
            {
                float x = getNormalizedX(band.frequencyHz);
                float y = getNormalizedY(band.gainDb);
                
                float dx = event.position.x - x;
                float dy = event.position.y - y;
                float dist = std::sqrt (dx * dx + dy * dy);
                if (dist <= hoverRadius)
                {
                    targetBandIndex = static_cast<int>(i);
                    setSelectedBandIndex (static_cast<int>(i)); // Update selection to the hovered band
                    break;
                }
            }
        }
        
        // 2. If not hovering over any band, fall back to the selected band (if valid and active)
        if (targetBandIndex == -1 && selectedBandIndex != -1)
        {
            const auto& band = bands[selectedBandIndex];
            if (band.enabled)
            {
                targetBandIndex = selectedBandIndex;
            }
        }
        
        // 3. Adjust Q of the target band (only if filter type uses Q, i.e., not Low Shelf (3) or High Shelf (4))
        if (targetBandIndex != -1)
        {
            auto& band = bands[targetBandIndex];
            bool usesQ = (band.type != 3 && band.type != 4);
            if (usesQ)
            {
                // Increment/decrement using a smooth multiplicative scale
                float scrollFactor = std::pow (1.15f, wheel.deltaY * 5.0f);
                float newQ = juce::jlimit (0.1f, 18.0f, band.q * scrollFactor);
                
                if (newQ != band.q)
                {
                    band.q = newQ;
                    repaint();
                    
                    if (onBandQChanged)
                        onBandQChanged(targetBandIndex, newQ);
                }
            }
        }
    }

    float EQGraphView::getNormalizedX(float frequencyHz) const
    {
        auto plotBounds = getLocalBounds().reduced(35, 20);
        float minF = 20.0f;
        float maxF = 20000.0f;
        
        // Logarithmic scale mapping
        float val = (std::log(frequencyHz) - std::log(minF)) / (std::log(maxF) - std::log(minF));
        
        return (float)plotBounds.getX() + val * (float)plotBounds.getWidth();
    }

    float EQGraphView::getNormalizedY(float gainDb) const
    {
        auto plotBounds = getLocalBounds().reduced(35, 20);
        float minG = -24.0f;
        float maxG = 24.0f;
        
        // Linear scale mapping (inverted so positive dB is higher up)
        float val = (gainDb - maxG) / (minG - maxG);
        
        return (float)plotBounds.getY() + val * (float)plotBounds.getHeight();
    }

    float EQGraphView::getFrequencyForX(float x) const
    {
        auto plotBounds = getLocalBounds().reduced(35, 20);
        if (plotBounds.getWidth() <= 0)
            return 20.0f;
        
        float xRelative = x - (float)plotBounds.getX();
        float width = (float)plotBounds.getWidth();
        float ratio = xRelative / width;
        ratio = juce::jlimit(0.0f, 1.0f, ratio);
        
        // Exact mathematical pixel-to-Hertz reverse formula required by Phase 23.0
        float f = 20.0f * std::pow(20000.0f / 20.0f, ratio);
        return f;
    }
}
