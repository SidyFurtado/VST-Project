# Comportamento de Temas (Theme Behavior)

Este documento detalha o comportamento dos temas (Light Mode e Dark Mode) nos plugins da **ASTRA Audio Suite** (desenvolvida no monorepo **VST - Project**), estabelecendo regras para a experiência de alternância, usabilidade, acessibilidade e preservação das preferências do usuário.

---

## 1. Objetivos de Cada Modo

### Light Mode (Modo Claro)
*   **Objetivo principal**: Proporcionar uma visualização confortável sob alta iluminação ambiente (luz solar direta, escritórios com lâmpadas fortes).
*   **Quando usar**: É mais confortável em sessões diurnas ou quando o usuário precisa ler de forma muito clara os displays numéricos detalhados sem forçar a visão.
*   **Estética**: Limpa, arejada, transmitindo precisão laboratorial e clareza cirúrgica.

### Dark Mode (Modo Escuro)
*   **Objetivo principal**: Reduzir a fadiga ocular em salas de estúdio escuras ou durante sessões noturnas de mixagem de longa duração.
*   **Quando usar**: Adequado para manter o foco total na DAW sob baixa luminosidade, evitando a projeção de luz branca brilhante nos olhos do usuário.
*   **Estética**: Sofisticada, moderna, transmitindo tecnologia, profundidade e elegância.

---

## 2. Escolha Manual de Tema vs. Sincronização do OS

*   **Abordagem Recomendada**: O plugin deve ter uma **escolha manual** direta e explícita dentro da interface do usuário. Embora a detecção automática do tema do sistema operacional (macOS/Windows) seja útil, a experiência prática com DAWs mostra que muitos produtores preferem trabalhar com a DAW e seus plugins em um tema fixo (frequentemente escuro), independente da hora do dia ou do tema do OS.
*   **Seletor de Tema na Interface**: O controle de troca de tema deve ser posicionado de forma discreta na barra superior do cabeçalho do plugin (Top Bar), no lado direito, próximo ao botão de configurações. Será representado por um botão de ícone minimalista (um ícone de Sol/Lua ou um interruptor deslizante limpo).
*   **Persistência do Estado**: A preferência do usuário pelo tema (Light ou Dark) deve ser salva no estado global do plugin. Quando o produtor fecha e reabre o projeto na DAW, ou insere uma nova instância do plugin, a interface deve restaurar o último tema selecionado automaticamente.

---

## 3. Elementos na Transição de Temas

### O que MUDA ao alternar o tema:
*   As cores de fundo de todas as superfícies (janela, painéis, displays e calhas de medidores).
*   As cores de todos os textos (títulos, labels de parâmetros, valores).
*   As cores de contorno e bordas finas de delimitação.
*   As cores do corpo dos knobs e dos switches desativados.
*   As sombras projetadas de painéis e botões (ajuste de opacidades).
*   A intensidade do brilho do glow ativo (cyan/mint).

### O que NUNCA muda ao alternar o tema:
*   A posição física e o tamanho de todos os controles na tela.
*   O comportamento mecânico de arraste de knobs ou cliques de botões.
*   As cores de aviso de sinal dos meters (`Safe`: Mint/Verde, `Warning`: Laranja/Ambar, `Clip`: Vermelho). Isso evita confundir o usuário sobre o comportamento de saturação de áudio.
*   O estado de processamento de áudio (DSP), buffers e parâmetros de sinal.

---

## 4. Cuidados Críticos de Design e Acessibilidade

### A. Legibilidade e Contraste
Tanto no Light quanto no Dark Mode, o texto dos rótulos de parâmetros e os valores de dB/Hz devem atender aos requisitos mínimos de contraste de cores (WCAG AA). 
*   No Light Mode, evite o uso de textos cinza-claros sobre fundo branco. 
*   No Dark Mode, evite usar cinza-escuro sobre fundo chumbo. A cor semântica `colorTextSecondary` deve ser testada e mantida em níveis confortáveis em ambos os temas.

### B. Glow no Dark Mode
O Dark Mode permite a aplicação de glows cyan brilhantes nos elementos ativos, mas eles devem ser calibrados:
*   Os glows não devem possuir raio de difusão (`blur-radius`) excessivo para não embaçar os números ou labels vizinhas.
*   A opacidade do glow ativo deve ficar limitada a `35%` (`rgba(34, 211, 238, 0.35)`).

### C. Sombras no Light Mode
No Light Mode, a percepção de profundidade depende fortemente de sombras macias projetadas. 
*   Evite sombras escuras e contrastantes com bordas duras, pois elas poluem a interface. 
*   Utilize tons de cinza escuro ou azul marinho muito diluídos (ex: `#0F172A` com opacidade de apenas `6%`) para criar uma transição de gradiente imperceptível, gerando um efeito de relevo limpo e premium.
