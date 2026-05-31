# Phase 18.2B — LUMINAR Air Stage Musical Calibration QA

## Status

**BLOCKED** — aguardando teste auditivo manual com audio real.

---

## 1. Objetivo da Fase

Preparar a validacao musical do Air Stage atual do LUMINAR antes de implementar Presence DSP, Smooth Guard ou qualquer ajuste de calibracao.

Esta fase e documental/QA. Nenhum codigo DSP, APVTS, UI, AUREQ ou installer foi alterado.

---

## 2. Contexto Tecnico

A Fase 18.2 implementou:

- Air path paralelo.
- HPF inicial em torno de 8 kHz.
- Waveshaper `tanh` normalizado.
- HPF pos-shaper em torno de 6.5 kHz.
- Mix conservador controlado por `luminar_air`.
- Output Gain preservado apos o Air Stage.

Validacao automatizada anterior:

- `cmake --build build`: PASS.
- `./build/tests/AUREQ_FilterTests`: PASS.
- AUREQ intacto.

---

## 3. Artefatos de QA Criados

- `docs/workflow/luminar-air-stage-musical-qa-checklist.md`
- `docs/workflow/phase-18-2b-luminar-air-stage-musical-calibration-qa-summary.md`

---

## 4. Fontes Obrigatorias Para Teste

- Vocal masculino.
- Vocal feminino.
- Rap vocal.
- Acoustic guitar.
- Piano/keys.
- Drum loop/overhead.
- Mix bus simples.
- Master/mix completo, se disponivel.

---

## 5. Checks Por Fonte

Cada fonte deve ser avaliada com:

- Air 0%.
- Air 30%.
- Air 60%.
- Air 100%.
- Output Gain.
- Clipping.
- NaN/Inf/crash.
- Harshness.
- Sibilancia.
- Volume jump.
- Preservacao de grave/medio.
- Status `PASS` / `PARTIAL` / `FAIL` / `BLOCKED`.
- Observacoes musicais.

---

## 6. Ajustes Permitidos Somente Depois Da QA Auditiva

Nenhum ajuste foi aplicado nesta fase. Caso a QA indique necessidade, uma fase posterior podera calibrar:

- `mix max`.
- curva `pow` do knob Air.
- `drive max`.
- HPF inicial.
- `postShapeGain`.
- HPF pos-shaper.

Qualquer ajuste de codigo devera ser aprovado separadamente e seguido por:

- `cmake --build build`.
- `./build/tests/AUREQ_FilterTests`.

---

## 7. Criterios Para Avancar Para 18.3 Smooth Guard

- Air 0% transparente.
- Air 30% musical como default.
- Air 60% claramente util.
- Air 100% forte, mas sem clipar/explodir volume.
- Problemas restantes sao principalmente sibilancia/harshness dinamica, adequados para Smooth Guard.
- AUREQ intacto.

---

## 8. Proximo Passo

Executar teste auditivo manual com material real usando:

- `build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
- `build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

Registrar os resultados no checklist antes de aprovar qualquer calibracao de DSP ou avancar para a Fase 18.3.
