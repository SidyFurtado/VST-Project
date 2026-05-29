# Processamento Sample-by-Sample vs Block Processing

O desenvolvimento de VSTs difere fundamentalmente do processamento offline de arquivos, exigindo a manipulação de dados de áudio em tempo real. A DAW alimenta nosso plugin com "blocos" de som e exige uma devolução pontual.

## Block Processing (Processamento por Blocos)
As DAWs gerenciam a carga da CPU juntando os samples de áudio em **Buffers** (Blocos), cujo tamanho geralmente é selecionado pelo usuário (ex: 64, 128, 256, 512, 1024 samples).
* Na classe principal do plugin, o método `processBlock(AudioBuffer<float>& buffer, ...)` recebe todo o pacote simultaneamente.
* **Latência**: Processar um bloco leva tempo. Por essência, o tamanho do bloco impõe uma latência de tempo que as DAWs compensam (*Plugin Delay Compensation - PDC*).
* **Vantagem em VSTs**: Processamento de operações matemáticas pesadas que podem ser vetorizadas (SIMD), e para transformadas complexas (FFT).
* **Aplicações Comuns**: Analisadores de Espectro (`01_EQ`), onde a FFT opera sobre grandes arrays; ganho estático.

## Processamento Sample-by-Sample
Para muitas classes de DSP, especialmente as recursivas (que dependem de valores históricos), o processo matemático de alterar uma matriz inteira por vetorização simplesmente não funciona, requerendo cálculos sucessivos iterando *amostra a amostra*.
* Em C++, isso se traduz num loop `for` passando sample a sample pelos canais do buffer (Left e Right).

## Diferenças Práticas no nosso Monorepo:

### Para o Equalizador (01_EQ)
Filtros digitais recursivos IIR (usados no EQ paramétrico) calculam a saída `y[n]` com base na entrada atual `x[n]`, nas entradas passadas `x[n-1]` e também nas **saídas passadas** `y[n-1]`.
Como a saída futura depende do resultado imediatamente anterior, o processamento **TEM QUE SER** sample-by-sample dentro do bloco.

### Para o Compressor (02_Compressor)
Um envelope follower, com Attack e Release, age iterativamente no tempo calculando coeficientes contínuos exponenciais e rastreando o sinal. Processar amostra por amostra é essencial para garantir o controle milimétrico dos transientes e não perder picos entre o bloco.

### Para o Limiter (04_Limiter)
Além do cálculo sample-by-sample, o limiter requer *Look-ahead*. Isso significa armazenar a "história do áudio" no tempo futuro através de um Delay Buffer extra grande (por exemplo 1 ou 2ms), analisar se há um pico excessivo se aproximando, e avisar o envelope do limiter *agora* para baixar o ganho preventivamente e não "espremer" o sinal instantaneamente (o que causa estalidos). Isso adiciona Latência nativa ao Plugin (que devemos reportar ao VST Host).

### Para os Analisadores Visuais
As UIs modernas do VST e displays espectrais capturam uma cópia visual do Buffer recebido no bloco de processamento e realizam operações pesadas de FFT. **Regra de Ouro**: Isso NÃO OCORRE na thread de áudio principal e não é feito sample-by-sample, pois prejudicaria a CPU de áudio com UI dropouts. Blocos copiados de forma thread-safe são analisados na thread da interface.
