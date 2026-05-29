# Fourier, FFT e Análise Espectral

## O Teorema de Fourier
A base de toda a análise em frequência reside no postulado matemático de Joseph Fourier: **Qualquer forma de onda complexa no tempo pode ser decomposta em uma soma de ondas senoidais e cossenoidais infinitas, cada uma com sua própria frequência, amplitude e fase.**
* Se temos um arquivo de áudio de uma nota de piano tocando e aplicarmos a Transformada de Fourier, saímos da representação do domínio do tempo ("gráfico da onda") e pulamos para a representação do **domínio da frequência** (espectro: o "gráfico de barras das frequências").

## DFT vs FFT
* A **DFT** (Discrete Fourier Transform) é a equação direta aplicada aos sinais digitais (discretizados e limitados). O problema é sua imensa ineficiência computacional `O(N²)`.
* A **FFT** (Fast Fourier Transform) é o algoritmo engenhoso que reorganiza e agiliza radicalmente o cálculo de Fourier (`O(N log N)`). **A FFT é o pilar de quase todas as ferramentas analíticas de áudio moderno e convolução em tempo real.** A FFT necessita que o tamanho do bloco analisado seja obrigatoriamente uma potência de 2 (256, 512, 1024, 2048, 4096...).

## O Resultado da FFT: Magnitude e Fase
A FFT de um bloco de áudio nos devolve um vetor complexo ("bins" de frequência correspondentes). Esse número complexo carrega duas informações vitais sobre cada franja de frequência:
1. **Magnitude**: Quanta "energia" há naquela frequência. Tirando o valor absoluto e convertendo-o logaritmicamente (dB), encontramos os pontos que sobem e descem no analisador espectral do equalizador.
2. **Fase**: O deslocamento rotacional de cada frequência específica em graus/radianos, que afeta alinhamento e ressonância espacial.

## Aplicações Práticas (Foco nos Plugins)

### 1. Analisadores de Espectro (Plugin 01_EQ)
Atrás dos knobs do nosso equalizador, existirá um canvas visual de resposta. Nele, a UI exibirá em tempo real os picos e vales do som passando.
* **Técnica:** Criaremos uma thread circular paralela na UI. Ela pescará o áudio em blocos (ex: buffers de 2048 ou 4096 amostras recebidos através de FIFOs assíncronos), aplicará uma Função de Janelamento (*Windowing*, p.ex. Janela Hann ou Hamming - necessária para evitar borramento lateral na FFT, chamado de *spectral leakage*) e calculará a FFT adiante pra tela. Nunca fazer na thread do áudio.

### 2. Processamento por Convolução/Frequência
Processamentos line-phase complexos ou compressores espectrais que trabalham as bandas "penteando" o áudio (FFT-based plugins), multiplicam bins no domínio da frequência e depois usam a iFFT (Transformada Inversa) para voltar o sinal ao tempo e enviar para o output. Apesar de não constar na lista inicial, as bases teóricas aqui servem como semente para upgrades e reverbs de convolução caso solicitados no futuro.
