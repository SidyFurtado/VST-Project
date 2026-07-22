#pragma once

#include <juce_graphics/juce_graphics.h>

namespace AUREQ
{
    enum class ThemeMode
    {
        Light,
        Dark
    };

    struct ThemeColors
    {
        juce::Colour background;
        juce::Colour backgroundTop;
        juce::Colour backgroundMiddle;
        juce::Colour backgroundBase;
        juce::Colour topBar;
        juce::Colour panel;
        juce::Colour panelElevated;
        juce::Colour gridLine;
        juce::Colour gridLineStrong;
        juce::Colour textPrimary;
        juce::Colour textSecondary;
        juce::Colour accent;
        juce::Colour accentSoft;
        juce::Colour border;
        juce::Colour shadow;
        juce::Colour warning;
        juce::Colour clip;
    };

    class Theme
    {
    public:
        static ThemeColors getColors(ThemeMode mode);
    };
}
