# Resumo da Fase 5.1: APVTS e Mapeamento de Parâmetros

Este documento resume as atividades realizadas na Fase 5.1 do projeto AUREQ no monorepo VST - Project.

---

## 1. Objetivo da Fase
O objetivo desta fase foi estruturar e registrar todos os parâmetros do equalizador paramétrico AUREQ utilizando a classe `juce::AudioProcessorValueTreeState` (APVTS), sem introduzir processamento de áudio digital (DSP real), filtros IIR, analisador FFT ou interface final com knobs. O plugin deve permanecer em modo pass-through estável e 100% compilável.

---

## 2. Parâmetros Globais Criados

Foram definidos 4 parâmetros globais no layout de parâmetros do APVTS:

1. **Input Gain** (`inputGain`):
   - Tipo: `juce::AudioParameterFloat`
   - Faixa: `-24.0 dB` até `+24.0 dB` (resolução de `0.1 dB`)
   - Default: `0.0 dB`
2. **Output Gain** (`outputGain`):
   - Tipo: `juce::AudioParameterFloat`
   - Faixa: `-24.0 dB` até `+24.0 dB` (resolução de `0.1 dB`)
   - Default: `0.0 dB`
3. **Global Bypass** (`globalBypass`):
   - Tipo: `juce::AudioParameterBool`
   - Default: `false`
4. **Theme Mode** (`themeMode`):
   - Tipo: `juce::AudioParameterChoice`
   - Choices: `Light`, `Dark`
   - Default: `Dark` (índice 1)

---

## 3. Parâmetros por Banda Criados

Para cada uma das 8 bandas (de 1 a 8), foram criados 6 parâmetros individuais. O ID de cada parâmetro segue a estrutura de nomenclatura `band[N][Propriedade]`.

1. **Band N Enabled** (`bandNEnabled`):
   - Tipo: `juce::AudioParameterBool`
   - Default: `true` para a Banda 1; `false` para as Bandas 2 a 8.
2. **Band N Type** (`bandNType`):
   - Tipo: `juce::AudioParameterChoice`
   - Choices: `Bell`, `Low Cut`, `High Cut`, `Low Shelf`, `High Shelf`, `Notch`, `Band Pass`
   - Default: `Bell` (índice 0)
3. **Band N Frequency** (`bandNFrequency`):
   - Tipo: `juce::AudioParameterFloat`
   - Faixa: `20 Hz` até `20000 Hz` (escala logarítmica com centro em `1000 Hz`)
   - Defaults:
     - Banda 1: `80 Hz`
     - Banda 2: `160 Hz`
     - Banda 3: `315 Hz`
     - Banda 4: `630 Hz`
     - Banda 5: `1250 Hz`
     - Banda 6: `2500 Hz`
     - Banda 7: `5000 Hz`
     - Banda 8: `10000 Hz`
4. **Band N Gain** (`bandNGain`):
   - Tipo: `juce::AudioParameterFloat`
   - Faixa: `-24.0 dB` até `+24.0 dB` (resolução de `0.1 dB`)
   - Default: `0.0 dB`
5. **Band N Q** (`bandNQ`):
   - Tipo: `juce::AudioParameterFloat`
   - Faixa: `0.1` até `18.0` (escala logarítmica com centro em `1.0`)
   - Default: `1.0`
6. **Band N Bypass** (`bandNBypass`):
   - Tipo: `juce::AudioParameterBool`
   - Default: `false`

---

## 4. Total de Parâmetros Criados

- Parâmetros Globais: 4
- Parâmetros por Banda: 8 bandas $\times$ 6 parâmetros = 48
- **Total Geral**: 52 parâmetros registrados no APVTS.

---

## 5. Arquivos Alterados ou Criados

*   [PluginProcessor.h](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/PluginProcessor.h) (Alterado) - Declaração da instância `apvts` e assinatura do método estático `createParameterLayout()`.
*   [PluginProcessor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/PluginProcessor.cpp) (Alterado) - Inicialização do `apvts`, implementação do layout com os 52 parâmetros e gerenciamento seguro de estado via XML em `getStateInformation` / `setStateInformation`.
*   [PluginEditor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/PluginEditor.cpp) (Alterado) - Atualização da UI de debug para exibir "APVTS Ready" e mostrar dinamicamente o número total de parâmetros.
*   [implementation-plan.md](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Docs/implementation-plan.md) (Alterado) - Atualização do roadmap de desenvolvimento marcando a Fase 5.1/5.2 como Concluída.
*   [phase-5-1-apvts-parameters-summary.md](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/workflow/phase-5-1-apvts-parameters-summary.md) (Novo) - Este documento de resumo de workflow.

---

## 6. Resultado do Build

O build foi executado utilizando o comando `cmake --build build` e obteve sucesso integral:

- **Plataforma**: macOS (Apple Clang 21)
- **Status do Build**: `[100%] Built target AUREQ_VST3` (Compilação e geração com sucesso)
- **Caminho do Binário Gerado**:
  `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

---

## 7. Próximo Passo Recomendado

Prosseguir para o desenvolvimento do **motor de processamento de áudio (DSP)**:
- Fase 5.3: Criar a classe e a infraestrutura de loop de áudio multicanal (`EQProcessorCore` e `EQBand`) para alocação em `prepareToPlay` e processamento estático neutro.
- Fase 5.4: Implementar o cálculo de coeficientes dos filtros IIR (topologia Transposta Direta II com Cookbook de Robert Bristow-Johnson) e suavização dos parâmetros.
