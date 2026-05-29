# Resumo de Validação Técnica: Fase 5.14 — Analisador FFT Simples

Este documento registra a implementação e validação da **Fase 5.14** do equalizador **AUREQ**, parte da **ASTRA Audio Suite**, que adiciona um analisador espectral em tempo real ao display gráfico do plugin.

---

## 1. Objetivo da Fase

Exibir o espectro de frequência do sinal pós-EQ no fundo do `EQGraphView`, utilizando uma pipeline de análise FFT assíncrona e completamente isolada da thread de áudio, respeitando os mandamentos de DSP do monorepo (zero alocação, zero locks na audio thread).

---

## 2. Arquitetura do Analisador

```
[processBlock]  ──push(canal L, pós-EQ)──▶  [AnalyzerFIFO]
(audio thread)    noexcept, lock-free         juce::AbstractFifo (8192 floats)
                  sem alocação                       │
                                              UI Timer (30 Hz)
                                                     │
                                         pop() → tempBuf[512]
                                                     │
                                         [AnalyzerProcessor]
                                         pushSamples() → acumula
                                         2048 amostras? → process():
                                           1. Janela Hann
                                           2. FFT 2048pts
                                           3. Magnitudes → dBFS
                                           4. Ballistics (decay)
                                           5. 50% overlap shift
                                                     │
                                         getBins() → 1025 bins
                                                     │
                                   [EQGraphView::setSpectrumData()]
                                         Renderiza gradiente
                                         cyan/mint translúcido
```

---

## 3. Funcionamento do AnalyzerFIFO

**Arquivo**: `plugins/01_EQ/Source/AnalyzerFIFO.h` / `AnalyzerFIFO.cpp`

> [!NOTE]
> O arquivo foi criado em `Source/` e não em `DSP/`, pois atua como **infraestrutura de transporte** entre threads, e não como processamento de áudio puro. Esta decisão está documentada aqui para futura revisão se necessário.

- Implementado com `juce::AbstractFifo` como mecanismo de indexação atômico
- Buffer interno de `8192 floats` pré-alocado na construção do objeto
- `push(data, numSamples)`: chamado **exclusivamente na audio thread**, marcado `noexcept`, sem alocação, sem lock
- `pop(dest, maxSamples)`: chamado **exclusivamente na message/UI thread** (dentro do `timerCallback`)
- Retorna o número real de amostras lidas

---

## 4. Funcionamento do AnalyzerProcessor

**Arquivo**: `plugins/01_EQ/Source/AnalyzerProcessor.h` / `AnalyzerProcessor.cpp`

> [!NOTE]
> Criado em `Source/` junto com o `AnalyzerFIFO` pela mesma razão de ser infraestrutura de análise visual, não DSP de produção de áudio.

- Acumula samples recebidas de `timerCallback` via `pushSamples()`
- Dispara `process()` automaticamente quando o buffer interno atinge **2048 amostras**
- Todo o processamento ocorre **apenas na message thread** — nunca na audio thread

---

## 5. FFT Size

| Parâmetro | Valor |
|---|---|
| Ordem FFT | 11 (`FFT_ORDER = 11`) |
| Tamanho do frame | **2048 amostras** (`FFT_SIZE = 2^11`) |
| Bins úteis | **1025** (DC a Nyquist, `NUM_BINS = FFT_SIZE/2 + 1`) |
| Overlap | 50% (1024 amostras mantidas após cada frame) |

Tamanho escolhido como **2048** por fornecer boa resolução em baixas frequências (~21 Hz/bin a 44100 Hz) com latência de exibição aceitável (~46ms a 44100 Hz).

---

## 6. Janela de Hann

A janela de Hann é pré-calculada no construtor do `AnalyzerProcessor`:

$$w[n] = 0.5 \cdot \left(1 - \cos\left(\frac{2\pi n}{N-1}\right)\right), \quad 0 \le n \le N-1$$

