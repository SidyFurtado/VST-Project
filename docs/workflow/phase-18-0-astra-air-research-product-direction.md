# Phase 18.0 — ASTRA AIR: Research & Product Direction

> **Status:** Planejamento — aguardando aprovação para implementação  
> **Plugin:** ASTRA AIR (nome provisório)  
> **Suite:** ASTRA Audio Suite  
> **Fase anterior:** Phase 17.1 (GitHub Actions Windows Build + Inno Installer Pipeline)  
> **Data:** 2026-05-31  
> **Regra crítica:** Nenhum código, CMake, APVTS, AUREQ, installers, Livros/PDFs foram alterados.

---

## 1. Contexto

O **AUREQ 0.9.0-rc1** está em beta privado com testers. Enquanto o feedback de QA musical é coletado, iniciamos o planejamento do segundo plugin da ASTRA Audio Suite.

Este documento registra a pesquisa técnica, a direção de produto, a arquitetura DSP recomendada e o plano de implementação em fases do **ASTRA AIR** — um high-frequency enhancer / air exciter original.

---

## 2. Referência Musical Estudada

**Referência:** Slate Digital Fresh Air  
**Aviso:** Não foi feito reverse engineering. Não será copiado nome, UI, marca, claims ou algoritmo proprietário.  
**Fonte de estudo:** Descrição pública do produto na página oficial do fabricante.

### O que o Fresh Air promete musicalmente (análise de marketing):
- "Dynamic high frequency processor"
- Baseado em "vintage exciter circuits" e "advanced dynamics processing"
- Promessa: adicionar high-end suave, brilliance, shine, clarity e shimmer
- Dois controles principais (Mid Air e High Air)
- Útil em vocals, drums, individual tracks e mix bus
- "Adicionar brilho sem harshness/brittleness"

### O que aprendemos com essa análise:
- O conceito de **dois controles de banda** (médio vs. alto) é musicalmente eficaz e pedagogicamente claro.
- A promessa de "sem harshness" indica processamento dinâmico ou proteção automática — não shelf simples.
- O mercado quer um plugin que "apenas funcione" sem necessidade de configuração complexa.
- O nome "Air" como conceito de produto é genericável — não pertence a nenhuma marca.

---

## 3. Estudo de Função Musical (Sem Copiar)

### Por que "ar" é desejável em áudio:
O conceito de "ar" em mixagem refere-se à percepção de espaço, abertura e presença nas frequências acima de 8 kHz. Fontes musicais gravadas frequentemente perdem energia nessa região por:
- Microfones com roll-off natural de alta frequência
- Compressão excessiva durante gravação
- Processamento em cadeia que satura as altas discretamente
- Acústica de sala que absorve altas frequências

O resultado é um som "fechado", "caixa", "longe". O objetivo do plugin é restaurar/criar essa percepção de abertura de forma musical e controlada.

### Como o brilho é percebido:
- **2–4 kHz:** presença, inteligibilidade vocal, articulação de instrumentos
- **4–8 kHz:** brilho, "sizzle", definição
- **8–12 kHz:** ar genuíno, "shimmer", abertura espacial
- **12–20 kHz:** "sparkle", sensação de espaço físico, microfone caro

### O diferencial de um exciter vs EQ:
Um high shelf cria percepção de "mais agudo". Um exciter cria percepção de "mais espaço". A diferença psicoacústica é real: harmônicos gerados por não-linearidade são interpretados pelo cérebro como "presença espacial", não apenas como "volume em agudos".

---

## 4. Arquitetura DSP Recomendada — Hybrid (C)

Após avaliação das três arquiteturas candidatas, a **Arquitetura C — Hybrid** foi selecionada.

### Diagrama do pipeline:

