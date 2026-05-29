# Fase 16.4 — Private Beta Tester Package & Feedback Kit

Data: 2026-05-29

## Objetivo

Criar documentacao de envio e coleta de feedback para testers privados do AUREQ 0.9.0-rc1 macOS beta.

## Artefato Beta

DMG para testers:

```text
installer/macOS/dmg/output/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg
```

Status:

- Build PASS.
- Testes PASS.
- PKG regenerado.
- DMG regenerado.
- VST3 atualizado.
- Spectrum/analyzer incluido.
- Top Bar polish incluido.
- Beta privada.
- Unsigned / not notarized.

## Documentos Criados

```text
docs/beta/AUREQ-0.9.0-rc1-private-beta-instructions.md
docs/beta/AUREQ-0.9.0-rc1-tester-feedback-form.md
docs/beta/AUREQ-0.9.0-rc1-known-limitations.md
```

## Conteudo Coberto

- Instrucoes de instalacao macOS via DMG/PKG.
- Caminho instalado: `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
- Rescan em DAWs.
- Aviso de beta privada unsigned/not notarized.
- DAWs alvo: FL Studio, Ableton Live, Reaper, Studio One, Cubase, Bitwig.
- Nota de que Logic Pro nao e alvo VST3 direto.
- Formulario de feedback cobrindo sistema, DAW, deteccao, UI, audio, spectrum, presets, user presets, A/B, Undo/Redo, Dynamic EQ, Channel Mode, salvar/reabrir projeto, crash/travamento e instalacao.
- Known limitations: Gatekeeper, Finder polish parcial, Windows/AU/AAX indisponiveis.

## Integridade De Escopo

Nao foram alterados:

- Codigo.
- CMake.
- APVTS.
- DSP.
- Analyzer.
- Presets.
- UI.
- PKG/DMG.
- Livros/PDFs.

Nao foi executado build.
