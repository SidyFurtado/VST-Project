# Diretrizes Gerais dos Componentes (Component Rules)

Este documento define as regras visuais e comportamentais para os componentes de interface compartilhados na linha de plugins do **VST - Project**. Todos os plugins criados no monorepo devem utilizar estes mesmos padrões de design para garantir consistência.

---

## 1. Knobs (Controles Giratórios)
*   **Regra de Uso**: É o controle principal de parâmetros contínuos de áudio (ex: ganho, frequência, threshold, saturação). Deve ter tamanho grande para ajuste confortável com o mouse.
*   **Aparência (Light Mode)**: Fundo preenchido com gradiente suave do cinza claro ao branco (`#F8FAFC` a `#FFFFFF`), contorno em `#E2E8F0` e arco de preenchimento de valor em `#0891B2` (Cyan).
*   **Aparência (Dark Mode)**: Fundo circular em `#1E2933` com arco de preenchimento ativo em `#22D3EE` (Cyan neon) e indicador de valor brilhante.
*   **Estados**:
    *   *Default*: Opacidade `1.0`. Arco de valor visível conforme o parâmetro atual.
    *   *Hover*: O corpo ganha contorno realçado (`#0891B2` no Light / `#38BDF8` no Dark).
    *   *Active (Arrastando)*: Arco e ponteiro brilham intensamente; valor numérico aparece destacado. Opacidade de outros elementos adjacentes reduz ligeiramente (`0.85`).
    *   *Disabled (Bypass)*: Corpo e arco com opacidade reduzida para `0.40`.
*   **Como evitar poluição**: Não adicione linhas de graduação em formato de relógio ao redor do knob. A indicação de valor é dada estritamente pelo arco de cor circular suave e pelo display numérico digital integrado.

---

## 2. Botões (Switches e Toggles)
*   **Regra de Uso**: Usado para seleções binárias de estado (on/off, ativo/inativo) ou disparos momentâneos. Devem possuir cantos bem arredondados (`radiusSmall` - `4px`).
*   **Aparência (Light Mode)**: Fundo branco `#FFFFFF` com texto `#475569`. Ao ser ativado, o fundo preenche-se com `#0D9488` ou ganha contorno cyan.
*   **Aparência (Dark Mode)**: Fundo escuro `#1E2933` com texto `#94A3B8`. Quando ativo, o fundo preenche-se com `#2DD4BF` ou ganha borda elétrica `#22D3EE`.
*   **Estados**:
    *   *Default*: Fundo e texto em cores base neutras.
    *   *Hover*: Luminosidade ligeiramente maior (`opacityHover` - `0.85`).
    *   *Pressed*: Opacidade reduzida para `0.70` com deslocamento visual de `1px` para baixo.
    *   *Active (On)*: Cor de acento aplicada no fundo ou no texto.
    *   *Disabled*: Opacidade em `0.40`.
*   **Como evitar poluição**: Não utilize gradientes metálicos tridimensionais pesados. Prefira designs "flat-design" limpos com contornos discretos e variações de cor sólidas.

---

## 3. Preset Browser (Navegador de Presets)
*   **Regra de Uso**: Posicionado no centro do cabeçalho do plugin (Top Bar). Permite carregar, salvar e navegar rapidamente pelos presets de fábrica e do usuário.
*   **Aparência (Light Mode)**: Caixa arredondada em `#F8FAFC` com borda fina em `#E2E8F0` e texto centrado `#0F172A`. Setas direcionais limpas à esquerda e direita.
*   **Aparência (Dark Mode)**: Caixa integrada preenchida em `#1E2933` com texto `#F8FAFC` e setas direcionais discretas em `#94A3B8`.
*   **Estados**:
    *   *Default*: Exibe o nome do preset ativo em fonte regular.
    *   *Hover*: Borda externa ganha destaque sutil.
    *   *Active (Clicado)*: Abre um menu suspenso (dropdown) com a lista de presets estruturada sobre sombra panel.
    *   *Disabled*: Não se aplica (cabeçalho sempre ativo).
*   **Como evitar poluição**: O tamanho do navegador de presets deve ser compacto e harmonioso em relação ao logotipo e botões da barra superior.

---

## 4. Theme Toggle (Seletor de Tema)
*   **Regra de Uso**: Posicionado no canto direito do cabeçalho. Permite alternar manualmente entre Light Mode e Dark Mode.
*   **Aparência (Light Mode)**: Ícone minimalista de Lua (`#475569`) indicando a opção de transitar para o modo escuro.
*   **Aparência (Dark Mode)**: Ícone minimalista de Sol (`#F8FAFC`) indicando a opção de transitar para o modo claro.
*   **Estados**:
    *   *Default*: Ícone regular e discreto.
    *   *Hover*: Opacidade sobe para `1.0` com micro-animação de rotação sutil.
    *   *Pressed*: Redução de opacidade temporária.
