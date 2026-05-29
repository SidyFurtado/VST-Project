# Resumo Técnico: Fase 8.2 — Factory Preset Library + Categorized Preset Browser

Este documento registra a expansão da biblioteca de presets de fábrica do **AUREQ** e a organização do browser de presets em categorias e submenus usando `juce::PopupMenu`.

---

## 1. Objetivo da Fase
Expandir o `PresetManager` de 8 presets para uma biblioteca inicial com 55 presets de fábrica, mantendo todos os presets embutidos em código e organizando o dropdown por categoria, fonte/instrumento e gênero musical.

Não foram implementados presets de usuário, carregamento externo, A/B, undo/redo, busca ou favoritos.

---

## 2. Alterações de Arquitetura
- `PresetParameter` foi preservado sem mudanças.
- `Preset` agora contém:
  - `name`
  - `category`
  - `subcategory`
  - `tags`
  - `parameters`
- Foram adicionados getters seguros:
  - `getPresetCategory (int index) const`
  - `getPresetSubcategory (int index) const`
  - `getPresetTags (int index) const`
- As APIs existentes foram preservadas:
  - `getNumPresets()`
  - `getPresetName()`
  - `getCurrentPresetIndex()`
  - `getCurrentPresetName()`
  - `loadPreset()`

---

## 3. Biblioteca Final de Presets
Quantidade final: **55 presets**.

### Acesso Direto
1. Default Flat

### Vocals
2. Vocal Presence
3. Vocal Air
4. Vocal De-Harsh
5. Vocal De-Esser Dynamic
6. Vocal Warmth
7. Rap Vocal Cut Through
8. Trap Vocal Shine
9. Forro Vocal Clarity
10. Sertanejo Vocal Air
11. Backing Vocal Tuck

### Drums
12. Kick Punch
13. Kick Sub Control
14. Snare Crack
15. Snare Body
16. Drum Bus Punch
17. Drum Bus Glue
18. Hi-Hat Softener
19. Percussion Brightness

### Bass / 808
20. Bass Tight Low
21. Bass Warm Body
22. 808 Cleanup
23. 808 Punch
24. Bass Side Cleanup

### Instruments
25. Acoustic Guitar Clarity
26. Electric Guitar Bite
27. Piano Presence
28. Keys Air

### Mix Bus / Master
29. Master Air
30. Master Low Cleanup
31. Mix Bus Gentle Smile
32. Side Air
33. Mid Focus
34. Stereo Width Cleanup

### Genres
35. Forro Mix Cleanup
36. Forro Accordion Presence
37. Trap 808 Control
38. Trap Vocal Bright
39. Hip-Hop Vocal Focus
40. Funk BR Low Control
41. Rock Guitar Edge
42. Sertanejo Vocal Polish
43. Gospel Vocal Lift
44. Pop Airy Vocal
45. Electronic Top End Control

### Dynamic EQ
46. Dynamic Vocal Control
47. Dynamic De-Harsh 3k
48. Dynamic Sibilance 6k
49. Dynamic Low-Mid Control
50. Dynamic Mix Bus Tame

### Utility / Cleanup
51. Low Cut Cleanup
52. Mud Remover
53. Boxiness Cut
54. Harshness Control
55. Sub Rumble Clean

---

## 4. Browser Categorizado
O `presetLabel` continua abrindo um `juce::PopupMenu`, agora organizado assim:
- `Default Flat` fica como primeiro item do menu principal.
- Depois entram submenus:
  - `Vocals`
  - `Drums`
  - `Bass / 808`
  - `Instruments`
  - `Mix Bus / Master`
  - `Genres`
  - `Utility / Cleanup`
  - `Dynamic EQ`
- Dentro de `Genres`, há submenus:
  - `Forro`
  - `Trap`
  - `Hip-Hop`
  - `Funk BR`
  - `Rock`
  - `Sertanejo`
  - `Pop`
  - `Gospel`
  - `Electronic`

Cada item usa ID `presetIndex + 1`, e o callback converte `selectedId - 1` de volta para o índice real do `PresetManager`.

---

## 5. Compatibilidade de Navegação
Previous/next continuam navegando linearmente pelo vetor completo de presets.

O `presetLabel` continua mostrando apenas `getCurrentPresetName()`, sem categoria, subcategoria ou tags.

---

## 6. Cuidados Musicais
- Presets de master/mix bus usam ajustes sutis.
- Boosts foram mantidos moderados, normalmente entre `+0.8 dB` e `+2.2 dB`.
- Cortes corretivos foram mantidos em valores seguros, normalmente entre `-1.0 dB` e `-3.0 dB`.
- Dynamic EQ usa `dynamicRange` negativo quando a intenção é controlar sibilância, harshness, low-mid ou mix bus.
- Bandas não usadas continuam seguras por causa do reset completo antes da aplicação seletiva do preset.

---

## 7. Confirmações de Escopo
- **APVTS**: não alterado.
- **IDs**: não alterados.
- **DSP**: não alterado.
- **Analyzer FFT**: não alterado.
- **Save user preset**: não implementado.
- **Preset externo de arquivo**: não implementado.
- **A/B**: não implementado.
- **Undo/redo**: não implementado.
- **Livros/PDFs**: não lidos, não movidos e não alterados.

---

## 8. Resultado do Build
Comando executado:
```bash
cmake --build build
```

Resultado:
```text
[ 23%] Built target shared_code
[ 59%] Built target AUREQ
[ 72%] Built target AUREQ_Standalone
[ 86%] Built target AUREQ_VST3
[100%] Built target AUREQ_FilterTests
```

Status: **aprovado**. O build apresentou apenas warnings já conhecidos de depreciação da assinatura antiga de `juce::AudioParameterFloat`, sem erros.

---

## 9. Resultado dos Testes
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

## 10. Caminhos dos Artefatos
- **VST3**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- **Standalone**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
- **Testes**: `/Users/sidyziin/Documents/VST - Project/build/tests/AUREQ_FilterTests`

---

## 11. Próximo Passo Recomendado
Validar a biblioteca em material real por categoria, priorizando vozes, 808/baixo, drum bus e mix bus antes de avançar para busca, favoritos, user presets ou carregamento externo.
