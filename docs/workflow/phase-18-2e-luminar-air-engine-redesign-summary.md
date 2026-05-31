# Phase 18.2E — LUMINAR Air Engine Redesign

## Status

Implementada; pronta para novo teste auditivo no FL Studio.

---

## 1. Objetivo

Redesenhar o motor sonoro do LUMINAR para que Presence e Air fiquem realmente audiveis, musicais e premium, antes de avancar para Smooth Guard.

---

## 2. Por Que O Motor Anterior Estava Fraco

O motor anterior dependia quase exclusivamente de uma banda muito alta:

- HPF em `8 kHz`.
- Waveshaper `tanh` normalizado.
- HPF pos-shaper em `6.5 kHz`.
- Mix paralelo.
- Presence ainda inativo.

Em material real, especialmente vocal e rap, pode haver pouca energia acima de 8 kHz. Alem disso, o `tanh` normalizado em uma banda estreita pode soar mais como compressao de topo do que como abertura/textura. Faltava uma camada de presenca em 2.5-7 kHz para trazer voz, inteligibilidade e textura para frente.

---

## 3. Nova Arquitetura Implementada

Foi implementado um Dual Parallel Enhancement Engine dentro de `AirProcessor`.

```text
Dry path preservado
    |
    +--> Presence path
    |       BPF ~4.2 kHz
    |       blend de banda limpa + textura tanh leve
    |       mix controlado por luminar_presence
    |
    +--> Air path
            HPF ~6.8 kHz
            tanh waveshaper mais audivel
            blend com highs filtrados
            HPF pos-shaper ~6.2 kHz
            mix controlado por luminar_air

Soma paralela protegida
Compensacao leve de loudness
Output Gain existente no final
```

---

## 4. Presence Agora Atua

`luminar_presence` deixou de ser apenas UI/APVTS e passou a controlar uma camada paralela de presenca.

Calibracao:

- Filtro band-pass: `4200 Hz`, Q `0.72`.
- Drive: `1.10 + presence * 1.90`.
- Mix: `0.34 * pow(presence, 0.80)`.
- Wet gain interno: `2.2`.
- Blend: `55%` banda limpa + `45%` textura saturada.

Objetivo: vocal/rap mais na frente, mais inteligibilidade e textura, sem depender do Air para resolver a faixa media-alta.

---

## 5. Air Foi Reforcado

`luminar_air` continua controlando a camada de brilho/ar, mas com calibracao mais audivel.

Calibracao:

- HPF inicial: `6800 Hz`.
- HPF pos-shaper: `6200 Hz`.
- Drive: `1.35 + air * 3.10`.
- Mix: `0.60 * pow(air, 0.75)`.
- Wet gain interno: `2.4`.
- Blend antes do pos-HPF: `shaped + filteredHighs * 0.35`.
- `AirWaveshaper` agora permite drive ate `5.0`.

Objetivo: Air 30% audivel, Air 60% claro/bonito e Air 100% forte/obvio para teste demo.

---

## 6. Protecoes

- `juce::ScopedNoDenormals` mantido.
- Inputs de parametro clampados em `[0, 1]`.
- Waveshaper mantem protecao contra NaN/Inf.
- Drive do waveshaper clampado.
- Wet path clampado antes da soma.
- Saida interna clampada em `[-4.0, +4.0]`.
- Compensacao leve de loudness: ate cerca de `-1.6 dB` conforme intensidade de Presence/Air.
- Smooth continua sem efeito nesta fase.
- Output Gain continua no final.

---

## 7. Arquivos Alterados

| Arquivo | Alteracao |
|---|---|
| `plugins/02_AIR/DSP/AirProcessor.h` | Assinatura passou a receber Presence + Air; novos filtros/smoothers internos |
| `plugins/02_AIR/DSP/AirProcessor.cpp` | Dual Parallel Enhancement Engine |
| `plugins/02_AIR/DSP/AirWaveshaper.h` | Drive maximo interno ampliado para `5.0` |
| `plugins/02_AIR/Source/PluginProcessor.cpp` | `luminar_presence` agora e passado ao `AirProcessor` |
| `plugins/02_AIR/Docs/implementation-plan.md` | Registro da Fase 18.2E |

Nao foram alterados:

- APVTS IDs.
- UI.
- CMake raiz.
- AUREQ.
- Installers.
- Livros/PDFs.
- Smooth Guard.
- Oversampling.

---

## 8. Validacao

- `cmake --build build`: **PASS**
- `./build/tests/AUREQ_FilterTests`: **PASS**
  - Biquad Coefficient Tests: PASS
  - EQBand Block Processing Tests: PASS
  - Filter Stability and Safety Tests: PASS
- `git diff -- plugins/01_EQ`: sem alteracoes.

Artefatos:

- `build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
- `build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## 9. Criterios Para O Proximo Teste Auditivo

- Presence 0% + Air 0% = transparente.
- Presence 50% = vocal/rap mais na frente.
- Air 30% = diferenca audivel.
- Air 60% = brilho bonito e claro.
- Air 100% = efeito forte e obvio.
- Presence + Air juntos = abertura, textura e sensacao premium.
- Se houver sibilancia moderada, documentar para 18.3 Smooth Guard.
- Se ainda for sutil demais, considerar FAIL e nova decisao de motor.

---

## 10. O Que Fica Para 18.3 Smooth Guard

- Detector real de harshness/sibilancia.
- Atenuacao dinamica em 4-8 kHz.
- Protecao automatica para Air/Presence altos.
- Possivel oversampling seletivo se aliasing se tornar audivel.
