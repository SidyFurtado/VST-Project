# Clean EQ - Código de Inicialização JUCE (plugins/01_EQ/Source)

Esta pasta conterá as classes essenciais do esqueleto do plugin geradas pelo JUCE.

## Componentes Principais
*   **`PluginProcessor` (`.h` / `.cpp`)**:
    *   Gerencia o ciclo de vida do plugin.
    *   Contém a thread de áudio principal e o método de processamento em tempo real `processBlock()`.
    *   Gerencia os parâmetros e a árvore de estados (`juce::AudioProcessorValueTreeState`).
    *   Não realiza desenho gráfico e não interage diretamente com eventos de tela.
*   **`PluginEditor` (`.h` / `.cpp`)**:
    *   Gerencia a janela principal de interface gráfica que o usuário vê.
    *   Responde a eventos de mouse, teclado e atualizações de parâmetros.
    *   Instancia e organiza espacialmente os widgets de tela (knobs, displays, meters).
    *   Escuta e consome o `ThemeManager` compartilhado para redesenhar a janela ao alternar o tema.
