# Resumo da Fase 5.6: Parameter Smoothing para a Band 1

Este documento resume a implementação técnica de suavização de parâmetros (Parameter Smoothing) realizada na Fase 5.6, garantindo que as transições de frequência, ganho e Q ocorram de forma fluida, sem estalos acústicos (*zipper noise*) ou instabilidade digital no processamento da Band 1.

---

## 1. Objetivo da Fase
Implementar suavização de parâmetros na Band 1 para frequência, ganho e fator Q. Isso evita saltos numéricos instantâneos durante movimentações bruscas feitas na UI ou via automação da DAW, preservando a estabilidade acústica do plugin.

---

## 2. Por que o Parameter Smoothing é necessário?
Quando o usuário altera um parâmetro na interface (ou via automação da DAW), os novos valores são transmitidos à thread de áudio de forma discreta a cada bloco. Sem suavização, essas mudanças geram degraus (saltos descontínuos) na amplitude e na resposta em frequência do filtro de áudio, resultando em estalos ou cliques indesejados conhecidos como *zipper noise*. A suavização de parâmetros atenua esses saltos convertendo as transições discretas em rampas exponenciais suaves de baixa frequência.

---

## 3. Detalhes Técnicos do Smoothing

### A. Parâmetros Suavizados
- **Frequência**: Suavização em $30\text{ ms}$ (inicial).
- **Ganho**: Suavização em $20\text{ ms}$ (inicial).
- **Q**: Suavização em $20\text{ ms}$ (inicial).

### B. Fórmula de Smoothing Utilizada
Implementamos um filtro passa-baixa de primeira ordem (one-pole low-pass filter) para obter a curva de rampa exponencial:
$$current[n] = current[n-1] + \alpha \cdot (target - current[n-1])$$

Onde o coeficiente $\alpha$ é calculado no método `prepare` com base na taxa de amostragem ($f_s$) e no tempo de constante de tempo ($\tau$):
$$\tau = \text{smoothingTimeMs} \times 0.001$$
$$\alpha = 1 - e^{-1 / (f_s \cdot \tau)}$$

Se a taxa de amostragem for inválida ($f_s \le 0$), definimos $\alpha = 1.0$, fazendo com que o valor pule diretamente ao alvo instantaneamente.

### C. Avanço por Bloco O(1)
Para atualizar o biquad de forma eficiente, calculamos os novos parâmetros uma única vez a cada início de bloco usando o método `getNextBlockValue(numSamples)`. 
A fórmula matemática que simula o processamento do smoother ao longo de $M = \text{numSamples}$ amostras em tempo constante é:
$$current_{novo} = target - (target - current_{antigo}) \cdot (1 - \alpha)^{M}$$

Esta abordagem elimina a necessidade de um laço de repetição amostra a amostra para o smoother, mantendo o consumo de CPU extremamente baixo e mantendo a taxa de suavização consistente com o tempo definido em milissegundos.

### D. Segurança Numérica
- **Snapping**: Quando a diferença absoluta $|target - current| < 10^{-6}$, o smoother força $current = target$ e entra em estado de repouso, prevenindo a presença de valores subnormais (*denormals*) e evitando que o processador execute cálculos desnecessários para variações irrelevantes.
- **NaN/Inf**: Adicionamos verificações rígidas após os cálculos para reverter o estado imediatamente para o target se qualquer computação resultar em valores inválidos.

---

## 4. Integração no EQBand
- **Prepare**: Os smoothers de frequência, ganho e Q são inicializados com o `sampleRate` correspondente.
- **Setters**: Métodos como `setFrequency`, `setGainDecibels` e `setQ` agora modificam apenas a variável `target` de seus respectivos smoothers (em vez de calcular coeficientes na hora). Caso o sinalizador de áudio não esteja rodando (`sampleRate <= 0`), os coeficientes são atualizados de forma imediata.
- **ProcessBlock**: Ao início do bloco de áudio, se houver suavizadores ativos, `getNextBlockValue(numSamples)` calcula os parâmetros suavizados e o biquad atualiza seus coeficientes uma vez por bloco.

---

## 5. Limitações Conhecidas
- **Precisão por bloco (Block-Accurate)**: O cálculo de coeficientes do biquad ocorre apenas uma vez por bloco (no início do processamento). O refinamento para processamento de coeficientes amostra a amostra (Sample-Accurate) com interpolação linear ou exponencial pode ser adicionado em fases futuras, caso maior precisão em automações extremamente rápidas de frequência/Q se faça necessária.

---

## 6. Resultado do Build e Caminhos
- **Status da Compilação**: Aprovado com sucesso.
- **Caminho do Binário VST3**:
  `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

---

## 7. Próximo Passo Recomendado
Avançar para a **Fase 5.7**, onde iniciaremos a construção da casca visual da interface gráfica (UI inicial de 680x420 pixels) com o gerenciamento de temas (Light/Dark Mode) e a moldura vazia do gráfico cartesiano.
