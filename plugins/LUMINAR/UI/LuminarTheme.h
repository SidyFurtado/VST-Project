#pragma once

#include <juce_graphics/juce_graphics.h>

// ==============================================================================
// LuminarTheme — ASTRA Audio Suite / LUMINAR
//
// Paleta de cores exclusiva do LUMINAR.
// Acento: Âmbar/Dourado #F6C90E — diferente do Cyan #22D3EE do AUREQ.
// Fase 18.1: Dark Mode apenas.
// ==============================================================================

namespace LUMINAR
{
    struct LuminarColors
    {
        // Backgrounds
        juce::Colour background;        // Fundo geral do plugin
        juce::Colour backgroundTop;     // Topo do gradiente de fundo
        juce::Colour backgroundAurora;  // Fundo da área de display aurora

        // Panels
        juce::Colour topBarBg;          // Top bar glass background
        juce::Colour panel;             // Glass panel (knob area)

        // Text
        juce::Colour textPrimary;       // Texto principal (plugin name, knob values)
        juce::Colour textSecondary;     // Texto secundário (company name, labels)
        juce::Colour textMuted;         // Texto muito sutil (footer, subtítulos)

        // Accent — Âmbar/Dourado
        juce::Colour accent;            // Âmbar dourado #F6C90E — acento principal
        juce::Colour accentSoft;        // Âmbar suave #FFB347 — track ativo dos knobs
        juce::Colour accentGlow;        // Aurora glow muito sutil

        // Structural
        juce::Colour border;            // Bordas glass translúcidas
        juce::Colour shadow;            // Sombras de profundidade
        juce::Colour knobTrack;         // Trilha inativa dos knobs
        juce::Colour knobCap;           // Tampa central dos knobs
    };

    class LuminarTheme
    {
    public:
        /** Returns the Dark Mode color palette for LUMINAR. */
        static LuminarColors getDarkColors();
    };

} // namespace LUMINAR
