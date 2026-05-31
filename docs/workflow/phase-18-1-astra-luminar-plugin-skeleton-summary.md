# Phase 18.1 — ASTRA LUMINAR Plugin Skeleton

## Status

Concluída.

---

## 1. Objetivo da Fase

Criar o esqueleto compilável do segundo plugin da ASTRA Audio Suite — **LUMINAR** — em `plugins/02_AIR/`, sem implementar ainda o DSP final de exciter, oversampling ou Smooth Guard completo.

---

## 2. Decisões Técnicas Aplicadas

| Decisão | Valor |
|---|---|
| Nome do produto | LUMINAR |
| PRODUCT_NAME CMake | LUMINAR |
| Plugin Code | Lmnr |
| Manufacturer Code | AStr |
| Bundle ID | br.com.sidyfurtado.luminar |
| APVTS State ID | LUMINAR_PARAMS |
| Prefixo de IDs APVTS | luminar_ |
| Namespace C++ | LUMINAR |
| Classe AudioProcessor | LuminarAudioProcessor |
| Classe AudioProcessorEditor | LuminarAudioProcessorEditor |
| Dimensão da UI | 800 × 420 px |
| Acento visual | Âmbar/Dourado #F6C90E |
| Output range | -12.0 dB a +6.0 dB |
| DSP ativo | Output Gain com juce::SmoothedValue (10ms) |

---

## 3. Arquivos Criados

| Arquivo | Tipo | Descrição |
|---|---|---|
| `plugins/02_AIR/CMakeLists.txt` | CMake | Configuração JUCE do plugin |
| `plugins/02_AIR/README.md` | Docs | Descrição do plugin |
| `plugins/02_AIR/Source/PluginProcessor.h` | C++ | Header do LuminarAudioProcessor |
| `plugins/02_AIR/Source/PluginProcessor.cpp` | C++ | Implementação: pass-through + Output Gain |
| `plugins/02_AIR/Source/ParameterIDs.h` | C++ | Namespace LUMINAR::Params com 4 IDs |
| `plugins/02_AIR/UI/PluginEditor.h` | C++ | Header: LuminarLookAndFeel + Editor |
| `plugins/02_AIR/UI/PluginEditor.cpp` | C++ | UI premium: top bar, aurora, knobs, footer |
| `plugins/02_AIR/UI/LuminarTheme.h` | C++ | Paleta de cores Dark Mode |
| `plugins/02_AIR/UI/LuminarTheme.cpp` | C++ | Implementação da paleta âmbar |
| `plugins/02_AIR/DSP/.gitkeep` | Git | Reservado para Fase 18.2 |
| `plugins/02_AIR/Presets/.gitkeep` | Git | Reservado para Fase 18.6 |
| `plugins/02_AIR/Tests/.gitkeep` | Git | Reservado para Fase 18.8 |
| `plugins/02_AIR/Docs/implementation-plan.md` | Docs | Plano de fases 18.1–18.10 |

---

## 4. Arquivos Alterados

| Arquivo | Alteração |
|---|---|
| `CMakeLists.txt` (raiz) | Adicionado `add_subdirectory(plugins/02_AIR)` |

Nenhum outro arquivo existente foi alterado. O AUREQ (`plugins/01_EQ/`) permanece intocado.

---

## 5. Integração CMake

O CMakeLists.txt raiz recebeu a seguinte adição entre o `add_subdirectory(tests)` e os placeholders:

```cmake
# Segundo plugin da ASTRA Audio Suite — LUMINAR (High-Frequency Enhancer)
add_subdirectory(plugins/02_AIR)
```

O plugin LUMINAR linka com `shared_code`, `juce::juce_audio_utils`, `juce::juce_dsp` e `juce::juce_gui_extra`, seguindo exatamente o padrão do AUREQ.

---

## 6. APVTS Criado

| ID APVTS | Nome | Range | Default |
|---|---|---|---|
| `luminar_presence` | Presence | 0.0 – 1.0 | 0.40 |
| `luminar_air` | Air | 0.0 – 1.0 | 0.30 |
| `luminar_smooth` | Smooth | 0.0 – 1.0 | 0.50 |
| `luminar_outputGain` | Output | -12.0 – +6.0 dB | 0.0 |

**State ID:** `"LUMINAR_PARAMS"` — sem colisão com `"PARAMETERS"` do AUREQ.

---

## 7. UI Criada

**Dimensões:** 800 × 420 px  
**Layout:**

| Seção | Y | Altura | Descrição |
|---|---|---|---|
| Top Bar | 0 | 56 px | Glass bar com "ASTRA AUDIO" (esquerda) e "L U M I N A R" (centro) |
| Aurora Display | 56 | 160 px | Gradiente escuro + glow âmbar radial + raios de luz + subtítulo |
| Knob Panel | 216 | 172 px | Glass panel com 4 knobs grandes (amber accent) |
| Footer | 388 | 32 px | "ASTRA Audio Suite • LUMINAR v0.1.0" |

