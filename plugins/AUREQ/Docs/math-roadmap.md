# Roadmap Matemático: AUREQ

Este documento descreve a fundamentação teórica e o roadmap de equações matemáticas para o processamento de áudio do **AUREQ**, parte da **ASTRA Audio Suite**. Ele serve como guia para a implementação de filtros IIR, resposta gráfica, análise espectral via FFT e equalização dinâmica.

---

## 1. Filtros Digitais de Segunda Ordem (Biquads)

O núcleo de processamento do AUREQ no MVP (v0.1) baseia-se em equações de diferenças lineares de tempo discreto de segunda ordem (filtros IIR biquad). A equação de diferenças que descreve a relação entre a entrada $x[n]$ e a saída $y[n]$ é:

$$y[n] = b_0 x[n] + b_1 x[n-1] + b_2 x[n-2] - a_1 y[n-1] - a_2 y[n-2]$$

Onde assumimos o coeficiente $a_0 = 1$. Em código C++, para evitar instabilidades e acumulação de erros de arredondamento sob ponto flutuante, a topologia recomendada para processamento sample-by-sample é a **Forma Transposta II Direta (Direct Form II Transposed)**:

```
          x[n] ---> ( b0 ) ---> ( + ) -------------> y[n]
                     |           ^       |
                     |         [z^-1]    |
                     |           |       |
                     ---> ( b1 )-+-( -a1 )
                     |           ^
                     |         [z^-1]
                     |           |
                     ---> ( b2 )---+-( -a2 )
```

As variáveis de estado interno (registradores de atraso) para cada canal de áudio são calculadas a cada amostra:

$$y[n] = b_0 x[n] + w_1[n-1]$$
$$w_1[n] = b_1 x[n] - a_1 y[n] + w_2[n-1]$$
$$w_2[n] = b_2 x[n] - a_2 y[n]$$

---

## 2. Coeficientes de Filtro (Audio EQ Cookbook)

Os coeficientes $a_i$ e $b_i$ serão calculados utilizando as fórmulas clássicas de Robert Bristow-Johnson (RBJ), que mapeiam parâmetros de Frequência Central ($f_0$), Frequência de Amostragem ($f_s$), Ganho linear ($A = 10^{\text{dB}/40}$) e Fator Q ($Q$) em coeficientes digitais através da **Transformada Bilinear** com pré-warping de frequência.

### Variáveis Comuns:
$$\omega_0 = 2 \pi \frac{f_0}{f_s}$$
$$\alpha = \frac{\sin(\omega_0)}{2 Q}$$

### Filtro Bell (Pico):
$$b_0 = 1 + \alpha A$$
$$b_1 = -2 \cos(\omega_0)$$
$$b_2 = 1 - \alpha A$$
$$a_0 = 1 + \frac{\alpha}{A}$$
$$a_1 = -2 \cos(\omega_0)$$
$$a_2 = 1 - \frac{\alpha}{A}$$

*Nota: Para aplicar na equação de diferenças, todos os coeficientes devem ser normalizados dividindo-os por $a_0$ (por exemplo, $b_0 \leftarrow b_0 / a_0$, $a_1 \leftarrow a_1 / a_0$, etc.).*

---

## 3. Renderização Gráfica da Curva de EQ

Para desenhar a curva de EQ na tela sem passar áudio real (método analítico), a UI calcula a **Resposta de Magnitude** teórica dos filtros. A função de transferência no domínio $z$ de um biquad é:

$$H(z) = \frac{b_0 + b_1 z^{-1} + b_2 z^{-2}}{1 + a_1 z^{-1} + a_2 z^{-2}}$$

Para avaliar em uma frequência específica $f$, substituímos $z = e^{i \omega}$, onde $\omega = 2 \pi \frac{f}{f_s}$:

$$H(e^{i \omega}) = \frac{b_0 + b_1 \cos(\omega) - i b_1 \sin(\omega) + b_2 \cos(2\omega) - i b_2 \sin(2\omega)}{1 + a_1 \cos(\omega) - i a_1 \sin(\omega) + a_2 \cos(2\omega) - i a_2 \sin(2\omega)}$$

### Cálculo Prático de Magnitude (dB) para Desenho:
Para um ponto $f$ no display gráfico:
1.  Calcula-se $z^{-1} = \cos(\omega) - i\sin(\omega)$ e $z^{-2} = \cos(2\omega) - i\sin(2\omega)$.
2.  Obtém-se o número complexo do numerador $N = (b_0 + b_1\cos(\omega) + b_2\cos(2\omega)) - i(b_1\sin(\omega) + b_2\sin(2\omega))$.
3.  Obtém-se o número complexo do denominador $D = (1 + a_1\cos(\omega) + a_2\cos(2\omega)) - i(a_1\sin(\omega) + a_2\sin(2\omega))$.
4.  A magnitude linear é a razão dos módulos: $|H(e^{i \omega})| = \frac{|N|}{|D|}$.
5.  A resposta em decibéis é: $H_{\text{dB}} = 20 \log_{10}(|H(e^{i \omega})|)$.
6.  **Curva Cascata**: A resposta final é a soma em dBs das magnitudes individuais de todas as bandas ativas: $H_{\text{total, dB}} = \sum_{k=1}^{N} H_{k, \text{dB}}$.

---

## 4. Análise Espectral FFT e Janelamento

O analisador espectral do AUREQ processa o sinal de áudio no domínio da frequência utilizando a Transformada Rápida de Fourier (FFT):

