# Product Brief — ASTRA AIR

> **Status:** Pesquisa e Planejamento — aguardando aprovação para implementação  
> **Plugin:** ASTRA AIR (nome provisório)  
> **Suite:** ASTRA Audio Suite  
> **Versão planejada:** 1.0.0  
> **Data:** 2026-05-31  
> **Regra:** Nenhum código foi alterado. Este documento é apenas planejamento de produto.

---

## 1. Visão Geral do Produto

O **ASTRA AIR** é um processador de alta frequência original da ASTRA Audio Suite — um high-frequency enhancer e air exciter que adiciona presença, brilho, shimmer e abertura a fontes musicais de forma suave, inteligente e sem harshness.

O plugin combina três camadas de processamento em uma cadeia simples e musical:
- **Presence enhancement** dinâmico e adaptativo na faixa de 2–6 kHz
- **Air exciter** via saturação paralela seletiva na faixa de 8–20 kHz
- **Smooth Guard** automático que previne sibilância e harshness em tempo real

O resultado sonoro é descrito internamente como: *"abrir o som sem machucar o ouvido."*

---

## 2. Proposta de Valor

| Necessidade musical | O que o ASTRA AIR entrega |
|---|---|
| Vocal parece fechado, sem brilho | Presence abre a inteligibilidade suavemente |
| Mix bus parece densa e sem ar | Air adiciona shimmer de alta frequência sem mudar graves |
| Acoustic guitar sem definição | Presence + Air trazem corpo e brilho natural |
| Vocal com sibilância — sem querer de-esser | Smooth Guard recua automaticamente nos picos de S |
| Plugin de "air" que soa como EQ | Cadeia de exciter harmônico que **gera** frequências em vez de apenas amplificar |

---

## 3. Nomes Candidatos

O nome **ASTRA AIR** é provisório e funcional. As alternativas abaixo foram avaliadas com base em originalidade, sonoridade, identidade da suite e ausência de conflito com marcas existentes:

### Candidatos principais:
| Nome | Razão |
|---|---|
| **ASTRA AIR** (codinome atual) | Direto, descritivo, dentro da identidade ASTRA. Pode soar genérico. |
| **LUMINA** | Remete a luz, brilho, luminosidade — excelente para um air enhancer. Bom som. |
| **AURELIO** | Coerente com AUREQ (AUR = ouro, luz). "Aurelio" em latim = feito de ouro, dourado, luminoso. |
| **SOLARA** | Sol, energia radiante, calor de alta frequência. Feminino e premium. |
| **ASTRA GLOW** | Expande o ASTRA com "glow" — shimmer, brilho. Mais descritivo que "AIR". |
| **ETHER** | Éter, ar puro, o elemento etéreo. Mistério e leveza. |
| **HALO** | Auréola de luz. Já usado por alguns plugins mas não como marca principal ASTRA. |
| **AURORA** | Aurora boreal — luminosidade, presença, espetáculo etéreo. Forte identidade visual. |

### Recomendação:
**LUMINA** ou **AURORA** como candidatos fortes. Ambos têm identidade visual poderosa, soam premium, e são coerentes com o universo ASTRA (espaço, luz, cosmos). **AURELIO** é interessante pela coerência com AUREQ (mesma raiz "AURE-").

> **Decisão final sobre nome:** a cargo do proprietário do produto. Nenhum nome foi registrado ou implementado.

---

## 4. Posicionamento no Mercado

**Categoria:** High-Frequency Enhancer / Air Exciter  
**Segmento:** Mixing & Mastering Tools — ASTRA Audio Suite  
**Público-alvo:** Produtores musicais, mixers, engenheiros de gravação  
**Mercados primários:** Brasil (PT-BR), mercado global (EN)

### Diferenciação em relação a concorrentes genéricos:
- Não é um high shelf EQ com novo nome.
- Não é uma cópia de qualquer produto existente.
- Unique Selling Points:
  1. **Smooth Guard inteligente** — proteção automática de sibilância integrada ao enhancer.
  2. **Cadeia exciter verdadeiro** — gera harmônicos, não apenas amplifica.
  3. **Dynamic Presence** — presença que respira com a dinâmica do material.
  4. **Identidade visual ASTRA** — coerente com AUREQ, premium, minimalista.
  5. **Presets por fonte musical** — prontos para vocal, guitar, piano, drums, mix bus.

---

## 5. Fontes Musicais-Alvo

O ASTRA AIR foi desenhado para funcionar bem em:

| Fonte | Benefício principal |
|---|---|
| Lead Vocal | Presença, brilho, inteligibilidade sem harshness |
| Rap Vocal | Definição e corte no mix denso |
| Backing Vocal | Air suave sem conflitar com o lead |
| Acoustic Guitar | Brilho natural das cordas, corpo das médias |
| Piano | Abertura e shimmer nas notas altas |
| Drum Bus | Top-end dos pratos e sensação de "overhead aberto" |
| Overheads | Shimmer de prato limpo e natural |
| Mix Bus | Air global sem alterar o balanço espectral |
| Master Bus (leve) | Polimento de topo sem comprometer o master |

