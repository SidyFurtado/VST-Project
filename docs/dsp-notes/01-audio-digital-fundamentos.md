# Fundamentos de Áudio Digital

Este documento resume a fundação teórica sobre áudio digital baseada nos materiais de referência, traduzida para o contexto de desenvolvimento de VSTs de mixagem e masterização.

## O que é Áudio Digital?
O som no mundo físico é uma onda contínua (analógica) de variação de pressão do ar. O **áudio digital** é a representação numérica aproximada dessa onda acústica para processamento computacional. No nosso caso (VSTs), o som já chega digitalizado pela DAW. Nosso trabalho é receber uma matriz numérica, manipular matematicamente esses números, e devolver uma nova matriz numérica.

## Sinais Discretos e Samples
Uma onda contínua é convertida em um **sinal discreto** através da amostragem ("fatiamento"). Um **sample** (amostra) é o valor instantâneo da amplitude da onda sonora num dado milionésimo de segundo. Nos VSTs com C++/JUCE, um sample geralmente é processado como um valor do tipo `float` (32-bit) ou `double` (64-bit), tipicamente variando entre `-1.0` e `+1.0`.

## Sample Rate (Taxa de Amostragem)
A velocidade em que esses samples são capturados/reproduzidos por segundo.
* Padrões comuns: 44.1 kHz (44.100 samples/segundo), 48 kHz, 88.2 kHz, 96 kHz.
* **Relação com VSTs**: Nossos algoritmos DSP operam *amostra a amostra*. Fórmulas matemáticas que envolvem tempo (como Attack/Release de um compressor ou o atraso num filtro) dependem da variável global `sampleRate` informada pela DAW. Um release de 100ms envolverá processar mais samples a 96kHz do que a 44.1kHz. Essa proporção tem de ser calculada!

## Amplitude, Frequência e Fase
* **Amplitude**: A "altura" da onda. Numéricas, dita o quão alto (loud) o som é percebido. No código, multiplicar um sample por `0.5` diminui a amplitude pela metade (-6dB).
* **Frequência**: Quão rápido a onda oscila, medida em Hertz (Hz). Define o tom grave ou agudo. Manipular frequências requer filtros (EQ).
* **Fase**: O deslocamento temporal de uma onda em relação a outra (ou a um referencial), medida em graus (0° a 360°). Ao desenvolver EQs IIR ou processamento paralelo, o "phase shift" se torna crítico. Se duas ondas fora de fase se somam, ocorre o cancelamento de fase (Comb Filtering).

## Resumo Prático para o Agente:
Ao lidar com o processador de áudio do VST:
1. Trabalharemos estritamente com **sinais discretos** (arrays/vetores numéricos).
2. O **Sample Rate** ditará a calibração do tempo. Sempre passe a taxa de amostragem (p. ex. do `prepareToPlay` do JUCE) para os módulos de processamento.
3. Não presuma sempre que a amplitude ficará confinada a `[-1.0, 1.0]`. Em processamento flutuante 32-bit interno, as DAWs lidam com níveis muito acima de `+1.0`. Nossos limites rígidos são mais impostos ao fim da cadeia, com limiters ou conversores DA, mas internamente temos headroom estendido.
