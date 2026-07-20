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
| macOS (Apple Silicon / Intel) | Audio Unit (AU) | ✅ Principal / recomendado |
| Windows (64-bit) | VST3 | ✅ Principal / recomendado |
| macOS | VST3 | 🧪 Compatibilidade/dev/QA |
| macOS | Standalone | 🧪 Ferramenta interna/dev/QA |

---

## Requisitos de Sistema — macOS

- **macOS**: 10.15 Catalina ou superior (recomendado: 12 Monterey+)
- **DAW compatível com Audio Unit (AU)**: Logic Pro, GarageBand, Final Cut Pro, Ableton Live, FL Studio macOS, Reaper, etc.
- **Arquitetura**: Universal Binary (Apple Silicon + Intel) — *confirmar na versão final*
- **Espaço em disco**: ~50 MB

> No macOS, a distribuição pública/tester é AU-first. VST3 macOS e Standalone permanecem caminhos de compatibilidade, desenvolvimento e QA.

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

1. **DAW Validation pendente** — validação humana em hosts ainda depende de testes em DAWs reais.
2. **Sem assinatura de código** — RC não assinado e não notarizado. macOS Gatekeeper pode exibir aviso ao abrir.
3. **Instalador macOS AU unsigned** — o pacote AU-first está publicado via GitHub Releases, mas ainda não possui Developer ID/notarização.
4. **Windows installer unsigned** — o instalador VST3 pode acionar SmartScreen.
5. **Standalone não é produto público principal** — permanece ferramenta interna/dev/QA.
6. **Atalhos Cmd+Z podem ser interceptados por DAW** — menu Settings > Undo/Redo é o caminho confiável em host.

---

## Caminhos dos Artefatos (Staging RC)

- **macOS AU Suite (público/tester)**: `https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`
- **Windows VST3 AUREQ (público/tester)**: `https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/AUREQ-1.0.0-Windows-Setup.exe`
- **VST3/Standalone macOS**: mantidos apenas como artefatos de compatibilidade/dev/QA.

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
