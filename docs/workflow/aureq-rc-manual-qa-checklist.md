# AUREQ RC Manual QA Checklist — Fase 14.1

Release Candidate: **0.9.0-rc1**  
Data de execução: 2026-05-27  
Executor: Antigravity (agente) + validação humana final

## Legenda

| Status | Significado |
|--------|-------------|
| `[PASS]` | Validado nesta fase (automático ou por abertura de processo) |
| `[PARTIAL]` | Auditado por código/plist mas requer clique/teste humano |
| `[PENDING]` | Pendente de validação humana no Standalone ou DAW |
| `[BLOCKED]` | Bloqueado por ambiente (sem DAW, sem permissão de captura) |
| `[FAIL]` | Falha detectada — bug documentado |

---

## Bloco A — Build & Artefatos

- [PASS] `cmake --build build` conclui sem erros
- [PASS] `./build/tests/AUREQ_FilterTests` — 100% PASS (3 suites, 0 falhas)
- [PASS] Artefato VST3 existe: `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- [PASS] Artefato Standalone existe: `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
- [PASS] `CFBundleIdentifier` = `br.com.sidyfurtado.aureq` (sem espaços) — verificado no plist do VST3
- [PASS] `CFBundleIdentifier` = `br.com.sidyfurtado.aureq` (sem espaços) — verificado no plist do Standalone
- [PASS] `CFBundleName` = `AUREQ` — verificado nos dois binários
- [PASS] `CFBundleShortVersionString` = `0.9.0` — verificado nos dois binários
- [PASS] `CFBundleVersion` = `0.9.0` — verificado nos dois binários
- [PARTIAL] `COMPANY_NAME` = `ASTRA Audio` — definido no CMakeLists.txt, verificação de exibição no host requer DAW

---

## Bloco B — Standalone (Smoke Test)

- [PASS] Standalone abre sem crash (processo PID 12225 confirmado em execução)
- [PARTIAL] UI principal renderiza corretamente (sem artefatos visuais) — requer inspeção visual humana
- [PARTIAL] Top Bar visível: logo AUREQ, preset browser, A/B, Settings, Theme, Bypass — requer inspeção visual
- [PARTIAL] Bottom Panel visível: Input Gain, Output Gain, Selected Band Card — requer inspeção visual
- [PARTIAL] EQGraph visível: grid, curva, handles — requer inspeção visual
- [PARTIAL] Analyzer FFT ativo no fundo do gráfico — requer inspeção visual
- [PENDING] Fechar e reabrir: Standalone reinicia sem crash
- [PENDING] Fechar e reabrir: user presets continuam listáveis

> **Nota de ambiente**: `screencapture -x` falhou com "could not create image from display" (limitação do sandbox macOS nesta sessão). Abertura confirmada por processo em execução.

---

## Bloco C — Localização PT-BR / EN

- [PARTIAL] Idioma padrão é Português (Brasil) na abertura — definido como padrão em código, requer confirmação visual
- [PARTIAL] Acentos exibidos corretamente: `Gêneros`, `Estéreo`, `Percussão`, `Violão` — corrigidos na Fase 13.2 com `juce::CharPointer_UTF8`, requer confirmação visual
- [PARTIAL] Separador do footer sem mojibake (`ASTRA Audio Suite · v0.1.0`) — corrigido na Fase 13.2, requer confirmação visual
- [PENDING] Trocar para English via `Config. > Language` — textos mudam
- [PENDING] Trocar de volta para `Português (Brasil)` — textos voltam
- [PENDING] Persistência de idioma: fechar/reabrir mantém último idioma selecionado
- [PENDING] Dialogs PT-BR: salvar user preset, deletar user preset (textos acentuados OK)

---

## Bloco D — Preset Browser

- [PARTIAL] `Default Flat` aparece no topo do menu — definido em código como primeiro item
- [PARTIAL] `Save Current Preset...` aparece no topo — definido em código
- [PARTIAL] `Genres` é o primeiro grupo musical após `Default Flat` — estrutura validada na Fase 8.3D
- [PENDING] `Genres > Forro` > fontes > presets navegáveis
- [PENDING] `Genres > Trap` > fontes > presets navegáveis
- [PENDING] `Genres > Hip-Hop` > fontes > presets navegáveis
- [PENDING] `Genres > Funk BR` > fontes > presets navegáveis
- [PENDING] `Genres > Rock` > fontes > presets navegáveis
- [PENDING] `Genres > Sertanejo` > fontes > presets navegáveis
- [PENDING] `Genres > Pop` > fontes > presets navegáveis
- [PENDING] `Genres > Gospel` > fontes > presets navegáveis
- [PENDING] `Genres > Electronic` > fontes > presets navegáveis
- [PENDING] `By Source` > Vocals, Drums, Bass / 808, Instruments, Mix Bus, Dynamic EQ, Utility navegáveis
- [PENDING] Carregar factory preset atualiza `presetLabel` corretamente
- [PENDING] Previous/Next navega linearmente pelos 148 factory presets
- [PENDING] Previous/Next não pula nem duplica presets

