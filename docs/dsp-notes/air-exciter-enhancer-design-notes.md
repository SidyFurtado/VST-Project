# DSP Design Notes — Air Exciter / High-Frequency Enhancer

> **Status:** Research / Pre-implementation  
> **Plugin:** ASTRA AIR (codinome provisório)  
> **Suite:** ASTRA Audio Suite  
> **Data:** 2026-05-31  
> **Autor:** ASTRA Audio / Planejamento interno  
> **Regra:** Nenhuma linha de código de plugin foi alterada durante a criação deste documento.

---

## 1. O Que É um Exciter Harmônico

Um exciter harmônico não amplifica frequências existentes — ele **gera novos harmônicos** a partir de frequências que já existem no sinal. É fundamentalmente diferente de um high shelf EQ.

### Mecanismo básico:
1. **Extração:** Um filtro high-pass (ou shelf) isola a porção de altas frequências do sinal.
2. **Saturação suave:** A banda extraída passa por um waveshaper não-linear suave (tipicamente `tanh()`, soft-clip assimétrico ou polinômio de baixa ordem).
3. **Geração de harmônicos:** A saturação produz harmônicos pares e ímpares acima das frequências originais isoladas.
4. **Mix paralelo:** O resultado é misturado de volta ao sinal original em quantidade controlada — nunca substitui, apenas adiciona.
5. **Filtragem de saída:** Um filtro remove excessos indesejados (especialmente sub-harmônicos acidentais e aliasing).

### Por que soa diferente de um EQ:
- Um high shelf em +3 dB amplifica o que **existe**.
- Um exciter em modo moderado **cria** parciais que não existiam — a percepção é de abertura e brilho sem a sensação de "mais volume nos agudos".
- O resultado é psicoacusticamente mais convincente: soa como "espaço" e "presença" em vez de "agudo empurrado".

---

## 2. Diferença Entre EQ High Shelf e Exciter

| Dimensão | High Shelf EQ | Harmonic Exciter |
|---|---|---|
| O que afeta | Amplifica frequências existentes | Gera novas frequências por não-linearidade |
| Percepção em vocal | "Agudo mais alto" | "Mais aberto, mais presente" |
| Comportamento em mix bus | Pode causar harshness se excessivo | Adiciona ar sem piorar sibilância |
| Interação com saturação | Neutra | Intrínseca (é a ferramenta) |
| Aliasing risk | Zero | Presente — requer cuidado com oversampling |
| Uso musical | Correção e tonalidade | Caráter, brilho, shimmer |

**Conclusão para o ASTRA AIR:** o diferencial é a combinação dos dois mundos: EQ dinâmico de alta frequência + saturação seletiva suave em paralelo, com controle de harsh automático.

---

## 3. Como Gerar Brilho Sem Harshness

O "brilho" musical vem de energia nas regiões **acima de 10 kHz** e de presença controlada em torno de **3 kHz–6 kHz**. O harshness vem de saturação excessiva ou ganho linear excessivo em **3 kHz–8 kHz**.

### Estratégias anti-harshness:

#### 3.1 Frequência de Cruzamento Cuidadosa
- A banda de presença (Presence) deve ter frequência central ao redor de **2 kHz–4 kHz**, mas com Q largo o suficiente para não concentrar energia em 3–5 kHz.
- A banda de ar (Air) deve começar acima de **8 kHz**, onde a saturação suave gera harmônicos bem acima de Nyquist — controlados por oversampling.

#### 3.2 Saturação Apenas na Banda de Ar
- Não saturar o sinal completo. Extrair apenas altas frequências via high-pass (fc ≈ 6–10 kHz) e saturar essa banda isolada.
- Resultado: o núcleo do sinal (100 Hz – 6 kHz) permanece limpo; apenas o "topo" do espectro ganha shimmer.

#### 3.3 Waveshaper de Baixa Ordem
- `f(x) = tanh(drive * x) / tanh(drive)` — normalizado para não aumentar ganho.
- Alternativamente, soft-clip polinomial de 3ª ordem com coeficientes ajustados para favorecer harmônicos pares (soam mais "calorosos" e menos agressivos).
- **Harmônicos pares (2ª, 4ª ordem):** mais musicais, associados a válvulas e transformadores analógicos.
- **Harmônicos ímpares (3ª, 5ª):** mais "transistor", mais penetrantes. Usar com moderação no air band.

#### 3.4 Dynamic High-Frequency Control (Smooth Guard)
- Um detector de envelope analisa a energia em 3–8 kHz em tempo real.
- Se a energia exceder um limiar interno, o ganho de presença é suavemente reduzido (upward limiting implícito).
- Isso evita sibilância durante consoantes explosivas de vocal sem precisar de de-esser separado.

---

## 4. Saturação Seletiva em Altas Frequências

### Pipeline recomendado para a banda Air:

