# Resumo de Validação Técnica: Fase 5.20 — APVTS State Validation

Este documento registra os procedimentos de validação técnica, contagem de parâmetros, integridade de identificadores (IDs), correção de valores padrões (defaults) e persistência de estado do equalizador **AUREQ** (ASTRA Audio Suite) após a introdução de novos parâmetros na Fase 5.19.

---

## 1. Objetivo da Validação

Garantir que a transição e a estruturação de estado do plugin AUREQ estejam 100% integradas ao `juce::AudioProcessorValueTreeState` (APVTS) sem regressões de funcionalidade ou colisão de parâmetros. O foco principal é a validação da contagem de parâmetros, integridade dos IDs legados e novos, exatidão dos valores padrões (defaults) e salvamento/carregamento correto via XML da DAW sem qualquer alteração do processamento de DSP ou comportamento visual original.

---

## 2. Contagem de Parâmetros e Estrutura de Grupos

A contagem final de parâmetros foi verificada e validada em **108 parâmetros** registrados no layout do APVTS.

A estrutura de parâmetros é dividida em dois grupos principais:

### A. Parâmetros Globais (4 parâmetros)
- **Input Gain** (`inputGain`): Ganho de entrada (-24 dB a +24 dB).
- **Output Gain** (`outputGain`): Ganho de saída (-24 dB a +24 dB).
- **Global Bypass** (`globalBypass`): Desativação completa do processamento de áudio (pass-through).
- **Theme Mode** (`themeMode`): Seleção do tema gráfico (Light / Dark).

### B. Parâmetros por Banda (13 parâmetros por banda × 8 bandas = 104 parâmetros)
Para cada uma das 8 bandas independentes (`bandIndex` de 0 a 7), são definidos:
1. **Enabled** (`bandNEnabled`): Ativação da banda na interface e fluxo de DSP.
2. **Type** (`bandNType`): Seleção do tipo de filtro (Bell, Low Cut, High Cut, Low Shelf, High Shelf, Notch, Band Pass).
3. **Frequency** (`bandNFrequency`): Frequência central do filtro (20 Hz a 20 kHz, centro geométrico em 1 kHz).
4. **Gain** (`bandNGain`): Ganho do filtro (-24 dB a +24 dB).
5. **Q** (`bandNQ`): Fator Q do filtro (0.1 a 18.0, centro geométrico em 1.0).
6. **Bypass** (`bandNBypass`): Bypass individual da banda.
7. **Slope** (`bandNSlope`): Atenuação por oitava para filtros Cut (12 dB, 24 dB, 48 dB).
8. **Dynamic Enabled** (`bandNDynamicEnabled`): Habilitação de equalização dinâmica (DSP inativo).
9. **Dynamic Range** (`bandNDynamicRange`): Faixa de modulação dinâmica (0 dB a 24 dB).
10. **Dynamic Threshold** (`bandNDynamicThreshold`): Limiar para atuação dinâmica (-60 dBFS a 0 dBFS).
11. **Dynamic Attack** (`bandNDynamicAttack`): Tempo de ataque para dinâmica (0.1 ms a 200 ms).
12. **Dynamic Release** (`bandNDynamicRelease`): Tempo de liberação para dinâmica (10 ms a 2000 ms).
13. **Channel Mode** (`bandNChannelMode`): Comutação de canais (Stereo, Mid, Side, Left, Right).

---

## 3. Integridade e Preservação de IDs

Foi comprovado no código que todos os identificadores (IDs) de parâmetros originais e novos seguem as convenções estabelecidas de forma estrita e sem duplicidades ou renomeações:

- **IDs Legados Preservados**:
  - `inputGain`, `outputGain`, `globalBypass`, `themeMode`
  - `band1Enabled` ... `band8Enabled`
  - `band1Type` ... `band8Type`
  - `band1Frequency` ... `band8Frequency`
  - `band1Gain` ... `band8Gain`
  - `band1Q` ... `band8Q`
  - `band1Bypass` ... `band8Bypass`
