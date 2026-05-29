# Filtros Digitais e Equalização

## Filtros Digitais na Prática
Filtros são algoritmos matemáticos que recebem um sinal de áudio digital e diminuem (atenuação) ou aumentam (ganho) a amplitude de frequências específicas, sem (idealmente) interferir severamente nas outras. 

## Tipos e Topologias de Filtros

*   **Low-Pass Filter (LPF)**: Passa-baixa. Corta os agudos progressivamente a partir da frequência de corte (Cutoff) com uma taxa definida (Q ou Slopes como 12dB/oct, 24dB/oct).
*   **High-Pass Filter (HPF)**: Passa-alta. Corta os graves a partir do Cutoff, fundamental para "limpar a lama" da mixagem.
*   **Band-Pass (BPF) / Band-Stop (Notch)**: Deixa passar uma faixa específica (BPF) ou remove cirurgicamente uma ressonância perniciosa muito estreita (Notch).
*   **Shelving (Low Shelf / High Shelf)**: Não tem "corte". Ele amplifica ou atenua progressivamente a partir da frequência de prateleira (Cutoff) e estaciona em um platô fixo de ganho constante no infinito. Fundamental em tons abertos, simulando "Air" e calor analógico.
*   **Peaking EQ (Bell / Sino)**: Base da equalização paramétrica. Ele cria um aumento ou depressão pontual centrada numa frequência alvo e influencia as frequências ao redor controladas pelo parâmetro `Q` (largura de banda).

## IIR vs FIR em Equalizadores VST

A maioria dos "Digital EQs" (como o Pro-Q em modo latência-zero, e EQs nativos de DAW) são baseados na topologia **IIR (Infinite Impulse Response)**.

### Filtros IIR (Infinite Impulse Response):
Filtros recursivos. Usam retroalimentação (o resultado sai e entra novamente na equação da próxima amostra).
* **Vantagens**: Muito leves para a CPU. Permitem processamento analítico com pouquíssima latência (zero nativa). São os análogos virtuais diretos de circuitos analógicos RC tradicionais. Padrão para os botões do nosso Equalizador e crossovers dinâmicos.
* **Cuidado: Fase**: Eles intrinsecamente alteram a "fase" do sinal (distorcem a relação de tempo microscópico ao redor da frequência alterada). Esse desvio de fase não é um defeito — é literalmente como EQs IIR e EQs analógicos de hardware funcionam.
* **Cuidado: Estabilidade**: Coeficientes errados, mudanças extremas instantâneas (Zipper noise, sem smoothing) ou polos matemáticos que saem do "círculo unitário" resultarão num loop de som estourado e infinito (Blow up, valores NaN) travando a DAW e possivelmente queimando alto-falantes se não mitigado.

### Filtros FIR (Finite Impulse Response):
Não usam retroalimentação. A saída depende estritamente das centenas/milhares de amostras passadas de entrada armazenadas em buffer, com coeficientes (kernels) grandes operando convolução ou transformações.
* **Vantagens**: Permitem criar **Linear Phase EQs** (onde não há desvio de fase, preservando o alinhamento total do grupo das frequências em EQs paralelos difíceis). Também permitem perfis de filtros extremamente abruptos sem distorcer fase.
* **Cuidado**: Muito pesados em computação (requerem FFT em processamento por bloco muitas vezes). Introduzem grande latência pré-ring (o áudio é borrado um pouquinho pra "trás" nos picos percussivos - o preringing).

> [!IMPORTANT]
> **Guia para o Plugin 01_EQ:**
> Nosso equalizador começará usando arquitetura Biquad tradicional (IIR) para os módulos (Peak, Shelf, LPF, HPF), com cálculos de coeficientes baseados nas famosas fórmulas do "Audio EQ Cookbook" de Robert Bristow-Johnson, com adaptações contra *cramping* nos agudos via transformações de projeto avançado caso necessário. É vital garantir estabilidade na alteração dos coeficientes sem gerar estalos durante arrasto na UI!
