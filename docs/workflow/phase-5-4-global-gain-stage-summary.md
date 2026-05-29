# Resumo da Fase 5.4: Gain Stage Global Seguro

Este documento detalha a implementação e validação do primeiro processamento de DSP real do equalizador **AUREQ** (fase 5.4).

---

## 1. Objetivo da Fase
Implementar um estágio de ganho global seguro no motor de áudio do equalizador, contemplando os parâmetros de **Input Gain**, **Output Gain** e **Global Bypass** com proteção numérica contra NaNs e infinitos.

## 2. Primeiro DSP Real Implementado
Esta fase remove o plugin do modo de pass-through neutro puro para realizar sua primeira manipulação ativa de áudio:
*   Ajuste do nível de sinal de entrada (Input Gain) e de saída (Output Gain).
*   Desvio total do processamento (Global Bypass) quando acionado pelo usuário.

## 3. Parâmetros Usados
Os parâmetros são integrados a partir do APVTS por meio do `ParameterAccessLayer` (`AUREQ::Params`):
*   `Input Gain` (`inputGainID()`): Amplitude em decibéis (dB), variando de -24.0 dB a +24.0 dB.
*   `Output Gain` (`outputGainID()`): Amplitude em decibéis (dB), variando de -24.0 dB a +24.0 dB.
*   `Global Bypass` (`globalBypassID()`): Booleano (`true`/`false`) que determina se o processamento é desviado.

## 4. Funcionamento do Global Bypass
No `EQProcessorCore::processBlock`:
*   Se `globalBypass` for ativo (`true`), a função realiza um retorno antecipado imediato (`return;`) sem modificar o buffer de áudio.
*   Isso garante um bypass seguro, com zero consumo de CPU adicional no bloco de DSP e sem alterar os dados de áudio contidos no buffer.
*   *Nota de Design*: Fades ou transições suaves (crossfade) de bypass não foram implementados nesta etapa, sendo delegados a uma fase de aprimoramento futuro para evitar cliques.

## 5. Conversão de dB para Linear
No processamento de sinal digital, os ganhos em dB são convertidos para o fator multiplicativo linear usando a função auxiliar do JUCE:
```cpp
float inputGainLinear = juce::Decibels::decibelsToGain(inGainDb);
float outputGainLinear = juce::Decibels::decibelsToGain(outGainDb);
```
Como ainda não existem filtros IIR ou bandas ativas processando o sinal entre a entrada e a saída nesta fase, a operação foi otimizada aplicando um ganho total combinado:
```cpp
float totalGain = inputGainLinear * outputGainLinear;
```
Este ganho único é aplicado a todos os canais do buffer usando a rotina otimizada do JUCE:
```cpp
buffer.applyGain(totalGain);
```

## 6. Proteções contra NaN / Infinito (Segurança Numérica)
Para blindar o processamento de áudio contra instabilidades numéricas, estouros de CPU ou NaNs oriundos da automação de parâmetros da DAW, foram inseridas as seguintes checagens:
1.  **Sanitização dos parâmetros em dB**:
    *   Se `inputGain` ou `outputGain` forem identificados como NaN (`std::isnan`) ou infinito (`std::isinf`), eles são forçados para `0.0f` (0.0 dB / ganho 1.0).
2.  **Sanitização dos ganhos lineares após conversão**:
    *   Se `inputGainLinear` ou `outputGainLinear` resultarem em NaN ou infinito, são forçados para `1.0f`.
3.  **Sanitização do ganho combinado total**:
    *   Se `totalGain` final for NaN, infinito ou negativo (por qualquer motivo inesperado), ele é imediatamente reajustado para `1.0f` (ganho neutro / 0.0 dB).

Essas verificações asseguram a integridade acústica e a integridade da CPU do plugin.

## 7. Status dos Filtros IIR
Fica expressamente confirmado que **não existem filtros IIR ativos nem processamento de bandas de equalização** nesta fase. Os estados das bandas são atualizados via APVTS para manter a consistência do modelo de dados, mas o `processBlock` processa apenas o Gain Stage global.

## 8. Resultado do Build e Caminho do VST3
A compilação foi executada e validada com sucesso via CMake.
*   **Comando de Build**: `cmake --build build`
*   **Status**: `[100%] Built target AUREQ_VST3` (Compilação bem sucedida)
*   **Caminho do Binário Gerado**:
    `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

## 9. Próximo Passo Recomendado
O próximo passo recomendado é a **Fase 5.5: Implementar Filtros Básicos IIR**, onde a estrutura de cálculo de coeficientes (RBJ) e os motores de biquad (Transposta Direta II) serão implementados.