```
Sinal de entrada
    │
    ├──────────────────────── (Dry path — permanece limpo)
    │
    └─ HPF (fc ≈ 8 kHz, 2ª ordem) ──► Waveshaper suave ──► Ganho de mix ──┐
                                       (tanh normalizado)    (Air knob)     │
                                                                            ▼
                                                               Mix Paralelo (soma)
                                                                            │
                                                                         Saída
```

### Parâmetros internos da saturação:
- **Drive interno:** fixo ou mínimo (não exposto ao usuário). Aproximadamente 1.5x–2.5x para gerar harmônicos perceptíveis sem saturação audível.
- **Output gain compensation:** obrigatório após saturação para manter loudness neutro.
- **Post-saturação HPF:** um high-pass suave adicional após o waveshaper elimina sub-harmônicos acidentais que a saturação pode introduzir abaixo da fc original.

---

## 5. Processamento Paralelo

O processamento paralelo é fundamental para a arquitetura do ASTRA AIR por duas razões:

1. **Integridade do transiente:** O sinal seco nunca é alterado pelo waveshaper. Transientes percussivos são preservados.
2. **Controle de intensidade:** O knob de controle ajusta o quanto da "camada excitada" é adicionado — de zero (bypass) a 100% (máximo).

### Implementação no pipeline JUCE:
- O AUREQ já usa `AudioBuffer<float>` e processamento inline. O ASTRA AIR seguirá o mesmo padrão.
- O buffer dry é mantido inalterado durante o processamento da cadeia de air/presence.
- A soma paralela é feita por multiplicação de ganho + `addFrom()` do JUCE no buffer principal.
- Não é necessário buffer auxiliar de memória alocada em tempo real — pode-se usar um bloco temporário de stack para pequenos processamentos de canal.

---

## 6. Dynamic EQ e Upward High-Frequency Enhancement

### O que é upward enhancement:
Ao invés de amplificar frequências altas estaticamente, o processamento dinâmico monitora quando o sinal de alta frequência está "fraco" (abaixo de um limiar interno) e eleva suavemente — "upward expansion" em altas frequências.

**Efeito musical:** soa como se o material estivesse "abrindo" e "respirando" naturalmente, especialmente em vocais com dinâmica variada (rap vocal, backing vocal).

### Implementação simplificada:
- Detector de envelope na banda 6–12 kHz.
- Comparer contra limiar interno (hardcoded ou semi-automático).
- Modulação suave do ganho de presence/air baseada na detecção.
- A modulação deve ter attack lento (~20–50 ms) e release lento (~100–300 ms) para evitar pumping.

### Referência ao AUREQ:
O AUREQ já implementou Dynamic EQ com envelope follower e soft-knee em `EQBand.cpp`. A mesma lógica de `dynamicGain = dynamicRange * (1 - exp(-over/12.0))` pode ser adaptada para o ASTRA AIR com polaridade invertida para upward enhancement.

---

## 7. Controle de Sibilância e Harshness

### Região problemática: 3 kHz – 10 kHz
- **3–6 kHz:** presença vocal, inteligibilidade, também onde harshness ocorre.
- **6–10 kHz:** sibilância (consoantes S, T, Ch), brilho real mas perigoso.
- **10–20 kHz:** ar genuíno, shimmer, sem sibilância se isolado corretamente.

### Estratégia Smooth Guard:
1. **Detector de sibilância:** high-pass na faixa 4–8 kHz + RMS de curta janela (~5–10 ms).
2. **Threshold automático:** baseado na amplitude média de longo prazo da mesma banda (~500 ms).
3. **Atenuação dinâmica:** quando o detector dispara, aplica uma atenuação suave e transparente (1–4 dB máximo) na banda de presença.
4. **Release controlado:** ~80–120 ms para evitar modulação audível.

**Importante:** O Smooth Guard não é um de-esser de precisão cirúrgica — é um guarda automático de musicalidade que mantém o plugin funcionando bem em vocais agressivos sem intervenção do usuário.

---

## 8. Aliasing em Saturação de Altas Frequências

### O problema específico do air band:
- Se a saturação opera em sinal com conteúdo em 10–18 kHz, os harmônicos gerados (2ª ordem: 20–36 kHz, 3ª ordem: 30–54 kHz) ultrapassam Nyquist em 44.1 kHz.
- Sem oversampling, esses harmônicos dobram de volta (foldover) para frequências audíveis na faixa 2–10 kHz como ruído inarmônico metálico e digital.

### Solução: oversampling seletivo
- **Oversampling de 2x** é suficiente para saturação suave de alta frequência (drive baixo, harmônicos moderados).
- **Oversampling de 4x** é necessário apenas se o drive for alto ou se quisermos garantia total de limpeza.
- **Recomendação para ASTRA AIR v1:** oversampling de **2x** interno apenas na cadeia de waveshaping, usando os filtros FIR polifásicos do módulo `juce::dsp::Oversampling`.
- O oversampling não é aplicado ao sinal dry — apenas ao split de alta frequência que passa pelo waveshaper.

