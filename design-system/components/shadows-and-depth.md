# Controle de Sombras, Glow e Profundidade (Shadows and Depth)

Este documento especifica a renderização tridimensional conceitual, profundidade de camadas, sombras projetadas e efeitos de iluminação (glow) nos componentes do **VST - Project**. Ele serve para instruir como simular estes elementos visuais premium em ambos os temas.

---

## 1. Camadas de Profundidade (Z-Index Conceitual)

Embora a interface seja bidimensional, organizamos os elementos em **4 níveis de profundidade virtual** por meio de elevações cromáticas e sombras. Quanto mais elevado o elemento, mais claro é seu fundo (especialmente no Dark Mode) e maior é o espalhamento de sua sombra projetada.

```text
  [Camada 3]  -  Controles Ativos e Pop-ups (Mais alto, com shadowPanel + glowActive)
  [Camada 2]  -  Knobs, Botões e Displays (Elevado, com shadowKnob ou contornos sutis)
  [Camada 1]  -  Painéis de agrupamento de parâmetros (Base flutuante, com shadowSoft)
  [Camada 0]  -  Plano de fundo geral da janela (Nível da janela principal do OS)
```

---

## 2. Especificação de Sombras Projetadas (Drop Shadows)

As sombras dão relevo e delimitam áreas sem poluir a interface com bordas pretas duras.

### A. Sombra de Painel (`shadowSoft`)
*   **Aplicação**: Nas caixas de agrupamento de controles.
*   **Light Mode**: `0px 2px 8px rgba(15, 23, 42, 0.06)`. Um gradiente cinza-azulado muito claro e diluído.
*   **Dark Mode**: `0px 2px 10px rgba(0, 0, 0, 0.30)`. Sombra escura e amortecida para se destacar do fundo geral.

### B. Sombra de Painel Elevado e Menus (`shadowPanel`)
*   **Aplicação**: Menus suspensos de presets e janelas modais de configurações.
*   **Light Mode**: `0px 4px 16px rgba(15, 23, 42, 0.08)`. Proporciona forte elevação física.
*   **Dark Mode**: `0px 4px 20px rgba(0, 0, 0, 0.45)`. Garante que os menus flutuantes se destaquem claramente sobre os knobs traseiros.

### C. Sombra de Knob (`shadowKnob`)
*   **Aplicação**: Na circunferência externa de knobs e botões rotativos grandes.
*   **Light Mode**: `0px 2px 4px rgba(15, 23, 42, 0.12)`.
*   **Dark Mode**: `0px 2px 6px rgba(0, 0, 0, 0.50)`.

---

## 3. Sombras Internas (Inner Shadows)

*   **Aplicação**: Calhas de medidores (meters) e campos de entrada de valores de texto para simular um efeito de "cavidade" ou "rebaixo" na superfície física.
*   **Visual**: Um gradiente linear sutil desenhado no topo interno e na esquerda interna do retângulo com opacidade de 8% de preto, dando profundidade negativa.

---

## 4. Efeitos de Iluminação (Glows)

Os glows representam a energia ativa dos parâmetros de áudio e do processamento de sinal.

### A. Glow Ativo (`glowActive`)
*   **Aplicação**: Anel circular externo de knobs de controle ativos ou indicador aceso do botão Bypass/Power.
*   **Cor**: `#06B6D4` (Light Mode) / `#22D3EE` (Dark Mode).
*   **Light Mode**: Opacidade de `15%`, raio de difusão de `6px`.
*   **Dark Mode**: Opacidade de `35%`, raio de difusão de `10px`. Projeta um brilho neon "neon-like" característico de interfaces modernas premium.

### B. Glow de Foco (`glowFocus`)
*   **Aplicação**: Quando um knob ou caixa de entrada de valor é selecionada via teclado para digitação exata de parâmetro.
*   **Visual**: Uma borda cyan suave de 1px com raio de blur de 4px ao redor do controle.

---

## 5. Efeito Frosted / Blur (Glassmorphism)

*   **Aplicação**: Pode ser aplicado na barra superior ou no fundo de menus dropdown de presets.
*   **Simulação em JUCE**: Como o JUCE não possui blur de GPU nativo simples para backgrounds, esse efeito será simulado na fase de código pintando um retângulo semi-transparente preenchido com a cor de painel elevada (`colorPanelElevated`) com opacidade de `85%` e adicionando uma linha superior branca de `1px` com `10%` de opacidade para imitar o reflexo de vidro físico.

---

## 6. O que EVITAR no Uso de Profundidade e Sombras

*   ❌ **Sombras Pretas Sem Opacidade (Hard Shadows)**: Evitar sombras pretas sólidas ou com raio de espalhamento zero, pois parecem inacabadas e geram poluição estética.
*   ❌ **Brilho em Knobs Bypassados**: Controles desativados nunca devem possuir glow ou sombras destacadas. Eles devem ser mantidos planos e com opacidade reduzida.
*   ❌ **Glows Coloridos Conflitantes**: Não misture glows vermelhos, azuis e verdes na mesma tela. Use apenas a assinatura de cor principal da marca (Cyan) e reserve o vermelho e verde exclusivamente para os status técnicos dos meters de áudio.
