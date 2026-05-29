# Estruturação de Painéis e Layout (Panel Layout)

Este documento especifica as regras de estruturação física da interface gráfica (GUI) dos plugins do **VST - Project**. Ele define o grid base, as margens, o fluxo visual e a divisão anatômica da janela para garantir consistência estética e layouts arejados de alta qualidade.

---

## 1. Anatomia Geral da Janela do Plugin

O layout padrão da interface dos plugins é dividido verticalmente em **4 seções bem delineadas**:

```text
+-------------------------------------------------------------+
| [LOGO]                [PRESET BROWSER]          [A/B] [SUN] | -> Top Bar (Cabeçalho)
+-------------------------------------------------------------+
|                                                             |
|                       [DISPLAY / METER]                     | -> Display / Área Visual
|                                                             |
+-------------------------------------------------------------+
|                                                             |
|          [KNOB 1]         [KNOB 2]         [KNOB 3]         | -> Control Area (Área de Controles)
|                                                             |
+-------------------------------------------------------------+
| [BYPASS]    [OVER]        [TOOLTIP]         [I/O] [VERSION] | -> Footer (Rodapé)
+-------------------------------------------------------------+
```

### A. Top Bar (Cabeçalho)
*   **Altura**: `48px` fixa.
*   **Elementos**: 
    *   *Esquerda*: Logotipo da marca do plugin (texto limpo em Outfit/Inter Bold).
    *   *Centro*: Preset Browser com setas de navegação.
    *   *Direita*: Botão A/B e o interruptor de seletor de tema (Light/Dark).

### B. Display / Área Visual
*   **Altura**: Variável (ex: `160px` a `220px`).
*   **Elementos**: Analisador de espectro de frequência (FFT) para EQs, medidores dinâmicos de redução de ganho e compressão, ou visualizadores de ondas em tempo real. Esta área é dispensada ou reduzida em plugins extremamente simples.

### C. Control Area (Área de Controles)
*   **Altura**: Variável dependendo do número de knobs.
*   **Elementos**: Painéis arredondados (`radiusMedium` - `8px`) agrupando os knobs de controle de DSP (ex: Threshold, Gain, Q, Drive).

### D. Footer (Rodapé)
*   **Altura**: `32px` fixa.
*   **Elementos**:
    *   *Esquerda*: Interruptor de Power/Bypass global e seletor de Oversampling (se aplicável).
    *   *Centro*: Tooltip fixo discreto que detalha o parâmetro sob o mouse.
    *   *Direita*: Medidor compacto de entrada e saída (Input/Output Gain) e o número da versão do plugin (ex: `v1.0.0`).

---

## 2. Grid Base e Espaçamentos

Para manter uma interface limpa, respirável e sem aperto visual:
*   **Margens Externas da Janela**: Mapeado pelo token `spacingExtraLarge` (`32px`). Isso garante que nenhum controle fique colado na borda externa do plugin.
*   **Espaçamento entre Knobs**: Mapeado pelo token `spacingMedium` (`16px`) no mínimo. Controles grandes devem manter preferencialmente `24px` de distância horizontal entre si.
*   **Padding Interno de Painéis**: Mapeado por `spacingLarge` (`24px`).
*   **Cantos da Janela Principal**: Arredondados com `radiusLarge` (`12px`) para dar um aspecto moderno e integrado com sistemas operacionais contemporâneos.

---

## 3. Comportamento por Complexidade do Plugin

### A. Plugins Simples (2 a 4 Controles)
*   *Exemplo*: Saturador ou Compressor de 2 botões.
*   *Layout*: Não necessita de uma seção dedicada de "Display / Área Visual". Os knobs devem ser desenhados de tamanho extra grande (`72px`) dispostos horizontalmente no centro da tela com bastante espaço negativo nas laterais para reforçar o foco e a leveza visual.
*   *Dimensões sugeridas*: `480px` de largura por `280px` de altura.

### B. Plugins Complexos (EQ Paramétrico de Múltiplas Bandas, Limitadores)
*   *Exemplo*: Equalizador de 5 bandas ou Limitador de Masterização.
*   *Layout*: Dividido estritamente com o Display de FFT no topo e um painel de controle inferior com knobs de seleção de banda integrados. Os painéis devem ser usados de forma inteligente para agrupar controles dependentes (ex: Gain, Freq, Q em uma caixa comum).
*   *Dimensões sugeridas*: `680px` de largura por `420px` de altura.

---

## 4. Diretrizes para um Visual Arejado e Premium

*   ✔️ **Espaço Negativo**: Pelo menos `35%` da área total do plugin deve ser composta por espaço negativo livre de controles ou marcações de escala. Isso ajuda o usuário a "respirar" enquanto trabalha.
*   ✔️ **Painéis Suaves**: Os painéis de agrupamento de parâmetros devem possuir a cor `colorPanelSurface` (`#FFFFFF` no Light / `#1E2933` no Dark) com sombras muito suaves (`shadowSoft`) e bordas quase invisíveis.
*   ❌ **Evitar Layout Assimétrico Desordenado**: Os controles devem estar perfeitamente alinhados a um grid vertical e horizontal comum. Use sempre os métodos de partição de retângulos do JUCE (`juce::Rectangle::removeFromTop`, `removeFromLeft`, etc.) para garantir cálculos matematicamente consistentes.
