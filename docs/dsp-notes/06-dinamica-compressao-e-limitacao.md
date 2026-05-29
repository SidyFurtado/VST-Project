# Dinâmica: Compressão e Limitação

Processadores de dinâmica reagem autonomamente à amplitude transitória do sinal, e aplicam uma resposta de ganho que afeta os volumes instantâneos sem alterar (majoritariamente) os espectros em si.

## Medição de Amplitude: RMS vs Peak
Antes de comprimir, o plugin precisa "escutar" a música. Existem duas formas do algoritmo detector perceber energia:
* **Peak Detection**: Detecta instantaneamente a amostra mais alta e reage aos menores estalidos ou picos da forma de onda. Resulta numa resposta de ataque rápida e cirúrgica. É a base do **Plugin 04_Limiter**.
* **RMS (Root Mean Square)**: Um cálculo da energia "média" contida numa pequena janela de tempo anterior, que simula de perto como nossos ouvidos percebem o "volume" da música. Os resultados de ataque são mais gordos, lerdos e suaves, característicos do **Plugin 02_Compressor**.

## O Envelope Follower e as Constantes de Tempo
O detector RMS/Peak passa seu valor a um "Rastreador de Envelope" (Envelope Follower), que rastreia os picos com suavidade programada pelos botões **Attack** e **Release**.
* Matematicamente são coeficientes IIR de filtro passa-baixa de primeira ordem (Low-pass) calculados baseados no Sample Rate.
* **Attack**: O quão lento o ganho "cai" para domar o pico.
* **Release**: O quão devagar o envelope volta ao normal.

## A Matemática do Ganho: Parâmetros e Curvas
Uma vez que o ganho decai seguindo o Envelope, ele bate na equação estática do ganho definida pelos demais Knobs:
1. **Threshold (Limiar)**: A barreira. O nível (geralmente em dB) acima do qual a redução de ganho passa a ser aplicada. Abaixo dele o ganho multiplicador é o ideal `1.0`.
2. **Ratio (Razão)**: O grau de esmagamento após cruzar a barreira. Se `Ratio = 4:1`, para cada 4 dB excedendo o threshold, apenas 1dB será emitido para a saída. A matemática de compressão é executada no domínio Logarítmico (ganho dB), convertida e então multiplicada sobre a amostra Linear.
3. **Knee (Joelho)**: Suaviza a transição imediata do Threshold. *Hard Knee* age imediatamente em forma de V (clássico SSL, ou VCA, ótimo para bateria); *Soft Knee* amortece a virada formando uma curva lenta antes do Threshold, comum em estilos valvulados/óticos musicais e amigáveis (Opto / LA-2A).
4. **Makeup Gain (Ganho de Compensação)**: Como o compressor achata o áudio, a percepção final do som fica mais baixa. Esse multiplicador (ativo manualmente ou num modo "Auto Makeup") restaura o pico final perdido, deixando a "carne" do áudio mais perceptível.

## Limiter e Suas Implicações Especiais (Plugin 04)
Um Limiter de Masterização é, em essência, um compressor de Attack virtualmente instantâneo (0.0 ms ou menor que 0.1 ms) associado a um Ratio infinito (`∞:1`). Seu teto (Ceiling) é sagrado.
Contudo, baixar o áudio de uma amostra para outra instantaneamente em picos agudos causaria rasgos e distorção na onda (Clipping quadrado audível).
Por isso, os Limiters Master possuem **Look-ahead** (armazenar buffers para ler o futuro e iniciar a queda da curva de *release invertida* milésimos de segundo antes da batida acontecer) protegendo o áudio sem estralos e distorções.
