# Compartilhado - Gerenciamento de Parâmetros (shared/parameters)

Esta pasta conterá as classes base, utilitários e wrappers para o gerenciamento de parâmetros nos plugins.

## Responsabilidades
*   **AudioProcessorValueTreeState (APVTS)**: Estruturação simplificada e padronizada de layouts de parâmetros do JUCE.
*   **Parâmetros Personalizados**: Wrappers em torno de `juce::AudioParameterFloat`, `juce::AudioParameterInt`, `juce::AudioParameterBool` e `juce::AudioParameterChoice` que facilitam:
    *   Amarração direta de parâmetros com os controles da interface (`SliderAttachment`, `ButtonAttachment`).
    *   Formatação personalizada de texto para exibição na tela (ex: adicionar sufixo " dB", " Hz", " ms").
    *   Conversão matemática entre valores lineares da interface e valores logarítmicos/geométricos do DSP (curvas de frequência, thresholds).
