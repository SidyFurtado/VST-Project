# Compartilhado - Processamento Digital de Sinais (shared/dsp)

Esta pasta conterá as classes base e utilitários de **DSP (Digital Signal Processing)** que serão compartilhados entre múltiplos plugins do monorepo.

## Responsabilidades
*   **Filtros Comuns**: Classes genéricas para filtros biquad IIR, filtros FIR e estruturas de crossovers.
*   **Oversampling**: Infraestrutura compartilhada para sobreamostragem (2x, 4x, 8x) usando as classes `juce::dsp::Oversampling` ou implementações personalizadas a fim de combater aliasing em plugins não-lineares (como saturadores e compressores).
*   **Limitadores e Detectores**: Algoritmos base de detecção de envelope (RMS, Peak Follower) e limitadores de pico (True Peak brickwall) de segurança.
*   **Suavização de Sinais (Parameter Smoothing)**: Mecanismos para suavizar variações de ganho e frequência em tempo real, evitando zipper noise.
