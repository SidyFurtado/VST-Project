# Fase 16.0 — Real-Time Spectrum Overlay Visualizer

Data: 2026-05-28

## Objetivo

Polir o spectrum analyzer em tempo real ja existente no AUREQ, tornando o overlay mais visivel, musical e confiavel dentro do `EQGraphView`, sem alterar audio, APVTS, IDs, presets, DSP principal ou packaging.

## Estado Antes

O pipeline de analyzer ja existia:

```text
processBlock -> AnalyzerFIFO -> PluginEditor::timerCallback -> AnalyzerProcessor -> EQGraphView::setSpectrumData -> paint
```

O analyzer ja era post-EQ, lock-free no transporte audio -> UI, e desenhado atras da curva de EQ.

Problema identificado:

- O decay visual multiplicava valores negativos em dB por `0.78`.
- Como dBFS e negativo, multiplicar por fator menor que 1 move o valor em direcao a `0 dB`, gerando queda visual incorreta.
- O overlay tambem estava discreto demais para uso pratico em DAW.

## Alteracoes

### Ballistics

Arquivo:

```text
plugins/01_EQ/Source/AnalyzerProcessor.cpp
```

O decay passou a ser feito no dominio de dB por subtracao:

```text
previousDb - decayDbPerFrame
```

com clamp para o floor de `-90 dBFS`.

Resultado esperado:

- Ataque visual imediato quando o sinal sobe.
- Decay natural em direcao ao floor quando o sinal cai.
- Silencio nao sobe artificialmente em direcao a `0 dB`.

### Visual

Arquivo:

```text
plugins/01_EQ/UI/EQGraphView.cpp
```

Ajustes:

- Fill cyan/mint com um pouco mais de opacidade.
- Stroke superior separado do path preenchido, evitando contorno no floor.
- Glow leve no topo do spectrum.
- Spectrum continua atras da curva de EQ e dos handles.
- Escala logaritmica 20 Hz - 20 kHz preservada.
- Range visual aproximado `-90 dB` a `0 dB` preservado.

## Integridade de Escopo

Nao foram alterados:

- APVTS layout.
- IDs.
- Parametros.
- Presets.
- DSP musical principal.
- Audio processado.
- CMake.
- Installers/PKG/DMG.
- Livros/PDFs.

Nenhum parametro novo foi criado.
Nenhum controle Pre/Post/Off foi criado nesta fase.

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

Artefatos gerados pelo build:

```text
build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3
build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app
```

Validacao manual pendente:

- Abrir Standalone ou FL Studio.
- Tocar audio.
- Confirmar spectrum visivel e acompanhando a musica.
- Confirmar que em silencio ele cai suavemente para o floor.
- Confirmar curva de EQ e handles legiveis por cima.
- Confirmar handles responsivos.
- Confirmar ausencia de crash.
- Confirmar CPU aceitavel.
- Confirmar que o som nao mudou.

## Futuro

Fica para fases futuras:

- Analyzer Pre/Post/Off.
- Analyzer stereo ou Mid/Side.
- Sensitivity/tilt visual.
- Freeze/hold.
- Sample-rate mapping adaptativo.
