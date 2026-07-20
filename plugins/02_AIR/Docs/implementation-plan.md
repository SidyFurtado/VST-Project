# LUMINAR — Implementation Plan
## ASTRA Audio Suite — High-Frequency Enhancer

**Plugin:** LUMINAR  
**Diretório:** `plugins/02_AIR/`  
**Versão:** 0.1.0 (skeleton)  
**Suite:** ASTRA Audio Suite

---

## Fases de Implementação

### ✅ Fase 18.0 — Research & Product Direction
- Estudo de DSP: exciter harmônico, saturação seletiva, aliasing, oversampling
- Product brief completo
- Arquitetura Hybrid (C) selecionada

### ✅ Fase 18.1 — Plugin Skeleton (atual)
**Escopo:**
- Estrutura de diretórios criada
- CMakeLists.txt com Plugin_Code Lmnr, Bundle ID br.com.sidyfurtado.luminar
- PluginProcessor com 4 parâmetros APVTS (LUMINAR_PARAMS)
- Output Gain funcional com smoothing exponencial (10ms)
- UI: LuminarLookAndFeel com acento âmbar #F6C90E
- UI: Top bar, aurora display estático, knob panel glass, footer
- 4 knobs: Presence, Air, Smooth, Output — todos conectados ao APVTS
- DSP: pass-through limpo; Presence/Air/Smooth sem efeito sonoro nesta fase

### ✅ Fase 18.2 — DSP Core
**Escopo implementado:**
- `DSP/AirWaveshaper.h/.cpp` — `tanh(drive*x)/tanh(drive)` normalizado com clamping
- `DSP/AirProcessor.h/.cpp` — Air Stage paralelo conservador
- HPF biquad proprio para split Air (~8 kHz)
- HPF pos-shaper (~6.5 kHz) para manter a camada no topo do espectro
- Air: drive + mix do exciter controlados por `luminar_air`
- Presence: adiado, permanece sem efeito sonoro nesta fase
- Smooth: adiado para Smooth Guard completo na Fase 18.3
- Oversampling: nao implementado nesta fase; aliasing documentado como risco futuro

### 🟨 Fase 18.2B — Air Stage Musical Calibration QA
**Status:** preparada; aguardando QA auditiva com audio real.

**Escopo documental/QA:**
- Checklist musical criado para validar o Air Stage atual.
- Fontes obrigatorias: vocal masculino, vocal feminino, rap vocal, acoustic guitar, piano/keys, drum loop/overhead, mix bus simples e master/mix completo se disponivel.
- Air deve ser avaliado em 0%, 30%, 60% e 100%.
- Nenhum DSP, APVTS, UI, AUREQ ou installer alterado nesta preparacao.
- Ajustes de `mix max`, curva `pow`, `drive max`, HPF inicial, `postShapeGain` ou HPF pos-shaper so devem ocorrer em fase posterior, depois da QA auditiva.

### ✅ Fase 18.2C — Standalone Wrapper UX Decision
**Status:** NO ACTION.

**Decisao:**
- A barra `Options` / `Settings...` / `Audio input is muted...` vem do `StandaloneFilterWindow` padrao do JUCE.
- Ela nao faz parte da UI interna do AUREQ/LUMINAR e nao aparece no VST3 dentro das DAWs.
- Produto principal continua sendo VST3.
- Standalone permanece ferramenta interna de desenvolvimento/QA.
- Installers beta continuam plugin-only VST3.
- Custom Standalone App Wrapper fica para fase futura.

### ✅ Fase 18.2D — Air Stage Audibility Calibration
**Status:** implementada; aguardando novo teste auditivo no FL Studio.

**Escopo:**
- Calibracao exclusiva do Air Stage para aumentar audibilidade.
- `postShapeGain`: `0.62 -> 1.25`.
- Drive: `1.05 + air * 1.35 -> 1.20 + air * 2.80`.
- Mix: `0.20 * pow(air, 1.25) -> 0.40 * pow(air, 0.85)`.
- HPF inicial mantido em ~8 kHz.
- HPF pos-shaper mantido em ~6.5 kHz.
- Nenhum APVTS, UI, AUREQ, installer, Presence DSP, Smooth Guard ou oversampling alterado.

### ✅ Fase 18.2E — Air Engine Redesign / Fresh-Air-Class Audibility Pass
**Status:** implementada; pronta para novo teste auditivo no FL Studio.

