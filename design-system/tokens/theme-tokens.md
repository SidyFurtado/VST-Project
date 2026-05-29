# Design Tokens System

Este documento define o sistema de **Design Tokens** do **VST - Project**. Ele serve como a única fonte de verdade para especificações visuais, garantindo consistência estética entre todos os plugins da coleção, tanto no **Light Mode** quanto no **Dark Mode**.

---

## O que são Design Tokens?

Design Tokens são os átomos visuais de um sistema de design. Eles representam decisões de design codificadas como valores abstratos (por exemplo, cores, tipografia, espaçamento, sombras e raios de borda). Em vez de utilizar valores fixos (hardcoded) no desenho da interface, os componentes fazem referência aos nomes semânticos dos tokens.

---

## Por que usaremos Design Tokens no projeto?

1. **Consistência Multi-Plugin**: Garante que o EQ, o Compressor, o Saturador e o Limiter compartilhem da mesma identidade visual e das mesmas proporções de design.
2. **Suporte Nativo a Temas (Light / Dark)**: A alternância de temas torna-se um simples mapeamento de chaves. A estrutura de layout e os nomes dos tokens permanecem idênticos; apenas os valores associados aos tokens são alterados.
3. **Manutenibilidade**: Alterações estéticas globais (como mudar a cor de destaque da marca de Cyan para Mint) podem ser feitas em um único ponto do sistema de design, propagando-se automaticamente por toda a interface.
4. **Agilidade no Desenvolvimento**: Programadores e designers trabalham com o mesmo vocabulário de tokens, eliminando ambiguidades de especificações (ex: "cinza um pouco mais escuro").

---

## Como os Tokens ajudam na consistência do Light e Dark Mode?

Os temas de nossos plugins compartilham rigorosamente o mesmo layout físico, espaçamentos, estruturas de painéis, tamanhos de botões e posicionamento de controles. Isso garante que a memória muscular e a usabilidade do usuário sejam preservadas. 

Ao aplicar a mudança de tema, trocamos apenas a "roupa" de cores e efeitos, mantendo o esqueleto intocado. Os tokens definem semanticamente o papel de cada valor, por exemplo:
* `colorBackgroundPrimary` sempre representa o fundo geral da janela, seja ele azul-gelo acinzentado no Light Mode ou grafite-petróleo escuro no Dark Mode.
* `spacingMedium` sempre representa 16 pixels de espaçamento, mantendo o layout arejado e alinhado em ambos os modos.

---

## Utilidade Futura em JUCE e Consumo pelos Plugins

No JUCE Framework, a interface do usuário é desenhada em C++ por meio de classes derivadas de `juce::Component` utilizando o método `paint()`. 
* **Cores**: Em vez de declarar `g.fillAll (juce::Colours::grey);`, usaremos um gerenciador de temas (`ThemeManager`) que busca a cor atual associada ao token, ex: `g.fillAll (ThemeManager::getColor (ThemeTokens::colorBackgroundPrimary));`.
* **Dimensões, Raios e Espaçamentos**: Serão definidos como constantes em cabeçalhos (ex: `ThemeTokens::radiusMedium` ou `ThemeTokens::spacingLarge`) e usados para posicionar componentes nos métodos `resized()` e desenhar retângulos arredondados com `g.drawRoundedRectangle()`.
* **Desacoplamento**: A lógica do DSP é completamente alheia a esses tokens. O editor de GUI (`AudioProcessorEditor`) consulta o `ThemeManager` ao registrar atualizações visuais e redesenha a tela quando o usuário muda o seletor de tema na interface.

---

## Categorias de Tokens

### A. Color Tokens (Cores de Painéis, Textos e Acentos)

| Token | Uso | Light Mode | Dark Mode | Observação |
| :--- | :--- | :--- | :--- | :--- |
| `colorBackgroundPrimary` | Fundo principal da janela | `#EDF2F4` | `#0B0F12` | Base geral do plugin |
| `colorBackgroundSecondary` | Fundo de painéis e seções | `#FFFFFF` | `#151D24` | Superfície base para controles |
| `colorPanelSurface` | Superfície de cartões de controle | `#F8FAFC` | `#1E2933` | Painéis para agrupar knobs de DSP |
| `colorPanelElevated` | Superfície elevada/foco | `#FFFFFF` | `#283642` | Controles ou modais em destaque |
| `colorBorderSubtle` | Divisores e bordas finas | `#E2E8F0` | `#1C2833` | Bordas suaves com cantos redondos |
| `colorTextPrimary` | Títulos e rótulos principais | `#0F172A` | `#F8FAFC` | Altíssima legibilidade |
| `colorTextSecondary` | Subtítulos e valores secundários | `#475569` | `#94A3B8` | Usado para unidades de medida (dB, Hz) |
| `colorTextDisabled` | Rótulos de controles inativos | `#94A3B8` | `#475569` | Legibilidade reduzida para bypass/off |
| `colorAccentPrimary` | Acento e foco principal | `#0891B2` | `#22D3EE` | Knobs ativos, destaque de banda |
| `colorAccentSecondary` | Acento de modulação/auxiliar | `#0D9488` | `#2DD4BF` | Alternadores e caminhos secundários |
| `colorHighlight` | Brilho sutil de topo ou reflexo | `#FFFFFF` | `#FFFFFF` | Opacidade baixa (10%) para relevo |

