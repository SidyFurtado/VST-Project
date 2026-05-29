# Estilo e Estados de Botões (Button Style)

Os **Botões** nos plugins do **VST - Project** seguem uma abordagem geométrica minimalista e flat, com cantos arredondados, transições de opacidade suaves e sombras delicadas. Este documento estabelece o design visual dos botões em todas as suas variantes e estados para ambos os temas.

---

## 1. Tipos de Botões e Funções

### A. Power / Bypass (Liga e Desliga do Plugin)
*   **Aparência**: Um círculo pequeno ou quadrado arredondado com o ícone padrão de energia.
*   **Modo Ativo (Ligado)**: O ícone acende em cyan/mint brilhante (`#22D3EE` ou `#2DD4BF` no Dark / `#0891B2` no Light) com um pequeno halo de luz.
*   **Modo Inativo (Bypass)**: O ícone apaga (cinza-claro no Light, chumbo no Dark), sinalizando o desvio de áudio.

### B. A/B Switch (Comparação de Parâmetros)
*   **Aparência**: Uma pílula retangular horizontal dividida em duas metades ("A" e "B").
*   **Comportamento**: Apenas uma das opções está ativa por vez. A metade ativa recebe cor de preenchimento ou texto contrastante, enquanto a inativa integra-se no fundo.

### C. Preset Next / Previous (Navegação de Presets)
*   **Aparência**: Pequenos botões quadrados contendo chevron simples apontando para a esquerda (`<`) ou direita (`>`).
*   **Uso**: Posicionados imediatamente ao lado do Preset Browser para navegação sequencial rápida de patches.

### D. Menu / Settings (Menu de Configurações)
*   **Aparência**: Ícone de engrenagem simples ou três pontos verticais/horizontais.
*   **Comportamento**: Ao clicar, abre um menu flutuante (dropdown) com opções como Oversampling, manual do usuário, etc.

### E. Theme Switch (Alternador de Tema)
*   **Aparência**: Um botão circular de ícone único (Sol para Light, Lua para Dark).
*   **Comportamento**: Troca instantaneamente o mapeamento de tokens do plugin.

### F. Botões Toggle (Chaves de Opção)
*   **Aparência**: Botões de texto (ex: "HPF", "OVERSAMPLING") que permanecem pressionados ao clicar.
*   **Visual**: Fundo muda de tom e texto fica ativo ao ligar.

### G. Botões Momentâneos (Ações Instantâneas)
*   **Aparência**: Botões que efetuam um disparo rápido (ex: "Reset" ou "Clear Peak").
*   **Visual**: Piscam brevemente e retornam ao estado default.

---

## 2. Estados Visuais por Tema

Os botões utilizam transições de opacidade e pequenas variações cromáticas definidas pelos tokens de estado.

### A. Light Mode Specification

| Estado | Aparência Visual | Cor do Fundo | Cor do Texto / Ícone | Borda / Sombras |
| :--- | :--- | :--- | :--- | :--- |
| **Default** | Estático, limpo | `#FFFFFF` | `#475569` | Borda `1px` em `#E2E8F0` |
| **Hover** | Destacado, atrativo | `#F1F5F9` | `#0F172A` | Aumento sutil de contraste |
| **Pressed** | Pressionado físico | `#E2E8F0` | `#0F172A` | Offset visual de `1px` para baixo |
| **Active (On)** | Ligado | `#0D9488` | `#FFFFFF` | Sem borda visível |
| **Disabled** | Indisponível | `#FFFFFF` (40%) | `#94A3B8` (40%) | Opacidade geral reduzida |

### B. Dark Mode Specification

| Estado | Aparência Visual | Cor do Fundo | Cor do Texto / Ícone | Borda / Sombras |
| :--- | :--- | :--- | :--- | :--- |
| **Default** | Estático, chumbo | `#1E2933` | `#94A3B8` | Sem borda |
| **Hover** | Brilho sutil | `#283642` | `#F8FAFC` | Borda opcional em `#22D3EE` (30%) |
| **Pressed** | Pressionado físico | `#151D24` | `#F8FAFC` | Offset visual de `1px` para baixo |
| **Active (On)** | Ligado | `#2DD4BF` | `#0B0F12` | Glow elétrico discreto |
| **Disabled** | Indisponível | `#1E2933` (40%) | `#475569` (40%) | Opacidade geral reduzida |

---

## 3. Diretrizes de Layout e Alinhamento

*   **Tamanho Mínimo de Toque (Click Area)**: Todo botão clicável deve possuir uma área mínima de `24px x 24px` para evitar erros de clique.
*   **Alinhamento**: Os botões da barra superior devem estar perfeitamente centralizados verticalmente em relação ao Preset Browser.
*   **Espaçamento Interno (Padding)**: Botões de texto devem possuir preenchimento lateral de no mínimo `12px` e vertical de `6px` para acomodar as labels de forma harmoniosa.

---

## 4. O que EVITAR no Design de Botões

*   ❌ **Bordas brilhantes grossas**: Evite contornos de 2px ou mais que chamam atenção desnecessária.
*   ❌ **Textos em Negrito Gigantes**: Use fontes de peso regular ou médio (`fontLabelSmall`, `fontMenuText`) e tamanhos reduzidos (`10px` a `12px`).
*   ❌ **Efeitos de Vidro (Glassmorphism) Carregados**: Evite gradientes espelhados excessivos que simulem plástico ou vidro real. Mantenha os preenchimentos sutis.
