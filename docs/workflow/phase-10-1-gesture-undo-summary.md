# Resumo Técnico: Fase 10.1 — Gesture Undo

Este documento registra a expansão do Undo/Redo básico do **AUREQ** para gestos contínuos de interface, preservando o escopo seguro da Fase 10.0.

---

## 1. Objetivo da Fase
Capturar checkpoints antes de gestos contínuos para que Undo retorne ao estado anterior ao gesto inteiro, sem criar parâmetros, sem alterar APVTS layout, IDs, DSP, analyzer FFT, save user preset ou presets externos.

---

## 2. Estratégia Implementada
- A Fase 10.1 reutiliza `audioProcessor.captureUndoCheckpoint()`.
- Nenhuma captura foi adicionada ao `timerCallback()`, analyzer, `parameterChanged()` ou callbacks de valor contínuo.
- Checkpoints são capturados no início do gesto, antes da primeira alteração APVTS.
- `captureUndoCheckpoint()` continua responsável por deduplicação, limite de histórico e limpeza do redo.

---

## 3. Drag de Handle
O drag no `EQGraphView` agora captura checkpoint em `graphView.onDragStart`, antes de `beginChangeGesture()` de Frequency/Gain.

`graphView.onBandDragged` permanece sem checkpoint e continua apenas atualizando Frequency/Gain durante o movimento.

---

## 4. Mouse Wheel de Q
O Q por mouse wheel captura checkpoint antes de aplicar o novo valor.

Para evitar spam óbvio, o editor usa uma janela curta de agrupamento por banda. Eventos de wheel próximos na mesma banda compartilham o mesmo checkpoint; uma nova janela ou outra banda gera novo checkpoint.

---

## 5. Knobs e Sliders Cobertos
Os seguintes controles capturam checkpoint em `Slider::onDragStart`:
- Frequency;
- Gain;
- Q;
- Dynamic Range;
- Dynamic Threshold;
- Dynamic Attack;
- Dynamic Release.

Não há captura em `valueChanged` contínuo.

---

## 6. DYN Toggle
O botão `DYN` captura checkpoint via `mouseDown`, antes do `ButtonAttachment` aplicar a mudança no APVTS.

---

## 7. Controles Deixados Para Fase Futura
- Input Gain;
- Output Gain;
- Global Bypass;
- Theme;
- Band Bypass;
- atalhos de teclado;
- agrupamento avançado de wheel com timer dedicado.

---

## 8. Confirmações de Escopo
- **APVTS layout**: não alterado.
- **IDs de parâmetros**: não alterados.
- **DSP**: não alterado.
- **Analyzer FFT**: não alterado.
- **Save user preset / Preset externo**: não implementados.
- **Livros/PDFs**: não alterados.

---

## 9. Resultado do Build
Comando executado:
```bash
cmake --build build
```

Resultado:
```text
[ 59%] Built target AUREQ
[ 72%] Built target AUREQ_Standalone
[ 86%] Built target AUREQ_VST3
[100%] Built target AUREQ_FilterTests
```

Status: **aprovado**. O build apresentou apenas warnings já conhecidos de depreciação da assinatura antiga de `juce::AudioParameterFloat`, sem erros.

---

## 10. Resultado dos Testes
Comando executado:
```bash
./build/tests/AUREQ_FilterTests
```

Resultado:
```text
[PASS] Biquad Coefficient Tests completed successfully.
[PASS] EQBand Block Processing Tests completed successfully.
[PASS] Filter Stability and Safety Tests completed successfully.
SUCCESS: All AUREQ DSP Filter Tests Passed!
```

Status: **aprovado**.

---

## 11. Caminhos dos Artefatos
- **VST3**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- **Standalone**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