---

## 6. Arquiteturas DSP Candidatas

### Arquitetura A — Dual-Band Dynamic High Shelf
```
Entrada → [Presence Dynamic Shelf ~2–8kHz] → [Air Dynamic Shelf ~8–20kHz] → Saída
```
- Fácil de implementar.
- Risco: pode soar como EQ comum se as curvas não forem bem calibradas.
- Não usa saturação real — sem geração de harmônicos.
- ❌ Não entrega o diferencial de exciter real.

### Arquitetura B — Split-Band Harmonic Exciter Puro
```
Entrada ─┬─ (Dry path) ──────────────────────────────────────────────────────┐
         │                                                                    │
         └─ HPF (8kHz) → Waveshaper → Post-LPF → Gain (Air) ─────────────── Σ → Saída
```
- Gera harmônicos reais via waveshaper.
- Oversampling necessário para limpeza.
- Risco: sem controle de presença na faixa 2–6 kHz.
- ❌ Incompleto para o objetivo de produto.

### Arquitetura C — Hybrid (RECOMENDADA)

```
Entrada ─────────────────────────────────────── Dry path ─────────────────┐
    │                                                                       │
    ├─ [PRESENCE STAGE]                                                     │
    │   BPF/Shelf (~2–6kHz) → Dynamic Gain (envelope follower) → Gain ─── Σ │
    │   Smooth Guard monitor ↗                                              │
    │                                                                       │
    └─ [AIR STAGE]                                                          │
        HPF (~8kHz) → Waveshaper (tanh suave) → HPF post ─────── Gain ─── Σ │
                                                                            │
                                                              [Output Gain] │
                                                                            ▼
                                                                          Saída
```

#### Por que a Arquitetura C é recomendada:
1. **Presence** resolve o problema de 2–6 kHz de forma dinâmica — nunca soa como shelf cru.
2. **Air** usa exciter verdadeiro — harmônicos reais gerados por waveshaping, não apenas amplificação.
3. **Smooth Guard** monitora a cadeia de Presence e recua automaticamente quando sibilância/harshness aparece.
4. **Mix paralelo total** — o sinal dry nunca é processado diretamente, apenas adicionado em cima.
5. **Coerente com AUREQ** — usa os mesmos princípios de envelope follower, biquad IIR, e ParameterSmoother já validados.

---

## 7. Controles Recomendados

### Avaliação das versões propostas:

| Versão | Controles | Parecer |
|---|---|---|
| Minimal | Presence, Air, Output | Simples demais — não expõe o Smooth Guard que é diferencial |
| Musical | Presence, Air, Smooth, Output | ✅ **Recomendada para v1** — equilibra simplicidade com controle |
| Avançada | Presence, Air, Glow, Smooth, Output | Reservar o "Glow" para v1.1 — evitar scope creep |

### Versão Musical — definição funcional:

| Controle | Range | Default | Função |
|---|---|---|---|
| **Presence** | 0–100% | 40% | Intensidade do presence enhancer dinâmico (~2–6 kHz) |
| **Air** | 0–100% | 30% | Mix do exciter harmônico de alta frequência (~8–20 kHz) |
| **Smooth** | 0–100% | 50% | Agressividade do Smooth Guard (proteção automática de sibilância) |
| **Output** | -12 dB a +6 dB | 0 dB | Ganho de saída global com smoothing |

#### Sobre o "Glow" (v1.1 futura):
- Controlaria o drive interno do waveshaper de forma exposta ao usuário.
- Em v1, o drive é interno e fixo em valor musical calibrado.
- Não entrar em v1 para manter simplicidade e reduzir risco técnico.

---

## 8. Lista Inicial de Presets de Fábrica

### Categoria Vocal:
1. `Lead Vocal — Natural Air` — Presence sutil + Air leve. Uso geral.
2. `Lead Vocal — Pop Shine` — Presence + Air moderados. Vocal pop brilhante.
3. `Lead Vocal — Smooth` — Presence alta + Smooth máximo. Vocal agressivo/rap.
4. `Rap Vocal — Cut Through` — Presence alta, Air moderado, Smooth baixo.
5. `Backing Vocal — Subtle` — Presence baixo, Air baixo. Camada de backing.
6. `Backing Vocal — Airy` — Air moderado sem Presence. Chorus etéreo.

### Categoria Instrumentos:
7. `Acoustic Guitar — Strings` — Presence média + Air leve. Brilho das cordas.
8. `Acoustic Guitar — Body + Air` — Presence + Air equilibrados.
9. `Piano — Brilliance` — Air moderado, Presence baixo.
10. `Piano — Full Shimmer` — Presence + Air altos, Smooth médio.
11. `Keys / Synth Air` — Air alto, Presence baixo.

### Categoria Drums:
12. `Drum Bus — Top End` — Air moderado no bus de bateria.
13. `Overhead — Natural Shimmer` — Air leve, Presence baixo.
14. `Overhead — Open Sky` — Air alto, Smooth alto.