1.  **Tamanho do Bloco ($N$)**: Recomenda-se $N = 2048$ ou $4096$ amostras para garantir boa resolução em baixas frequências (graves).
2.  **Windowing (Janelamento)**: Para evitar descontinuidade nas bordas do bloco de tempo (que gera ruído fantasma de alta frequência conhecido como *spectral leakage*), os dados são multiplicados por uma função de janela antes do cálculo. A **Janela de Hann** é sugerida:
    $$w[n] = 0.5 \left(1 - \cos\left(\frac{2 \pi n}{N - 1}\right)\right), \quad 0 \le n \le N-1$$
3.  **FFT**: O sinal janelado $x_w[n]$ é transformado em um array de números complexos $X[k]$ representando os bins de frequência.
4.  **Magnitude**: A magnitude de cada bin é calculada como $|X[k]| = \sqrt{\text{Re}(X[k])^2 + \text{Im}(X[k])^2}$.
5.  **Conversão Logarítmica**: Transformada em dBFS: $X_{\text{dB}}[k] = 20 \log_{10}(|X[k]| / N)$.
6.  **Suavização Temporal (Ballistics)**: Para evitar que o gráfico pisque ou pule freneticamente, aplica-se uma rampa de decaimento suave (envelope follower de liberação de pico):
    $$Y[k] = \max(X_{\text{dB}}[k], Y_{\text{anterior}}[k] \times g_{\text{decay}})$$
    Onde $g_{\text{decay}}$ é calibrado para gerar um decaimento visual agradável.

---

## 5. Dinâmica por Banda (Dynamic EQ - Roadmap v1.0)

A equalização dinâmica exige a inserção de um **Detector de Envelope** acoplado a cada banda. O detector calcula o nível de amplitude em tempo real do sinal que passa pela banda e determina uma atenuação ou amplificação temporária.

### Envelope Follower de Tempo Real:
O nível de sinal retificado $v[n] = |x_{\text{banda}}[n]|$ passa por um detector exponencial de ataque/release:

$$y_{\text{env}}[n] = (1 - \alpha) \cdot y_{\text{env}}[n-1] + \alpha \cdot v[n]$$

Onde o coeficiente $\alpha$ varia dinamicamente:
*   Se $v[n] > y_{\text{env}}[n-1]$, $\alpha = \alpha_{\text{attack}} = 1 - e^{-1 / (f_s \cdot \tau_{\text{attack}})}$
*   Se $v[n] \le y_{\text{env}}[n-1]$, $\alpha = \alpha_{\text{release}} = 1 - e^{-1 / (f_s \cdot \tau_{\text{release}})}$

### Modulação do Ganho da Banda:
O ganho dinâmico é calculado comparando o nível de envelope $y_{\text{env}}$ em dBFS contra o Threshold ($T_{\text{dB}}$) definido pelo usuário:

$$\text{Excesso}_{\text{dB}} = y_{\text{env, dB}} - T_{\text{dB}}$$

Se $\text{Excesso}_{\text{dB}} > 0$:
$$\text{Ganho}_{\text{alvo, dB}} = -\text{Excesso}_{\text{dB}} \times \left(1 - \frac{1}{\text{Ratio}}\right)$$

Este $\text{Ganho}_{\text{alvo, dB}}$ modula de forma aditiva o ganho fixo da banda antes de recomputar os coeficientes do biquad correspondente, atenuando frequências dinamicamente quando elas passam do limite.

---

## 6. Roadmap Matemático Avançado

### A. Linear Phase (Fase Linear via Convolução FIR)
Em vez de IIR, filtros de fase linear usam coeficientes simétricos de resposta finita ao impulso (FIR).
*   **Equação**: $y[n] = \sum_{k=0}^{M-1} b_k x[n-k]$
*   **Implementação**: Para kernels grandes ($M > 512$), a convolução no tempo é inviável por consumo de CPU. Utiliza-se **Convolução na Frequência** (algoritmo Overlap-Save ou Overlap-Add via FFT):
    $$y = \text{iFFT}(\text{FFT}(x) \times \text{FFT}(h))$$
    Isso exige buffers de latência de tamanho $N$ que devem ser reportados à DAW para compensação automática (PDC).

### B. Spectral Dynamics (Dinâmica Espectral por Bins)
Em vez de envelope followers em bandas discretas, o sinal de FFT $X[k]$ é atenuado de forma independente para cada bin bin-a-bin com base em thresholds espectrais contínuos:
$$X_{\text{processado}}[k] = G[k] \cdot X[k]$$
Onde $G[k]$ é um multiplicador de ganho calculado dinamicamente para cada bin de frequência, retornando depois ao domínio do tempo via iFFT com reconstrução de janelamento.

---

## 7. Estabilidade Numérica e Prevenção de Artefatos

*   **Prevenção de Zipper Noise**: Suavização de coeficientes rodando em rampa linear ou passa-baixas exponencial, limitando a taxa máxima de variação da frequência central para $\Delta f_{\max} = 20 \text{ Hz/sample}$ a fim de evitar modulação inarmônica indesejada.
*   **Limites de Q**: Restrição por software do fator Q a um teto máximo de $18.0$. Valores maiores aproximam os polos do círculo unitário em excesso, gerando auto-oscilação ruidosa interminável.
*   **Double Precision**: Cálculo de coeficientes e acumulação de estado interno utilizando precisão binária de 64 bits (`double`) para minimizar distorção harmônica causada por truncation noise digital.
*   **Sanity Checks**: Verificação a cada iteração do bloco:
    $$\text{isnan}(y[n]) \lor \text{isinf}(y[n]) \rightarrow \text{Reset Filtro}()$$