**Escopo:**
- Dual Parallel Enhancement Engine dentro de `AirProcessor`.
- Presence deixou de ser inativo e passou a controlar uma camada paralela em ~4.2 kHz.
- Air foi reforcado com HPF inicial em ~6.8 kHz, mix maximo maior e drive mais audivel.
- `AirWaveshaper` agora permite drive interno ate `5.0`.
- Compensacao leve de loudness aplicada dentro do motor antes do Output Gain.
- Smooth continua sem efeito nesta fase.
- Nenhum parametro novo, APVTS ID, UI, AUREQ, installer, CMake raiz, Smooth Guard ou oversampling alterado.

### ✅ Fase 18.2F — Fresh-Air-Class Texture Calibration
**Status:** implementada; pronta para novo teste auditivo no FL Studio.

**Escopo:**
- Calibracao de forca e textura do motor Presence + Air.
- Presence reforcado: centro em ~3.9 kHz, wet gain `2.6`, mix maximo `0.42`.
- Air reforcado: HPF inicial em ~6.4 kHz, pos-shaper em ~5.9 kHz, wet gain `2.8`, mix maximo `0.70`.
- Texture Layer interna criada dentro do `AirProcessor`, sem parametro novo, derivada de Presence + Air.
- Texture Layer: band-pass em ~7.2 kHz, Q largo, saturacao suave paralela e mix pequeno.
- Compensacao de loudness reduzida para nao apagar a sensacao do enhancer.
- Nenhum APVTS ID, UI, AUREQ, installer, CMake raiz, Smooth Guard final ou oversampling alterado.

### ✅ Fase 18.3 — Smooth Guard
**Status:** implementada; pronta para teste auditivo no FL Studio.

**Escopo:**
- `luminar_smooth` agora e passado ao `AirProcessor`.
- Smooth Guard implementado no wet bus Presence + Texture + Air.
- Detector por canal em banda de harshness/sibilancia, centrado em ~6.5 kHz.
- Envelope follower com attack ~10 ms e release ~130 ms.
- Smooth 0% = praticamente sem reducao.
- Smooth 50% = protecao musical moderada.
- Smooth 100% = protecao forte, mas limitada para nao matar o brilho.
- Dry path, Output Gain, APVTS IDs, UI, AUREQ, installers, CMake raiz, oversampling e calibracao base 18.2F preservados.

### ✅ Fase 18.3C — LUMINAR Presence & Smooth Guard Audibility Calibration
**Status:** implementada; calibrada e pronta para build e testes.

**Escopo:**
- Calibração do braço Presence para ser audível a partir de 40-50%.
- Curva de mix alterada de `pow(presence, 0.78)` para `pow(presence, 0.60)` com mix máximo elevado de `0.42` para `0.55`.
- Aumento do `presenceWetGain` base de `2.6` para `3.2` e leve bump no drive máximo de `3.5` para `4.0`.
- Ativação do detector do Smooth Guard com ganho pré-detector de `2.5x` no sidechain.
- Constantes de tempo do envelope follower do Smooth Guard otimizadas para sibilância (Ataque: `4 ms`, Release: `90 ms`).
- Aumento da redução máxima do Smooth Guard de `5.0 dB` para `7.0 dB` para garantir atenuação real de `-6 dB` (ganho `0.50` no clamp).

### ✅ Fase 18.3D — LUMINAR Smooth Guard Aggressive Calibration
**Status:** implementada; calibrada para controle agressivo de sibilância.

**Escopo:**
- Aumento do ganho pré-detector (sidechain) de `2.5f` para `10.0f` para melhor rastreamento de sibilância.
- Expansão do limite de atenuação máxima (`mapSmoothMaxReductionDb`) de `7.0 dB` para `15.0 dB`.
- Redução do piso do clamp inferior de ganho de `0.50f` (-6 dB) para `0.18f` (aprox -15 dB) para permitir compressão nítida de asperezas em 100%.
- Preservação da transparência total em `0%` e resposta musical em `50%`.

### ✅ Fase 18.4 — Restauração do DNA Sonoro 18.2F
**Status:** implementada; DNA sonoro 18.2F totalmente restaurado e Smooth Guard temporariamente desativado (transparência total/bypass).