*   **Como evitar poluição**: Usar apenas um ícone limpo desenhado por vetor (SVG) sem molduras quadradas ou textos explicativos (ex: "LIGHT/DARK").

---

## 5. Medidores Elegantes (Meters)
*   **Regra de Uso**: Visualização e monitoramento da amplitude do sinal de áudio (Peak, RMS ou Gain Reduction).
*   **Aparência (Light Mode)**: Calha interna clara `#E2E8F0` com barra de preenchimento mint `#10B981` e luzes de pico discretas.
*   **Aparência (Dark Mode)**: Calha interna cinza-grafite `#151D24` com barra de preenchimento neon `#34D399` brilhante.
*   **Estados**:
    *   *Default*: Monitoramento dinâmico do áudio.
    *   *Disabled*: Calha e barras perdem a cor viva, passando para cinza opaco `#475569`.
*   **Como evitar poluição**: Escala de dB com marcações minimalistas nas laterais. Não preencher a barra de sinal com dezenas de divisões pretas grossas; use um design de barra de cor contínua ou com divisões imperceptíveis de 1px.

---

## 6. Sliders Discretos (Controles Deslizantes)
*   **Regra de Uso**: Usados para parâmetros auxiliares menos frequentes (como controle de Mix seco/molhado ou largura estéreo) ou onde o espaço vertical é limitado.
*   **Aparência (Light Mode)**: Linha de trilha fina `#E2E8F0` e manipulador (thumb) circular branco `#FFFFFF` com sombra macia.
*   **Aparência (Dark Mode)**: Linha de trilha fina `#1C2833` com thumb circular preenchido em `#F8FAFC`.
*   **Estados**:
    *   *Default*: Manipulador integrado à trilha.
    *   *Hover*: O manipulador aumenta levemente de tamanho.
    *   *Active (Arrastando)*: A trilha ativa preenche-se com a cor de acento cyan.
    *   *Disabled*: Opacidade geral de `0.40`.
*   **Como evitar poluição**: Evitar manipuladores em formato de bloco retangular de metal vintage. Utilize formas circulares geométricas limpas.

---

## 7. Labels Pequenas (Rótulos)
*   **Regra de Uso**: Textos curtos identificando a função de cada controle (ex: "FREQ", "GAIN", "RATIO"). Posicionados sempre abaixo ou acima do knob correspondente.
*   **Aparência**: Fonte `Inter` (`fontLabelSmall`, `10px`). Texto escuro `#475569` no Light Mode; texto claro `#94A3B8` no Dark Mode.
*   **Como evitar poluição**: Sempre em caixa alta (uppercase) para visualização técnica unificada e tamanho muito pequeno, liberando espaço negativo.

---

## 8. Value Displays (Displays de Valores)
*   **Regra de Uso**: Exibição numérica e exata do valor atual do parâmetro (ex: `12.5 kHz`, `-6.0 dB`). Geralmente posicionado dentro do círculo do knob ou em uma caixa dedicada logo abaixo.
*   **Aparência**: Fonte monoespaçada `Roboto Mono` para evitar oscilações horizontais de texto durante o ajuste do mouse.
*   **Como evitar poluição**: Sem caixas de fundo pesadas; o valor deve flutuar suavemente ou usar uma área preenchida em `#F8FAFC` (Light) ou `#1E2933` (Dark) muito sutil.

---

## 9. Tooltips Discretos (Dicas de Ferramenta)
*   **Regra de Uso**: Pequenas caixas explicativas que aparecem no rodapé do plugin ao passar o mouse sobre um controle.
*   **Aparência**: Texto técnico e claro explicando a função do knob em fonte `Inter` de `10px`.
*   **Como evitar poluição**: Não devem abrir como caixas flutuantes sobre os controles, pois bloqueiam a visão do usuário. Devem ser exibidos de forma fixa e elegante em uma área dedicada no rodapé do plugin.

---

## 10. A/B Switch
*   **Regra de Uso**: Botão duplo posicionado na barra superior para comparar rapidamente duas configurações de parâmetros diferentes (Estado A e Estado B).
*   **Aparência**: Duas caixas lado a lado ("A" e "B") integradas em uma única pílula arredondada. O estado ativo recebe destaque em cor cyan.
*   **Como evitar poluição**: Design plano muito compacto.

---

## 11. Bypass / Power Button (Botão de Liga/Desliga)
*   **Regra de Uso**: Botão geral de ativação do plugin ou de seções internas de DSP.
*   **Aparência**: Ícone padrão de liga/desliga mecânico. Vermelho ou cinza apagado quando desativado; acende em cyan/mint brilhante com glow ativo quando ativado.
*   **Como evitar poluição**: Colocado de forma limpa na barra superior ou inferior, livre de contornos excessivamente chamativos.
