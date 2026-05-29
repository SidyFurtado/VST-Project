# Templates de Layout de Plugins (Plugin Layout Template)

Este documento descreve os mockups estruturais textuais para a linha de plugins da **ASTRA Audio Suite** (desenvolvida no monorepo **VST - Project**). Ele define um modelo estrutural universal e fornece layouts específicos para o **AUREQ (Equalizer)**, **GRAVITY COMP (Compressor)**, **NEBULA SAT (Saturation)** e **EVENT LIMITER (Limiter)**, garantindo uniformidade em toda a coleção.

---

## 1. Modelo Geral de Interface (Universal Skeleton)

Todos os plugins da marca devem seguir rigorosamente esta organização de quadrantes verticais:

*   **1. Top Bar (Cabeçalho - 48px)**:
    *   *Alinhamento à esquerda*: Logo da Coleção (`ASTRA`) + Nome do Plugin (ex: `AUREQ`).
    *   *Alinhamento central*: Preset Browser (botão dropdown com setas `<` e `>` integradas).
    *   *Alinhamento à direita*: Botão A/B de comparação rápida + Menu Settings (engrenagem) + Alternador de Tema (ícone Sol/Lua).
*   **2. Display / Área Visual (160px a 220px)**:
    *   Exibe analisadores gráficos de espectro, curvas de atenuação de dinâmica ou visualizadores de formas de onda. Apresenta escalas discretas de frequência (Hz) ou amplitude (dB) nos eixos.
*   **3. Área Principal de Controles (180px a 240px)**:
    *   Organiza de 2 a 5 controles rotativos grandes (Knobs de 64px de diâmetro) com espaçamento generoso. Cada knob é acompanhado de sua Label em caixa alta (`fontLabelSmall`) abaixo do controle e do seu respectivo Value Display (`fontValueDisplay`) monoespaçado. Os knobs podem ser agrupados em caixas ou painéis arredondados (`radiusMedium`).
*   **4. Rodapé (Footer - 32px)**:
    *   *Alinhamento à esquerda*: Botão Bypass (Power) geral do plugin + Seletor de Oversampling (dropdown compacto: `Off`, `2x`, `4x`).
    *   *Alinhamento central*: Barra de Tooltip Dinâmico (exibe explicações curtas dos parâmetros).
    *   *Alinhamento à direita*: Medidor de Ganho de Entrada (Input) e Ganho de Saída (Output) duplo vertical e compacto + Assinatura de Versão (ex: `v1.0.0`).

---

## 2. Modelos Específicos de Plugins

### A. AUREQ - Equalizador Paramétrico Flagship (01_EQ)

*   **Propósito**: Ajustes cirúrgicos e musicais de frequência, equalização dinâmica avançada e controle tonal de alta fidelidade.
*   **Dimensões da Janela**: `680px x 420px` (Layout Complexo).
*   **Organização das Áreas**:
    *   **Área Visual (Display)**: Grid de FFT em tempo real mostrando o espectro de entrada (translúcido cinza) e saída (translúcido cyan) sobreposto à curva de equalização ativa gerada pelas bandas.
    *   **Área de Controles**: Painel flutuante inteligente de controle dinâmico para a banda selecionada (frequência, ganho, Q, bypass por banda, tipo e controles dinâmicos de threshold e ratio). Criação e seleção de bandas (até 24) ocorrem diretamente por interação com nós no gráfico, evitando poluição visual.
        *   `KNOB 1 (Grande)`: **FREQUENCY** (Frequência de corte - 20 Hz a 20 kHz)
        *   `KNOB 2 (Grande)`: **GAIN** (Ganho da banda - bipolar: -12 dB a +12 dB)
        *   `KNOB 3 (Médio)`: **Q-FACTOR** (Largura de banda - 0.1 a 18.0)
        *   `SWITCHES (Pequenos)`: Tipo de Filtro (dropdown/botões: Low-shelf, Peak, High-pass, Low-pass, High-shelf, Notch, Band-pass).
    *   **Rodapé**: Bypass da banda selecionada, Bypass Global do EQ, Analisador FFT (On/Off), Medidores de I/O de áudio.

### B. GRAVITY COMP - Compressor Dinâmico (02_Compressor)

