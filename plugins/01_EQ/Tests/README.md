# Clean EQ - Testes e Calibração (plugins/01_EQ/Tests)

Esta pasta conterá as lógicas de testes de unidade e verificação para o motor de DSP e do fluxo de processamento de áudio do equalizador.

## Testes Previstos
*   **Filtros Estáticos (Frequency Response Verification)**:
    *   Verificar se os coeficientes gerados para frequências de amostragem comuns (44.1 kHz, 48 kHz, 96 kHz) produzem a atenuação e a fase corretas de acordo com a teoria matemática de Nyquist.
*   **Estabilidade de Processamento**:
    *   Submeter o motor a testes com sinais sintéticos extremos (impulsos unitários, ruído branco de alta amplitude, silêncio absoluto) para testar a robustez contra NaNs, estouro numérico e denormais.
*   **Teste de Suavização**:
    *   Verificar se a transição entre saltos de frequência ocorre sem transientes indesejados ou cliques rápidos.
*   **Integração**:
    *   Validar se o estado do XML (`getStateInformation`/`setStateInformation`) serializa e recupera perfeitamente os parâmetros de cada uma das 5 bandas de EQ.
