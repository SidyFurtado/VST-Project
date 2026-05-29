#include "Theme.h"

namespace AUREQ
{
    ThemeColors Theme::getColors(ThemeMode mode)
    {
        ThemeColors colors;

        if (mode == ThemeMode::Light)
        {
            colors.background       = juce::Colour(228, 233, 238); // Frosted slate ice
            colors.backgroundTop    = juce::Colour(238, 242, 246);
            colors.backgroundMiddle = juce::Colour(228, 233, 238);
            colors.backgroundBase   = juce::Colour(220, 226, 232);
            colors.topBar           = juce::Colour(255, 255, 255).withAlpha(0.45f); // Frosted light glass
            colors.panel            = juce::Colour(255, 255, 255).withAlpha(0.30f); // High translucency glass
            colors.panelElevated    = juce::Colour(255, 255, 255).withAlpha(0.65f); // HUD glass panel
            colors.gridLine         = juce::Colour(180, 195, 210);
            colors.gridLineStrong   = juce::Colour(8, 145, 178).withAlpha(0.35f);
            colors.textPrimary      = juce::Colour(15, 23, 42);
            colors.textSecondary    = juce::Colour(71, 85, 105);
            colors.accent           = juce::Colour(8, 145, 178);
            colors.accentSoft       = juce::Colour(13, 148, 136);
            colors.border           = juce::Colour(255, 255, 255).withAlpha(0.40f); // Glass reflection edge
            colors.shadow           = juce::Colour(15, 23, 42).withAlpha(0.05f);
            colors.warning          = juce::Colour(245, 158, 11);
            colors.clip             = juce::Colour(239, 68, 68);
        }
        else // ThemeMode::Dark
        {
            colors.background       = juce::Colour(10, 16, 26); // Lighter dark space navy
            colors.backgroundTop    = juce::Colour(16, 25, 40); // Richer navy
            colors.backgroundMiddle = juce::Colour(10, 16, 26);
            colors.backgroundBase   = juce::Colour(18, 29, 46);
            colors.topBar           = juce::Colour(20, 30, 48).withAlpha(0.28f); // Translucent navy glass
            colors.panel            = juce::Colour(30, 45, 65).withAlpha(0.22f); // Lighter glass panel
            colors.panelElevated    = juce::Colour(40, 58, 82).withAlpha(0.35f); // Luminous HUD panel
            colors.gridLine         = juce::Colour(45, 62, 85); // Slate blue
            colors.gridLineStrong   = juce::Colour(34, 211, 238).withAlpha(0.40f); // Cyan line
            colors.textPrimary      = juce::Colour(248, 250, 252);
            colors.textSecondary    = juce::Colour(148, 163, 184); // Improved contrast secondary text
            colors.accent           = juce::Colour(34, 211, 238); // Premium cyan
            colors.accentSoft       = juce::Colour(45, 212, 191); // Suave mint
            colors.border           = juce::Colour(255, 255, 255).withAlpha(0.12f); // Elevated edge refraction
            colors.shadow           = juce::Colour(2, 4, 8).withAlpha(0.40f); // Softer shadow
            colors.warning          = juce::Colour(251, 191, 36);
            colors.clip             = juce::Colour(248, 113, 113);
        }

        return colors;
    }
}
