# Glossário DSP

Este glossário define termos técnicos comuns frequentemente usados ao desenvolver plugins VST neste projeto.

*   **Sample (Amostra)**: O valor instantâneo (numérico) que descreve a amplitude do áudio num microsseção temporal específica.
*   **Buffer**: Um bloco arranjado de Samples (array/vetor) na memória aguardando para ser processado (ex: blocos de 512 amostras na thread do VST).
*   **Sample Rate (Taxa de Amostragem)**: Quantidade de amostras capituradas em 1 segundo (ex: 44.100 amostras = 44.1 kHz).
*   **Bit Depth (Profundidade de Bits)**: Número de valores possíveis usados para descrever a amplitude no digital (ex: 16 bits, 24 bits), determinando o ruído de fundo digital base.
*   **Amplitude**: Nível do valor instantâneo do som (geralmente interpretado no eixo Y entre -1.0 a +1.0 no motor de áudio).
*   **RMS (Root Mean Square)**: Método matemático para calcular a "média contínua de energia" de uma onda de som; aproxima-se do "volume" que percebemos.
*   **Peak**: O pico imediato/extremo de amplitude de um sample específico que ditará clipping.
*   **dB (Decibel)**: Unidade logarítmica para expressar a proporção relativa entre duas pressões sonoras; essencial na modelagem do volume audível humano.
*   **Hz (Hertz)**: Unidade que expressa o número de ciclos/vibrações de uma onda por segundo (frequência).
*   **Phase (Fase)**: Posição ao longo de um ciclo (0 a 360 graus) e a defasagem temporal angular entre duas ondas.
*   **Filter (Filtro)**: Dispositivo matemático para atenuar ou remover frequências específicas do espectro do som.
*   **FFT (Fast Fourier Transform)**: Algoritmo ultrarrápido para decompor um sinal do seu domínio temporal transformando-o num gráfico de frequências (espectro).
*   **IIR (Infinite Impulse Response)**: Tipo de filtro que usa retroalimentação/recursão matemática. Possui zero latência nativa mas desvia a fase. Muito usado em Equalizadores VST e synths.
*   **FIR (Finite Impulse Response)**: Tipo de filtro convolucional que varre amostras estáticas; sem feedback. Pode criar Linear Phase EQs pesados.
*   **Oversampling**: Artefato onde as amostras do som recebem uma "re-amostragem artificial interna" multiplicando a taxa momentaneamente para limpar saturação e evitar o dobramento do som ruim (aliasing).
*   **Latency (Latência)**: O atraso em milissegundos imposto por processamentos em bloco, processamento look-ahead ou FIR/FFT, e do próprio buffer do PC.
*   **Envelope**: O contorno (Attack, Decay, Sustain, Release) da amplitude de um som com o tempo. Essencial para rastreamento de compressores.
*   **Compressor**: Dispositivo automagente que diminui os picos de um áudio caso eles cruzem o "Threshold", mantendo uma textura e dinâmica coesa.
*   **Limiter**: Como um compressor num modo tijolo extremo que garante com unhas e dentes que o volume não passe nem mesmo 1 fio de cabelo de um teto pré-estabelecido.
*   **Saturation (Saturação)**: Modificação distorcida harmônica com waveshaping nas ondas limpas; gerando um preenchimento sônico agradável de amplificadores e velhas fitas K7 / Válvulas.
*   **Aliasing**: Som áspero e metálico não-linear causado pela incapacidade de samples registrarem frequências mais altas do que o Teorema de Nyquist suporta, causando dobramento (Foldover).