```
┌─────────────────────────────────────────────────────────────────┐
│                    ASTRA AIR — Pipeline v1                      │
└─────────────────────────────────────────────────────────────────┘

Entrada (L/R) ──────────────────────────── Dry Path (bypass) ─────────────────────┐
      │                                                                             │
      ├─── PRESENCE STAGE ───────────────────────────────────────────────────────  │
      │    │                                                                        │
      │    ├── Envelope Follower (3–8kHz) → Smooth Guard → Gain Modulation         │
      │    │                                                                        │
      │    └── Dynamic Bell/Shelf (fc ~3kHz, Q ~0.6) → Gain controlled by:         │
      │         - Presence knob (static offset)                                    │
      │         - Envelope modulation (upward dynamic)                             │
      │         - Smooth Guard (downward limit on harshness)                       │
      │                                                                            │
      │    Result: +0 to +4dB on presence band, dynamically controlled ──────────  Σ
      │                                                                             │
      └─── AIR STAGE ────────────────────────────────────────────────────────────  │
           │                                                                        │
           ├── HPF BiquadFilter (fc ~8kHz, Butterworth 2nd order)                  │
           │                                                                        │
           ├── Waveshaper (tanh normalized, drive internal 1.5–2.5x)               │
           │   Input clamp → tanh(drive*x)/tanh(drive) → output                   │
           │                                                                        │
           ├── Post-saturação HPF (fc ~6kHz) — remove sub-harmônicos              │
           │                                                                        │
           ├── Output gain compensation (auto, -3 to -6dB post-sat)               │
           │                                                                        │
           └── Air Mix Gain (Air knob, 0–100%) ────────────────────────────────── Σ
                                                                                    │
                                                                             [Output Gain]
                                                                                    │
                                                                                  Saída
```

### Classes DSP necessárias (novas para ASTRA AIR):

| Classe | Localização proposta | Descrição |
|---|---|---|
| `AirProcessor` | `plugins/02_AIR/DSP/AirProcessor.h/.cpp` | Núcleo do pipeline Air + Presence |
| `AirWaveshaper` | `plugins/02_AIR/DSP/AirWaveshaper.h/.cpp` | Waveshaper tanh normalizado com proteção |
| `AirEnvelopeFollower` | `plugins/02_AIR/DSP/AirEnvelopeFollower.h/.cpp` | Envelope para Smooth Guard e dynamic presence |
| `AirParameterIDs` | `plugins/02_AIR/Source/ParameterIDs.h/.cpp` | IDs APVTS isolados do AUREQ |

### Reutilização do shared/:

| Componente shared | Uso no ASTRA AIR |
|---|---|
| `BiquadFilter` | HPF split e Presence shelf/bell |
| `ParameterSmoother` | Suavização dos 4 knobs |
| `shared/gui/` componentes | Knobs, tema, layout base |

---

## 5. Parâmetros APVTS — Versão Musical v1

Total: **4 parâmetros** (mínimo viável, máximo musical)

| ID | Tipo | Range | Default | Descrição |
|---|---|---|---|---|
| `"PRESENCE"` | Float | 0.0 – 1.0 | 0.40 | Intensidade do presence enhancer dinâmico |
| `"AIR"` | Float | 0.0 – 1.0 | 0.30 | Mix do exciter harmônico de alta frequência |
| `"SMOOTH"` | Float | 0.0 – 1.0 | 0.50 | Agressividade do Smooth Guard |
| `"OUTPUT_GAIN_DB"` | Float | -12.0 – +6.0 | 0.0 | Ganho de saída global (dB) |

> **Nota:** IDs serão diferentes dos IDs do AUREQ para zero risco de conflito de automação.

---

## 6. UI — Direção Visual

### Identidade:
- Coerente com a linguagem **Premium Glass & Cosmic Navy** do AUREQ.
- **Diferente:** sem EQ graph, sem handles de banda. A UI deve comunicar "ar, luz, espaço".
- Dimensões: **800 × 420 px** (mais compacto que o AUREQ — plugin de propósito único).

### Elementos visuais propostos:
- **Display central:** visualização abstrata de "aura/shimmer" — ondas de luz ou partículas — que reage aos knobs (não é analyzer FFT). Apenas cosmético/reativo. Implementado depois da v1 core.
- **Knobs grandes:** 4 knobs principais, centrais, em layout horizontal.
- **Top Bar:** identidade ASTRA AIR / AURORA / LUMINA com tipografia premium.
- **Sem preset browser complexo:** dropdown simples.
- **Indicadores de Smooth Guard:** LED ou animação sutil que pisca/acende quando o guard está ativo.

