# Resumo Técnico: Fase 11.1 — User Preset Management

Este documento registra a implementação do gerenciamento básico de presets de usuário do **AUREQ**, mantendo factory presets em código e sem alterar APVTS layout, IDs, DSP ou analyzer FFT.

---

## 1. Objetivo da Fase
Adicionar ações de gerenciamento para presets de usuário existentes:
- Refresh User Presets;
- Reveal Presets Folder;
- Delete User Preset...

As ações de gerenciamento não capturam Undo porque não alteram o estado APVTS atual.

---

## 2. APIs Adicionadas no PresetManager
O `PresetManager` passou a expor:
- `getUserPresetFile (int index) const`;
- `getUserPresetDirectoryForUser() const`;
- `revealUserPresetDirectory() const`;
- `deleteUserPreset (int index)`.

Essas APIs operam somente sobre a pasta oficial de user presets:
```text
juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
    / AUREQ / Presets
```

No macOS, isso resolve para um caminho equivalente a:
```text
~/Library/Application Support/AUREQ/Presets/
```

---

## 3. Wrappers no PluginProcessor
O `PluginProcessor` expõe wrappers simples para a UI:
- `revealUserPresetDirectory() const`;
- `deleteUserPreset (int index)`.

Esses wrappers não capturam Undo e não modificam APVTS.

---

## 4. Browser de Presets
O submenu `User Presets` continua listando presets de usuário normalmente para carregamento.

Além disso, agora inclui:
- `Refresh User Presets`;
- `Reveal Presets Folder`;
- submenu `Delete User Preset...`.

IDs mantidos em faixas separadas:
- factory load: `presetIndex + 1`;
- save: `10000`;
- user load: `20000 + userPresetIndex`;
- user delete: `30000 + userPresetIndex`.

---

## 5. Refresh User Presets
`Refresh User Presets` chama `refreshUserPresets()` e atualiza apenas a lista interna carregada do disco.

Não altera APVTS, não captura Undo e não muda parâmetros.

---

## 6. Reveal Presets Folder
`Reveal Presets Folder` garante que a pasta oficial exista e chama `revealToUser()`.

Se a pasta não puder ser criada ou revelada, a UI mostra uma mensagem simples de erro sem crash.

---

## 7. Delete User Preset
`Delete User Preset...` mostra somente presets de usuário.

Fluxo:
1. o usuário escolhe um user preset no submenu de delete;
2. a UI mostra confirmação com o nome do preset;
3. se confirmado, o `PresetManager` tenta mover o arquivo para o Trash;
4. a lista é atualizada com `refreshUserPresets()`;
5. se falhar, a UI mostra mensagem de erro sem crash.

Se o preset deletado era o preset atual, o `presetLabel` permanece como estava nesta fase.

---

## 8. Proteções de Delete
Antes de mover para o Trash, `deleteUserPreset()` valida:
- índice dentro do range de user presets;
- arquivo existe e é arquivo real;
- extensão é exatamente `.aureqpreset`;
- diretório pai é exatamente a pasta oficial `AUREQ/Presets`;
- a ação usa `moveToTrash()`;
- se `moveToTrash()` falhar, não há fallback para delete permanente.

Factory presets nunca entram nessa lista e não podem ser deletados.

---

## 9. Fora Desta Fase
- Rename user preset;
- busca;
- tags;
- favoritos;
- subpastas;
- import/export avançado;
- cloud sync;
- marketplace;
- delete permanente como fallback.

---

## 10. Confirmações de Escopo
- **APVTS layout**: não alterado.
- **IDs de parâmetros**: não alterados.
- **DSP**: não alterado.
- **Analyzer FFT**: não alterado.
- **Factory presets**: preservados em código.
- **Livros/PDFs**: não alterados.

---

## 11. Resultado do Build
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

Observação: o build manteve warnings preexistentes de `AudioParameterFloat` deprecado.

---

## 12. Resultado dos Testes
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

## 13. Caminhos dos Artefatos
- **VST3**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- **Standalone**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
