# Fase 15.2D — DAW Validation Manual Documentation Summary

Data: 2026-05-28

## Objetivo

Criar documentacao para validacao manual do AUREQ em DAWs apos a etapa de instalacao local macOS.

Esta fase nao executa build, nao altera binarios e nao modifica o PKG. O objetivo e fornecer um checklist claro para validar o plugin instalado no host.

## Contexto Herdado da Fase 15.2C

- A tentativa de `sudo installer` da Fase 15.2C-R1 ficou bloqueada porque a autenticacao sudo nao foi concluida nesta sessao.
- Ja existe um bundle instalado em `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
- Ja existe receipt `br.com.sidyfurtado.aureq.vst3` com version `0.9.0`.
- A validacao manual em DAW pode seguir usando o plugin atualmente instalado.
- A validacao do PKG novo deve ser repetida manualmente no Terminal com senha administrativa.

Comando para repetir manualmente a validacao do PKG novo:

```bash
sudo installer -pkg "installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg" -target /
```

## Artefato Criado

Checklist manual:

```text
docs/workflow/phase-15-2d-daw-validation-manual-checklist.md
```

O checklist cobre:

- Ableton Live.
- Reaper.
- Qualquer DAW disponivel.
- Rescan de plugins.
- Confirmacao do AUREQ como VST3.
- Carregamento em track.
- Abertura da UI.
- Passagem de audio.
- Preset browser.
- User presets.
- A/B.
- Undo/Redo.
- Dynamic EQ.
- Channel Mode.
- Salvamento e reabertura de sessao.
- Observacao de crashes e logs.

## Status

Status documental da Fase 15.2D: **CONCLUIDA**.

Status da validacao manual em DAW: **BLOCKED / pendente de execucao humana**.

## Integridade de Escopo

Nao foram alterados:

- Codigo C++.
- CMake.
- APVTS.
- DSP.
- Analyzer FFT.
- Presets.
- UI interna.
- PKG gerado.
- Livros/PDFs.

Nao foi executado build por padrao.

## Proximo Passo

Executar a validacao manual em DAW usando o checklist da Fase 15.2D. Depois, repetir a instalacao do PKG novo em Terminal interativo com senha administrativa para fechar a pendencia da Fase 15.2C-R1.
