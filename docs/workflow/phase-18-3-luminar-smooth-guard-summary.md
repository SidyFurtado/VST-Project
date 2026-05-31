# Phase 18.3 — LUMINAR Smooth Guard

## Status

Implementada; pronta para teste auditivo no FL Studio.

---

## 1. Objetivo

Implementar o Smooth Guard usando o parametro existente `luminar_smooth`, preservando a sonoridade aprovada na Fase 18.2F e controlando sibilancia/harshness de forma dinamica.

---

## 2. Arquitetura Implementada

O Smooth Guard foi implementado dentro do `AirProcessor` como reducao dinamica no wet bus.

Fluxo:

```text
Dry path preservado
Presence + Texture + Air wet bus
    -> Smooth Guard gain
    -> soma com dry
    -> compensacao de loudness existente
    -> Output Gain no PluginProcessor
```

O dry path nao e processado pelo Smooth Guard.

---

## 3. Detector

O detector usa uma banda de harshness/sibilancia:

- Filtro band-pass por canal em `6500 Hz`, Q `0.70`.
- Envelope follower por canal.
- Attack: aproximadamente `10 ms`.
- Release: aproximadamente `130 ms`.
- Entrada do detector: sinal dry copiado internamente antes do processamento paralelo.

Smooth 0% retorna ganho `1.0`, preservando praticamente o som da Fase 18.2F.

---

## 4. Reducao

A reducao e calculada a partir do envelope detectado:

- Threshold varia com Smooth:
  - Smooth 0%: guard desligado.
  - Smooth 50%: threshold intermediario.
  - Smooth 100%: threshold mais sensivel.
- Max reduction:
  - Smooth 50%: aproximadamente `2-3 dB`.
  - Smooth 100%: aproximadamente `5 dB`.
- Ganho final clampado entre `0.50` e `1.0`.

A reducao e aplicada somente no wet bus Presence + Texture + Air, antes da soma ao dry.

---

## 5. Arquivos Alterados

| Arquivo | Alteracao |
|---|---|
| `plugins/02_AIR/DSP/AirProcessor.h` | Assinatura passou a receber `smoothAmount`; estado interno do Smooth Guard |
| `plugins/02_AIR/DSP/AirProcessor.cpp` | Detector, envelope follower e reducao dinamica no wet bus |
| `plugins/02_AIR/Source/PluginProcessor.cpp` | `luminar_smooth` agora e passado ao `AirProcessor` |
| `plugins/02_AIR/Docs/implementation-plan.md` | Registro da Fase 18.3 |

Nao foram alterados:

- APVTS IDs.
- UI.
- CMake raiz.
- AUREQ.
- Installers.
- Livros/PDFs.
- Oversampling.
- Calibracao base Presence/Air/Texture da Fase 18.2F.

---

## 6. Validacao

- `cmake --build build`: **PASS**
- `./build/tests/AUREQ_FilterTests`: **PASS**
  - Biquad Coefficient Tests: PASS
  - EQBand Block Processing Tests: PASS
  - Filter Stability and Safety Tests: PASS

Artefatos:

- `build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
- `build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## 7. Teste Auditivo Recomendado

Reinstalar o `LUMINAR.vst3` no FL Studio e testar:

- Smooth 0% com Presence/Air nos ajustes aprovados da 18.2F.
- Smooth 50% em vocal sibilante e Air alto.
- Smooth 100% em material agressivo.
- Air 100% com Smooth 50/100.
- Presence + Air juntos com Smooth 0/50/100.

Criterios:

- Smooth 0% mantem o som aprovado da 18.2F.
- Smooth 50% controla sibilancia/aspereza sem matar brilho.
- Smooth 100% deixa material agressivo mais seguro.
- Sem pumping obvio.
- Sem clipping, crash, NaN ou Inf.
- Output continua funcionando.

---

## 8. Fases Futuras

- Guard por path separado, se o wet-bus guard ficar amplo demais.
- Indicador visual/LED do Smooth Guard ativo.
- Oversampling seletivo se aliasing ficar audivel.
- Testes unitarios especificos do LUMINAR DSP.
