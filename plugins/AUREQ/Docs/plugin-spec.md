# Especificação de Produto: AUREQ

Este documento especifica os requisitos de DSP, comportamento de interface e planejamento técnico para o **AUREQ**, o equalizador paramétrico flagship da coleção **ASTRA Audio Suite** (desenvolvido no monorepo **VST - Project**).

---

## 1. Visão Geral do Produto

*   **Nome Temporário**: AUREQ (AUREQ Flagship Equalizer)
*   **Coleção**: ASTRA Audio Suite
*   **Categoria**: Equalizador Paramétrico Moderno e Dinâmico
*   **Objetivo**: Criar um equalizador profissional capaz de competir em precisão cirúrgica, workflow rápido, flexibilidade funcional e fidelidade de áudio com os principais padrões do mercado, mantendo uma identidade visual limpa e autoral baseada no Design System do projeto.
*   **Estética**: Visual moderno e clean, priorizando interação direta com a curva gráfica de equalização e painel de controles flutuante inteligente, suportando nativamente os modos Light Mode e Dark Mode.

---

## 2. Estratégia de Versões e Fases de Recursos

Para garantir um desenvolvimento robusto e incremental sem comprometer a estabilidade do monorepo, as funcionalidades estão distribuídas em quatro lançamentos progressivos:

### Versão 0.1 (MVP Funcional)
O objetivo desta versão é estabelecer a infraestrutura básica estável (JUCE + APVTS + GUI Tema), o motor IIR básico de 8 bandas e o analisador espectral essencial.
*   **Capacidade do Motor**: 8 bandas de equalização independentes e simultâneas.
*   **Tipos de Filtro Disponíveis**:
    *   Bell (Sino) - Filtro paramétrico principal com ganho e Q ajustáveis.
    *   Low Cut (High-Pass) - Filtro de corte de graves.
    *   High Cut (Low-Pass) - Filtro de corte de agudos.
    *   Low Shelf - Prateleira de graves.
    *   High Shelf - Prateleira de agudos.
    *   Notch - Rejeição de banda cirúrgica.
    *   Band Pass - Passa-banda para isolamento de frequência.
*   **Controles de Banda**:
    *   Frequência de Corte/Central (20 Hz a 20 kHz, resposta logarítmica).
    *   Ganho de Banda (-12 dB a +12 dB, bipolar).
    *   Fator Q / Largura de Banda (0.1 a 18.0).
    *   Bypass individual por banda.
*   **Controles Globais**:
    *   Bypass Global do plugin.
    *   Input Gain (-24 dB a +12 dB).
    *   Output Gain (-24 dB a +12 dB).
*   **Análise Visual**:
    *   Analisador de Espectro FFT pós-EQ simples, elegante e de decaimento suave.
*   **Interface e Workflow**:
    *   Suporte completo a Light Mode e Dark Mode via Design System (Design Tokens).
    *   Presets de fábrica básicos e salvamento/restauração de estado via APVTS.
*   **Segurança DSP**:
    *   Suavização temporal de parâmetros (parameter smoothing) para evitar *zipper noise*.
    *   Proteção ativa contra valores inválidos (NaNs, infinitos e denormais).

### Versão 0.2 (Workflow Profissional)
Focada em ferramentas de produtividade para engenheiros de mixagem e masterização.
*   **Comparações e Histórico**:
    *   Sistema de comparação rápida A/B de estados de equalização.
    *   Histórico ilimitado de Undo/Redo na interface.
*   **Refinamento Visual**:
    *   Analisador espectral com visualização simultânea pré-EQ (cinza translúcido) e pós-EQ (cyan translúcido).
*   **Interações de Edição**:
    *   Solo de banda (permite ouvir de forma isolada a região de frequências afetada por uma banda específica ao segurar uma tecla de modificação).
    *   Copy/Paste de parâmetros entre bandas.

### Versão 1.0 (Flagship EQ Completo)
Consolidação do AUREQ como ferramenta flagship, introduzindo processamento dinâmico básico por banda e redimensionamento completo de interface.
*   **Expansão de Bandas**: Suporte a até 24 bandas dinâmicas criadas livremente com duplo-clique no display gráfico.
*   **Dynamic EQ**: Mapeamento dinâmico em cada banda (Bell, Shelvings) com controles de threshold, ratio e envelope de attack/release, permitindo compressão ou expansão de frequências em tempo real.
*   **Controles Globais Avançados**:
    *   Auto Gain (compensação automática de ganho baseada na curva de EQ).
    *   Gain Scale (controle macro que escala a ação de ganho de todas as bandas simultaneamente de 0% a 200%).
