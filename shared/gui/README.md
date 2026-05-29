# Compartilhado - Interface Gráfica (shared/gui)

Esta pasta conterá as classes base, widgets e utilitários de **GUI (Graphical User Interface)** compartilhados entre toda a linha de plugins.

## Responsabilidades
*   **Controles Customizados**:
    *   `CustomKnob`: Classe herdada de `juce::Slider` que desenha knobs circulares limpos e modernos conforme a identidade visual.
    *   `CustomButton`: Botões planos com suporte aos estados de hover, active, pressed e disabled.
    *   `InteractiveMeter`: Componente de medição dinâmica vertical de nível RMS/Pico.
*   **Elementos Estruturais**:
    *   `PresetBrowserWidget`: Barra central de navegação de presets.
    *   `TopBarWidget`: Cabeçalho unificado contendo o logo, preset browser, botão A/B e alternador de tema.
    *   `FooterWidget`: Rodapé com medidores compactos, bypass global e displays de tooltips.
*   **Gerenciamento de Temas (pasta subjacente /theme)**:
    *   Classes para alternar e propagar as cores de Light Mode e Dark Mode.
