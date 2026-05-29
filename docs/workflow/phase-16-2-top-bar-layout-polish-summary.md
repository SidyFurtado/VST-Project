# Fase 16.2 — Top Bar Layout Polish

Data: 2026-05-29

## Objetivo

Corrigir apenas o layout horizontal da Top Bar do AUREQ para evitar colisao visual entre preset browser, `next`, A/B, Reset e botoes da direita, preservando a estetica premium e sem alterar comportamento de audio ou logica funcional.

## Causa

O layout anterior misturava:

- Preset browser centralizado com largura fixa.
- Grupo direito posicionado por offsets absolutos.
- Ausencia de area segura calculada entre branding esquerdo e botoes direitos.
- Ausencia de gap explicito entre `nextPresetBtn`, A/B e Reset.

Em hosts/escala/larguras especificas, isso deixava `>` muito perto de A/B e A/B muito perto de Reset.

## Estrategia Aplicada

Arquivo alterado:

```text
plugins/01_EQ/UI/PluginEditor.cpp
```

Alteracoes:

- Layout da Top Bar continua em `AureqAudioProcessorEditor::resized()`.
- Botao direito agora e calculado de tras para frente.
- Gaps fixos de `8px` entre botoes.
- Larguras fixas para controles compactos:
  - previous/next: `26px`
  - A/B: `44px`
  - Reset: `54px`
  - Settings/Config.: `72px`
  - Theme/Tema: `62px`
  - Bypass: `70px`
- Preset browser usa apenas a area segura entre branding esquerdo e grupo direito.
- `presetLabel` encolhe primeiro se faltar espaco.
- Botoes mantem hit targets e nao colidem.

## Integridade de Escopo

Nao foram alterados:

- APVTS.
- IDs.
- DSP.
- Analyzer FFT.
- Presets.
- User presets.
- Packaging/installers.
- CMake.
- Livros/PDFs.
- Logica de audio.
- Logica de preset browser.
- Logica de A/B.
- Logica de Reset, Settings, Theme ou Bypass.

## Validacao

Validacao executada:

```bash
cmake --build build
./build/tests/AUREQ_FilterTests
```

Resultado:

```text
cmake --build build — PASS
./build/tests/AUREQ_FilterTests — PASS
```

Validacao manual recomendada:

- Abrir Standalone.
- Conferir PT-BR e English.
- Conferir area preset/next/A-B/reset.
- Abrir preset browser.
- Testar previous/next.
- Testar A/B.
- Testar Reset.
- Testar Settings/Config.
- Testar Theme.
- Testar Bypass.
- Abrir no FL Studio e confirmar que nao ha overlap visual.
