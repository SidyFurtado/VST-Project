#pragma once

#include <juce_graphics/juce_graphics.h>

// ==============================================================================
// GravityTheme — ASTRA Audio Suite / GRAVITY
//
// Paleta de cores para o compressor GRAVITY.
// Acento: Vermelho coral / Crimson #E84040 — energia, compressão, impacto.
// Diferente do Cyan #22D3EE do AUREQ e do Âmbar #F6C90E do LUMINAR.
// Dark Mode apenas (fase inicial).
// ==============================================================================

namespace GRAVITY
{
    struct GravityColors
    {
        // Backgrounds
        juce::Colour background;        ///< Fundo geral (azul-noite profundo)
        juce::Colour backgroundTop;     ///< Topo do gradiente de fundo
        juce::Colour backgroundDisplay; ///< Fundo do display central

        // Panels
        juce::Colour topBarBg;          ///< Top bar glass background
        juce::Colour panel;             ///< Glass panel (controles)

        // Text
        juce::Colour textPrimary;       ///< Texto principal (plugin name, valores)
        juce::Colour textSecondary;     ///< Texto secundário (company, labels)
        juce::Colour textMuted;         ///< Texto sutil (footer, subtítulos)

        // Accent — Vermelho Crimson
        juce::Colour accent;            ///< Crimson #E84040 — acento principal
        juce::Colour accentSoft;        ///< Coral suave — track ativo dos sliders
        juce::Colour accentGlow;        ///< Glow muito sutil do acento

        // Meters
        juce::Colour meterGreen;        ///< Nível de entrada / saída (verde)
        juce::Colour meterAmber;        ///< GR médio (âmbar)
        juce::Colour meterRed;          ///< GR alto (vermelho)

        // Structural
        juce::Colour border;            ///< Bordas glass translúcidas
        juce::Colour shadow;            ///< Sombras de profundidade
        juce::Colour sliderTrack;       ///< Trilha inativa dos sliders verticais
        juce::Colour sliderThumb;       ///< Tampa/thumb dos sliders
    };

    class GravityTheme
    {
    public:
        /** Returns the Dark Mode color palette for GRAVITY. */
        static GravityColors getDarkColors();
    };

} // namespace GRAVITY
