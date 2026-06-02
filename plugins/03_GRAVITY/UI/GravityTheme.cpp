#include "GravityTheme.h"

// ==============================================================================
// GravityTheme — ASTRA Audio Suite / GRAVITY
//
// Dark Mode: espaço profundo com pulsar vermelho crimson.
// ==============================================================================

namespace GRAVITY
{
    GravityColors GravityTheme::getDarkColors()
    {
        GravityColors c;

        // --- Backgrounds --------------------------------------------------------
        c.background       = juce::Colour (  6,  10,  20);  // Deep space black-blue
        c.backgroundTop    = juce::Colour ( 10,  16,  32);  // Slightly lighter navy
        c.backgroundDisplay= juce::Colour (  8,  13,  24);  // Display area bg

        // --- Panels -------------------------------------------------------------
        c.topBarBg = juce::Colour (12,  18,  36).withAlpha (0.80f); // Glass bar
        c.panel    = juce::Colour (16,  24,  46).withAlpha (0.30f); // Glass panel

        // --- Text ---------------------------------------------------------------
        c.textPrimary   = juce::Colour (245, 248, 255);           // Almost white
        c.textSecondary = juce::Colour (140, 155, 180);           // Slate blue-grey
        c.textMuted     = juce::Colour ( 65,  80, 105);           // Very dim slate

        // --- Accent: Crimson / Coral (#E84040) ----------------------------------
        c.accent     = juce::Colour (232,  64,  64);              // Hot crimson
        c.accentSoft = juce::Colour (255, 100,  90);              // Coral warm
        c.accentGlow = juce::Colour (232,  64,  64).withAlpha (0.08f); // Subtle pulse

        // --- Meters -------------------------------------------------------------
        c.meterGreen = juce::Colour ( 52, 211, 153);              // Emerald green
        c.meterAmber = juce::Colour (251, 191,  36);              // Warm amber
        c.meterRed   = juce::Colour (232,  64,  64);              // Same as accent

        // --- Structural ---------------------------------------------------------
        c.border      = juce::Colour (255, 255, 255).withAlpha (0.08f); // Glass edge
        c.shadow      = juce::Colour (  2,   4,   8).withAlpha (0.50f);
        c.sliderTrack = juce::Colour ( 26,  38,  66);              // Inactive track
        c.sliderThumb = juce::Colour ( 12,  18,  34);              // Thumb base dark

        return c;
    }

} // namespace GRAVITY
