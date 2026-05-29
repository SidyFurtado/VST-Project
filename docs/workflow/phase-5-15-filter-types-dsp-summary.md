# Resumo de Validação Técnica: Fase 5.15 — Múltiplos Tipos de Filtro DSP

Este documento registra a implementação da **Fase 5.15** do equalizador **AUREQ**, parte da **ASTRA Audio Suite**, que expande o motor DSP com suporte completo a todos os 7 tipos de filtro parametrizados pelo parâmetro existente `bandNType`.

---

## 1. Objetivo da Fase

Transformar o AUREQ de um EQ de 8 bandas Bell-only em um equalizador paramétrico com suporte real a múltiplos tipos de filtro, usando o parâmetro APVTS `bandNType` já existente. Zero novos parâmetros criados.

---

## 2. Tipos de Filtro Implementados

| Índice APVTS | FilterType | Descrição |
|---|---|---|
| 0 | Bell | Peaking EQ — pico ou vale centrado em f₀ |
| 1 | Low Cut | High-Pass 12 dB/oct — corta graves abaixo de f₀ |
| 2 | High Cut | Low-Pass 12 dB/oct — corta agudos acima de f₀ |
| 3 | Low Shelf | Shelf de graves — ganho constante abaixo de f₀ |
| 4 | High Shelf | Shelf de agudos — ganho constante acima de f₀ |
| 5 | Notch | Rejeição de banda — null exato em f₀ |
| 6 | Band Pass | Passa-banda — passa somente em torno de f₀ |

---

## 3. Fórmulas Usadas — Audio EQ Cookbook (R. Bristow-Johnson)

### Variáveis comuns
```
ω₀ = 2π·f₀/fs
α  = sin(ω₀) / (2·Q)
A  = 10^(gainDdB/40)   [Bell e Shelf]
```

### Bell (setPeaking) — pré-existente
```
b0 = 1+α·A,  b1 = -2cos,  b2 = 1-α·A
a0 = 1+α/A,  a1 = -2cos,  a2 = 1-α/A
```

### High-Pass (setHighPass) — Low Cut
```
b0 = (1+cos)/2,  b1 = -(1+cos),  b2 = (1+cos)/2
a0 = 1+α,        a1 = -2cos,      a2 = 1-α
```

### Low-Pass (setLowPass) — High Cut
```
b0 = (1-cos)/2,  b1 = 1-cos,     b2 = (1-cos)/2
a0 = 1+α,        a1 = -2cos,      a2 = 1-α
```

### Low Shelf (setLowShelf) — S=1 (Butterworth slope)
```
twoSqAα = 2·√A·sin(ω₀)/√2
b0 = A·[(A+1)-(A-1)·cos+twoSqAα]
b1 = 2A·[(A-1)-(A+1)·cos]
b2 = A·[(A+1)-(A-1)·cos-twoSqAα]
a0 = (A+1)+(A-1)·cos+twoSqAα
a1 = -2·[(A-1)+(A+1)·cos]
a2 = (A+1)+(A-1)·cos-twoSqAα
```

### High Shelf (setHighShelf) — S=1 (Butterworth slope)
*(mesma estrutura, ± cos invertido)*

### Notch (setNotch)
```
b0 = 1,   b1 = -2cos,  b2 = 1
a0 = 1+α, a1 = -2cos,  a2 = 1-α
```

### Band Pass (setBandPass) — constant peak gain Q variant
```
b0 = sin/2,  b1 = 0,   b2 = -sin/2
a0 = 1+α,    a1 = -2cos, a2 = 1-α
```

---

## 4. Mapeamento FilterType → Parâmetros DSP

| Tipo | Usa Ganho | Usa Q | Observação |
|---|---|---|---|
| Bell | ✅ | ✅ | Bell/Peaking EQ clássico |
| Low Cut | ❌ | ✅ | Q controla ressonância no cutoff |
| High Cut | ❌ | ✅ | Q controla ressonância no cutoff |
| Low Shelf | ✅ | ❌ | Slope fixo S=1 (Butterworth) nesta fase |
| High Shelf | ✅ | ❌ | Slope fixo S=1 (Butterworth) nesta fase |
| Notch | ❌ | ✅ | Q controla largura do dip |
| Band Pass | ❌ | ✅ | Q controla largura da passagem |

---

## 5. Mapeamento Low Cut / High Cut

> [!IMPORTANT]
> **Low Cut** usa internamente `BiquadFilter::setHighPass()` (passa-alta, corta graves).  
> **High Cut** usa internamente `BiquadFilter::setLowPass()` (passa-baixa, corta agudos).  
> Este mapeamento segue a nomenclatura de produto (o que o usuário corta) vs. a nomenclatura matemática do filtro (o que passa).