---

## Bloco E — User Presets

- [PENDING] `Save Current Preset...` abre dialog de nome (texto acentuado OK em PT-BR)
- [PENDING] Salvar preset cria arquivo `.aureqpreset` em `~/Library/Application Support/AUREQ/Presets/`
- [PENDING] Preset salvo aparece em `User Presets` após salvar
- [PENDING] `Refresh User Presets` recarrega lista sem alterar APVTS
- [PENDING] `Reveal Presets Folder` abre pasta no Finder
- [PENDING] Carregar user preset aplica configuração corretamente
- [PENDING] `Delete User Preset...` exibe lista apenas de user presets
- [PENDING] Delete move arquivo para Lixeira (não deleta permanentemente)
- [PENDING] Após delete: preset some do submenu após refresh
- [PENDING] Fechar/reabrir Standalone: user presets permanecem no menu

---

## Bloco F — A/B Compare

- [PENDING] Clicar A/B alterna entre slots A e B (label muda)
- [PENDING] Slot A e B podem ter configurações distintas de curva
- [PENDING] Alternância A→B→A preserva a curva de cada slot
- [PENDING] Clique direito / Ctrl-clique no botão A/B abre menu contextual
- [PENDING] Menu contextual: `Copy A to B`
- [PENDING] Menu contextual: `Copy B to A`
- [PENDING] Menu contextual: `Copy Current to Other`
- [PENDING] Menu contextual: `Reset B to Current`
- [PENDING] Ações de copy não trocam o slot ativo

---

## Bloco G — Undo / Redo

- [PENDING] `Settings > Undo` está disponível após uma ação
- [PENDING] `Settings > Redo` está disponível após um Undo
- [PENDING] `Cmd+Z` executa Undo
- [PENDING] `Cmd+Shift+Z` executa Redo
- [PENDING] `Cmd+Y` executa Redo (fallback)
- [PENDING] Undo após carregar preset restaura estado anterior
- [PENDING] Undo após arrastar handle restaura posição anterior
- [PENDING] Undo após mover knob restaura valor anterior
- [PENDING] Undo após toggle DYN restaura estado anterior
- [PENDING] Undo após toggle Band Bypass restaura estado anterior
- [PENDING] Undo após toggle Global Bypass restaura estado anterior
- [PENDING] Undo stack limitado a 32 — sem memory leak perceptível

---

## Bloco H — Dynamic EQ

- [PENDING] Botão `DYN` no Selected Band Card ativa/desativa equalização dinâmica
- [PENDING] Knob `Range` aplica range dinâmico (positivo ou negativo)
- [PENDING] Knob `Threshold` responde ao nível de entrada
- [PENDING] Knob `Attack` aplica tempo de ataque
- [PENDING] Knob `Release` aplica tempo de release
- [PENDING] Badge `DYN` aparece no handle de banda com DYN ativo
- [PENDING] Linha tracejada de range dinâmico aparece no gráfico
- [PENDING] Tipos Low/High Cut ficam sem DYN (desabilitado)
- [PENDING] Bell, Low Shelf, High Shelf habilitam DYN normalmente

---

## Bloco I — Channel Mode

- [PENDING] Botões `ST / M / S / L / R` aparecem no Selected Band Card
- [PENDING] `ST` (Stereo) processa L e R normalmente
- [PENDING] `M` (Mid) processa apenas canal Mid
- [PENDING] `S` (Side) processa apenas canal Side
- [PENDING] `L` (Left) processa apenas canal esquerdo
- [PENDING] `R` (Right) processa apenas canal direito
- [PENDING] Handle da banda indica Channel Mode ativo visualmente
- [PENDING] Floating HUD exibe Channel Mode ativo
- [PENDING] Trocar Channel Mode não afeta bandas não selecionadas

---

## Bloco J — Analyzer FFT

- [PARTIAL] Analyzer FFT presente no código (`AnalyzerFIFO`, `AnalyzerProcessor`, pipeline 30 Hz) — sem alteração nesta fase
- [PENDING] Analyzer visível no fundo do gráfico como gradiente cyan/mint
- [PENDING] Analyzer responde a sinal de entrada (movimento visível)
- [PENDING] Analyzer não interfere na curva de EQ
- [PENDING] Analyzer não causa crash ou travamento durante uso contínuo

---

## Bloco K — Metadados no Host (DAW)

