# Clean EQ - Relação Visual e Painéis (plugins/01_EQ/UI)

Esta pasta conterá as classes específicas para a interface do usuário do equalizador.

## Responsabilidades
*   **EQ Curve Component (Visualizador de Curva)**:
    *   Desenha a curva combinada de resposta em frequência das 5 bandas em tempo real.
    *   Permite a manipulação interativa da curva arrastando nós (pontos de controle) diretamente na tela.
    *   Exibe o analisador de espectro de áudio FFT por trás da curva de equalização.
*   **Band Editor Component (Controles de Banda)**:
    *   Contém os Knobs específicos para controle de Frequency, Gain e Q-Factor da banda selecionada.
    *   Inclui botões seletores para escolher o tipo de filtro da banda.
    *   Esmaece visualmente (opacidade a 40%) os controles de bandas que estiverem desativadas (bypassadas) de forma independente.
*   **Header e Footer Especializados**:
    *   Mapeamento específico para preencher o preset browser com patches do EQ e tooltips dedicados de equalização no rodapé.