*   **Propósito**: Controle de faixa dinâmica, peso, cola, punch e modelagem de transientes.
*   **Dimensões da Janela**: `620px x 380px`.
*   **Organização das Áreas**:
    *   **Área Visual (Display)**: Medidor horizontal ou vertical destacado de **Gain Reduction (GR)** variando de `0 dB` a `-24 dB` com decaimento balístico rápido e retorno suave. Uma curva de transferência estática de compressão (Knee display) acompanha o fundo.
    *   **Área de Controles**: 5 controles organizados em um painel comum:
        *   `KNOB 1 (Grande - Central)`: **THRESHOLD** (Ponto de início de compressão: -60 dB a 0 dB)
        *   `KNOB 2 (Grande)`: **RATIO** (Razão de compressão: 1:1 a 20:1 / Limiter)
        *   `KNOB 3 (Médio)`: **ATTACK** (Tempo de ataque: 0.1 ms a 100 ms)
        *   `KNOB 4 (Médio)`: **RELEASE** (Tempo de liberação: 10 ms a 1000 ms)
        *   `KNOB 5 (Pequeno)`: **MAKEUP GAIN** (Ganho de compensação de saída: 0 dB a +24 dB)
    *   **Rodapé**: Power/Bypass, seletor de joelho (Soft/Hard Knee), filtro Sidechain (HPF Sidechain), medidores I/O.

### C. NEBULA SAT - Saturador Harmônico (03_Saturator)

*   **Propósito**: Adição de harmônicos, calor analógico, coloração sutil e clipping controlado.
*   **Dimensões da Janela**: `520px x 320px` (Layout Simples e Focado).
*   **Organização das Áreas**:
    *   **Área Visual**: Não possui display de FFT ou gráfico complexo. Apenas um indicador visual simples em formato de arco superior central que brilha proporcionalmente ao nível de distorção aplicada.
    *   **Área de Controles**: Apenas 3 controles principais destacados para respiro visual máximo:
        *   `KNOB 1 (Extra Grande)`: **DRIVE** (Quantidade de saturação - 0 dB a +36 dB)
        *   `KNOB 2 (Grande)`: **MIX** (Balanço Dry/Wet - 0% a 100%)
        *   `KNOB 3 (Médio)`: **OUTPUT** (Atenuação de ganho de saída - bipolar: -24 dB a +12 dB)
        *   `TOGGLE BUTTONS`: Tipo de Saturação (dropdown ou botões de rádio discretos: `Tube`, `Tape`, `Clapper`).
    *   **Rodapé**: Power/Bypass, Oversampling switch (essencial para saturadores evitarem aliasing), medidores I/O compactos com indicadores de clip de pico.

### D. EVENT LIMITER - Limitador de Masterização (04_Limiter)

*   **Propósito**: Elevação de volume final, controle de transientes inter-amostras (True Peak) e teto protetor absoluto.
*   **Dimensões da Janela**: `680px x 420px`.
*   **Organização das Áreas**:
    *   **Área Visual (Display)**: Gráfico de forma de onda deslizante em tempo real (Scrolling Waveform Display). Mostra a atenuação de ganho em vermelho sobreposta à onda de áudio de saída em cyan, indicando exatamente onde o limitador está atuando sobre os transientes.
    *   **Área de Controles**: 4 parâmetros técnicos:
        *   `KNOB 1 (Grande)`: **THRESHOLD / GAIN** (Elevação de sinal de entrada - 0 dB a +30 dB)
        *   `KNOB 2 (Grande)`: **CEILING** (Teto de pico máximo de saída - -20 dBFS a 0 dBFS)
        *   `KNOB 3 (Médio)`: **RELEASE** (Tempo de recuperação - auto-adaptável ou manual de 10 ms a 500 ms)
        *   `KNOB 4 (Pequeno)`: **LOOK-AHEAD** (Tempo de leitura antecipada - 0.0 ms a 5.0 ms)
        *   `TOGGLE BUTTONS`: Detecção de True Peak (On/Off).
    *   **Rodapé**: Power/Bypass, medidor de entrada, saída e redução de ganho ultra-precisos de alta escala, estatísticas de LOUDNESS (exibindo valores Integrados de LUFS de forma digital).
