# Amostragem, Quantização e Aliasing

## Amostragem (Sampling) e o Teorema de Nyquist
A amostragem (capturar o valor da onda em intervalos fixos de tempo) segue uma lei de limite de frequência: O Teorema de Nyquist-Shannon. O teorema diz que para representar adequadamente uma frequência X, o Sample Rate precisa ser pelo menos `2 * X`.
* A **Frequência de Nyquist** é a maior frequência teórica que pode ser representada. Ex: a 44.1 kHz, Nyquist é 22.050 Hz.
* Em EQs VST, os filtros se "comprimem" perto da frequência de Nyquist (o chamado *cramping*). Para manter a curva do sino perfeita nos agudos, os VSTs modernos usam "Nyquist matching" ou oversampling.

## Quantização
O processo de converter uma voltagem (analógica) contínua em um valor discreto digital de degraus limitados (Bit Depth).
* Áudio 16-bit: 65.536 valores.
* Áudio 24-bit: mais de 16 milhões de valores.
* No nosso monorepo, como em VSTs modernos, a quantização "rígida" geralmente só importa no output final (Master Limiter) onde podemos aplicar *Dither* se formos converter a bit depth do render. Internamente, o C++ lidando com `float` ou `double` é virtualmente isento do "ruído de quantização" em cenários musicais normais.

## Aliasing e Foldover
Quando se tenta gerar ou gravar uma frequência **acima** do Limite de Nyquist, os samples "não são rápidos o suficiente" para capturá-la. Em vez de sumir, a frequência reflete (foldover) "para baixo" de Nyquist, tornando-se uma frequência inarmônica e audível. Esse "fantasma" digital soa metálico, estridente e destrutivo, conhecido como **Aliasing**.

## O Problema do Aliasing em VSTs:
Um Equalizador normal e linear não gera frequências novas, apenas amplifica ou atenua as existentes, então não cria Aliasing.
Porém, **Processadores Não-Lineares** mudam as ondas distorcendo-as, multiplicando ou gerando harmônicos de frequências originais altas. Se criarmos harmônicos que passam o Nyquist, teremos foldover grave.

### Componentes Suscetíveis:
* **Saturadores e Distorções**: Waveshapers adicionam incontáveis harmônicos ímpares e pares. É quase certeza de que eles ultrapassarão a barreira de Nyquist em áudios ricos em agudos.
* **Limiters/Compressores (com attack/release rápidos)**: Podem deformar a onda gerando intermodulação ou distorção não-linear.

## A Solução: Oversampling
Para prevenir o aliasing nos plugins `03_Saturation` e `04_Limiter`, usaremos **Oversampling**.
1. **Upsampling**: Aumenta-se temporariamente a taxa de amostragem do bloco de áudio (ex: de 44.1 kHz -> 176.4 kHz, ou 4x). A "janela" de Nyquist agora sobe para ~88.2 kHz, onde temos muito espaço "limpo" para os harmônicos.
2. **Processamento Não-Linear**: Executamos nossa saturação com a nova taxa. Os harmônicos que ultrapassariam Nyquist original sobem livres.
3. **Low-Pass Filter (Anti-Aliasing)**: Cortamos rudemente todas as frequências geradas acima do Nyquist original (~20 kHz).
4. **Downsampling**: Reduzimos a taxa de volta para 44.1 kHz, entregando à DAW um sinal rico, saturado e livre dos ecos digitais ruidosos (foldover).

> [!IMPORTANT]
> **Instrução DSP:** Todo plugin de saturação, hard-clipping ou distorção forte no **VST - Project** requererá o planejamento e uso de filtros FIR polifásicos ou IIR IIR elípticos/Butterworth pesados para implementar oversampling no motor de DSP, separando o sinal e reduzindo o aliasing agressivo.
