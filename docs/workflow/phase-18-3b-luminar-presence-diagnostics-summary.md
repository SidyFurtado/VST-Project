# Phase 18.3B — LUMINAR Presence Path & Smooth Guard Integration Diagnostics

## Status

Implementada; pronta para reinstalacao limpa e novo teste no FL Studio.

---

## 1. Objetivo

Diagnosticar a ausencia percebida do sinal de Presence na DAW e refatorar a arvore do wet bus para deixar o roteamento 100% explicito, sem alterar a calibracao musical aprovada na Fase 18.2F.

---

## 2. Diagnostico

O codigo ja lia `luminar_presence` no `PluginProcessor` e repassava o valor para o `AirProcessor`.

Tambem nao havia evidencia direta de sobrescrita de buffer: Presence, Texture e Air eram somados com `+=` em uma variavel local `wet`.

Causa provavel:

- **Cache/instalacao da DAW ainda e uma possibilidade forte**, especialmente se o FL Studio estava usando uma copia antiga do VST3.
- A refatoracao foi feita para remover qualquer ambiguidade estrutural no wet bus e facilitar auditoria futura.

---

## 3. Refatoracao Aplicada

Antes, os branches somavam diretamente em `wet` usando `+=`.

Agora o processamento usa variaveis explicitas:

```cpp
float presenceWet = 0.0f;
float textureWet = 0.0f;
float airWet = 0.0f;

// ...

float wet = presenceWet + textureWet + airWet;
wet *= processSmoothGuard (channel, airRead[sample], safeSmooth);
```

O Smooth Guard continua aplicado exclusivamente ao wet bus final, antes da soma com o dry path.

---

## 4. Arquivos Alterados

| Arquivo | Alteracao |
|---|---|
| `plugins/02_AIR/DSP/AirProcessor.cpp` | Wet bus explicitado como `presenceWet + textureWet + airWet` |
| `plugins/02_AIR/Docs/implementation-plan.md` | Registro da Fase 18.3B |

Nao foram alterados:

- APVTS IDs.
- UI.
- AUREQ.
- CMake raiz.
- Installers.
- Livros/PDFs.
- Calibracao Presence/Air/Texture da 18.2F.

---

## 5. Validacao

- `cmake --build build`: **PASS**
- `./build/tests/AUREQ_FilterTests`: **PASS**
  - Biquad Coefficient Tests: PASS
  - EQBand Block Processing Tests: PASS
  - Filter Stability and Safety Tests: PASS

Artefatos:

- `build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
- `build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## 6. Reinstalacao Limpa Recomendada

Depois do build, reinstalar o VST3 no macOS:

```bash
sudo rm -rf "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"
sudo cp -R "/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3" "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"
sudo xattr -dr com.apple.quarantine "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"
```

---

## 7. Teste Auditivo Recomendado

- Presence 70% / Air 0% / Smooth 0%: Presence isolado deve trazer vocal para frente.
- Presence 0% / Air 70% / Smooth 0%: Air isolado deve funcionar.
- Presence 70% / Air 70% / Smooth 50%: brilho, textura e controle de picos devem coexistir.
- Presence 0% / Air 0%: plugin transparente.