### Inspiração estética:
- Aurora boreal → gradientes de azul-índigo para ciano e mint no display
- "Raytraced light" — linhas de luz vertical suave no fundo, simulando raios de sol
- Glassmorphism nos painéis dos knobs (mesmo padrão AUREQ)

---

## 7. Presets de Fábrica — Lista Completa v1

### Vocal (6):
1. `Lead Vocal — Natural` — Presence 40%, Air 25%, Smooth 50%
2. `Lead Vocal — Pop Shine` — Presence 60%, Air 40%, Smooth 55%
3. `Lead Vocal — Smooth & Silky` — Presence 50%, Air 20%, Smooth 80%
4. `Rap Vocal — Cut Through` — Presence 70%, Air 35%, Smooth 30%
5. `Backing Vocal — Subtle` — Presence 25%, Air 20%, Smooth 60%
6. `Backing Vocal — Airy Chorus` — Presence 20%, Air 55%, Smooth 65%

### Instrumentos (5):
7. `Acoustic Guitar — Strings` — Presence 45%, Air 30%, Smooth 45%
8. `Acoustic Guitar — Full Shimmer` — Presence 55%, Air 50%, Smooth 50%
9. `Piano — Brilliance` — Presence 30%, Air 45%, Smooth 55%
10. `Piano — Grand Shimmer` — Presence 50%, Air 60%, Smooth 60%
11. `Keys / Synth Air` — Presence 15%, Air 65%, Smooth 50%

### Drums (3):
12. `Drum Bus — Top End` — Presence 35%, Air 40%, Smooth 40%
13. `Overhead — Natural` — Presence 20%, Air 35%, Smooth 55%
14. `Overhead — Open Sky` — Presence 25%, Air 65%, Smooth 65%

### Mix Bus / Master (4):
15. `Mix Bus — Gentle Air` — Presence 20%, Air 20%, Smooth 60%
16. `Mix Bus — Full Open` — Presence 40%, Air 40%, Smooth 55%
17. `Master — Polish` — Presence 15%, Air 15%, Smooth 70%
18. `Master — Presence Lift` — Presence 35%, Air 10%, Smooth 65%

### Utilitários (2):
19. `Default` — Presence 40%, Air 30%, Smooth 50%, Output 0
20. `Bypass Safe` — Presence 0%, Air 0%, Smooth 50%, Output 0

**Total: 20 presets de fábrica na v1.**

---

## 8. Plano de Implementação em Fases

### Fase 18.0 — Research & Product Direction (este documento)
- [x] Leitura da estrutura do monorepo
- [x] Estudo de DSP notes existentes
- [x] Definição de arquitetura
- [x] Product brief
- [x] DSP design notes

### Fase 18.1 — Skeleton do Plugin (sem código funcional)
- Criar `plugins/02_AIR/CMakeLists.txt`
- Criar `plugins/02_AIR/Source/`, `DSP/`, `UI/`, `Presets/`, `Docs/`
- Adicionar `add_subdirectory(plugins/02_AIR)` no CMake raiz (comentado)
- Criar `PluginProcessor.h/.cpp` esqueleto (apenas estrutura, sem DSP)
- Criar `ParameterIDs.h/.cpp` com os 4 parâmetros APVTS
- Confirmar compilação antes de prosseguir

### Fase 18.2 — DSP Core
- Implementar `BiquadFilter` (reutilizado de shared/)
- Implementar `ParameterSmoother` (reutilizado de shared/)
- Implementar `AirWaveshaper.h/.cpp` — tanh normalizado com clamping
- Implementar `AirEnvelopeFollower.h/.cpp` — attack/release exponencial
- Implementar `AirProcessor.h/.cpp` — pipeline completo sem UI
- Testes unitários: saída neutra com knobs em zero, ausência de NaN, gain compensation correta

