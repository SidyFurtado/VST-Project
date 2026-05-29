# Resumo Técnico: Fase 10.0 — Undo / Redo Básico

Este documento registra a implementação do histórico básico de Undo/Redo do **AUREQ**, mantendo o escopo limitado a snapshots APVTS temporários de sessão.

---

## 1. Objetivo da Fase
Adicionar Undo/Redo interno para ações grandes do equalizador sem criar parâmetros novos, sem alterar o layout do APVTS, sem alterar IDs, sem modificar DSP, analyzer FFT, presets externos ou save user preset.

---

## 2. Arquitetura Implementada
- O histórico fica no `PluginProcessor`, sobrevivendo à recriação do editor.
- `undoStack` e `redoStack` usam `std::deque<juce::ValueTree>`.
- Cada checkpoint salva `apvts.copyState().createCopy()`.
- Undo/Redo restaura com `apvts.replaceState (state.createCopy())`.
- O limite inicial do histórico é de 32 estados.
- O histórico é temporário de sessão e não é persistido em `getStateInformation()`.
- `setStateInformation()` limpa Undo/Redo ao carregar estado do host.

---

## 3. Checkpoints
Checkpoints são capturados uma vez antes de ações grandes:
- carregar preset;
- alternar A/B;
- trocar tipo de filtro;
- trocar slope;
- trocar channel mode;
- adicionar banda;
- remover banda;
- criar banda por double-click;
- resetar ganhos.

Não há captura em `timerCallback()`, analyzer, `parameterChanged()`, automação contínua ou cada `mouseDrag`.

---

## 4. Proteções Contra Histórico Excessivo
- Estados idênticos ao último checkpoint são ignorados.
- `redoStack` é limpo quando um novo checkpoint manual é criado.
- Undo e Redo usam uma guarda interna para evitar captura recursiva.
- Drag/knobs contínuos ficaram fora da fase para evitar snapshots por movimento.

---

## 5. Integração Com UI
O botão `Settings` abre um menu pequeno com:
- `Undo`;
- `Redo`.

Os itens são habilitados somente quando `canUndo()` ou `canRedo()` retornam verdadeiro. Após uma restauração, a UI atualiza controles da banda selecionada, gráfico, analyzer visual e repaint.

---

## 6. Interação Com Presets
Carregar preset gera um único checkpoint antes do preset inteiro. Undo após preset restaura o estado anterior; Redo reaplica o estado pós-preset salvo na pilha de redo.

---

## 7. Interação Com A/B
Alternar A/B é undoável porque restaura estado APVTS via `replaceState()`. As ações `Copy A to B`, `Copy B to A`, `Copy Current to Other` e `Reset B to Current` não entram no Undo nesta fase porque não alteram o APVTS atual.

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
