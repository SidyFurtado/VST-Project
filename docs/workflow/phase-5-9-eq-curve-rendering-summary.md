# Resumo de Validação Técnica: Fase 5.9 — Renderizar Curva de EQ Teórica

Este documento descreve as implementações realizadas na **Fase 5.9** do equalizador **AUREQ**, parte da **ASTRA Audio Suite**, com a criação da curva visual teórica das 8 bandas de equalização Bell/Peaking sem alteração de DSP ou interações de arraste.

---

## 1. Objetivo da Fase
O objetivo principal desta fase foi desenhar no `EQGraphView` uma curva visual representando a resposta de magnitude combinada das 8 bandas Bell/Peaking ativas, consumindo os parâmetros do APVTS e atualizando o gráfico de forma contínua e assíncrona.

---

## 2. Como a Curva Visual é Calculada
A curva de equalização é calculada analiticamente no domínio visual (Thread de Mensagens da UI) mapeando cada pixel horizontal do display de volta para frequências em escala logarítmica:

1. **Mapeamento de Coordenada X**:
   A função `getFrequencyForX(float x)` converte a coordenada horizontal do display em uma frequência logarítmica correspondente na faixa de $20\text{ Hz}$ a $20\text{ kHz}$:
   $$f(x) = 20 \times \exp\left( \text{val} \times \ln(1000) \right)$$
   Onde $\text{val}$ é a fração normalizada do pixel $x$ em relação aos limites do plot.

2. **Contribuição de Banda Bell**:
   Para cada frequência $f$, calculamos a contribuição de ganho em decibéis ($dB$) de cada banda ativa ($enabled == true$, $bypassed == false$, $type == Bell$) utilizando um modelo de aproximação em escala de oitavas ($\log_2$):
   $$\text{distOctaves} = \log_2(f / f_0)$$
   $$\text{shape} = \frac{1}{1 + (\text{distOctaves} \times Q)^2}$$
   $$\text{dB}_{\text{contribuição}} = \text{gainDb} \times \text{shape}$$

3. **Soma e Clamp**:
   As contribuições individuais das bandas são somadas algebricamente e o ganho final acumulado é limitado na faixa visual de $-24\text{ dB}$ a $+24\text{ dB}$, sendo mapeado para a coordenada Y de pixel correspondente usando `getNormalizedY()`.

> [!NOTE]
> A curva calculada é uma **aproximação visual** rápida e leve. A resposta exata de magnitude baseada na avaliação exata da função de transferência dos biquads digitais do DSP poderá ser introduzida em fases futuras do roadmap.

---

## 3. Leitura de Parâmetros e Atualização da Curva
Para garantir que a UI seja atualizada dinamicamente com as automações ou carregamento do plugin, a atualização é realizada da seguinte forma:

- **Timer Periódico**:
  O `PluginEditor` herda de `juce::Timer` de forma privada e dispara atualizações a **30 Hz** (`startTimerHz(30)`).
- **Leitura Atômica do APVTS**:
  Na função `timerCallback()`, lemos os valores atômicos do APVTS (`enabledParam`, `bypassedParam`, `typeParam`, `freqParam`, `gainParam`, `qParam`) para todas as 8 bandas de forma thread-safe e direta.
- **Propagação e Repaint**:
  Os parâmetros lidos são encapsulados em uma estrutura leve `VisualEQBand` e passados para a `graphView` usando `graphView.setBands()`, que armazena os valores locais e aciona o redesenho via `repaint()`.

---

## 4. Confirmações de Escopo e Regras Estritas
* **Zero Alterações no DSP**: As classes de processamento de áudio digital `BiquadFilter`, `EQBand`, `EQProcessorCore`, `ParameterSmoother` e `PluginProcessor` continuam exatamente idênticas, sem nenhuma linha alterada.
* **APVTS e Parâmetros Preservados**: A árvore APVTS continua com seus 52 parâmetros originais. Não foram adicionados novos parâmetros ou alterada a estrutura de persistência.
* **Sem Interatividade**: Não há handles móveis de arrastar, controle por mouse wheel, knobs/sliders adicionais, painel flutuante de edição ou analisador espectral FFT nesta fase. A interface gráfica permanece visualmente idêntica à Fase 5.8, exceto pela adição da curva de EQ desenhada de forma nítida.

---

## 5. Desenho da Curva
A curva é renderizada acima do grid de fundo com:
- Traço suave de **2.2px** usando a cor de acento do tema (`ThemeColors::accent`).
- Glow discreto com traço de **4.5px** de opacidade baixa desenhado por baixo da linha principal.
- Coexistência nos dois modos de cor: Light Mode (acento cyan suave com glow translúcido leve) e Dark Mode (acento cyan neon brilhante e glow elétrico discreto).

---

## 6. Resultado do Build e Caminhos
A compilação foi concluída com 100% de sucesso.

* **Caminho do Plugin VST3**:
  `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

* **Caminho do Standalone Executável (Mac App)**:
  `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

---

## 7. Próximo Passo Recomendado
Avançar para a **Fase 5.10: Adicionar Analisador FFT Simples**, onde configuraremos o spectrum analyzer pós-equalização rodando no fundo do gráfico de forma integrada através de cópia assíncrona de blocos via FIFO de áudio.
