# Estilo e Comportamento dos Knobs (Knob Style)

Os **Knobs** são os elementos centrais de interação do usuário com os parâmetros contínuos de nossos plugins. Este documento especifica sua aparência, dimensões, estados e comportamento visual para garantir uma sensação premium e moderna, evitando referências a hardware vintage.

---

## 1. Características Visuais Principais

*   **Geometria Limpa**: O corpo do knob é um círculo perfeito, sem marcações físicas salientes, dentes ou texturas ranhuradas. O relevo é obtido por meio de gradientes sutis e sombras projetadas.
*   **Indicador de Valor**: O valor atual do parâmetro é representado por um **arco circular externo** fino que acompanha a circunferência do knob. Esse arco se preenche na cor de acento (`#0891B2` no Light Mode / `#22D3EE` no Dark Mode).
*   **Ponteiro de Direção**: Uma linha radial muito fina ou ponto luminoso na borda interna indica o ângulo exato do controle.
*   **Acessibilidade**: O valor numérico e a label do parâmetro estão sempre associados visualmente de forma clara e limpa.

---

## 2. Aparência por Tema

### Light Mode
*   **Cor do Corpo**: Gradiente linear sutil de `#F8FAFC` (topo) a `#FFFFFF` (base), dando sensação de relevo convexo suave.
*   **Contorno do Corpo**: Borda fina de `1px` em `#E2E8F0`.
*   **Arco de Valor**: Preenchimento contínuo em cyan escuro `#0891B2` sobre uma calha de fundo `#E2E8F0`.
*   **Sombra**: Sombra projetada circular macia (`shadowKnob` - `rgba(15, 23, 42, 0.12)`).

### Dark Mode
*   **Cor do Corpo**: Gradiente radial sutil de `#1E2933` no centro a `#151D24` na borda externa.
*   **Contorno do Corpo**: Borda fina de `1px` em `#283642`.
*   **Arco de Valor**: Preenchimento elétrico brilhante em `#22D3EE` (Cyan neon) com um glow ativo sutil por trás, sobre calha de fundo `#151D24`.
*   **Sombra**: Sombra projetada circular escura (`shadowKnob` - `rgba(0, 0, 0, 0.50)`).

---

## 3. Dimensões e Escalonamento (Sizes)

O knob deve ser desenhado em três tamanhos padrão, dependendo de sua importância hierárquica no painel:

| Categoria | Diâmetro (px) | Caso de Uso Recomendado |
| :--- | :--- | :--- |
| **Knob Grande (Large)** | `64px` | Parâmetros centrais do processador (ex: *Frequência de Corte principal no EQ*, *Threshold no Compressor*, *Drive no Saturador*). |
| **Knob Médio (Medium)** | `48px` | Parâmetros de suporte secundários (ex: *Attack/Release no Compressor*, *Oversampling Amount*). |
| **Knob Pequeno (Small)** | `32px` | Controles auxiliares ou de ganho de saída (ex: *Input/Output Gain*). |

---

## 4. Tipos de Valores e Arcos

### Valores Unipolares
*   **Definição**: Parâmetros que começam em um valor mínimo (esquerda) e sobem até um valor máximo (direita), como *Mix (0% a 100%)* ou *Gain (0 dB a +18 dB)*.
*   **Comportamento do Arco**: O arco começa preenchido no canto inferior esquerdo (ângulo aproximado de `-135°` em relação ao topo) e expande-se no sentido horário até o canto inferior direito (`+135°`).

### Valores Bipolares
*   **Definição**: Parâmetros com centro neutro, permitindo atenuação e ganho (ex: *Gain de Banda de EQ: -12 dB a +12 dB*, ou *Pan: L a R*).
*   **Comportamento do Arco**: O arco inicia preenchido exatamente no centro superior (`0°`). Quando o knob gira para a esquerda, o arco estende-se no sentido anti-horário (azul ou cinza escuro); quando gira para a direita, estende-se no sentido horário (cyan brilhante).

---

## 5. Estados e Comportamento Dinâmico

### Default (Padrão)
O knob é exibido com sua opacidade total (`1.0`) e o arco exibe a posição estática do parâmetro.

### Hover / Focus (Mouse sobre o controle)
*   O corpo do knob clareia levemente no gradiente.
*   O contorno externo ganha contorno cyan.
*   A sombra projetada aumenta discretamente de tamanho para dar sensação de aproximação física (efeito 3D de elevação).
*   O cursor do mouse muda para um indicador de arraste vertical.

### Active / Dragging (Arrastando com o mouse)
*   O arco de valor e o ponteiro aumentam de brilho (glow ativo).
*   Um pequeno indicador numérico flutuante ou o display de valor correspondente entra em estado ativo, exibindo os dados em tempo real com precisão de uma casa decimal (ex: `4.5 dB` ou `1.2 kHz`) em fonte `Roboto Mono`.
*   O usuário pode manter pressionada a tecla `Shift` para ajustes finos (velocidade de arraste reduzida).
*   Um clique duplo com o botão esquerdo redefine o knob para seu valor padrão de fábrica.

### Disabled (Bypass)
*   A opacidade do corpo, do arco de valor, do ponteiro e da etiqueta cai para `0.40`.
*   O controle recusa eventos de arraste ou clique, e o cursor do mouse permanece como a seta padrão.

---

## 6. Como Manter o Design Premium

Para garantir que o knob pareça um software moderno de alta tecnologia e não um hardware antigo de rack:
*   ❌ **Não use marcações numéricas físicas na circunferência** (como traços impressos de 0 a 10). A tela digital resolve a exibição exata do valor.
*   ❌ **Não adicione efeito de brilho metálico escovado** (radial angulado) agressivo.
*   ❌ **Não desenhe parafusos ou fendas** no centro do knob.
*   ✔️ **Priorize a fluidez da animação**: O redesenho do arco circular no método `paint()` do JUCE deve ocorrer de forma suave e otimizada (sem flickering) durante a movimentação do mouse.
