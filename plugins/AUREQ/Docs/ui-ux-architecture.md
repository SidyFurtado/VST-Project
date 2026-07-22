# Arquitetura de UI/UX: AUREQ

Este documento descreve o projeto e a organização da interface gráfica do usuário (GUI) e a experiência de usuário (UX) do equalizador **AUREQ**, parte da **ASTRA Audio Suite**. A interface é projetada para ser limpa, intuitiva e moderna, alinhada aos tokens de design do projeto e focada no controle direto do sinal de áudio com o mínimo de poluição visual.

---

## 1. Organização do Layout da Janela

O AUREQ possui uma dimensão padrão de **680px x 420px**. O layout é estruturado verticalmente em quatro seções funcionais principais, garantindo respiro visual e espaço negativo abundante (mínimo de 35% de área livre de controles).

```
+-------------------------------------------------------------------------+
| [ASTRA]  AUREQ             [ Default Preset v ]            (A/B) (Theme)| -> Top Bar (48px)
+-------------------------------------------------------------------------+
|                                                                         |
|  dB                                                                     |
|  +12  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   |
|   +6  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   |
|    0  ___________________________(o)_________________________________  | -> Display / EQ Graph
|   -6  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   |      (180px)
|  -12  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   |
|      20Hz       100Hz        1kHz        5kHz        10kHz      20kHz       |
+-------------------------------------------------------------------------+
|                                                                         |
|        +-------------------------------------------------------+        |
|        | [Band 1]  Freq: 1.00kHz   Gain: +3.0dB   Q: 1.00  (x) |        | -> Floating Control
|        |  (Bell)   Threshold: -18dB  Ratio: 2.1:1  Attack: 5ms |        |    Panel (160px)
|        +-------------------------------------------------------+        |
+-------------------------------------------------------------------------+
| (Power) [OS: 2x]                  Tooltip                  In[|] Out[|] | -> Footer (32px)
+-------------------------------------------------------------------------+
```

---

## 2. Componentes e Áreas da Interface

### A. Top Bar (Cabeçalho - 48px)
*   **Logo da Coleção & Nome**: Posicionado à esquerda (`ASTRA | AUREQ`), impresso com a tipografia de alta legibilidade `fontHeaderTitle` em caixa alta.
*   **Preset Browser (Navegador Central)**: Um menu dropdown elegante e centralizado para carregar, salvar e navegar rapidamente entre as predefinições de fábrica e do usuário. Possui botões compactos de seta `<` e `>` integrados para navegação rápida passo a passo.
*   **Controles de Utilitários (Direita)**:
    *   Botão **A/B**: Comparador rápido de dois estados de equalização distintos.
    *   **Theme Toggle (Alternador de Tema)**: Ícone minimalista (Sol/Lua) que ativa a comutação de cores em tempo real do sistema de Design Tokens (Light Mode e Dark Mode).

### B. Display Principal (EQ Graph & Analyzer - 180px)
Esta é a área central de visualização de dados e a principal zona de interação do usuário:
*   **Grid de Frequência e Amplitude**: Eixo X cartesiano representando a escala logarítmica de frequências (20 Hz a 20 kHz) com linhas de grade verticais discretas. Eixo Y representando a amplitude linear em decibéis (-12 dB a +12 dB) com linhas horizontais suaves.
*   **Spectrum Analyzer (Analisador de Espectro)**: Renderiza em tempo real um gráfico preenchido com gradiente translúcido suave. No MVP (v0.1) exibe o espectro pós-equalização. Na v0.2 exibe o sinal pré-EQ (cinza de baixa opacidade) e pós-EQ (cyan/mint translúcido) simultaneamente.
*   **Curva de Equalização Combinada (EQ Curve)**: Uma linha contínua, nítida e brilhante (desenhada via algoritmos de curva Bézier ou interpolação linear rápida baseada na resposta teórica de magnitude dos filtros em cascata) que exibe graficamente a soma de todas as bandas de EQ ativas.

### C. Nós de Manipulação de Banda (Band Handles)
*   Cada banda ativa é representada no display gráfico por um círculo centralizado (nó) numerado de 1 a 24.
*   **Gesto de Arraste (X/Y)**: Arrastar horizontalmente o nó altera a **Frequência** da banda. Arrastar verticalmente altera o **Ganho**.
*   **Gesto de Ajuste de Q**: Girar a roda do mouse (scroll) ou fazer um gesto de zoom (pinch) no trackpad enquanto o cursor estiver posicionado sobre o nó altera o **Fator Q** (largura de banda), exibindo uma "aura" semitransparente ao redor do nó que mostra visualmente a largura da banda afetada.
*   **Seleção de Banda**: Clicar uma vez no nó seleciona a banda, destacando-a visualmente e abrindo instantaneamente o Painel Flutuante na área inferior de controles.
*   **Ativação Rápida**: Duplo-clique em qualquer espaço vazio do display cria uma nova banda (do tipo Bell por padrão) na frequência e ganho correspondentes à posição do clique.

