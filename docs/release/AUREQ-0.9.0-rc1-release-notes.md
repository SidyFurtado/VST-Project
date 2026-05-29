# AUREQ 0.9.0-rc1 — Release Notes

**Produto**: AUREQ — Parametric Equalizer  
**Fabricante**: ASTRA Audio  
**Versão**: 0.9.0  
**Label**: 0.9.0-rc1 (Release Candidate Interno)  
**Data**: 2026-05-27  
**Bundle ID**: `br.com.sidyfurtado.aureq`

---

## Sumário do Produto

AUREQ é um equalizador paramétrico premium de 8 bandas desenvolvido com JUCE/C++17, parte da **ASTRA Audio Suite**. Combina DSP musical de alta qualidade com interface gráfica premium inspirada em vidro líquido e estética espacial.

---

## Plataformas Suportadas (neste RC)

| Plataforma | Formato | Status |
|------------|---------|--------|
| macOS (Apple Silicon / Intel) | VST3 | ✅ Disponível |
| macOS | Standalone | ✅ Disponível |
| Windows (64-bit) | VST3 | 🔄 Build não gerado neste ambiente |
| Windows | Standalone | 🔄 Build não gerado neste ambiente |

---

## Requisitos de Sistema — macOS

- **macOS**: 10.13 High Sierra ou superior (recomendado: 12 Monterey+)
- **DAW compatível com VST3**: Reaper, Ableton Live, Bitwig, Logic Pro*, GarageBand*
- **Arquitetura**: Universal Binary (Apple Silicon + Intel) — *confirmar na versão final*
- **Espaço em disco**: ~50 MB (VST3 + Standalone)

> *Logic Pro e GarageBand suportam AU (Audio Units); o AUREQ neste RC está disponível apenas como VST3. Suporte AU é roadmap futuro.

---

## Funcionalidades do AUREQ 0.9.0-rc1

### Motor DSP
- 8 bandas paramétricas independentes
- 7 tipos de filtro por banda: Bell, Low Shelf, High Shelf, Low Cut, High Cut, Notch, Band Pass
- Slopes para Low/High Cut: 12, 24, 48 dB/oitava
- Input Gain e Output Gain globais
- Global Bypass
- Band Bypass individual

### Dynamic EQ
- Seguidor de envelope peak stereo-linked por banda
- Parâmetros por banda: DYN toggle, Range (±24 dB), Threshold, Attack, Release
- Soft-knee exponencial musical
- Disponível para Bell, Low Shelf e High Shelf

### Channel Mode
- 5 modos por banda: Stereo, Mid, Side, Left, Right
- Processamento Mid/Side com conversão e reconstrução L/R

### Preset System
- **148 factory presets** organizados por `Genres > Gênero > Fonte > Preset`
- 9 gêneros: Forró, Trap, Hip-Hop, Funk BR, Rock, Sertanejo, Pop, Gospel, Electronic
- Toolbox `By Source` com 22 presets técnicos genéricos
- User presets: salvar, carregar, deletar, refresh, reveal pasta
- Formato: `.aureqpreset` (XML APVTS)
- Localização: `~/Library/Application Support/AUREQ/Presets/`

### Comparação e Histórico
- A/B Compare com slots independentes e ações de copy
- Undo/Redo até 32 estados (gestos, presets, bypass, knobs)
- Atalhos: Cmd+Z / Cmd+Shift+Z / Cmd+Y

### Interface Gráfica
- Display gráfico com curva de EQ teórica em tempo real
- Handles arrastáveis por frequência e ganho
- Mouse wheel para Q
- Floating HUD por banda selecionada
- Analyzer FFT 2048 pontos, janelamento Hann, 30 Hz

### Localização
- Português do Brasil (padrão)
- English
- Persistência de preferência de idioma

---

## Changelog (da v0.1.0 ao 0.9.0-rc1)

| Fase | Feature |
|------|---------|
| 5.0–5.7 | Core JUCE/CMake, APVTS, motor DSP 8 bandas Bell |
| 5.8–5.8.2 | UI premium liquid glass, temas Dark/Light |
| 5.9–5.12 | Curva EQ, handles visuais, drag, mouse wheel Q |
| 5.13–5.13.3 | Redesenho premium, Bottom Panel, knobs |
| 5.14 | Analyzer FFT |
| 5.15–5.17 | 7 tipos de filtro DSP + UI, Band Activation |
| 5.18–5.20 | Cut Slopes, APVTS avançado (108 parâmetros) |
| 5.21 | Testes unitários DSP |
| 6.0–6.2 | Dynamic EQ core + UI + gain meter |
| 7.0–7.2 | Channel Mode core + UI + Bottom Panel polish |
| 8.0–8.4 | Preset System + 148 factory presets genre-first |
| 9.0–9.1 | A/B Compare + copy actions |
| 10.0–10.2 | Undo/Redo completo + keyboard shortcuts |
| 11.0–11.1 | User Presets save/load/delete/reveal |
| 12.0 | Product Readiness QA |
| 13.0–13.2 | Localização PT-BR/EN + acentos UTF-8 |
| 14.0 | Metadados RC: VERSION 0.9.0, Bundle ID sem espaços |
| 14.1–14.2 | QA RC automatizado + sign-off humano do Standalone |
| 15.0 | Packaging / Distribution Prep (este release) |

---

## Limitações Conhecidas neste RC

1. **DAW Validation pendente** — nenhuma DAW disponível no ambiente de build; o VST3 não foi testado em host.
2. **Sem assinatura de código** — RC não assinado e não notarizado. macOS Gatekeeper pode exibir aviso ao abrir.
3. **Sem instalador** — instalação manual conforme os guias em `docs/release/`.
4. **Windows build não disponível** — o ambiente de build atual é macOS. Build Windows é roadmap.
5. **AU (Audio Units) não disponível** — apenas VST3 e Standalone neste RC.
6. **Atalhos Cmd+Z podem ser interceptados por DAW** — menu Settings > Undo/Redo é o caminho confiável em host.

---

## Caminhos dos Artefatos (Staging RC)

- **VST3 (macOS)**: `release/AUREQ-0.9.0-rc1/macOS/VST3/AUREQ.vst3`
- **Standalone (macOS)**: `release/AUREQ-0.9.0-rc1/macOS/Standalone/AUREQ.app`

---

## Próximas Etapas

| Fase | Descrição |
|------|-----------|
| 15.1 | Visual Identity — ícones `.icns`/`.ico`, assets visuais |
| 15.2 | macOS Installer — `.pkg` via `pkgbuild`/`productbuild` |
| 15.3 | Windows Installer — Inno Setup ou NSIS |
| Futura | Code Signing + Notarização Apple e Windows |
| Futura | DAW Validation quando DAW disponível |
| Futura | AU (Audio Units) para Logic Pro/GarageBand |
