# Resumo Técnico: Fase 9.1 — A/B Copy Actions

Este documento registra a implementação e a validação das ações avançadas de cópia e reset para os slots de comparação A/B no **AUREQ**, mantendo a compatibilidade e a integridade de todas as fases anteriores.

---

## 1. Objetivo da Fase
Implementar as ações avançadas de cópia e reset (Copy A to B, Copy B to A, Copy Current to Other, Reset B to Current) para os slots A/B, disparadas por meio de um menu contextual (clique direito) no botão A/B da Top Bar, sem afetar o comportamento básico de alternância por clique esquerdo e sem criar novos parâmetros ou alterar o motor DSP, o analisador FFT ou o APVTS.

---

## 2. Ações Adicionadas
As seguintes ações foram implementadas na lógica interna do processador (`PluginProcessor`) e conectadas ao menu contextual da UI:
- **Copy A to B**: Copia o estado salvo no slot A para o slot B.
- **Copy B to A**: Copia o estado salvo no slot B para o slot A.
- **Copy Current to Other**: Salva o estado atual do APVTS no slot ativo e clona esse mesmo estado para o outro slot.
- **Reset B to Current**: Salva o estado atual do APVTS diretamente no slot B.

---

## 3. Comportamento e Interação da Interface (UI)
- **Clique Esquerdo**: Continua a alternar os slots ativo A e B. A troca recupera o estado correspondente chamando `apvts.replaceState()`.
- **Clique Direito (ou Ctrl-clique no macOS)**: Abre um menu contextual (`juce::PopupMenu`) com as quatro ações avançadas.
  - Para abrir o menu contextual, foi adicionado um mouse listener em `abBtn` no construtor do editor, o qual é devidamente removido no destrutor.
  - A função `mouseUp` do editor intercepta o evento com `event.mods.isPopupMenu()` para exibir o menu.
  - O callback de clique (`abBtn.onClick`) possui uma proteção para retornar imediatamente caso detecte um clique de menu contextual (`juce::ModifierKeys::getCurrentModifiers().isPopupMenu()`), evitando alternar o slot ativo concomitantemente.
  - O menu contextual utiliza o `aureqLookAndFeel` para que a identidade visual premium translúcida seja preservada.

---

## 4. Funcionamento Interno (PluginProcessor)
Todos os novos métodos chamam `ensureABSlotsInitialized()` antes de operar e utilizam `createCopy()` para clonar o estado do `juce::ValueTree` de forma profunda e segura, evitando compartilhamento indesejado de instâncias em memória.
- **Confirmação Importante**: Nenhuma das ações de cópia/reset chama `replaceState()`.
- **Confirmação Importante**: Nenhuma das ações de cópia/reset altera o slot ativo atual.

---

## 5. Confirmações de Escopo e Proteção
- **APVTS Layout**: Não alterado. A contagem total permanece em 108 parâmetros.
- **IDs de Parâmetros**: Não alterados.
- **DSP**: Não alterado.
- **Analyzer FFT**: Não alterado.
- **Undo/Redo**: Não implementado.
- **Save User Preset / Presets Externos**: Não implementados.
- **Livros e PDFs**: Não alterados, não movidos.

---

## 6. Resultado do Build
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
Status: **Compilação concluída com 100% de sucesso**.

---

## 7. Resultado dos Testes
Comando executado:
```bash
./build/tests/AUREQ_FilterTests
```

Resultado:
```text
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
Status: **Aprovado. Todos os testes de DSP e estabilidade passaram**.

---

## 8. Caminho dos Artefatos Binários
- **VST3**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- **Standalone**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

---

## 9. Próximo Passo Recomendado
Validar manualmente o comportamento no standalone `AUREQ.app`:
1. Fazer uma alteração no equalizador no slot A (ex: boost nos agudos).
2. Dar clique direito no botão A/B e escolher `Copy A to B`.
3. Dar clique esquerdo para ir para o slot B e certificar que a mesma curva do slot A aparece.
4. Fazer uma alteração no slot B (ex: corte nos graves).
5. Dar clique direito e escolher `Reset B to Current` ou outras ações e verificar as transições corretas.