- **Novos IDs das Fases 5.19 e 5.20**:
  - `band1Slope` ... `band8Slope`
  - `band1DynamicEnabled` ... `band8DynamicEnabled`
  - `band1DynamicRange` ... `band8DynamicRange`
  - `band1DynamicThreshold` ... `band8DynamicThreshold`
  - `band1DynamicAttack` ... `band8DynamicAttack`
  - `band1DynamicRelease` ... `band8DynamicRelease`
  - `band1ChannelMode` ... `band8ChannelMode`

A não-duplicidade foi auditada pela lógica de concatenação das funções utilitárias em [ParameterIDs.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/ParameterIDs.cpp), que utilizam o formato `"band" + String(bandIndex + 1) + "Suffix"`, garantindo chaves de estado de strings totalmente exclusivas.

---

## 4. Confirmação e Ajuste de Valores Padrão (Defaults)

Os valores iniciais e limites dos parâmetros foram auditados e corrigidos para garantir exatidão com os requisitos do produto:

- **Slope**: Default `12 dB/oct` (Choice index 0)
- **Dynamic Enabled**: Default `false`
- **Dynamic Range**: Default `0.0 dB` (corrigido de `6.0 dB` na Fase 5.20 para assegurar resposta de magnitude neutra)
- **Dynamic Threshold**: Default `-24.0 dBFS`
- **Dynamic Attack**: Default `10.0 ms`
- **Dynamic Release**: Default `120.0 ms` (corrigido de `100.0 ms` na Fase 5.20 para obter comportamento musical refinado)
- **Channel Mode**: Default `Stereo` (Choice index 0)

---

## 5. Persistência de Estado (XML State)

A persistência do estado foi verificada em [PluginProcessor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Source/PluginProcessor.cpp):
- O método `getStateInformation` copia com segurança a árvore de estado de parâmetros `ValueTree` via `apvts.copyState()` e cria um elemento XML correspondente.
- O método `setStateInformation` recupera o XML binário fornecido pela DAW host e recarrega o estado de parâmetros de forma atômica por meio de `apvts.replaceState()`.
- Como os 56 novos parâmetros estão formalmente registrados no `createParameterLayout`, a gravação e recuperação de seus valores no XML (incluindo declives de corte, parâmetros dinâmicos e modo de canais) ocorrem de forma **automática** e robusta, sem necessidade de lógica de serialização ad-hoc.

---

## 6. Status do DSP e Isenção de Alterações Acústicas

Em estrita concordância com as diretrizes da fase:
- **Dynamic EQ**: Os parâmetros de dinâmica estão registrados e expostos, mas o processamento de áudio em `EQBand` e `EQProcessorCore` permanece inalterado. Não há modulação dinâmica aplicada aos coeficientes dos filtros biquads. O recurso está completamente inerte.
- **Channel Mode**: O seletor de modo de canal (Stereo, Mid/Side, Left/Right) foi registrado, mas o motor DSP em `EQProcessorCore::processBlock` continua operando no modo estéreo padrão sem afetar o balanço de canais. O recurso está completamente inerte.

---

## 7. Resultados do Build e Binários

O projeto compilou com sucesso em ambiente macOS usando o CMake:

- **Comando Executado**: `cmake --build build`
- **Erros**: `0`
- **Warnings**: `5` avisos de deprecation pré-existentes na biblioteca JUCE (relacionados ao uso da API legacy de `juce::AudioParameterFloat` headless em frameworks externos, sem impacto no código ou estabilidade do equalizador). Não há warnings novos ou gerados pelas nossas modificações.

### Localização dos Binários Gerados
- **Formato VST3**: [AUREQ.vst3](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3)
- **Formato Standalone**: [AUREQ.app](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app)

---

## 8. Próximos Riscos Conhecidos

- **Automação de Parâmetros inativos na DAW**: O usuário na DAW agora pode gravar curvas de automação para parâmetros de Dynamic EQ e Channel Mode. Como esses recursos de DSP ainda não estão ativos, a DAW exibirá a automação ocorrendo, mas nenhum impacto acústico será percebido até a implementação das fases subsequentes. Isso deve ser comunicado claramente em notas de pré-lançamento.
- **Tamanho dos Arquivos de Projeto**: O aumento de 52 para 108 parâmetros acarreta um pequeno acréscimo no tamanho das strings nos arquivos de estado salvos pelos hosts (DAWs), porém o impacto é desprezível e totalmente tolerado pela especificação do formato XML JUCE.
