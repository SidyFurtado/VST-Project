# Resumo Técnico: Fase 8.0 — Preset Manager & Factory Presets

Este documento resume a especificação técnica, a arquitetura e a validação do novo gerenciador de presets (`PresetManager`) e os 8 presets de fábrica integrados ao equalizador **AUREQ**.

---

## 1. Skills Consultadas
- **safety-guardian.skill.md**: Para assegurar a ausência de alterações destrutivas no APVTS/DSP e preservar a contagem de 108 parâmetros.
- **juce-cmake-engineer.skill.md**: Para integrar e compilar o `PresetManager` de forma limpa.
- **ui-design-system.skill.md**: Para manter a UI premium ("liquid glass") intacta, estendendo a lógica do Preset Browser na Top Bar.
- **dsp-engineer.skill.md**: Para garantir o correto mapeamento musical e seguro dos parâmetros DSP de cada preset de fábrica.
- **qa-build-validator.skill.md**: Para validar a compilação e rodar a suíte de testes unitários automatizados.

---

## 2. Arquivos Alterados
- [PluginEditor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/PluginEditor.cpp): Conectou botões previous/next da Top Bar, atualizou cliques e adicionou sincronização periódica de `presetLabel` e recarga de controles selecionados.
- [implementation-plan.md](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Docs/implementation-plan.md): Atualizou o roadmap para marcar a Fase 8.0 como concluída.

*(Nota: Os arquivos abaixo foram adicionados/modificados nos commits anteriores desta fase e revisados para compilação/teste final:)*
- [PresetManager.h](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/PresetManager.h)
- [PresetManager.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/PresetManager.cpp)
- [PluginProcessor.h](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/PluginProcessor.h)
- [PluginProcessor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/PluginProcessor.cpp)
- [CMakeLists.txt](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/CMakeLists.txt)

---

## 3. Arquivos Criados
- [phase-8-0-preset-manager-summary.md](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/workflow/phase-8-0-preset-manager-summary.md) (este arquivo).

---

## 4. Arquitetura do PresetManager
O `PresetManager` foi implementado em memória pura (evitando arquivos XML externos e manipulação de disco nesta etapa). 
- Ele recebe uma referência do `juce::AudioProcessorValueTreeState` no construtor.
- Define a estrutura `Preset` contendo o nome e uma lista de pares chave-valor (`PresetParameter` com ID e valor bruto/musical).
- Expõe métodos públicos thread-safe na mensagem thread para navegação e consulta de presets (`getCurrentPresetIndex()`, `getCurrentPresetName()`, `loadPreset()`).

---

## 5. Lista de Presets Criados
1. **Default Flat**: Todas as bandas em estado neutro/desativadas.
2. **Vocal Presence**: Limpeza de sub-graves e reforço nos agudos/médios-agudos para destaque de vozes.
3. **Drum Punch**: Preservação de transientes de grave/agudo e atenuação de ressonâncias de caixa.
4. **Master Air**: Ajustes refinados de agudos e ar com atuação espacial separada (Mid vs. Side).
5. **Low Cut Cleanup**: Limpeza rigorosa de infra-graves por canal (Mid/Side).
6. **Dynamic Vocal Control**: Uso de equalização dinâmica para controle de sibilância.
7. **Mid Focus**: Foco de frequências médias focado no canal Mid.
8. **Side Air**: Abertura estéreo por meio de ganho de altas frequências apenas nas laterais (Side).

---

## 6. Como cada Preset usa Recursos do AUREQ
- **Vocal Presence**: Usa HPF (Low Cut) de 24 dB/oct em 80 Hz, Bell com +2.5 dB em 3.0 kHz e High Shelf com +2.0 dB em 10.0 kHz.
- **Drum Punch**: Usa HPF de 24 dB/oct em 30 Hz, Bell com +4.0 dB em 60 Hz para punch, atenuação Bell com -3.0 dB em 250 Hz e Bell de +3.0 dB em 5.0 kHz para ataque/estalo.
- **Master Air**: Usa sutil reforço de +1.2 dB High Shelf em 15.0 kHz no canal **Mid**, e +2.0 dB High Shelf em 12.0 kHz no canal **Side**.
- **Low Cut Cleanup**: Usa HPF de 48 dB/oct em 35 Hz globalmente e HPF secundário de 24 dB/oct em 120 Hz apenas nos canais **Side**.
- **Dynamic Vocal Control**: Ativa o Dynamic EQ (DYN) nas bandas 4 (2.8 kHz, threshold -22 dBFS, range 4.0 dB) e 5 (6.5 kHz, threshold -18 dBFS, range 6.0 dB) para redução dinâmica de sibilância/aspereza.
- **Mid Focus**: Usa reforço Bell nos canais **Mid** em 500 Hz (+2.0 dB) e 1.5 kHz (+1.5 dB).
- **Side Air**: Usa reforço High Shelf nos canais **Side** em 8.0 kHz (+3.0 dB).