- **Objetivo**: Eliminar descontinuidades nas bordas do frame, prevenindo *spectral leakage* (ruído fantasma de frequência que distorceria a visualização)
- Coeficientes armazenados em `std::array<float, FFT_SIZE> hannWindow` — alocação em construção, zero runtime

---

## 7. Smoothing / Ballistics

Após cada frame, a magnitude de cada bin passa por um **envelope follower de pico com decaimento exponencial**:

$$Y_{bin}[k] = \max\left(X_{dBFS}[k],\ Y_{anterior}[k] \times g_{decay}\right)$$

| Parâmetro | Valor | Efeito |
|---|---|---|
| `DECAY_FACTOR` | `0.78f` | ~135 ms de decaimento visual a 30 Hz |
| `FLOOR_DB` | `-90.0f dBFS` | Nível de silêncio absoluto |

- Se o sinal sobe → snap imediato para o novo valor (ataque instantâneo)
- Se o sinal cai → decaimento suave com fator 0.78/frame (animação fluida)

---

## 8. Renderização Visual

No `EQGraphView::paint()`, o espectro é desenhado **antes** da curva de EQ e dos handles de banda para garantir que a curva sempre apareça acima:

- **Preenchimento**: gradiente vertical `accent.withAlpha(0.18f)` → `alpha(0.0f)`
- **Contorno superior**: `accent.withAlpha(0.35f)`, stroke de 0.8px
- **Mapeamento de frequência**: logarítmico, sincronizado com o eixo X logarítmico do gráfico
- **Ref sample rate**: 44100 Hz (diferença visual com 48kHz < 1px na escala log)
- **Interpolação**: linear entre bins adjacentes para suavidade na curva

---

## 9. Confirmação: DSP/APVTS Intocados

> [!IMPORTANT]
> As seguintes classes foram **completamente preservadas** sem nenhuma linha alterada:
> - `BiquadFilter.h/.cpp`
> - `EQBand.h/.cpp`
> - `EQProcessorCore.h/.cpp`
> - `ParameterSmoother.h/.cpp`
>
> O layout do APVTS foi **completamente preservado**: zero parâmetros novos criados.
>
> A única modificação no `PluginProcessor.cpp` foi uma **adição de 4 linhas** após o processamento EQ existente, sem alterar nenhum comportamento DSP.

---

## 10. Caminhos dos Artefatos

| Formato | Caminho |
|---|---|
| **VST3** | `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3` |
| **Standalone** | `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app` |

---

## 11. Resultado do Build

```
cmake --build build
✅ [100%] Built target AUREQ_VST3
✅ [100%] Built target AUREQ_Standalone
```

- **Erros novos**: nenhum
- **Warnings novos**: nenhum
- **Warnings preexistentes**: deprecation de `AudioParameterFloat` (signature antiga do JUCE) — existia antes desta fase, não introduzidos pela Fase 5.14

---

## 12. Limitações Conhecidas

| Limitação | Descrição | Plano |
|---|---|---|
| **Sample rate fixo (44100 Hz)** | O mapeamento bin→frequência usa `REF_SAMPLE_RATE = 44100.0f` como referência. A diferença visual para 48kHz é < 1px na escala log, mas o valor não é adaptativo. | Versão futura pode expor o `sampleRate` do processador para o editor via membro atômico |
| **Apenas canal L** | O push da FIFO usa apenas `buffer.getReadPointer(0)` (canal esquerdo). Para analisar sinal estéreo com precisão seria necessário misturar L+R antes do push. | Aceitável para análise mono de referência nesta fase |
| **Localização em `Source/`** | `AnalyzerFIFO` e `AnalyzerProcessor` foram colocados em `Source/` em vez de `DSP/`, pois sua função é transporte e análise visual, não produção de áudio. | Decisão documentada; eventual reorganização pode ser feita em fase dedicada de arquitetura |
| **Sem display de pré-EQ** | Apenas o sinal pós-EQ é analisado. A v0.2 prevê mostrar pré e pós-EQ simultaneamente. | Roadmap Fase 6 |
