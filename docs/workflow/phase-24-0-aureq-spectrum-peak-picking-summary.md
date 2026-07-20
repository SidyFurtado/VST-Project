# Fase 24.0 — AUREQ Spectrum Peak Picking / Cacador de Ressonancias

Data: 2026-06-04

## Objetivo

Adicionar marcadores visuais de picos/resonancias ao analyzer FFT do AUREQ, usando os bins ja existentes e mantendo a feature 100% visual.

## Arquitetura

O pipeline de analyzer permanece:

```text
processBlock -> AnalyzerFIFO -> PluginEditor::timerCallback -> AnalyzerProcessor -> EQGraphView::setSpectrumData -> paint
```

A deteccao de picos foi implementada em `EQGraphView::setSpectrumData()`, apos a copia dos bins para `spectrumBins`.

Nao foram adicionados:

- FFT nova.
- Transporte audio/UI novo.
- FIFO novo.
- Locks.
- Alocacao na audio thread.
- Parametros APVTS.

## Algoritmo

Regras implementadas:

- Entrada: bins FFT em dBFS vindos do `AnalyzerProcessor`.
- Threshold minimo: `-60 dB`.
- Faixa analisada: aproximadamente `40 Hz` a `18 kHz`.
- Maximo local: `bin[i] > bin[i - 1]` e `bin[i] > bin[i + 1]`.
- Proeminencia minima: `6 dB` acima da media local.
- Janela local: `+/- 8` bins.
- Ordenacao por proeminencia e magnitude.
- Limite final: ate 5 picos.
- Supressao de picos proximos: distancia minima de `1/6` de oitava.

## Persistencia Temporal

Cada marcador visual mantem:

- `frequencyHz`
- `magnitudeDb`
- `normalizedX`
- `normalizedY`
- `alpha`
- `holdFrames`
- `ageFrames`
- `active`

O matching entre frames e feito por proximidade logaritmica de frequencia, com tolerancia de `1/5` de oitava. Marcadores que reaparecem perto do frame anterior sao suavizados; marcadores ausentes seguram por alguns frames e depois desaparecem em fade out.

## Visual

Os marcadores sao desenhados no `EQGraphView`:

- Apos o spectrum analyzer.
- Antes da curva de EQ e dos handles.
- Sem hit-test.
- Sem callbacks.
- Sem interferencia em drag, double-click, wheel ou context menu de bandas.

Cada marcador usa uma bolinha pequena com glow discreto e label curta de frequencia (`Hz` ou `kHz`).

## Integridade de Escopo

Alterados:

- `plugins/01_EQ/UI/EQGraphView.h`
- `plugins/01_EQ/UI/EQGraphView.cpp`
- `plugins/01_EQ/Docs/implementation-plan.md`

Criado:

- `docs/workflow/phase-24-0-aureq-spectrum-peak-picking-summary.md`

Nao alterados:

- `PluginProcessor.*`
- `PluginEditor.*`
- `AnalyzerFIFO.*`
- `AnalyzerProcessor.*`
- CMake.
- DSP sonoro.
- APVTS.
- ParameterIDs.
- Presets.
- LUMINAR.
- GRAVITY.
- Installers.
- Site/download docs.
- GitHub Actions/workflows.

## Validacao

Obrigatoria:

```bash
cmake --build build
./build/tests/AUREQ_FilterTests
```

Status inicial: pendente no momento de criacao deste resumo.

Resultado executado:

```text
cmake --build build — PASS
./build/tests/AUREQ_FilterTests — PASS
```

Observacao: o build ainda emite warnings preexistentes de deprecation em `juce::AudioParameterFloat`; a Fase 24.0 nao alterou esses parametros.

## Validacao Manual

Pendente:

- Abrir AUREQ.
- Tocar material com picos evidentes.
- Confirmar ate 5 marcadores.
- Confirmar fade in/out.
- Confirmar ausencia de flicker exagerado.
- Confirmar que handles continuam arrastaveis.
- Confirmar ausencia de estalos, crash, CPU alta ou alteracao sonora.

## Riscos Restantes

- O mapeamento bin->frequencia ainda usa `44100 Hz` como referencia visual fixa, seguindo o analyzer atual. Em fase futura, o sample rate real pode ser exposto ao editor de forma segura.
- A calibracao visual de threshold/proeminencia pode precisar de ajuste fino apos teste musical real em DAWs.

## Proxima Fase Recomendada

Validar manualmente a Fase 24.0 no AUREQ e, depois, planejar a Fase 25.0 — Spectrum Grab, onde os picos detectados poderao virar sugestoes ou criacao assistida de bandas.

## Status

PARTIAL: implementacao e validacao automatizada concluidas com sucesso; validacao manual em DAW/Standalone ainda pendente.
