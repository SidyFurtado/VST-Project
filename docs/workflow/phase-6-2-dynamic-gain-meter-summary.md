# Resumo de Validação Técnica: Fase 6.2 — Dynamic Gain Meter

Este documento registra a especificação, implementação e validação técnica da medição visual e feedbacks gráficos de atuação dinâmica por banda no equalizador **AUREQ**, integrante da **ASTRA Audio Suite**.

---

## 1. Objetivo da Fase

Adicionar medição visual em tempo real da atuação dinâmica por banda, mostrando a redução (gain reduction) ou expansão (gain expansion) de ganho atual de forma sutil, reativa e premium. O display do gráfico, o Floating HUD e o Selected Band Card no painel inferior agora apresentam de forma sutil a atuação dinâmica em decibéis.

---

## 2. Como o dynamicGainDb é Calculado e Exposto

O DSP realiza a modulação de ganho dinâmico uma vez por bloco de áudio na thread de áudio em tempo real (`EQBand::processBlock`), armazenando o valor resultante em um membro atômico `currentDynamicGainDb` de forma segura, sem lock e livre de alocações:
- O valor é `0.0f` quando o Dynamic EQ está desativado (`dynamicEnabled == false`).
- O valor é `0.0f` quando o `dynamicRange == 0.0f`.
- O valor é `0.0f` para tipos de filtros não suportados (Low Cut, High Cut, Notch, Band Pass).
- É positivo para expansão (dynamic boost) e negativo para redução (dynamic attenuation).

Esse valor é exposto a threads externas através do getter seguro:
- `float EQBand::getCurrentDynamicGainDb() const` que realiza uma operação atômica `.load()`.
- O `EQProcessorCore` e o `AureqAudioProcessor` expõem o wrapper `getBandDynamicGainDb(int bandIndex)` para permitir consultas diretas pela UI.

---

## 3. Como a UI Lê o Meter

A interface gráfica realiza leituras periódicas assíncronas do estado em `AureqAudioProcessorEditor::timerCallback()` a uma taxa aproximada de 30 Hz:
- O valor instantâneo de cada banda é copiado do processador e injetado na array de bandas visuais do gráfico:
  `visualBands[i].dynamicGainDb = audioProcessor.getBandDynamicGainDb (i);`
- Em seguida, a estrutura `visualBands` atualizada é enviada para o `EQGraphView`.
- O editor também lê o valor dinâmico da banda selecionada no momento para alimentar o rótulo local `dynGainMeterLabel` no Selected Band Card.

---

## 4. Representação Visual no Gráfico

No `EQGraphView::paint()`, para qualquer banda habilitada que possua o Dynamic EQ ativado e opere em um filtro compatível (Bell, Low Shelf ou High Shelf):
- Um indicador sutil e luminoso é desenhado verticalmente sobreposto à linha tracejada de range dinâmico do handle.
- A linha estende-se do ganho estático da banda até o ganho modular instantâneo: `gainDb + dynamicGainDb`.
- O traço de atuação dinâmica tem espessura de `2.0f` pixels e opacidade balanceada (`0.85f` no Dark Mode).
- A cor do traço de atuação é reativa à modulação:
  - **Cyan** (`themeColors.accent`) para expansão (boost, ganho dinâmico positivo).
  - **Mint/Teal** (`themeColors.accentSoft`) para compressão/redução (cut, ganho dinâmico negativo).
- Um pequeno círculo de acento indica visualmente a ponta da atuação em tempo real.

---

## 5. Representação Visual no HUD

O Floating HUD exibe a atuação dinâmica na sua segunda linha de leituras:
- **DYN**: ON / OFF / N/A (se o tipo de filtro não for compatível).
- **Δ**: Mostra a variação em tempo real (ex: `+1.8 dB` ou `-2.4 dB`) baseada no valor instantâneo de `dynamicGainDb`. Se estiver desativado ou inativo, exibe `-`.

---

## 6. Representação Visual no Selected Band Card

No Selected Band Card (painel de controles inferior), a atuação da dinâmica é exibida por um visor compacto colocado logo à esquerda do botão de toggle **DYN**:
- A etiqueta do visor (`dynGainMeterLabel`) exibe o delta de atuação formatado em tempo real, ex: `Δ -2.4` ou `Δ +1.2`.
- Para máxima harmonia visual e integridade premium:
  - A cor do texto da label é alterada dinamicamente: Cyan para boost e Mint/Teal para atenuação.
  - Se o Dynamic EQ estiver desligado, inativo ou não suportado, a label exibe `Δ -` com opacidade reduzida (`0.35f`).
- Para acomodar este novo visor sem amassar a interface, as coordenadas de `filterTypeButtons` foram ajustadas de forma inteligente com espaçamento refinado (step de `40` pixels, largura de `38` pixels), posicionando o visor no retângulo `[X = 574, Y = bottomY + 12, W = 38, H = 20]` de forma limpa e livre de colisões ou overlaps.

---

## 7. Conformidade das Regras Estritas

- **APVTS Não Alterado**: A contagem de parâmetros permanece rigidamente em 108. Nenhuma declaração ou ID de parâmetro foi adicionado, alterado ou removido.
- **Comportamento Sonoro Preservado**: O algoritmo de DSP permanece intacto. A leitura de `currentDynamicGainDb` é uma operação estritamente passiva de monitoramento.
- **Mid/Side, Left/Right e Linear Phase**: Inativos e não implementados nesta fase.

---

## 8. Resultados de Compilação & Execução de Testes

### A. Compilação
- **Comando**: `cmake --build build`
- **Status**: Compilação limpa concluída com sucesso (0 erros).

### B. Execução de Testes Unitários de Regressão
- **Comando**: `./build/tests/AUREQ_FilterTests`
- **Status**: Todos os testes passaram sem falhas. Foram validados os novos cenários de testes unitários em `EQBandProcessingTests.cpp` e asserções de estabilidade de medição em `FilterStabilityTests.cpp`.
```
========================================================
         AUREQ - DSP FILTER VALIDATION TESTS            
========================================================

[SUITE] Running Biquad Coefficient Tests...
[PASS] Biquad Coefficient Tests completed successfully.

[SUITE] Running EQBand Block Processing Tests...
[PASS] EQBand Block Processing Tests completed successfully.

[SUITE] Running Filter Stability and Safety Tests...
[SUB-SUITE] Running EQBand Dynamic EQ stability sub-suite...
[PASS] Filter Stability and Safety Tests completed successfully.

========================================================
      SUCCESS: All AUREQ DSP Filter Tests Passed!      
========================================================
```

---

## 9. Caminho dos Binários Gerados

- **VST3**: [AUREQ.vst3](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3)
- **Standalone**: [AUREQ.app](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app)
- **Executável de Teste**: [AUREQ_FilterTests](file:///Users/sidyziin/Documents/VST%20-%20Project/build/tests/AUREQ_FilterTests)

---

## 10. Próximo Passo Recomendado

Prosseguir para o desenvolvimento da **Fase 6.2 (continuação): Sistema de Comparação Rápida A/B** de estados de equalização ou iniciar o preset browser definitivo.
