# Compartilhado - Gerenciamento de Estado (shared/state)

Esta pasta conterá as classes base e utilitários para a serialização e persistência de dados de estado dos plugins.

## Responsabilidades
*   **Salvamento de Estado (Bancos de dados)**: Métodos auxiliares para converter o estado do plugin (`juce::ValueTree`) em blocos de memória XML binários e vice-versa, manipulados pelas funções `getStateInformation` e `setStateInformation` do JUCE.
*   **Navegador de Presets (Preset Backend)**:
    *   Leitura e gravação de arquivos de preset (.xml ou formato próprio) no disco do usuário.
    *   Mecanismo de pesquisa rápida e indexação de presets em pastas de fábrica (Factory Presets) e usuário (User Presets).
*   **A/B Comparison**: Lógica compartilhada para clonar árvores de estado completas temporariamente em memória para permitir a alternância instantânea entre as configurações A e B na tela do usuário.