**LuminarLookAndFeel:** knob com:
- Trilha inativa (azul-ardósia escuro)
- Trilha ativa (âmbar #F6C90E com glow)
- Tampa central (gradiente dark com reflexo especular)
- Ponteiro âmbar
- Valor exibido na base do knob

---

## 8. DSP Implementado na Fase 18.1

- **Output Gain:** `juce::Decibels::decibelsToGain()` aplicado via `juce::SmoothedValue<float>` com ramp de 10 ms — sem zipper noise.
- **Presence / Air / Smooth:** lidos do APVTS mas não alteram o sinal sonoro nesta fase (reservado para Fase 18.2).
- **Pass-through:** o sinal de áudio é copiado limpo para a saída, modificado apenas pelo Output Gain.

---

## 9. Salvaguardas Contra Colisão com AUREQ

| Ponto | AUREQ | LUMINAR |
|---|---|---|
| Plugin Code (DAW registry) | `Aurq` | `Lmnr` |
| APVTS State ID | `"PARAMETERS"` | `"LUMINAR_PARAMS"` |
| Parameter IDs | `outputGain`, `band1Freq`... | `luminar_presence`, `luminar_air`... |
| Namespace C++ | `AUREQ::Params` | `LUMINAR::Params` |
| Bundle ID | `br.com.sidyfurtado.aureq` | `br.com.sidyfurtado.luminar` |
| Acento UI | Cyan `#22D3EE` | Âmbar `#F6C90E` |

---

## 10. Próximo Passo Recomendado

**Fase 18.2 — DSP Core**

Criar:
- `plugins/02_AIR/DSP/AirWaveshaper.h/.cpp` — waveshaper `tanh` normalizado
- `plugins/02_AIR/DSP/AirEnvelopeFollower.h/.cpp` — detector de envelope
- `plugins/02_AIR/DSP/AirProcessor.h/.cpp` — pipeline Presence + Air + mix paralelo

Conectar o AirProcessor ao processBlock do LuminarAudioProcessor, fazendo Presence e Air começarem a afetar o sinal de forma musical.

---

## 11. Reconstrução e Verificação Pós-Sessão

**Data da reconstrução:** 2026-05-31  
**Motivo:** a sessão anterior foi fechada e o relatório final foi perdido. Esta seção registra a verificação forense do estado já existente no repositório.

### Arquivos encontrados em `plugins/02_AIR/`

```text
plugins/02_AIR/CMakeLists.txt
plugins/02_AIR/DSP/.gitkeep
plugins/02_AIR/Docs/implementation-plan.md
plugins/02_AIR/Presets/.gitkeep
plugins/02_AIR/README.md
plugins/02_AIR/Source/ParameterIDs.h
plugins/02_AIR/Source/PluginProcessor.cpp
plugins/02_AIR/Source/PluginProcessor.h
plugins/02_AIR/Tests/.gitkeep
plugins/02_AIR/UI/LuminarTheme.cpp
plugins/02_AIR/UI/LuminarTheme.h
plugins/02_AIR/UI/PluginEditor.cpp
plugins/02_AIR/UI/PluginEditor.h
```

### Checks de identidade do plugin

- `CMakeLists.txt` raiz contem `add_subdirectory(plugins/02_AIR)`.
- `plugins/02_AIR/CMakeLists.txt` contem `PRODUCT_NAME "LUMINAR"`.
- `plugins/02_AIR/CMakeLists.txt` contem `BUNDLE_ID "br.com.sidyfurtado.luminar"`.
- `plugins/02_AIR/CMakeLists.txt` contem `PLUGIN_CODE Lmnr`.
- `plugins/02_AIR/CMakeLists.txt` contem `MANUFACTURER_CODE AStr`.

### Checks APVTS

- `PluginProcessor.cpp` inicializa o APVTS com State ID `"LUMINAR_PARAMS"`.
- `ParameterIDs.h` define:
  - `luminar_presence`
  - `luminar_air`
  - `luminar_smooth`
  - `luminar_outputGain`
- `PluginEditor.cpp` conecta os quatro controles da UI aos IDs acima.

### Busca por sobras do nome antigo

Busca exata executada para:

```text
LUMINA
Lumina
lumina_
br.com.sidyfurtado.lumina
LUMINA_PARAMS
```

Resultado: **nenhuma sobra real encontrada**. Ocorrencias de `LUMINAR`/`Luminar` sao esperadas e nao indicam o nome antigo.

### Build e testes

- `cmake --build build`: **PASS**
  - Targets AUREQ mantidos.
  - Targets LUMINAR gerados: `LUMINAR`, `LUMINAR_Standalone`, `LUMINAR_VST3`.
- `./build/tests/AUREQ_FilterTests`: **PASS**
  - Biquad Coefficient Tests: PASS
  - EQBand Block Processing Tests: PASS
  - Filter Stability and Safety Tests: PASS

### Artefatos gerados

- VST3: `build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
- Standalone app: `build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

### Verificação AUREQ

- `plugins/01_EQ/` continua presente.
- `build/plugins/01_EQ/AUREQ_artefacts/` continua presente.
- Artefatos AUREQ encontrados:
  - `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
  - `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
- `git diff -- plugins/01_EQ` nao mostrou alteracoes no AUREQ.

### Status final

**PASS** — skeleton LUMINAR presente, integrado ao build, metadados corretos, APVTS correto, sem sobras reais do nome antigo, build aprovado, testes AUREQ aprovados e AUREQ preservado.