**Escopo:**
- Restauração de Presence: wet gain em `2.6f`, curva `pow(presence, 0.78f)` com mix máximo de `0.42`, drive máximo de `3.5`.
- Restauração de Air: wet gain em `2.8f`, curva `pow(air, 0.72f)` com mix máximo de `0.70`, drive máximo de `4.75`.
- Restauração de Texture: wet gain em `1.8f`, curva `pow(texture, 0.70f)` com mix de `0.16`.
- Loudness Compensation: teto máximo mantido em `-0.9 dB`.
- Desativação do Smooth Guard: `processSmoothGuard` alterado para retornar constantemente `1.0f` (transparência total para isolar a qualidade do áudio base).

### ⬜ Fase 18.4B — UI Base Refinement
- Animação leve do aurora display (reatividade ao knob Air)
- Refinamento de tipografia e espaçamento

### ✅ Fase 18.5 — LUMINAR Smooth Guard Precision Calibration (Surgical Mode)
**Status:** implementada; de-esser cirúrgico com threshold fixo e controle proporcional soft-knee funcional.

**Escopo:**
- Detector do Smooth Guard ativado usando o biquad de sidechain de ~6.5 kHz.
- Rastreamento por envelope follower clássico retificado.
- Limiar interno fixado em `0.025f` (~ -32 dB) para sensibilidade adequada a vocais comuns.
- Controle de atenuação soft-knee baseado em `excess / (excess + 0.050f)` e proporcional a `smoothAmount` (máximo de -12 dB a 100% de knob).
- Clamp de segurança rígido estabelecido em `0.25f` (-12 dB) para evitar silenciamento do wet bus.
- Preservação intacta de Presence, Air e Texture Layer com seus respectivos ganhos e curvas de mix da Fase 18.2F.

### ⬜ Fase 18.5B — UI Premium & LED
- LED indicador de Smooth Guard ativo
- Aurora glow sutil reativo ao Air knob

### ✅ Fase 18.6 — Remoção Definitiva do Smooth Guard e Simplificação do LUMINAR
**Status:** implementada; todos os vestígios do Smooth Guard removidos da APVTS, UI, Headers e DSP. Layout reorganizado para 3 knobs.

**Escopo:**
- Remoção do parâmetro `luminar_smooth` da APVTS em `PluginProcessor.cpp`.
- Remoção do ID correspondente no header `ParameterIDs.h`.
- Limpeza dos Sliders, Labels e Attachments de Smooth em `PluginEditor.cpp` e `PluginEditor.h`.
- Redesenho do painel de controle da UI com `kKnobGap = 90` e reposicionamento dinâmico centrado dos 3 knobs (`Presence`, `Air`, `Output`).
- Remoção completa dos filtros, envelopes e métodos do Smooth Guard do `AirProcessor.cpp` e `AirProcessor.h`.
- Preservação intacta de Presence, Air e Texture Layer com seus respectivos ganhos e curvas de mix da Fase 18.2F.

### ⬜ Fase 18.6B — Presets de Fábrica
- 20 presets: Vocal, Guitar, Piano, Drums, Mix Bus, Master, Utilitários
- PresetManager simplificado (sem user presets in v1)

