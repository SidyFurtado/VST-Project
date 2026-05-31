# Phase 18.2D — LUMINAR Air Stage Audibility Calibration

## Status

Implementada; aguardando novo teste auditivo no FL Studio.

---

## 1. Objetivo

Tornar o Air Stage do LUMINAR claramente audivel e musical, mantendo Air 0% transparente e sem criar parametros novos.

---

## 2. Diagnostico

O Air Stage estava sendo chamado corretamente, mas a calibracao da Fase 18.2 estava conservadora demais para material real em caixas de referencia.

Valores antigos:

| Item | Valor antigo |
|---|---|
| HPF inicial | `8000 Hz` |
| HPF pos-shaper | `6500 Hz` |
| `postShapeGain` | `0.62` |
| Drive | `1.05 + air * 1.35` |
| Mix | `0.20 * pow(air, 1.25)` |

Impacto provavel:

- Air 30% retornava wet signal baixo demais.
- Air 100% ainda ficava conservador por causa do `postShapeGain` e dos filtros.
- Resultado auditivo reportado: diferenca nao perceptivel no FL Studio em caixas de referencia.

---

## 3. Calibracao Aplicada

| Item | Valor novo |
|---|---|
| HPF inicial | `8000 Hz` |
| HPF pos-shaper | `6500 Hz` |
| `postShapeGain` | `1.25` |
| Drive | `1.20 + air * 2.80` |
| Mix | `0.40 * pow(air, 0.85)` |

Nao foram alterados:

- APVTS.
- IDs de parametros.
- UI.
- CMake.
- AUREQ.
- Installers.
- Presence DSP.
- Smooth Guard.
- Oversampling.

---

## 4. Arquivos Alterados

| Arquivo | Alteracao |
|---|---|
| `plugins/02_AIR/DSP/AirProcessor.cpp` | Calibracao de drive, mix e ganho pos-shaper |
| `plugins/02_AIR/Docs/implementation-plan.md` | Registro documental da Fase 18.2D |

---

## 5. Criterios Auditivos Para O Proximo Teste

- Air 0% transparente.
- Air 30% perceptivel em vocal seco.
- Air 60% claramente util.
- Air 100% forte/demo, mas sem clipar.
- Output continua funcionando.
- Sem crash.
- Se ficar levemente sibilante, documentar para 18.3 Smooth Guard.
- Se ficar aspero demais, documentar para nova calibracao.

---

## 6. Validacao

- `cmake --build build`: **PASS**
- `./build/tests/AUREQ_FilterTests`: **PASS**
  - Biquad Coefficient Tests: PASS
  - EQBand Block Processing Tests: PASS
  - Filter Stability and Safety Tests: PASS

AUREQ:

- `git diff -- plugins/01_EQ` nao mostrou alteracoes.

Artefatos gerados:

- `build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
- `build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## 7. Proximo Passo

Reinstalar/testar o novo `LUMINAR.vst3` no FL Studio e repetir a QA auditiva:

- Air 0% transparente.
- Air 30% perceptivel em vocal seco.
- Air 60% claramente util.
- Air 100% forte/demo, mas sem clipar.

Se ainda estiver fraco, considerar nova calibracao posterior. Se ficar sibilante/harsh de forma dinamica, documentar para a Fase 18.3 Smooth Guard.