*   **Interface**:
    *   Modo de piano/note frequency integrado na régua de frequência (mapeamento Hz para notas musicais).
    *   Interface totalmente redimensionável e suporte a modo Fullscreen.

### Versão Avançada (Roadmap Futuro)
Recursos de altíssima complexidade técnica e diferenciação comercial.
*   **Fase Avançada**:
    *   Linear Phase Mode (processamento de fase linear com latência compensada via convolução FIR/FFT).
    *   Natural / Minimum Phase Mode avançado (filtros sem cramping de Nyquist que mimetizam resposta analógica analítica).
*   **Processamento de Imagem Estéreo**:
    *   Modo Mid/Side por banda (equalizar canal Mid e Side separadamente).
    *   Modo Left/Right por banda (equalizar canais esquerdo e direito de forma independente).
*   **Ferramentas Espectrais Inteligentes**:
    *   Spectral Dynamic EQ (compressão/expansão dinâmica contínua operando em múltiplos bins de FFT, agindo como um compressor multibanda adaptativo).
    *   Spectrum Grab (congelamento visual do espectro que permite capturar e arrastar picos ressonantes detectados).
    *   EQ Match (captura o espectro de uma faixa de referência e ajusta as bandas automaticamente para imitar a curva de equalização).
*   **Conectividade e Ambientes**:
    *   Collision Detection (detecção automática de mascaramento de frequências entre diferentes instâncias do AUREQ no mesmo projeto via barramento compartilhado).
    *   Suporte a formatos de áudio espacial e surround (5.1, 7.1.4 Dolby Atmos).

---

## 3. Requisitos de DSP e Estabilidade de Áudio

O núcleo de processamento do AUREQ seguirá regras rígidas para garantir qualidade técnica impecável:
*   **Filtros IIR de Alta Precisão**: Inicialmente baseados nas fórmulas do Cookbook de Robert Bristow-Johnson (RBJ), com resolução interna em ponto flutuante de 64-bit (`double`) e compensação de cramping na região próxima à Frequência de Nyquist.
*   **Parameter Smoothing**: Mapeamento dos coeficientes de filtros através de rampas de suavização com constantes de tempo ajustadas entre 10ms a 30ms para eliminação de cliques digitais ao arrastar nós no gráfico.
*   **Prevenção de Subnormais (Denormals)**: Inserção de ruído dither de baixíssima amplitude (`1e-15`) no loop recursivo de processamento IIR e ativação dos modos de CPU Flush-to-Zero (FTZ) e Denormals-Are-Zero (DAZ) nas instâncias de processamento de bloco.
*   **Segurança Contra Estouros**: Estruturas de validação condicional antes e depois da chamada de filtros para detectar NaNs e infinitos. Se um valor inválido for detectado no canal de áudio, o estado do filtro instável será resetado instantaneamente e um valor seguro (zero) será injetado para proteger os canais de monitoração do usuário.

---

## 4. Requisitos Visuais e de Workflow

Seguindo o manual de marca da ASTRA Audio Suite, a interface gráfica do AUREQ buscará o máximo de minimalismo dinâmico:
*   **Operação Centrada no Gráfico**: A criação de bandas é feita por duplo-clique sobre a curva gráfica de resposta. A frequência e o ganho são ajustados arrastando o nó de controle (Band Handle) no plano cartesiano X/Y. O fator Q é ajustado girando a roda do mouse (scroll) ou por gesto de pinça no trackpad sobre o nó selecionado.
*   **Painel Flutuante Inteligente (Floating Control Panel)**: Para evitar uma fileira estática de dezenas de knobs poluindo a tela, a interface apresenta uma pequena caixa flutuante semitransparente na base do gráfico que renderiza os parâmetros específicos apenas da banda selecionada no momento. Se nenhuma banda estiver selecionada, o painel oculta-se suavemente dando respiro visual à tela.
*   **Estética do Analisador**: O espectro de FFT deve ser renderizado ao fundo do gráfico de forma sutil, em gradientes translúcidos, de modo a não se sobrepor visualmente à linha contínua da curva de EQ, assegurando uma leitura limpa em qualquer um dos temas.
*   **Controles Físicos do Rodapé**: Controles estáticos de ganho geral (Input/Output) com meters verticais discretos, interruptor de Oversampling da cadeia (Off/2x/4x) e botão bypass global do processador de áudio.