---

## 6. Proteções de Segurança Numérica

Todos os 6 novos métodos em `BiquadFilter` implementam:
- Clamp de frequência: 20 Hz → min(20000 Hz, Nyquist·0.95)
- Clamp de Q: 0.1 → 18.0
- Clamp de ganho (shelf): ±24 dB
- Guard `|a0| < 1e-12` → fallback para filtro identidade (bypass)
- Verificação NaN/Inf em todos os 5 coeficientes normalizados → fallback para filtro identidade

---

## 7. Curva Visual (EQGraphView)

A renderização da curva teórica no `EQGraphView` foi expandida com aproximações analíticas para todos os tipos:

| Tipo | Aproximação Visual |
|---|---|
| Bell | Gaussiana em log-frequency (existente) |
| Low Cut | Rolloff quadrático: `1 - 1/(1+(f/f0)²·Q²)` → dB |
| High Cut | Rolloff quadrático: `1 - 1/(1+(f0/f)²·Q²)` → dB |
| Low Shelf | Sigmoide logística em escala de oitavas |
| High Shelf | Sigmoide logística invertida |
| Notch | Gaussiana negativa (-24 dB pico) |
| Band Pass | Gaussiana positiva (+12 dB pico) |

> [!NOTE]
> A curva visual é uma **aproximação analítica para display**, não os coeficientes reais do biquad. O DSP usa os coeficientes RBJ exatos. A curva visual exata (via transfer function H(z)) é escopo de fase futura.

---

## 8. Limitações Conhecidas

| Limitação | Descrição |
|---|---|
| **Slopes** | Nesta fase todos os filtros de corte usam 12 dB/oct (1 estágio biquad). Slopes de 24, 36, 48 dB/oct (múltiplos estágios) são escopo de fase futura |
| **Shelf Q** | Low/High Shelf usam slope fixo S=1 (Butterworth, maximally flat). Q ajustável para shelf será fase futura |
| **Curva visual** | A curva visual usa aproximações geométricas, não a transfer function H(z) exata do biquad. A curva exata será fase futura |
| **UI de seleção** | Não há menu ou botão visual para selecionar o tipo de filtro ainda. O tipo só pode ser alterado via automação DAW no parâmetro `bandNType`. UI de seleção é fase futura |

---

## 9. Dynamic EQ

> [!IMPORTANT]
> **Dynamic EQ NÃO foi implementado.** Não há envelope followers, thresholds, ratios, attack ou release nesta fase. Continua escopo de Fase 7 do roadmap.

---

## 10. Confirmação: APVTS Intocado

> [!IMPORTANT]
> O layout do APVTS foi **completamente preservado**:
> - Zero parâmetros novos criados
> - `bandNType` é o parâmetro existente (índices 0–6 mapeados para as 7 strings já declaradas)
> - `PluginProcessor.h`, `PluginProcessor.cpp` e `ParameterIDs.h/cpp` **não foram alterados**
> - `EQProcessorCore.cpp` **não foi alterado**

---

## 11. Arquivos Modificados

| Arquivo | Mudança |
|---|---|
| [`DSP/BiquadFilter.h`](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/DSP/BiquadFilter.h) | +6 declarações de método (setHighPass, setLowPass, setLowShelf, setHighShelf, setNotch, setBandPass) |
| [`DSP/BiquadFilter.cpp`](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/DSP/BiquadFilter.cpp) | +2 helpers estáticos (sanitizeCommonInputs, normaliseCoeffs) + 6 implementações RBJ |
| [`DSP/EQBand.cpp`](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/DSP/EQBand.cpp) | processBlock + updateFilterCoefficients expandidos com switch para 7 tipos |
| [`UI/EQGraphView.cpp`](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/EQGraphView.cpp) | Curva visual expandida com aproximações analíticas para todos os 7 tipos |

---

## 12. Resultado do Build

```
cmake --build build
✅ [100%] Built target AUREQ_VST3
✅ [100%] Built target AUREQ_Standalone
```

- **Erros novos**: nenhum
- **Warnings novos**: nenhum
- **Warnings preexistentes**: deprecation `AudioParameterFloat` (JUCE) — pré-existente

---

## 13. Artefatos Gerados

| Formato | Caminho |
|---|---|
| **VST3** | `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3` |
| **Standalone** | `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app` |

---

## 14. UI de Seleção de Tipo — Fase Futura

A UI de seleção de tipo de filtro (menu dropdown ou botões por banda no painel inferior) **não foi implementada nesta fase**. Será abordada em uma fase dedicada de UI, onde cada handle terá um seletor visual contextual para alternar entre os 7 tipos sem precisar de automação DAW.
