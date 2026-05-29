# Resumo de Validação Técnica: Fase 7.0 — Channel Mode Core

Este documento registra a especificação, implementação e validação técnica da infraestrutura de processamento por modos de canal (**Channel Mode**) no motor DSP do equalizador **AUREQ**, parte da **ASTRA Audio Suite**.

---

## 1. Objetivo da Fase

Implementar o núcleo inicial de processamento por modos de canal (Stereo, Mid, Side, Left e Right) de forma individualizada por banda no DSP, utilizando o parâmetro APVTS existente `bandNChannelMode` sem alterar o APVTS ou criar novos parâmetros, e garantindo que as mudanças sejam 100% livres de alocação dinâmica e estáveis numericamente contra NaNs/Infs.

---

## 2. Modos Implementados & Mapeamento do APVTS

Os modos de canal são controlados individualmente em cada banda (0 a 7) através do parâmetro `bandNChannelMode` no APVTS. O mapeamento é:
- **0 = Stereo**: Processa os canais esquerdo e direito normalmente (comportamento padrão).
- **1 = Mid**: Processa apenas o sinal comum/central (Mid) da imagem estéreo.
- **2 = Side**: Processa apenas o sinal lateral/diferencial (Side) da imagem estéreo.
- **3 = Left**: Processa apenas o canal esquerdo (Left).
- **4 = Right**: Processa apenas o canal direito (Right).

---

## 3. Funcionamento do Processamento de Áudio por Modo

### A. Stereo
O processamento em modo Stereo chama os métodos padrão de bloco `BiquadFilter::processBlock` em cascata sobre todos os canais do buffer, preservando o comportamento tradicional e a fidelidade acústica anterior do plugin.

### B. Left / Right
- **Left**: O processamento extrai o ponteiro de escrita para o canal 0 e executa o processamento do biquad e eventuais cascatas de slope amostra por amostra apenas sobre ele:
  `x = filter.processSample(0, x);`
  O canal 1 (Right) permanece completamente inalterado por esta banda.
- **Right**: Extrai o ponteiro de escrita para o canal 1 e executa o biquad e cascatas de slope apenas sobre ele:
  `x = filter.processSample(1, x);`
  O canal 0 (Left) permanece completamente inalterado por esta banda.

### C. Mid / Side
Para os modos Mid e Side, o buffer de áudio é transformado em tempo real e de forma em-place (sample-by-sample) usando as seguintes fórmulas com conservação perfeita de ganho (unity gain):
- **Mid**:
  $$M = (L + R) \cdot 0.5$$
  $$S = (L - R) \cdot 0.5$$
  $$M_{\text{filtered}} = \text{Filter}(M)$$
  $$L_{\text{new}} = M_{\text{filtered}} + S$$
  $$R_{\text{new}} = M_{\text{filtered}} - S$$
  - O estado do filtro utilizado para processar o sinal Mid é isolado no canal index 0 da instância do filtro.

- **Side**:
  $$M = (L + R) \cdot 0.5$$
  $$S = (L - R) \cdot 0.5$$
  $$S_{\text{filtered}} = \text{Filter}(S)$$
  $$L_{\text{new}} = M + S_{\text{filtered}}$$
  $$R_{\text{new}} = M - S_{\text{filtered}}$$
  - O estado do filtro utilizado para processar o sinal Side é isolado no canal index 1 da instância do filtro.

---

## 4. Tratamento do Buffer Mono (Mono Fallback)

Se o buffer passado pelo host contiver apenas 1 canal (Mono):
- **Stereo, Mid, Left**: Fao fallback seguro e processam o canal 0 normalmente. No caso do Mid, o fallback age como Stereo processando o sinal único disponível.
- **Right, Side**: Fazem bypass automático e seguro para esta banda. Como não há canais Right ou Side disponíveis no sinal físico mono, a banda não introduz alterações e os dados passam inalterados, evitando falhas de segmento ou acessos inválidos de memória.

---

## 5. Integração com o Dynamic EQ

O equalizador dinâmico permanece integrado com segurança ao sistema de modos de canal:
- **Detector Stereo-Linked**: O detector de dinâmica (envelope follower) continua analisando de forma stereo-linked o sinal estéreo de entrada (analisando o máximo dos valores absolutos de L e R).
- O ganho dinâmico é calculado e as constantes dos coeficientes do filtro são recalculadas para o bloco. Em seguida, os filtros agem sobre o alvo de canal selecionado (Left, Right, Mid ou Side).
- **Sem NaNs/Infs**: Clamps matemáticos e detecção de denormais evitam qualquer instabilidade ou explosão numérica sob qualquer combinação de tipo de filtro, Q elevado ou modo de canal dinâmico.
- O detector por canal/modo (ex: detectar apenas no Mid ou apenas no Side) será refinado em uma fase futura do roadmap.

---

## 6. Limitações Conhecidas

- O detector dinâmico permanece estéreo para todas as configurações de modo nesta fase inicial.
- Em mono, os modos Right e Side sofrem bypass.

---

## 7. Conformidade das Regras Estritas

- **APVTS Não Alterado**: A contagem de parâmetros permanece rigidamente em 108. Nenhuma declaração ou ID de parâmetro foi adicionada, alterada ou removida.
- **Sem Novas UIs**: Nenhuma tela ou componente de controle de UI foi adicionado, mantendo a interface visual premium aprovada intacta.
- **Pasta Livros e PDFs**: Intocadas e totalmente preservadas.

---

## 8. Resultados de Compilação & Execução de Testes

### A. Compilação
- **Comando**: `cmake --build build`
- **Status**: Compilação concluída com sucesso (0 erros).

### B. Execução de Testes
- **Comando**: `./build/tests/AUREQ_FilterTests`
- **Resultado**: Todos os testes unitários e de estabilidade (incluindo os novos testes para os 5 channel modes e mono fallback) passaram com sucesso absoluto.
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

---

## 9. Caminho dos Binários Gerados

- **VST3**: [AUREQ.vst3](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3)
- **Standalone**: [AUREQ.app](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app)
- **Executável de Teste**: [AUREQ_FilterTests](file:///Users/sidyziin/Documents/VST%20-%20Project/build/tests/AUREQ_FilterTests)

---

## 10. Próximo Passo Recomendado

Prosseguir para o desenvolvimento da **Fase 7.1: Channel Mode UI Selector** (ou preset browser), adicionando seletores visuais em formato de glass pills no painel inferior para comutar o Channel Mode da banda selecionada de forma reativa e conectando-os aos parâmetros APVTS validados nesta fase.
