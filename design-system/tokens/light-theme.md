# Light Mode Specification

Este documento define a identidade e as regras de design para a versão **Light Mode** dos plugins do **VST - Project**. O objetivo é oferecer uma interface limpa, iluminada e arejada, ideal para sessões longas de estúdio sob luz natural ou ambientes claros, mantendo o apelo estético premium e minimalista.

---

## Descrição Geral

O Light Mode é caracterizado por cores claras e frias, utilizando uma paleta sutil baseada em brancos puros e tons de azul acinzentado (Ice Blue/Slate). O contraste é mantido através de variações sutis de luminosidade e sombras muito suaves, criando uma sensação física de camadas sobrepostas (profundidade de papel/glassmorphism leve). 

Os acentos de cores brilhantes (Cyan e Mint) fornecem uma âncora visual nos parâmetros importantes, impedindo que a interface pareça apagada ou monótona.

---

## Paleta Recomendada (Light Mode)

- **Fundo Principal (Window Base)**: `#EDF2F4` (Azul-gelo acinzentado, reduz a fadiga ocular em comparação com o branco puro)
- **Fundo de Painéis**: `#FFFFFF` (Branco puro, gerando contraste com a base)
- **Painéis Elevados / Controles**: `#FFFFFF` com bordas sutis e sombras macias.
- **Divisores / Bordas**: `#E2E8F0` (Borda fina de 1px)
- **Texto Principal (Labels/Títulos)**: `#0F172A` (Slate 900)
- **Texto Secundário (Valores/Unidades)**: `#475569` (Slate 600)
- **Acento Primário (Knobs Ativos)**: `#0891B2` (Cyan 600)
- **Acento Secundário (Toggle Switches)**: `#0D9488` (Teal 600)
- **Glow Ativo**: `rgba(8, 145, 178, 0.15)` (Aura cyan sutil ao redor dos elementos ativos)

---

## Tokens Principais no Light Mode

*   `colorBackgroundPrimary`: `#EDF2F4`
*   `colorBackgroundSecondary`: `#FFFFFF`
*   `colorPanelSurface`: `#F8FAFC`
*   `colorTextPrimary`: `#0F172A`
*   `colorAccentPrimary`: `#0891B2`
*   `shadowSoft`: `0px 2px 8px rgba(15, 23, 42, 0.06)`

---

## Regras de Contraste

1. **Relação de Contraste do Texto**: Todos os textos e valores importantes devem manter uma taxa de contraste mínima de `4.5:1` em relação aos fundos, em conformidade com as diretrizes de acessibilidade (WCAG AA). Títulos principais utilizam `#0F172A` sobre fundo `#FFFFFF` ou `#EDF2F4`.
2. **Separação Visual**: Evitar linhas escuras de contorno. A divisão de seções deve ser feita prioritariamente por **espaço negativo** ou por variação do fundo de `#EDF2F4` para `#FFFFFF`, com bordas muito sutis de tom `#E2E8F0`.
3. **Indicadores Ativos**: Os caminhos de knobs ativos ou botões acionados devem possuir uma cor saturada e contrastante (`#0891B2` ou `#0D9488`) para identificação instantânea.

---

## Comportamento dos Componentes no Light Mode

### A. Knobs (Controles Giratórios)
*   **Superfície**: Corpo circular preenchido com gradiente suave do cinza claro ao branco (`#F8FAFC` a `#FFFFFF`), com contorno sutil de `1px` em `#E2E8F0`.
*   **Sombra**: Sombra projetada pequena e suave (`shadowKnob`) para dar relevo físico tridimensional.
*   **Ponteiro / Arco de Valor**: O arco que preenche o valor do knob deve ser desenhado na cor `#0891B2` (Cyan). Um pequeno indicador luminoso (`glowActive`) pode aparecer sob o indicador de valor quando selecionado.
*   **Hover**: O corpo do knob clareia ligeiramente e a sombra projetada aumenta discretamente de tamanho para dar sensação de aproximação física.

### B. Botões (Switches e Toggles)
*   **Default**: Fundo `#FFFFFF` com borda sutil `#E2E8F0` e texto `#475569`.
*   **Hover**: Transição suave para fundo `#F1F5F9` (cinza muito claro).
*   **Pressed**: Deslocamento visual de 1px para baixo e redução da sombra para simular clique mecânico.
*   **Active (Ligado)**: Fundo preenchido com a cor de acento secundário `#0D9488` ou contorno cyan `#0891B2` com texto escuro, dependendo do papel do botão.

### C. Medidores (Meters)
*   **Fundo da Calha (Track)**: `#E2E8F0` (Cinza claro neutro).
*   **Barras de Sinal**: Segmentos ou barras contínuas em verde-claro mint `#10B981`. Conforme o sinal atinge níveis altos, transiciona para o amarelo-âmbar `#F59E0B`.
*   **Clipping**: O indicador de clip de pico acende em `#EF4444` e mantém uma opacidade reduzida de decaimento lento para visualização segura.

### D. Painéis (Surfaces)
*   Devem possuir cantos arredondados (`radiusMedium` - `8px`).
*   O fundo deve ser branco sólido `#FFFFFF` sobre a janela `#EDF2F4`, destacando o conteúdo dos controles.
*   Sombra externa extremamente sutil (`shadowSoft`) para "flutuar" sobre o fundo geral da janela.

---

## O que EVITAR no Light Mode

*   ❌ **Bordas Pretas ou Escuras**: Não usar contornos em tons escuros de cinza ou preto, pois poluem visualmente a interface e quebram a leveza do design.
*   ❌ **Brancos Excessivamente Brilhantes como Fundo Geral**: Evitar preencher toda a janela com `#FFFFFF` puro, pois causa fadiga visual rápida no usuário.
*   ❌ **Glows Fortes / Excessivos**: No Light Mode, glows com alta opacidade criam borrões artificiais. Use glows de baixíssima opacidade (máximo 15%) e prefira sombras suaves para destacar elementos.
*   ❌ **Texturas e Gradientes Pesados**: Nada de imitação de metal escovado, texturas de fibra de carbono ou gradientes lineares de alto contraste. Mantenha os preenchimentos o mais flat e limpos possível.