### B. Typography Tokens (Tipografia Técnico e Limpa)

| Token | Finalidade | Família de Fonte | Tamanho (px) | Peso | Estilo / Caso de Uso |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `fontHeaderTitle` | Título do Plugin | `Inter` ou `Outfit` | `18px` | Bold (`700`) | Letras maiúsculas, espaçadas |
| `fontSectionTitle` | Título de Seções | `Inter` | `13px` | SemiBold (`600`) | Identificação de blocos (ex: INPUT) |
| `fontLabelSmall` | Rótulos de Knobs | `Inter` | `10px` | Medium (`500`) | Identificação de parâmetros |
| `fontValueDisplay` | Exibição numérica de valores | `Roboto Mono` | `11px` | Regular (`400`) | Fonte monoespaçada para valores |
| `fontMenuText` | Preset browser e menus | `Inter` | `12px` | Regular (`400`) | Navegador e dropdowns |

### C. Spacing Tokens (Layout Arejado e Negativo)

| Token | Finalidade | Valor (px) | Observação de Uso |
| :--- | :--- | :--- | :--- |
| `spacingSmall` | Espaçamento interno | `8px` | Distância entre label e knob, ou texto e borda |
| `spacingMedium` | Espaçamento entre controles | `16px` | Distância padrão entre knobs do mesmo grupo |
| `spacingLarge` | Espaçamento entre seções | `24px` | Margem interna dos painéis principais |
| `spacingExtraLarge`| Margem externa da janela | `32px` | Espaço negativo nas bordas gerais do plugin |

### D. Radius Tokens (Cantos Suaves)

| Token | Finalidade | Valor (px) | Observação de Uso |
| :--- | :--- | :--- | :--- |
| `radiusSmall` | Cantos de botões e switches | `4px` | Bordas levemente arredondadas |
| `radiusMedium` | Cantos de painéis de parâmetros | `8px` | Cantos padrão do sistema |
| `radiusLarge` | Cantos da janela e displays | `12px` | Visual moderno e fluido, bastante suave |

### E. Shadow Tokens (Profundidade por Camadas)

| Token | Finalidade | Light Mode | Dark Mode | Observação de Uso |
| :--- | :--- | :--- | :--- | :--- |
| `shadowSoft` | Sombra para profundidade geral | `0px 2px 8px rgba(15,23,42,0.06)` | `0px 2px 10px rgba(0,0,0,0.30)` | Bordas externas de painéis |
| `shadowPanel` | Sombra para painéis elevados | `0px 4px 16px rgba(15,23,42,0.08)` | `0px 4px 20px rgba(0,0,0,0.45)` | Cartões e menus suspensos |
| `shadowKnob` | Sombra de controles giratórios | `0px 2px 4px rgba(15,23,42,0.12)` | `0px 2px 6px rgba(0,0,0,0.50)` | Efeito 3D sutil nos knobs |

### F. Glow Tokens (Acentos Luminosos e Feedback)

| Token | Finalidade | Light Mode | Dark Mode | Observação de Uso |
| :--- | :--- | :--- | :--- | :--- |
| `glowActive` | Brilho de status ativo | `0px 0px 8px rgba(8,145,178,0.3)` | `0px 0px 12px rgba(34,211,238,0.5)` | Indicadores ligados e anéis de knob |
| `glowFocus` | Destaque temporário de foco | `0px 0px 4px rgba(13,148,136,0.2)` | `0px 0px 8px rgba(45,212,191,0.4)` | Foco de teclado ou seleção |

### G. Meter Tokens (Medidores Discretos)

| Token | Finalidade | Light Mode | Dark Mode | Observação de Uso |
| :--- | :--- | :--- | :--- | :--- |
| `colorMeterSafe` | Níveis normais de sinal (< -12 dB) | `#10B981` | `#34D399` | Mint/Verde sutil, sem saturação |
| `colorMeterWarning` | Níveis de atenção (-12 a -2 dB) | `#F59E0B` | `#FBBF24` | Âmbar/Laranja discreto |
| `colorMeterClip` | Clipping / Sobrecarga (> -2 dB) | `#EF4444` | `#F87171` | Vermelho técnico, bem visível |

### H. State Tokens (Opacidade e Estados)

| Token | Finalidade | Valor (Light & Dark) | Observação de Uso |
| :--- | :--- | :--- | :--- |
| `opacityDefault` | Estado normal do componente | `1.0` | Totalmente opaco |
| `opacityHover` | Mouse sobre o controle | `0.85` | Mudança sutil de luminosidade |
| `opacityActive` | Controle pressionado / em arraste | `0.70` | Feedback imediato ao toque |
| `opacityDisabled` | Controle bypassado ou inativo | `0.40` | Claramente fora de uso |