### Categoria Mix Bus / Master:
15. `Mix Bus — Gentle Air` — Presence baixo + Air muito leve.
16. `Mix Bus — Full Open` — Presence + Air moderados.
17. `Master — Polish` — Air mínimo, Presence mínimo. Toque final sutil.
18. `Master — Presence` — Presence moderado para mix fechado.

### Utilidade:
19. `Default` — Presence 40%, Air 30%, Smooth 50%, Output 0 dB.
20. `Bypass Safe` — Todos em zero. Confirmação de bypass neutral.

---

## 9. O Que Fica Fora da v1

As funcionalidades abaixo foram avaliadas e **deliberadamente excluídas da v1** para manter escopo gerenciável e evitar regressões:

| Feature | Razão da exclusão |
|---|---|
| **A/B Compare** | Pode ser herdado do AUREQ em sprint futuro |
| **Undo/Redo** | Idem — padrão AUREQ pode ser reusado mas não é crítico v1 |
| **Spectrum Analyzer** | Custoso em CPU e tempo. Reservar para v1.1 |
| **Glow knob (drive exposto)** | Scope creep. Drive interno calibrado é suficiente para v1 |
| **Oversampling manual** | Automático é melhor UX; controle manual confunde usuário |
| **Mid/Side processing** | Complexidade desnecessária para v1. Boa adição para v2 |
| **Sidechain input** | Não é necessário para o propósito do plugin |
| **Automação de presets** | Herdar arquitetura do AUREQ quando necessário |
| **AU format (Apple Audio Unit)** | VST3 primeiro; AU como segunda fase após validação |
| **AAX format** | Pro Tools — requer SDK especial; não na v1 |

---

## 10. Riscos Técnicos

| Risco | Nível | Mitigação |
|---|---|---|
| Aliasing do waveshaper em sr baixo | Alto | Oversampling 2x condicional via `juce::dsp::Oversampling` |
| Ganho excessivo na saída (loudness pumping) | Médio | Output gain compensation automática pós-waveshaper |
| Smooth Guard introduzir pumping audível | Médio | Attack ~20–50ms, Release ~80–120ms. Testes em vocal agressivo |
| Zipper noise nos knobs | Médio | ParameterSmoother idêntico ao AUREQ (já testado e validado) |
| Plugin soar como EQ comum | Médio | Calibração cuidadosa do waveshaper e mix paralelo |
| NaN/Infinity no waveshaper | Baixo-Médio | Input clamping antes do waveshaper. Proteção de denormals |
| CPU spike com oversampling ativo | Baixo | Oversampling desativado em sr ≥ 88.2 kHz automaticamente |
| Conflito visual com AUREQ na DAW | Baixo | UI diferente (sem EQ graph). Identity clara da ASTRA AIR |
| Nome em conflito com marca existente | Baixo | Verificação de trademark antes de finalizar nome |

---

## 11. Critérios de Sucesso Sonoro

O ASTRA AIR v1 será considerado aprovado musicalmente quando:

1. **Air knob em 30–60%** em vocal feminino pop não introduz sibilância adicional audível.
2. **Presence em 50%** em rap vocal adiciona inteligibilidade perceptível sem causar harshness em 3–5 kHz.
3. **Em mix bus** com Presence + Air em 40%, o resultado soa "mais aberto" sem mudança perceptível de loudness.
4. **Smooth Guard em 50%** reduz a agressividade de consoantes S audíveis em vocal sibilante sem soar como de-esser bombeando.
5. **Bypass comparison:** ao bypassar o plugin com settings musicais, a diferença é perceptível mas o signal sem plugin não soa "fechado ou danificado" — o processamento é aditivo e controlado.
6. **CPU em tempo real:** o plugin não ultrapassa 5% de CPU individual em projeto de 64 tracks em MacBook com sample rate 44.1 kHz.
7. **Ausência de artefatos digitais** em qualquer sample rate de 44.1 kHz a 192 kHz.
8. **Sem clipping de saída** com qualquer combinação de knobs em sinal normalizado a -3 dBFS de entrada.

---

## 12. Próximas Etapas

1. ✅ Pesquisa técnica DSP (este documento + `air-exciter-enhancer-design-notes.md`)
2. ✅ Product Brief (este documento)
3. ⬜ Aprovação do nome final
4. ⬜ Mockup de UI (Design System ASTRA aplicado)
5. ⬜ Fase 18.0: Estrutura de diretório `plugins/02_AIR/` (sem código, apenas skeleton)
6. ⬜ Fase 18.1: APVTS — 4 parâmetros da versão musical
7. ⬜ Fase 18.2: DSP core — BiquadFilter split + waveshaper + envelope follower
8. ⬜ Fase 18.3: Smooth Guard
9. ⬜ Fase 18.4: UI — layout, knobs, visual premium ASTRA
10. ⬜ Fase 18.5: Presets de fábrica
11. ⬜ Fase 18.6: Localization PT-BR / EN
12. ⬜ Fase 18.7: Testes unitários DSP
13. ⬜ Fase 18.8: QA musical em vocal, guitar, drums, mix bus
14. ⬜ Fase 18.9: Packaging macOS + Windows

---

*Este documento é planejamento de produto. Nenhum código ou CMake foi alterado.*