### Fase 18.3 — Smooth Guard
- Implementar detector de energia 4–8 kHz
- Implementar atenuação dinâmica com attack ~30ms, release ~100ms
- Integrar ao AirProcessor
- Testar em vocal sibilante: antes/depois auditivo

### Fase 18.4 — UI Base
- Implementar `PluginEditor.h/.cpp` com layout básico
- Implementar `Theme.h/.cpp` (adaptação do Theme AUREQ para ASTRA AIR palette)
- 4 knobs grandes com LookAndFeel do shared/
- Top Bar com nome e preset browser simples
- Validar compilação e funcionamento básico

### Fase 18.5 — UI Premium & Animação
- Gradiente de fundo aurora/ciano
- Glassmorphism nos painéis dos knobs
- LED / indicador de Smooth Guard ativo
- Display abstrato de "shimmer" (pode ser simplificado em v1 para aurora estática)
- Validação visual em DAW

### Fase 18.6 — Presets de Fábrica
- Implementar `AirPresetManager` (simplificado — 20 presets fixos)
- Implementar preset browser no dropdown
- Validação musical de todos os 20 presets em material real

### Fase 18.7 — Localization PT-BR / EN
- Adaptar padrão de Localization do AUREQ (Localization.h/.cpp)
- Traduzir labels dos knobs, preset categories, menu settings
- Validação visual com ambos os idiomas

### Fase 18.8 — Testes Unitários DSP
- Suite equivalente ao `AUREQ_FilterTests`
- Testar: waveshaper bounds, envelope follower convergência, gain compensation
- Testar: comportamento a sample rates 44.1, 48, 88.2, 96, 192 kHz

### Fase 18.9 — QA Musical
- Vocal feminino pop
- Rap vocal
- Acoustic guitar com overdubs
- Mix bus de projeto completo
- Checklist baseado no `aureq-manual-qa-checklist.md`

### Fase 18.10 — Packaging macOS + Windows
- Seguir pipeline do AUREQ (Fases 15.x + 17.x)
- Bundle ID proposto: `br.com.sidyfurtado.astrair` (ou nome final)
- Plugin code: 4 chars únicos (ex: `Aair`)
- Manufacturer code: `AStr` (mesmo do AUREQ)

---

## 9. CMake — Previsão de Integração

Quando a implementação for iniciada, o `CMakeLists.txt` raiz será atualizado para incluir:

```cmake
# Segundo plugin — ASTRA AIR (air exciter / high-frequency enhancer)
add_subdirectory(plugins/02_AIR)
```

A linha já existe como placeholder comentado (`plugins/02_Compressor`). Será adaptada para o diretório correto.

> **IMPORTANTE:** O diretório `plugins/02_Compressor/` já existe como placeholder. O ASTRA AIR poderá usar `plugins/02_AIR/` como diretório dedicado, mantendo o compressor para futura implementação em `plugins/03_Compressor/` ou similar. Decisão de numeração a cargo do proprietário.

---

## 10. O Que Este Documento NÃO Fez

Para registro explícito de escopo:

- ❌ Não alterou nenhum arquivo `.cpp` ou `.h`
- ❌ Não alterou `CMakeLists.txt` (raiz nem plugins)
- ❌ Não alterou o AUREQ (nenhum arquivo em `plugins/01_EQ/`)
- ❌ Não alterou installers nem scripts de build
- ❌ Não acessou, leu nem moveu arquivos em `Livros/`
- ❌ Não criou APVTS, PluginProcessor ou qualquer código funcional
- ❌ Não rodou nenhum comando de build

---

## 11. Próximo Passo

Aguardar aprovação do proprietário do produto para:
1. Confirmar nome final do plugin.
2. Confirmar versão de controles (Minimal / Musical / Avançada).
3. Confirmar numeração de diretório (`02_AIR` vs outro).
4. Iniciar Fase 18.1 — Skeleton do Plugin.

---

*Este documento é pesquisa e planejamento. Nenhuma implementação foi iniciada.*