### D. Painel Flutuante de Banda (Floating Band Controls - 160px)
*   Em vez de exibir permanentemente dezenas de knobs para todas as bandas de áudio possíveis (o que polui a janela e confunde o usuário), o AUREQ utiliza uma caixa flutuante semitransparente arredondada (`radiusMedium`) que é renderizada na base da área gráfica de forma contextual.
*   O painel exibe e permite ajustar de forma detalhada apenas os parâmetros da **banda selecionada no momento**:
    *   Seletor de Tipo de Filtro (Bell, Low Cut, High Cut, Low Shelf, High Shelf, Notch, Band Pass).
    *   Campos numéricos interativos de arrasto (drag-labels) para Frequência (Hz), Ganho (dB) e Q.
    *   Botão Bypass local para desativar temporariamente a banda.
    *   Controles Dinâmicos (Threshold, Ratio e botão On/Off de dinâmica) disponíveis a partir da Versão 1.0.
*   Se nenhuma banda estiver selecionada, o painel oculta-se suavemente, liberando espaço de tela e dando maior sensação de clareza à interface.

### E. Rodapé Técnico (Footer - 32px)
*   **Bypass Geral (Power Button)**: Posicionado no canto inferior esquerdo, permite desviar todo o processamento de áudio com uma transição de sinal livre de cliques.
*   **Oversampling Selector**: Menu dropdown compacto para selecionar o processamento em sobre-amostragem da cadeia (Off, 2x, 4x), fundamental para controle fino de aliasing se acoplado a saturação ou dinâmica.
*   **Barra de Tooltips (Central)**: Exibe explicações contextuais dinâmicas em tempo real quando o usuário passa o cursor do mouse sobre qualquer controle do plugin (ex: "FREQUENCY: Controla a frequência de corte do filtro de 20 Hz a 20 kHz").
*   **Medidores de I/O (Input/Output Meters - Direita)**: Medidores verticais duplos compactos que mostram a presença de sinal de entrada e saída em tempo real com indicador de clipping de pico acendendo em vermelho se o sinal passar de 0 dBFS.

---

## 3. Comportamento Visual de Temas (Light e Dark Modes)

A interface do AUREQ consome exclusivamente os tokens do Design System, garantindo integridade visual perfeita:

| Elemento de UI | Light Mode | Dark Mode | Comportamento e Feedback |
| :--- | :--- | :--- | :--- |
| **Fundo da Janela** | `#EDF2F4` (Cinza Frio) | `#0B0F12` (Petróleo Escuro) | Base sólida geral |
| **Fundo do Display** | `#FFFFFF` (Branco Puro) | `#151D24` (Grafite Escuro) | Grid e grade sutil de frequências |
| **Borda de Contorno** | `#E2E8F0` (Borda Fina) | `#1C2833` (Cinza Chumbo) | Divisão física de seções |
| **Curva de EQ** | `#0891B2` (Cyan Slate) | `#22D3EE` (Cyan Neon) | Linha contínua brilhante da resposta |
| **Analisador FFT** | `rgba(8,145,178,0.08)` | `rgba(34,211,238,0.12)` | Gradiente translúcido sob a curva |
| **Texto de Labels** | `#0F172A` (Slate 900) | `#F8FAFC` (Slate 50) | Máximo contraste e legibilidade |
| **Nó Selecionado** | Aura Cyan sutil | Glow Neon Cyan com desfoque | Destaca a banda em edição |

---

## 4. Proposta de Componentes de Interface

A arquitetura do código da UI do AUREQ (utilizando C++/JUCE) será estruturada em classes de interface modulares:

1.  **`EQGraphDisplay`**
    *   **Responsabilidade**: O canvas do gráfico cartesiano. Desenha as linhas de grade de Hz e dB, captura cliques de criação de bandas e gerencia as coordenadas X/Y de interação do cursor do mouse.
2.  **`EQCurveRenderer`**
    *   **Responsabilidade**: Calcula e renderiza em alta performance a linha geométrica combinada da curva de EQ, redesenhando apenas quando coeficientes de filtro mudam.
3.  **`SpectrumAnalyzerView`**
    *   **Responsabilidade**: Renderiza o gradiente translúcido do espectro de FFT ao fundo do gráfico, utilizando interpolação temporal de decaimento para resposta visual suave.
4.  **`BandHandle`**
    *   **Responsabilidade**: O widget visual representativo do nó da banda. Desenha a bolha numérica na tela e reage a arraste e scroll do mouse.
5.  **`FloatingBandControls`**
    *   **Responsabilidade**: O container semitransparente que se posiciona contextualmente abaixo do nó selecionado ou na base do gráfico, contendo controles rápidos de edição.
6.  **`BandControlPanel`**
    *   **Responsabilidade**: O painel de knobs e seletores de parâmetros da banda para edição estendida.
7.  **`ThemeToggle`**
    *   **Responsabilidade**: Widget de comutação visual que alterna o estado de cores do sistema e redesenha os elementos gráficos.
8.  **`PresetBrowser`**
    *   **Responsabilidade**: Barra dropdown e botões de navegação sequencial de arquivos XML de predefinições.
9.  **`InputMeter` e `OutputMeter`**
    *   **Responsabilidade**: Componentes gráficos que renderizam as barras verticais de sinal com decaimento balístico e luzes vermelhas de sobrecarga (clipping).
