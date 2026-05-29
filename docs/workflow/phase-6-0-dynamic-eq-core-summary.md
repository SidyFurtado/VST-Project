# Resumo de Validação Técnica: Fase 6.0 — Dynamic EQ Core

Este documento registra a implementação e validação do primeiro núcleo funcional de equalização dinâmica (**Dynamic EQ Core**) por banda do equalizador **AUREQ**, parte da **ASTRA Audio Suite**.

---

## 1. Objetivo da Fase

Adicionar a infraestrutura interna do DSP em cascata para suportar equalização dinâmica (atenuação ou ganho dinâmicos sob estímulo do sinal) de forma incremental, leve e musical. O objetivo é ler os parâmetros dinâmicos já registrados no APVTS e modular o ganho dos filtros IIR biquads correspondentes em tempo real sem alocações de memória, sem locks de threads e garantindo a estabilidade numérica contra NaNs/Infinitos.

---

## 2. Estratégia de Dynamic EQ e Modulação de Ganho

O Dynamic EQ foi implementado de forma aditiva e proporcional ao ganho estático da banda:
$$\text{effectiveGain}_{\text{dB}} = \text{staticGain}_{\text{dB}} + \text{dynamicGain}_{\text{dB}}$$

- **Soft-Knee Knee Curve**: O ganho dinâmico é calculado de forma progressiva e assintótica baseando-se em uma curva knee analítica exponencial a partir do excesso do envelope acima do limiar ($\text{over}_{\text{dB}}$):
  $$\text{dynamicGain}_{\text{dB}} = \text{dynamicRange}_{\text{dB}} \cdot \left(1 - e^{-\text{over}_{\text{dB}} / 12.0}\right)$$
  - Esta curva impede cortes abruptos ("hard clipping") na modulação e se aproxima musicalmente do limite máximo estabelecido pelo parâmetro `dynamicRange`.
- **Ajuste Fino de Neutro**: Se `dynamicRange == 0.0 dB` ou se a dinâmica estiver desativada (`dynamicEnabled == false`), o processador executa a rota estática nativa sem processar o detector e sem recalcular coeficientes, otimizando o consumo de CPU.
- **Coeficientes por Bloco**: Para garantir alta performance e impedir operações trigonométricas custosas sample-by-sample, a recomputação de coeficientes do biquad é executada **no máximo uma vez por bloco de áudio**, logo no início de `processBlock`.

---

## 3. Envelope Follower (Seguidor de Envelope)

- **Leitura do Sinal de Entrada (Ajuste 1)**: O detector analisa o sinal de áudio do buffer de **entrada** (antes do processamento do filtro da própria banda) para evitar retroalimentação indevida de controle ("feedback control instability").
- **Detector Stereo Link (Ajuste 4)**: Opera no modo estéreo acoplado, extraindo a amplitude de pico combinada a cada amostra:
  $$\text{absSample} = \max(|L|, |R|)$$
- **Constantes de Tempo Exponenciais**: Attack e Release em milissegundos são mapeados em coeficientes exponenciais a cada inicialização de sample rate ($\text{coeff} = e^{-1000.0 / (f_s \cdot \tau)}$). Se a taxa de amostragem for inválida, é adotado um coeficiente de rampa de segurança ($0.99$).
- **Segurança Numérica (Ajuste 6)**: O seguidor é protegido contra subnormais (clampando sinais $< 10^{-15}$ para zero para evitar *denormal slowdown*) e contra valores indeterminados IEEE-754 (como NaN ou infinitos).

---

## 4. Filtros Suportados e Restrições

Para preservar a estabilidade numérica e focar na usabilidade musical:

| Tipo de Filtro | Suporte Dynamic EQ | Comportamento do DSP |
|---|---|---|
| **Bell (Pico)** | ✅ Habilitado | Modulação dinâmica completa de ganho |
| **Low Shelf** | ✅ Habilitado | Modulação dinâmica completa de ganho |
| **High Shelf** | ✅ Habilitado | Modulação dinâmica completa de ganho |
| **Low Cut (HPF)** | ❌ Desativado | Ignora dinâmica e processa de forma estática |
| **High Cut (LPF)** | ❌ Desativado | Ignora dinâmica e processa de forma estática |
| **Notch** | ❌ Desativado | Ignora dinâmica e processa de forma estática |
| **Band Pass** | ❌ Desativado | Ignora dinâmica e processa de forma estática |

---

## 5. Confirmações Importantes de Regras

- **APVTS Intocado**: O layout do APVTS e os 108 parâmetros registrados não sofreram nenhuma alteração. Nomes e IDs foram preservados.
- **Mid/Side inativo**: O parâmetro de modo de canais (`bandNChannelMode`) permanece inerte no DSP (sem processamento Mid/Side).
- **Linear Phase inativo**: O equalizador continua operando de forma recursiva IIR pura com latência zero.
- **UI Preservada**: Nenhuma alteração foi realizada nas visualizações gráficas ou sliders no PluginEditor.

---

## 6. Resultados de Compilação e Testes Unitários

O projeto compilou e passou com total sucesso nos testes matemáticos de validação no macOS.

### A. Compilação
- **Comando**: `cmake --build build`
- **Erros**: `0`
- **Warnings**: Apenas warnings pré-existentes do JUCE e bundle ID.

### B. Execução das Suites de Testes
- **Comando**: `./build/tests/AUREQ_FilterTests`
- **Resultado**:
```
========================================================
         AUREQ - DSP FILTER VALIDATION TESTS            
========================================================

[SUITE] Running Biquad Coefficient Tests...
[PASS] Biquad Coefficient Tests completed successfully.

[SUITE] Running EQBand Block Processing Tests...
[PASS] EQBand Block Processing Tests completed successfully.

[SUITE] Running Filter Stability and Safety Tests...
[SUB-SUITE] Running EQBand Dynamic EQ stability sub-suite...
[PASS] Filter Stability and Safety Tests completed successfully.

========================================================
      SUCCESS: All AUREQ DSP Filter Tests Passed!      
========================================================
```
- **Taxa de aprovação**: 100% (0 falhas). ✅

---

## 7. Caminho dos Binários Gerados

- **VST3**: [AUREQ.vst3](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3)
- **Standalone**: [AUREQ.app](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app)
- **Executável de Teste**: [AUREQ_FilterTests](file:///Users/sidyziin/Documents/VST%20-%20Project/build/tests/AUREQ_FilterTests)

---

## 8. Próximo Passo Recomendado

Prosseguir para o desenvolvimento da **Fase 6.1: Dynamic EQ UI Controls**, adicionando sliders, botões e indicações gráficas de atenuação e ganho dinâmicos no display e noSelected Band Card, conectando-os aos parâmetros APVTS validados nesta fase.
