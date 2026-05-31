# LUMINAR Air Stage — Musical QA Checklist

## Status

**BLOCKED** ate teste auditivo manual com audio real.

---

## Objetivo

Validar musicalmente o Air Stage atual do LUMINAR antes de qualquer calibracao de DSP, Presence DSP ou Smooth Guard.

Artefatos para teste:

- `build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
- `build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## Procedimento Geral

Para cada fonte:

1. Ajustar Output para `0 dB`.
2. Manter Presence e Smooth no default ou em qualquer posicao conveniente, pois ainda nao devem afetar o audio nesta fase.
3. Testar Air em `0%`, `30%`, `60%` e `100%`.
4. Comparar o resultado com ganho percebido compensado quando necessario.
5. Registrar status: `PASS`, `PARTIAL`, `FAIL` ou `BLOCKED`.

Legenda sugerida:

- `PASS`: musical e seguro.
- `PARTIAL`: promissor, mas precisa calibracao pequena.
- `FAIL`: harsh, fraco demais, bugado, clipando ou instavel.
- `BLOCKED`: fonte ou condicao de teste indisponivel.

---

## 1. Vocal Masculino

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED |  |
| Air 30% adiciona brilho sutil | BLOCKED |  |
| Air 60% fica perceptivel e util | BLOCKED |  |
| Air 100% forte sem clipar/explodir volume | BLOCKED |  |
| Output Gain funciona | BLOCKED |  |
| Sem clipping | BLOCKED |  |
| Sem NaN/Inf/crash | BLOCKED |  |
| Sem harshness excessiva | BLOCKED |  |
| Sem sibilancia exagerada | BLOCKED |  |
| Sem volume jump enganoso demais | BLOCKED |  |
| Grave/medio preservados | BLOCKED |  |
| Status da fonte | BLOCKED |  |

## 2. Vocal Feminino

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED |  |
| Air 30% adiciona brilho sutil | BLOCKED |  |
| Air 60% fica perceptivel e util | BLOCKED |  |
| Air 100% forte sem clipar/explodir volume | BLOCKED |  |
| Output Gain funciona | BLOCKED |  |
| Sem clipping | BLOCKED |  |
| Sem NaN/Inf/crash | BLOCKED |  |
| Sem harshness excessiva | BLOCKED |  |
| Sem sibilancia exagerada | BLOCKED |  |
| Sem volume jump enganoso demais | BLOCKED |  |
| Grave/medio preservados | BLOCKED |  |
| Status da fonte | BLOCKED |  |

## 3. Rap Vocal

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED |  |
| Air 30% adiciona brilho sutil | BLOCKED |  |
| Air 60% fica perceptivel e util | BLOCKED |  |
| Air 100% forte sem clipar/explodir volume | BLOCKED |  |
| Output Gain funciona | BLOCKED |  |
| Sem clipping | BLOCKED |  |
| Sem NaN/Inf/crash | BLOCKED |  |
| Sem harshness excessiva | BLOCKED |  |
| Sem sibilancia exagerada | BLOCKED |  |
| Sem volume jump enganoso demais | BLOCKED |  |
| Grave/medio preservados | BLOCKED |  |
| Status da fonte | BLOCKED |  |

## 4. Acoustic Guitar

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED |  |
| Air 30% adiciona brilho sutil | BLOCKED |  |
| Air 60% fica perceptivel e util | BLOCKED |  |
| Air 100% forte sem clipar/explodir volume | BLOCKED |  |
| Output Gain funciona | BLOCKED |  |
| Sem clipping | BLOCKED |  |
| Sem NaN/Inf/crash | BLOCKED |  |
| Sem harshness excessiva | BLOCKED |  |
| Sem sibilancia exagerada | BLOCKED |  |
| Sem volume jump enganoso demais | BLOCKED |  |
| Grave/medio preservados | BLOCKED |  |
| Status da fonte | BLOCKED |  |

## 5. Piano/Keys

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED |  |
| Air 30% adiciona brilho sutil | BLOCKED |  |
| Air 60% fica perceptivel e util | BLOCKED |  |
| Air 100% forte sem clipar/explodir volume | BLOCKED |  |
| Output Gain funciona | BLOCKED |  |
| Sem clipping | BLOCKED |  |
| Sem NaN/Inf/crash | BLOCKED |  |
| Sem harshness excessiva | BLOCKED |  |
| Sem sibilancia exagerada | BLOCKED |  |
| Sem volume jump enganoso demais | BLOCKED |  |
| Grave/medio preservados | BLOCKED |  |
| Status da fonte | BLOCKED |  |

## 6. Drum Loop/Overhead

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED |  |
| Air 30% adiciona brilho sutil | BLOCKED |  |
| Air 60% fica perceptivel e util | BLOCKED |  |
| Air 100% forte sem clipar/explodir volume | BLOCKED |  |
| Output Gain funciona | BLOCKED |  |
| Sem clipping | BLOCKED |  |
| Sem NaN/Inf/crash | BLOCKED |  |
| Sem harshness excessiva | BLOCKED |  |
| Sem sibilancia exagerada | BLOCKED |  |
| Sem volume jump enganoso demais | BLOCKED |  |
| Grave/medio preservados | BLOCKED |  |
| Status da fonte | BLOCKED |  |

## 7. Mix Bus Simples

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED |  |
| Air 30% adiciona brilho sutil | BLOCKED |  |
| Air 60% fica perceptivel e util | BLOCKED |  |
| Air 100% forte sem clipar/explodir volume | BLOCKED |  |
| Output Gain funciona | BLOCKED |  |
| Sem clipping | BLOCKED |  |
| Sem NaN/Inf/crash | BLOCKED |  |
| Sem harshness excessiva | BLOCKED |  |
| Sem sibilancia exagerada | BLOCKED |  |
| Sem volume jump enganoso demais | BLOCKED |  |
| Grave/medio preservados | BLOCKED |  |
| Status da fonte | BLOCKED |  |

## 8. Master/Mix Completo

| Check | Resultado | Observacoes musicais |
|---|---|---|
| Air 0% praticamente transparente | BLOCKED | Se disponivel |
| Air 30% adiciona brilho sutil | BLOCKED | Se disponivel |
| Air 60% fica perceptivel e util | BLOCKED | Se disponivel |
| Air 100% forte sem clipar/explodir volume | BLOCKED | Se disponivel |
| Output Gain funciona | BLOCKED | Se disponivel |
| Sem clipping | BLOCKED | Se disponivel |
| Sem NaN/Inf/crash | BLOCKED | Se disponivel |
| Sem harshness excessiva | BLOCKED | Se disponivel |
| Sem sibilancia exagerada | BLOCKED | Se disponivel |
| Sem volume jump enganoso demais | BLOCKED | Se disponivel |
| Grave/medio preservados | BLOCKED | Se disponivel |
| Status da fonte | BLOCKED | Se disponivel |

---

## Criterios Para Seguir Para 18.3 Smooth Guard

- Air 0% transparente.
- Air 30% musical como default.
- Air 60% claramente util.
- Air 100% forte, mas sem clipar ou explodir volume.
- Problemas restantes sao principalmente sibilancia/harshness dinamica, adequados para Smooth Guard.
- AUREQ intacto.

---

## Ajustes Permitidos Somente Em Fase Posterior

Depois da QA auditiva, se necessario e com nova aprovacao:

- `mix max`
- curva `pow` do knob Air
- `drive max`
- HPF inicial
- `postShapeGain`
- HPF pos-shaper
