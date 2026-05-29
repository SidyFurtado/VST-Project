# Resumo Técnico: Fase 11.0 — User Presets / Save Preset

Este documento registra a implementação inicial de presets de usuário do **AUREQ**, mantendo os presets de fábrica em código e salvando presets do usuário em disco.

---

## 1. Objetivo da Fase
Permitir salvar o estado atual do plugin como preset de usuário e carregar esses presets pelo browser existente, sem criar parâmetros novos, sem alterar APVTS layout, IDs, DSP ou analyzer FFT.

---

## 2. Arquitetura
- Factory presets continuam no vetor `presets` em código.
- User presets usam lista separada `userPresets`, carregada de arquivos no disco.
- O `PresetManager` passa a expor:
  - `refreshUserPresets()`;
  - `getNumUserPresets()`;
  - `getUserPresetName()`;
  - `loadUserPreset()`;
  - `saveCurrentUserPreset()`.
- O `PluginProcessor` expõe wrappers simples para a UI.

---

## 3. Local de Salvamento
Os arquivos são salvos em:
```text
juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
    / AUREQ / Presets
```

No macOS, isso resolve para um caminho equivalente a:
```text
~/Library/Application Support/AUREQ/Presets/
```

---

## 4. Formato
Cada user preset usa XML do APVTS:
- salvar: `apvts.copyState().createXml()`;
- carregar: `juce::XmlDocument::parse()`;
- validar root: `apvts.state.getType()`;
- restaurar: `apvts.replaceState()`.

Extensão: `.aureqpreset`.

---

## 5. Browser de Presets
O preset dropdown mantém as categorias de fábrica e adiciona:
- `Save Current Preset...`;
- submenu `User Presets`.

Quando não existem user presets, o submenu mostra `No User Presets` como item desabilitado.

IDs do menu:
- factory: `presetIndex + 1`;
- save: `10000`;
- user: `20000 + userPresetIndex`.

---

## 6. Save Current Preset
`Save Current Preset...` abre um `juce::AlertWindow` simples com campo de texto.

O nome é sanitizado removendo/substituindo caracteres inválidos:
```text
/ \ : * ? " < > |
```

Nomes vazios são recusados. Se já existir arquivo com o mesmo nome, é gerado um nome incremental:
```text
My Preset
My Preset 2
My Preset 3
```

Salvar preset não captura Undo, porque não altera o APVTS atual.

---

## 7. Load User Preset
Carregar user preset:
1. captura Undo checkpoint antes da restauração;
2. lê e valida XML;
3. restaura APVTS com `replaceState()`;
4. atualiza `presetLabel`, controles da banda selecionada, gráfico/analyzer visual e repaint.

Se leitura ou XML falhar, a operação retorna sem crash.

---

## 8. Fora Desta Fase
- Delete user preset;
- rename user preset;
- cloud sync;
- marketplace;
- import/export avançado;
- busca;
- subpastas;
- tags/favoritos para user presets;
- XML externo para factory presets.

---

## 9. Confirmações de Escopo
- **APVTS layout**: não alterado.
- **IDs de parâmetros**: não alterados.
- **DSP**: não alterado.
- **Analyzer FFT**: não alterado.
- **Factory presets**: preservados em código.
- **Livros/PDFs**: não alterados.

---

## 10. Resultado do Build
Comando executado:
```bash
cmake --build build
```

Resultado:
```text
[ 59%] Built target AUREQ
[ 72%] Built target AUREQ_Standalone
[ 86%] Built target AUREQ_VST3
[100%] Built target AUREQ_FilterTests
```

Status: **aprovado**.

---

## 11. Resultado dos Testes
Comando executado:
```bash
./build/tests/AUREQ_FilterTests
```

Resultado:
```text
[PASS] Biquad Coefficient Tests completed successfully.
[PASS] EQBand Block Processing Tests completed successfully.
[PASS] Filter Stability and Safety Tests completed successfully.
SUCCESS: All AUREQ DSP Filter Tests Passed!
```

Status: **aprovado**.

---

## 12. Caminhos dos Artefatos
- **VST3**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- **Standalone**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
