# LUMINAR — ASTRA Audio Suite

**LUMINAR** é o segundo plugin da ASTRA Audio Suite — um high-frequency enhancer e air exciter original que adiciona presença, brilho e abertura a fontes musicais de forma suave e musical.

## Visão Geral

- **Produto:** LUMINAR
- **Suite:** ASTRA Audio Suite
- **Categoria:** High-Frequency Enhancer / Air Exciter
- **Versão atual:** 0.1.0 (skeleton — Fase 18.1)
- **Formatos:** VST3, Standalone
- **Plataformas:** macOS, Windows

## Controles

| Knob | Range | Default | Função |
|---|---|---|---|
| Presence | 0–100% | 40% | Presence enhancement dinâmico (~2–6 kHz) |
| Air | 0–100% | 30% | Air exciter harmônico (~8–20 kHz) |
| Smooth | 0–100% | 50% | Smooth Guard — proteção automática de sibilância |
| Output | -12 dB a +6 dB | 0 dB | Ganho de saída global |

## Estrutura de Diretórios

```
plugins/02_AIR/
├── CMakeLists.txt     — configuração JUCE
├── Source/            — PluginProcessor + ParameterIDs
├── UI/                — PluginEditor + LuminarTheme
├── DSP/               — reservado: Fase 18.2 (AirProcessor, AirWaveshaper, AirEnvelopeFollower)
├── Presets/           — reservado: Fase 18.6
├── Tests/             — reservado: Fase 18.8
└── Docs/              — implementation-plan.md
```

## Status de Desenvolvimento

- [x] Fase 18.0 — Research & Product Direction
- [x] Fase 18.1 — Plugin Skeleton (este)
- [ ] Fase 18.2 — DSP Core (AirWaveshaper + EnvelopeFollower)
- [ ] Fase 18.3 — Smooth Guard
- [ ] Fase 18.4 — UI Base refinement
- [ ] Fase 18.5 — UI Premium & Animação
- [ ] Fase 18.6 — Presets de Fábrica
- [ ] Fase 18.7 — Localization PT-BR / EN
- [ ] Fase 18.8 — Testes Unitários DSP
- [ ] Fase 18.9 — QA Musical
- [ ] Fase 18.10 — Packaging macOS + Windows

## Parâmetros APVTS

| ID APVTS | Tipo | Range | Default |
|---|---|---|---|
| `luminar_presence` | Float | 0.0 – 1.0 | 0.40 |
| `luminar_air` | Float | 0.0 – 1.0 | 0.30 |
| `luminar_smooth` | Float | 0.0 – 1.0 | 0.50 |
| `luminar_outputGain` | Float | -12.0 – +6.0 dB | 0.0 |

State ID APVTS: `LUMINAR_PARAMS`  
Plugin Code: `Lmnr`  
Bundle ID: `br.com.sidyfurtado.luminar`

## Referências de Design

- `docs/product/astra-air-product-brief.md`
- `docs/dsp-notes/air-exciter-enhancer-design-notes.md`
- `docs/workflow/phase-18-0-astra-air-research-product-direction.md`