### Impacto na latência:
- Oversampling 2x com `juce::dsp::Oversampling` introduz latência de alguns samples (tipicamente 8–32 dependendo do filtro).
- Em v1, considerar manter o oversampling desativado por padrão com flag interna e validar se o aliasing percebido é problemático antes de ativar.

---

## 9. Quando Usar Oversampling

| Condição | Oversampling recomendado |
|---|---|
| Air knob em zero (bypass de saturação) | Não — skip completo da cadeia |
| Air knob < 30%, drive interno suave | Não — saturação tão leve que aliasing é imperceptível |
| Air knob 30–70%, drive moderado | 2x — remove artefatos visíveis no espectro |
| Air knob > 70%, uso em mix bus/master | 2x ou 4x — garantia de limpeza |
| Sample rate 88.2 kHz ou 96 kHz | Não — Nyquist já está acima de 40 kHz; aliasing não atinge faixa audível |

**Regra prática para v1:** ativar oversampling 2x automaticamente quando Air knob > limiar e sample rate ≤ 48 kHz. Desativar automaticamente em sample rates altos para economizar CPU.

---

## 10. CPU Budget e Performance

### Estimativa de custo relativo:
| Módulo | Custo estimado (relativo) |
|---|---|
| HPF high-pass split (biquad) | Muito baixo |
| LPF post-saturação (biquad) | Muito baixo |
| Envelope follower Smooth Guard | Baixo |
| Waveshaper `tanh` (sample-by-sample) | Médio |
| Oversampling 2x (`juce::dsp::Oversampling`) | Médio–Alto |
| Spectrum analyzer (se incluído na UI) | Alto |

### Estratégias de otimização:
1. **Processamento condicional:** Se Air == 0 e Presence == 0, bypass completo da cadeia de saturação. Zero custo adicional.
2. **Coeficientes por bloco:** Recalcular coeficientes dos filtros de split apenas uma vez por bloco, não por sample.
3. **Oversampling condicional:** Ativar apenas se Air > limiar E sample rate baixo (conforme tabela acima).
4. **SIMD via JUCE:** O `juce::dsp::ProcessorChain` e `ProcessContext` aproveitam vetorização nativa.
5. **Float32 interno:** Manter o processamento em `float` (não `double`) para melhor performance SIMD. Usar `double` apenas nos coeficientes dos filtros split (como já faz o BiquadFilter do AUREQ).

---

## 11. Controles Simples Para Cadeia Complexa

### Filosofia de design de controle:
O objetivo é que o engenheiro ouça e mova um único knob para adicionar mais "ar" — não que configure frequências de cruzamento, drives, attack/release, ratios, etc.

### Mapeamento interno recomendado (Versão Musical):

#### Knob `Presence` (0–100%):
- Gain da banda de presença dinâmica: 0 a ~+4 dB (soft-limit interno)
- Frequência de atuação: ~2–6 kHz (shelf leve ou bell largo)
- Comportamento dinâmico: upward — mais ganho quando sinal está fraco, recuo automático quando sinal está forte
- Smooth Guard ativo automaticamente

#### Knob `Air` (0–100%):
- Mix de retorno da cadeia exciter: 0% a 100% do bloco processado
- Drive interno do waveshaper: cresce proporcionalmente mas com teto (máximo drive ≈ 3x)
- Oversampling automático ativado em sample rates baixos quando > 30%
- Output gain compensation automática

#### Knob `Smooth` (0–100%, opcional na versão Musical):
- Controla a agressividade do Smooth Guard
- 0% = sem suavização automática (comportamento linear)
- 100% = Smooth Guard máximo (proteção de sibilância ativa)
- Default recomendado: ~50% para a maioria das fontes

#### Knob `Output` (-12 dB a +6 dB):
- Gain de saída global após toda a cadeia
- Com ParameterSmoother (mesmo padrão do AUREQ) para evitar zipper noise
- Meter VU de saída opcional na UI

---

## 12. Referências Técnicas Internas

- `docs/dsp-notes/07-saturacao-waveshaping-e-distorcao.md` — fundamentos de waveshaping e harmônicos
- `docs/dsp-notes/02-amostragem-quantizacao-e-aliasing.md` — teoria de aliasing e oversampling
- `docs/dsp-notes/06-dinamica-compressao-e-limitacao.md` — envelope follower, attack/release
- `docs/dsp-notes/04-filtros-e-equalizacao.md` — biquad filters, high shelf, high pass
- `docs/dsp-notes/08-regras-dsp-para-vsts.md` — regras de segurança DSP
- `plugins/01_EQ/DSP/BiquadFilter.h/.cpp` — implementação de referência de filtros IIR
- `plugins/01_EQ/DSP/ParameterSmoother.h/.cpp` — smoothing de parâmetros
- `docs/workflow/phase-6-0-dynamic-eq-core-summary.md` — dynamic EQ com soft-knee (reutilizável)

---

*Este documento é de pesquisa interna. Nenhuma implementação foi iniciada.*
