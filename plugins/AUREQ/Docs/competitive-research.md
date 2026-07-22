# Pesquisa Competitiva de Equalizadores Premium

Este documento apresenta uma análise de benchmarking competitivo para o desenvolvimento do **AUREQ**, comparando os líderes de mercado, identificando as expectativas dos usuários profissionais, delimitando os recursos fundamentais de entrada e os diferenciais originais, além de mitigar os riscos técnicos do projeto.

---

## 1. Mapeamento de Mercado: Benchmarks de Referência

O mercado de equalizadores digitais é altamente maduro e dominado por produtos premium que estabeleceram padrões de usabilidade e processamento de sinal:

### A. FabFilter Pro-Q (Referência de Workflow e Ergonomia)
*   **Pontos Fortes**: 
    *   Interface gráfica fluida, vetorial e acelerada por GPU.
    *   Interação impecável e intuitiva com a curva de EQ (criação rápida de bandas com duplo clique, atalhos de mouse).
    *   Painel flutuante inferior dinâmico que exibe controles específicos da banda selecionada.
    *   Recurso de *Spectrum Grab* (congelamento espectral para pegar picos ressonantes diretamente na tela).
    *   Equalização dinâmica integrada e altamente simplificada.
*   **Desvantagem Relativa**: 
    *   Visual estritamente escuro, sem opção nativa de Light Mode (o que pode causar fadiga ocular em salas claras).
    *   Não focado em modelagem analógica hiper-realista.

### B. Kirchhoff EQ (Referência de Precisão de DSP e Modelagem)
*   **Pontos Fortes**:
    *   Filtros analógicos modelados com alta precisão e 32 tipos de topologias de filtro.
    *   Estabilidade em frequências extremamente altas graças a algoritmos de compensação de warping proprietários.
    *   Processamento interno em 117-bit (precisão double estendida).
    *   Equalização dinâmica extremamente personalizável, com detecção e sidechain ultra-detalhados.
*   **Desvantagem Relativa**:
    *   Interface visual congestionada de knobs e números, o que diminui a velocidade do fluxo de trabalho diário.
    *   Curva de aprendizado íngreme para iniciantes.

### C. DMG Audio Equilibrium (Referência de Customização Extrema)
*   **Pontos Fortes**:
    *   Flexibilidade de configuração quase infinita (o usuário pode escolher o tamanho da janela de FFT, tipo de janela, comprimento do kernel FIR e a topologia exata de cada filtro).
    *   Qualidade de áudio absoluta.
*   **Desvantagem Relativa**:
    *   Interface gráfica austera, antiquada e pouco interativa.
    *   Exige amplo conhecimento técnico do usuário para configurar o processador corretamente.

---

## 2. O que o Usuário Profissional Espera em um EQ Flagship

Para que um equalizador seja adotado em estúdios de masterização e mixagem comerciais, ele precisa satisfazer requisitos de qualidade e ergonomia consolidados:

1.  **Workflow Rápido e Sem Fricção**: Tempo é dinheiro. O usuário precisa ajustar frequências com poucos cliques. Knobs virtuais estáticos em excesso são rejeitados em favor de interações diretas com nós gráficos na curva de EQ.
2.  **Transparência e Fidelidade DSP**: O EQ não deve "colorir" o sinal indesejadamente em modos padrão de operação. Ele precisa soar limpo nos agudos (sem cramping de filtros perto de 20kHz) e não introduzir distorção harmônica ou ruídos ao arrastar os nós (ausência de clicks/zipper noise).
3.  **Análise de Espectro Precisa**: O analisador visual precisa reagir de forma instantânea e balística, permitindo identificar graficamente ressonâncias incômodas com clareza.
4.  **Consistência de Estado**: Salvamento estável de presets, compatibilidade total com automação de DAW e integridade em diferentes taxas de amostragem.

---

## 3. Estratégia de Diferenciação e Posicionamento do AUREQ

O AUREQ não deve copiar a interface ou o código dos concorrentes. Ele focará em diferenciais próprios alinhados com o Design System da **ASTRA Audio Suite**:

### Recursos Obrigatórios (Tabela de Entrada no Mercado)
*   **Múltiplas Bandas (8 a 24)** com resposta gráfica interativa em tempo real.
*   **Analisador FFT duplo** (exibindo espectro de entrada e saída simultaneamente).
*   **Tipos de filtros clássicos**: HPF, LPF, Bell, Shelvings, Notch e Band Pass.
*   **Suavização de parâmetros de alta performance** (zero clicks durante automações).

### Diferenciais Próprios da ASTRA Audio Suite
*   **Interface Dual-Theme Premium**: Diferente dos concorrentes focados estritamente em visual escuro, o AUREQ implementará um **Light Mode** e **Dark Mode** simétricos de altíssimo refinamento estético, baseados em tokens de cores de HSL calibrados para reduzir fadiga visual sob qualquer ambiente de iluminação.
*   **Workflow Sem Distrações**: Ocultamento automático de controles de bandas inativas via caixa flutuante inteligente, maximizando a área livre do display e o espaço negativo (mínimo de 35% de respiro na janela).
*   **Código Altamente Otimizado e Modular**: Arquitetura C++ leve, sem bibliotecas pesadas e dependências proprietárias, facilitando carregamento ultrarápido no VST Host.

---

## 4. Riscos de Desenvolvimento e Estratégia de Mitigação

### Risco principal: Feature Creep e Paralisia de Escopo
Tentar implementar todos os recursos avançados de equalizadores de ponta (como convolução FIR linear phase, equalização dinâmica contínua, detecção Mid/Side e analisadores multibanda) na primeira versão criará um código complexo, propenso a bugs e vazamentos de memória, atrasando o lançamento.

### Estratégia de Mitigação por Fases (Faseamento do AUREQ)
*   **Fase 0.1 (MVP)**: Foco estrito em estabilidade do áudio. Filtros IIR tradicionais de 8 bandas, APVTS, transição suave de parâmetros, bypass seguro, medidores básicos de sinal e interface clean estática nos modos Light/Dark. **O objetivo é ter um equalizador linear que funcione perfeitamente sem travar a DAW.**
*   **Fase 0.2**: Introdução de recursos utilitários que melhoram o workflow (Undo/Redo, A/B, Solo de banda) e analisador FFT de entrada/saída mais refinado.
*   **Fase 1.0**: Expansão para até 24 bandas e inserção de dinâmica (Dynamic EQ) utilizando envelope followers locais por canal. Redimensionamento de interface.
*   **Fase 2.0+ (Roadmap)**: Processamento de fase linear (Linear Phase via FIR), Mid/Side estéreo e ferramentas inteligentes de auto-equalização.

Ao estruturar o desenvolvimento de forma modular e sequencial, garantimos que o monorepo sempre compile uma versão funcional e estável do AUREQ antes de avançar na complexidade matemática e visual.