---

## 7. Como Presets são Aplicados ao APVTS
Para evitar poluição residual de parâmetros entre presets, a rotina de carregamento executa o seguinte fluxo ordenado:
1. **Reset Completo**: Todas as 8 bandas de EQ são definidas para os valores padrão seguros (Gain = 0 dB, Type = Bell, Bypass = false, Slopes = 12 dB/oct, Channel Mode = Stereo, Dynamic = false, etc.).
2. **Aplicação Seletiva**: Apenas as chaves explícitas do preset são escritas por cima do estado resetado.
3. **Mapeamento de Parâmetros**: A escrita usa `setValueNotifyingHost()` passando o valor mapeado na faixa normalizada `[0.f, 1.f]` através da função `convertTo0to1()` pertencente ao parâmetro obtido no APVTS.
4. **Gestures**: Toda modificação é encapsulada entre `beginChangeGesture()` e `endChangeGesture()`.

---

## 8. Como Previous/Next Funcionam
No `PluginEditor.cpp`, as ações `prevPresetBtn.onClick` e `nextPresetBtn.onClick` realizam a navegação cíclica matemática:
```cpp
int nextIndex = (pm->getCurrentPresetIndex() + offset + numPresets) % numPresets;
audioProcessor.loadPreset (nextIndex);
```
Após o carregamento, a UI chama `updateSelectedBandControls()` para restabelecer os attachments de sliders/knobs com a nova configuração da banda selecionada e dispara um `repaint()` para atualizar o display do gráfico em tempo real.

---

## 9. Como o presetLabel é Atualizado
- O `presetLabel` exibe o nome atual obtido de `audioProcessor.getCurrentPresetName()`.
- Ele é atualizado imediatamente no clique dos botões previous/next.
- Além disso, para lidar com automação e modificação externa, a sincronização é feita periodicamente dentro do `timerCallback()` na thread principal (Message Thread).

---

## 10. Limitações Conhecidas
- Presets são embutidos e não-modificáveis pelo usuário nesta fase (carregamento de arquivos XML externos ou "Save User Preset" serão abordados em fases posteriores).
- Não há dropdown de seleção direta (browser dropdown / popup menu) nesta versão, apenas navegação linear cíclica por botões.

---

## 11. Resultado do Build
A compilação do projeto com o CMake completou com sucesso absoluto:
```bash
cmake --build build
# [ 23%] Built target shared_code
# [ 59%] Built target AUREQ
# [ 72%] Built target AUREQ_Standalone
# [ 86%] Built target AUREQ_VST3
# [100%] Built target AUREQ_FilterTests
```

---

## 12. Resultado dos Testes
Todos os testes de validação DSP executados via terminal foram concluídos com sucesso:
```bash
./build/tests/AUREQ_FilterTests
# ========================================================
#          AUREQ - DSP FILTER VALIDATION TESTS            
# ========================================================
# [SUITE] Running Biquad Coefficient Tests...
# [PASS] Biquad Coefficient Tests completed successfully.
# [SUITE] Running EQBand Block Processing Tests...
# [PASS] EQBand Block Processing Tests completed successfully.
# [SUITE] Running Filter Stability and Safety Tests...
# [PASS] Filter Stability and Safety Tests completed successfully.
# ========================================================
#       SUCCESS: All AUREQ DSP Filter Tests Passed!      
# ========================================================
```

---

## 13. Confirmação de que APVTS não foi Alterado
Confirmado. Nenhuma modificação estrutural foi efetuada nos parâmetros definidos pelo APVTS. O layout mantém rigorosamente os **108 parâmetros** registrados sem introduzir novos IDs ou alterar tipos.

---

## 14. Confirmação de que DSP não foi Alterado
Confirmado. O núcleo DSP (`BiquadFilter`, `EQBand`, `EQProcessorCore`, etc.) permaneceu inalterado. Os presets agem apenas como modificadores externos via automação APVTS.

---

## 15. Caminho do VST3
- `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

---

## 16. Caminho do Standalone
- `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

---

## 17. Próximo Passo Recomendado
- Seguir para a **Fase 6.2 (A/B Comparison)** ou **Undo/Redo** de acordo com a ordem de prioridades do monorepo.
