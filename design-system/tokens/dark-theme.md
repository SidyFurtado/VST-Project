# Dark Mode Specification

Este documento define a identidade e as regras de design para a versão **Dark Mode** dos plugins do **VST - Project**. O objetivo é criar um visual sofisticado, focado e de alta tecnologia, ideal para estúdios com pouca iluminação e longas sessões noturnas de mixagem ou masterização.

---

## Descrição Geral

O Dark Mode do projeto adota uma estética minimalista moderna baseada em tons profundos de cinza-chumbo, grafite e azul petróleo escuro, evitando o preto puro (`#000000`) para mitigar o contraste excessivo que causa fadiga visual. 

A profundidade é estabelecida por camadas de superfícies progressivamente mais claras (quanto mais elevado o painel, mais claro ele é, seguindo as diretrizes de design de material moderno). Acentos vibrantes de cor neon (como Cyan elétrico e Mint brilhante) e halos de glow sutis dão vida aos controles ativos, criando um visual moderno e "arejado" mesmo sob condições de pouca luz.

---

## Paleta Recomendada (Dark Mode)

- **Fundo Principal (Window Base)**: `#0B0F12` (Cinza escuro com fundo azul petróleo profundo)
- **Fundo de Painéis**: `#151D24` (Grafite azulado)
- **Painéis Elevados / Controles**: `#1E2933` (Cinza escuro em destaque)
- **Divisores / Bordas**: `#1C2833` (Borda de contorno sutil)
- **Texto Principal (Labels/Títulos)**: `#F8FAFC` (Slate 50, alto brilho, leitura suave)
- **Texto Secundário (Valores/Unidades)**: `#94A3B8` (Slate 400, cinza médio confortável)
- **Acento Primário (Knobs Ativos)**: `#22D3EE` (Cyan 400, elétrico e definido)
- **Acento Secundário (Toggle Switches)**: `#2DD4BF` (Teal 400, mint dinâmico)
- **Glow Ativo**: `rgba(34, 211, 238, 0.35)` (Aura cyan elétrica ao redor dos controles ativos)

---

## Tokens Principais no Dark Mode

*   `colorBackgroundPrimary`: `#0B0F12`
*   `colorBackgroundSecondary`: `#151D24`
*   `colorPanelSurface`: `#1E2933`
*   `colorTextPrimary`: `#F8FAFC`
*   `colorAccentPrimary`: `#22D3EE`
*   `shadowSoft`: `0px 2px 10px rgba(0, 0, 0, 0.30)`

---

## Regras de Contraste

1. **Leitura Confortável**: Textos essenciais usam `#F8FAFC` sobre fundos escuros para garantir legibilidade instantânea. Textos menos prioritários utilizam `#94A3B8` ou `#64748B`. O contraste deve manter-se sempre acima de `4.5:1` (WCAG AA).
2. **Camadas de Elevação**: Em vez de contornar tudo com bordas claras e brilhantes, usamos elevações de fundo:
    *   Nível 0 (Fundo Geral): `#0B0F12`
    *   Nível 1 (Painéis de parâmetros): `#151D24`
    *   Nível 2 (Botões / Knobs / Displays): `#1E2933`
3. **Feedback Luminoso**: O status de bypass deve reduzir a opacidade de todo o painel de controle e rótulos para `0.40` (`opacityDisabled`), deixando apenas o botão "Power" apagado, minimizando distrações visuais de controles inativos.

---

## Comportamento dos Componentes no Dark Mode

### A. Knobs (Controles Giratórios)
*   **Superfície**: Corpo circular escuro preenchido com gradiente radial sutil de `#1E2933` no centro a `#151D24` nas bordas. Uma linha de contorno de 1px em `#283642` define a circunferência externa.
*   **Sombra**: Sombra externa escura (`shadowKnob`) com raio de blur moderado para fundir o knob sobre a superfície do painel.
*   **Ponteiro / Arco de Valor**: Desenhado em cyan neon brilhante `#22D3EE`. Opcionalmente, projeta um halo luminoso suave (`glowActive`) com blur de `4px` para imitar o visual de knobs iluminados por LED traseiro.
*   **Hover/Focus**: O arco de valor intensifica o brilho e o anel externo do knob ganha contorno em `#38BDF8` (Cyan claro).

### B. Botões (Switches e Toggles)
*   **Default**: Fundo `#1E2933` com texto `#94A3B8` e sem bordas chamativas.
*   **Hover**: Fundo eleva-se para `#283642`, clareando suavemente.
*   **Pressed**: O botão recua ligeiramente na tela (offset visual de 1px) e a opacidade cai temporariamente para `0.70` (`opacityActive`).
*   **Active (Ligado)**: Fundo preenchido com `#2DD4BF` ou contorno elétrico `#22D3EE` com o texto principal ativo.

### C. Medidores (Meters)
*   **Fundo da Calha (Track)**: `#151D24` (Cinza escuro integrado ao painel).
*   **Barras de Sinal**: Segmentos ou barras contínuas em verde-esmeralda `#34D399` brilhante. Conforme o sinal atinge níveis altos (-12 a -2 dB), transiciona para o amarelo-âmbar `#FBBF24`.
*   **Clipping**: O indicador de clip de pico acende em vermelho elétrico `#F87171` com um glow sutil.

### D. Painéis (Surfaces)
*   Devem possuir cantos arredondados (`radiusMedium` - `8px`).
*   O fundo deve ser grafite `#151D24`. As bordas divisórias de `1px` devem ser desenhadas com a cor `#1C2833`.
*   Sombra externa escura (`shadowSoft` - `rgba(0,0,0,0.30)`) para se destacar sutilmente sobre a base geral do plugin `#0B0F12`.

---

## O que EVITAR no Dark Mode

*   ❌ **Fundos Completamente Pretos**: Não usar `#000000` como plano de fundo. Elementos desenhados sobre preto puro criam alto contraste e dão uma aparência "barata" ou datada à interface (anos 2000).
*   ❌ **Brilhos / Glows Cegantes**: Glows mal calibrados causam borrões na tela e dificultam a leitura dos números. Mantenha os níveis de transparência do glow estritamente controlados (`rgba` com opacidade inferior a 50%).
*   ❌ **Bordas Brancas ou Prateadas Finas**: Evite contornar painéis com linhas finas e claras para delimitar espaços. Isso quebra a estética clean e premium e polui a visualização.
*   ❌ **Texturas Vintage / Carbono**: Sem simulações de hardware vintage analógico arranhado ou tampas de ferro. O visual deve ser suave, moderno e focado na clareza do sinal.
