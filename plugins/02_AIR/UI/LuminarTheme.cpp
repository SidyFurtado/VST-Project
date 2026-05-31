#include "LuminarTheme.h"

// ==============================================================================
// LuminarTheme — ASTRA Audio Suite / LUMINAR
//
// Dark Mode: espaço cósmico profundo com acento âmbar/dourado.
// ==============================================================================

namespace LUMINAR
{
    LuminarColors LuminarTheme::getDarkColors()
    {
        LuminarColors c;

        // --- Backgrounds --------------------------------------------------------
        c.background       = juce::Colour ( 8,  12,  22);  // Deep space navy
        c.backgroundTop    = juce::Colour (12,  20,  36);  // Slightly lighter navy
        c.backgroundAurora = juce::Colour (10,  15,  28);  // Aurora display bg

        // --- Panels -------------------------------------------------------------
        c.topBarBg = juce::Colour (14,  22,  40).withAlpha (0.75f); // Translucent glass bar
        c.panel    = juce::Colour (18,  28,  50).withAlpha (0.28f); // Knob area glass

        // --- Text ---------------------------------------------------------------
        c.textPrimary   = juce::Colour (248, 250, 252);          // Almost white
        c.textSecondary = juce::Colour (148, 163, 184);          // Slate blue-grey
        c.textMuted     = juce::Colour ( 71,  85, 105);          // Very dim slate

        // --- Accent: Âmbar/Dourado (#F6C90E) ------------------------------------
        c.accent     = juce::Colour (246, 201,  14);             // Amber gold
        c.accentSoft = juce::Colour (255, 179,  71);             // Soft warm amber
        c.accentGlow = juce::Colour (246, 180,  14).withAlpha (0.07f); // Very subtle aurora

        // --- Structural ---------------------------------------------------------
        c.border    = juce::Colour (255, 255, 255).withAlpha (0.09f); // Glass edge
        c.shadow    = juce::Colour (  2,   4,   8).withAlpha (0.45f);
        c.knobTrack = juce::Colour ( 30,  46,  72);              // Inactive arc
        c.knobCap   = juce::Colour ( 14,  22,  36);              // Cap base dark

        return c;
    }

} // namespace LUMINAR
