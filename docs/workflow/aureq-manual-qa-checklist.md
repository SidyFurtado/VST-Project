# AUREQ Manual QA Checklist

Checklist interno para a passagem de Product Readiness do AUREQ.

Status:
- `[PASS]`: validado nesta fase.
- `[PARTIAL]`: auditado por código e/ou abertura visual, mas requer clique manual humano para confirmação final.
- `[PENDING]`: não validado nesta fase.
- `[BLOCKED]`: bloqueado por ambiente ou permissão.

---

## 1. Build e Testes
- [PASS] `cmake --build build` conclui com sucesso.
- [PASS] `./build/tests/AUREQ_FilterTests` conclui com sucesso.
- [PASS] Artefato VST3 existe em `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`.
- [PASS] Artefato Standalone existe em `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`.

---

## 2. Standalone
- [PASS] Standalone abre pelo macOS.
- [PARTIAL] UI principal deve aparecer sem crash e sem erro visível.
- [PARTIAL] Fechar e reabrir deve manter user presets listáveis em Application Support.

---

## 3. Preset Browser
- [PASS] Menu de presets possui acesso direto a `Default Flat`.
- [PASS] Factory presets são organizados por categorias e submenus.
- [PASS] Factory preset load usa faixa de IDs `presetIndex + 1`.
- [PASS] Previous/Next navegam pelo vetor completo de presets de fábrica.
- [PARTIAL] Clicar em cada categoria e carregar presets deve ser validado manualmente no Standalone.

---

## 4. User Presets
- [PASS] `Save Current Preset...` está no preset browser.
- [PASS] User presets usam submenu separado `User Presets`.
- [PASS] User preset load usa faixa `20000 + userPresetIndex`.
- [PASS] Save usa XML APVTS com extensão `.aureqpreset`.
- [PASS] Refresh chama `refreshUserPresets()` sem alterar APVTS.
- [PASS] Reveal cria/revela a pasta oficial de presets.
- [PASS] Delete mostra apenas user presets e usa faixa `30000 + userPresetIndex`.
- [PASS] Delete valida índice, arquivo, extensão `.aureqpreset` e pasta oficial antes de `moveToTrash()`.
- [PARTIAL] Criar, carregar, deletar e confirmar sumiço no menu deve ser validado por clique manual no Standalone.

---

## 5. A/B Compare e A/B Copy Actions
- [PASS] Clique esquerdo no botão A/B alterna entre slots.
- [PASS] Clique direito/Ctrl-clique abre menu contextual.
- [PASS] Menu contextual expõe `Copy A to B`, `Copy B to A`, `Copy Current to Other` e `Reset B to Current`.
- [PASS] Ações de copy/reset não chamam `replaceState()` e não alteram slot ativo.
- [PARTIAL] Teste auditivo/visual com mudanças distintas em A e B deve ser validado manualmente.

---

## 6. Undo/Redo
- [PASS] Settings menu expõe `Undo` e `Redo`.
- [PASS] Itens usam `canUndo()` e `canRedo()` para enabled/disabled.
- [PASS] `Cmd/Ctrl+Z` chama Undo.
- [PASS] `Cmd/Ctrl+Shift+Z` chama Redo.
- [PASS] `Cmd/Ctrl+Y` chama Redo.
- [PASS] Helpers de UI atualizam A/B, preset label, controles, timer e repaint.
- [PARTIAL] Atalhos podem ser interceptados por host/DAW; menu Settings permanece caminho confiável.

---

## 7. EQGraphView
- [PASS] Handles respondem a `mouseDown`, `mouseDrag` e `mouseUp`.
- [PASS] Drag de handle atualiza frequência/ganho e usa callbacks de gesto.
- [PASS] Double-click em área vazia chama criação de banda.
- [PASS] Mouse wheel altera Q com hit-test de hover ou fallback para banda selecionada.
- [PASS] Clique em área vazia desseleciona a banda.
- [PARTIAL] Precisão visual e sensação de arraste devem ser confirmadas manualmente no Standalone.

---

## 8. Bottom Panel
- [PASS] Input/Output Gain capturam Undo em `onDragStart`.
- [PASS] Knobs Frequency/Gain/Q capturam Undo em `onDragStart`.
- [PASS] Dynamic EQ sliders capturam Undo em `onDragStart`.
- [PASS] Filter Type, Slopes e Channel Mode usam botões existentes.
- [PASS] Band Bypass captura Undo via `mouseDown` quando há banda selecionada.
- [PARTIAL] Verificar visualmente labels, truncamentos e estados disabled no Standalone.

---

## 9. Top Bar
- [PASS] Preset browser, Previous/Next, A/B, Settings, Theme e Global Bypass permanecem na Top Bar aprovada.
- [PASS] Global Bypass captura Undo antes do toggle.
- [PASS] Settings menu não adiciona botões grandes.
- [PARTIAL] Verificar visualmente truncamento do preset label com nomes longos.

---

## 10. Visual Premium
- [PASS] Nenhum redesign foi realizado nesta fase.
- [PASS] `EQGraphView` mantém graph card, curva, analyzer, handles, HUD, DYN badge e channel badge existentes.
- [PARTIAL] Checar screenshot/Standalone para colisões visuais, textos cortados e estados disabled.

---

## 11. Persistência e Regressão
- [PASS] User presets continuam fora do projeto/repo, em Application Support.
- [PASS] Factory presets continuam em código.
- [PASS] APVTS layout, IDs, DSP e analyzer FFT não foram alterados nesta fase.
- [PARTIAL] Fechar/reabrir Standalone e confirmar user presets no menu deve ser validado por clique manual.

---

## 12. Itens para Validação Humana Final
- Clicar por todas as categorias de factory presets.
- Criar user preset, fechar/reabrir Standalone, carregar e deletar.
- Confirmar A/B visualmente com curvas diferentes.
- Confirmar Undo/Redo após cada gesto principal.
- Verificar atalhos em Standalone e posteriormente em DAWs alvo.
- Confirmar que labels longas de user preset não degradam a Top Bar.