- [BLOCKED] Plugin aparece no scanner da DAW como "AUREQ" — nenhuma DAW detectada no ambiente
- [BLOCKED] Fabricante exibido como "ASTRA Audio" — nenhuma DAW detectada
- [BLOCKED] Bundle ID sem espaços confirmado no host — nenhuma DAW detectada
- [BLOCKED] Versão exibida como `0.9.0` no host — nenhuma DAW detectada
- [BLOCKED] Parâmetros APVTS expostos para automação — nenhuma DAW detectada
- [BLOCKED] Automação de parâmetro funciona em playback — nenhuma DAW detectada
- [BLOCKED] Estado do plugin salva/carrega com o projeto da DAW — nenhuma DAW detectada
- [BLOCKED] Abrir/fechar GUI do plugin dentro da DAW sem crash — nenhuma DAW detectada
- [BLOCKED] Múltiplas instâncias do plugin são independentes — nenhuma DAW detectada

> **DAWs verificadas**: Reaper, Ableton Live, Logic Pro, Bitwig, GarageBand, Cubase, Studio One, Nuendo, FL Studio, Pro Tools.  
> **Resultado**: nenhuma encontrada em `/Applications/`. Bloco K marcado como `[BLOCKED]` conforme plano.

---

## Bloco L — Estabilidade Geral

- [PASS] Nenhum crash no Standalone durante abertura (processo ativo confirmado)
- [PENDING] Nenhum crash ao carregar 5+ factory presets em sequência
- [PENDING] Nenhum crash ao criar e deletar user preset
- [PENDING] Nenhum crash ao navegar A/B rapidamente
- [PENDING] Nenhum crash ao executar Undo/Redo múltiplos
- [PENDING] Nenhum travamento de UI ao usar Dynamic EQ com sinal de entrada
- [PENDING] Nenhum vazamento de processo após fechar Standalone

---

## Resumo de Status RC

| Bloco | PASS | PARTIAL | PENDING | BLOCKED | FAIL |
|-------|------|---------|---------|---------|------|
| A — Build & Artefatos | 9 | 1 | 0 | 0 | 0 |
| B — Standalone | 1 | 5 | 2 | 0 | 0 |
| C — Localização | 0 | 3 | 4 | 0 | 0 |
| D — Preset Browser | 0 | 3 | 13 | 0 | 0 |
| E — User Presets | 0 | 0 | 10 | 0 | 0 |
| F — A/B Compare | 0 | 0 | 9 | 0 | 0 |
| G — Undo/Redo | 0 | 0 | 12 | 0 | 0 |
| H — Dynamic EQ | 0 | 0 | 9 | 0 | 0 |
| I — Channel Mode | 0 | 0 | 9 | 0 | 0 |
| J — Analyzer FFT | 0 | 1 | 4 | 0 | 0 |
| K — DAW (Host) | 0 | 0 | 0 | 9 | 0 |
| L — Estabilidade | 1 | 0 | 7 | 0 | 0 |
| **Total** | **11** | **13** | **79** | **9** | **0** |

---

## Critérios de Aprovação RC

| Critério | Status |
|----------|--------|
| `cmake --build build` passa | ✅ PASS |
| `AUREQ_FilterTests` — 100% PASS | ✅ PASS |
| Artefato VST3 existe | ✅ PASS |
| Artefato Standalone existe | ✅ PASS |
| Standalone abre sem crash | ✅ PASS |
| Metadados corretos nos binários | ✅ PASS |
| APVTS / IDs / DSP / analyzer / presets não alterados | ✅ PASS |
| VST3 carrega em DAW | ⚠️ BLOCKED (sem DAW no ambiente) |
| Zero FAIL | ✅ PASS |

---

## Itens para Sessão Humana de Click-Through

Os itens `[PENDING]` abaixo requerem uma sessão humana no Standalone:

1. **Preset browser**: navegar por `Genres > cada gênero > cada fonte > cada preset` e confirmar carregamento.
2. **Previous/Next**: percorrer os 148 factory presets linearmente.
3. **User presets**: salvar, fechar/reabrir, carregar, deletar e confirmar.
4. **A/B**: configurar curvas distintas, alternar, usar menu contextual de copy.
5. **Undo/Redo**: executar cada gesto principal e confirmar restauração.
6. **Dynamic EQ**: ativar DYN em banda Bell/Shelf, ajustar Range/Threshold com sinal de entrada.
7. **Channel Mode**: testar cada modo ST/M/S/L/R com sinal estéreo.
8. **Analyzer FFT**: confirmar movimento e ausência de crash durante uso contínuo.
9. **Localização**: trocar idiomas, confirmar acentos, confirmar persistência.
10. **DAW** (quando disponível): carregar VST3, automatizar, salvar/reabrir projeto.