### ✅ Fase 18.7 — Professional Deployment Infrastructure & Site Update
- Pipeline de build unificado `.github/workflows/astra-suite-build.yml` compilando LUMINAR e AUREQ (macOS Universal & Windows x64)
- Geração automática de instalador macOS Pro (`.pkg` via productbuild) com targets VST3 fixo em `/Library/Audio/Plug-Ins/VST3` e Standalone em `/Applications`
- Geração de instalador Windows Pro (`.exe` via Inno Setup) com escolha de componentes (VST3 e Standalone)
- Configuração de trigger do GitHub Releases para gerar Draft Release automático em tags `v*`
- Sincronização do site (`docs/site/index.html`): adição do card LUMINAR (Amber #F6C90E, curva high-shelf animada em SVG) e correção do mojibake do footer

### ✅ Fase 23.5A — AU Format Validation / Manufacturer Code Fix
**Status:** concluída; identidade AU corrigida antes da distribuição macOS AU-first.

**Escopo:**
- Substituição de `MANUFACTURER_CODE AStr` por `PLUGIN_MANUFACTURER_CODE AStr` no `juce_add_plugin(LUMINAR)`.
- Preservação de `PRODUCT_NAME "LUMINAR"`, `PLUGIN_CODE Lmnr`, `BUNDLE_ID "br.com.sidyfurtado.luminar"`, versão, DSP, APVTS, presets, UI e installers.
- Rebuild dos artefatos e confirmação de que o AU gerado usa manufacturer `AStr` em vez do fallback JUCE `Manu`.
- Validação com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.

### ✅ Fase 23.5C — macOS AU-First Suite PKG Generation
**Status:** concluída; `LUMINAR.component` incluído no primeiro instalador macOS AU-only da ASTRA Audio Suite.

**Escopo:**
- Cópia de `LUMINAR.component` para `installer/macOS/au-first-suite/payload/Library/Audio/Plug-Ins/Components/`.
- Geração de `ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU-component.pkg` e `ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`.
- Validação não destrutiva com `pkgutil --expand`, `lsbom` e `pkgutil --check-signature`.
- Confirmação de pacote AU-only, sem VST3, sem Standalone, sem instalação em `/Applications` e sem destino `/Library/Audio/Plug-Ins/VST3`.
- Nenhum DSP, APVTS, UI, preset, installer Windows ou GitHub Actions alterado.

### ✅ Fase 23.5D-R — AU Duplicate Cleanup & AudioUnit Cache Refresh
**Status:** concluída.

**Escopo:**
- Criação de backup das duplicatas em `backups/au-user-duplicates-cleanup/`.
- Remoção do componente de usuário legado em `~/Library/Audio/Plug-Ins/Components/LUMINAR.component`.
- Remoção do cache `~/Library/Caches/AudioUnitCache/com.apple.audiounits.cache`.
- Reinicialização do `AudioComponentRegistrar`.
- Revalidação bem-sucedida com `auval` sob o fabricante `AStr`.

### ✅ Fase 23.5E — Download Page / Beta Docs Update
**Status:** concluída.

**Escopo:**
- Atualizar links e especificações visuais de formato nos arquivos `docs/index.html` e `docs/site/index.html`.
- Substituir caixa de avisos simples por painel de avisos estruturado com notas beta, notas macOS de cache e notas Windows de SmartScreen.
- Refatorar os guias `AUREQ-0.9.0-rc1-private-beta-instructions.md`, `AUREQ-0.9.0-rc1-windows-beta-instructions.md` e `AUREQ-macos-installation-guide.md`.
- Injetar notas de limpeza de cache de Audio Units e aviso de SmartScreen no README.txt da suite.
- Atualizar checklist e planos de implementação no monorepo.

### ⬜ Fase 18.8 — Localization PT-BR / EN
- Adaptar padrão `Localization.h/.cpp` do AUREQ
- Labels dos knobs, categorias de presets, settings menu

### ⬜ Fase 18.9 — Testes Unitários DSP
- Suite `LUMINAR_DSPTests`
- Testes: waveshaper bounds, envelope follower, gain compensation
- Sample rates: 44.1, 48, 88.2, 96, 192 kHz

### ⬜ Fase 18.10 — QA Musical
- Lead vocal feminino pop
- Rap vocal
- Acoustic guitar
- Mix bus de projeto completo
- Checklist baseado no `aureq-manual-qa-checklist.md`

---

## Parâmetros APVTS

| ID | Nome | Range | Default |
|---|---|---|---|
| `luminar_presence` | Presence | 0.0 – 1.0 | 0.40 |
| `luminar_air` | Air | 0.0 – 1.0 | 0.30 |
| `luminar_smooth` | Smooth | 0.0 – 1.0 | 0.50 |
| `luminar_outputGain` | Output | -12.0 – +6.0 dB | 0.0 |

**APVTS State ID:** `LUMINAR_PARAMS`  
**Plugin Code:** `Lmnr`  
**Bundle ID:** `br.com.sidyfurtado.luminar`  
**Manufacturer Code:** `AStr`

---

## Arquitetura DSP Planejada (Fase 18.2+)

```
Entrada (L/R) ─────────────────── Dry path ─────────────────────────────────┐
      │                                                                        │
      ├─ PRESENCE STAGE (Fase 18.2)                                            │
      │   Dynamic Bell/Shelf (~3kHz) + Envelope → Presence knob → Gain ──── Σ │
      │   ↑ Smooth Guard (Fase 18.3)                                           │
      │                                                                        │
      └─ AIR STAGE (Fase 18.2)                                                 │
          HPF (8kHz) → tanh Waveshaper → HPF post → Gain Comp → Air mix ──── Σ │
                                                                               │
                                                                     Output Gain
                                                                               │
                                                                             Saída
```
