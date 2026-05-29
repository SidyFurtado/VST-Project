# Color Palette System

Este documento define a paleta de cores oficial do **VST - Project** para os modos **Light Mode** e **Dark Mode**. Ela fornece especificações exatas em formato Hexadecimal (com correspondência RGB para uso em C++/JUCE), garantindo consistência e harmonia cromática em todas as superfícies e componentes.

---

## Princípios de Design de Cores

1. **Base Fria e Suave**: Os fundos utilizam bases frias (cinza-azulados) para criar um ambiente moderno e atenuar a fadiga ocular em sessões prolongadas de áudio.
2. **Acentos Elétricos**: Os acentos em tons de Cyan e Mint representam o sinal e a interação humana, dando foco para onde a atenção do usuário deve ir.
3. **Mapeamento Semântico**: Cada cor possui uma finalidade funcional explícita. Não misture cores de status ou modifique opacidades sem justificar com o mapeamento semântico.

---

## 1. Light Mode Palette

| Elemento Visual | Código Hex | Valor RGB (R, G, B) | Aplicação e Significado |
| :--- | :--- | :--- | :--- |
| **Background Principal** | `#EDF2F4` | `(237, 242, 244)` | Base geral da janela do plugin |
| **Background Secundário** | `#FFFFFF` | `(255, 255, 255)` | Fundo dos painéis internos de parâmetros |
| **Painel Principal** | `#FFFFFF` | `(255, 255, 255)` | Áreas de agrupamento de controles de áudio |
| **Painel Elevado** | `#F8FAFC` | `(248, 250, 252)` | Destaque para displays numéricos ou modais |
| **Top Bar** | `#FFFFFF` | `(255, 255, 255)` | Barra superior (preset browser, logo) |
| **Borda Sutil** | `#E2E8F0` | `(226, 232, 240)` | Contornos discretos e linhas divisórias |
| **Texto Principal** | `#0F172A` | `(15, 23, 42)` | Nomes de parâmetros, títulos principais |
| **Texto Secundário** | `#475569` | `(71, 85, 105)` | Unidades de medida, subrótulos de status |
| **Texto Desativado** | `#94A3B8` | `(148, 163, 184)` | Parâmetros bypassados ou indisponíveis |
| **Acento Principal** | `#0891B2` | `(8, 145, 178)` | Knob ativo, indicadores de seleção principal |
| **Acento Secundário** | `#0D9488` | `(13, 148, 136)` | Botões ativos, seletores A/B secundários |
| **Glow Ativo** | `#06B6D4` (15% opacity) | `(6, 182, 212)` | Aura de foco luminoso sob knobs e seletores |
| **Meter Safe** | `#10B981` | `(16, 185, 129)` | Níveis saudáveis de sinal (< -12 dB) |
| **Meter Warning** | `#F59E0B` | `(245, 158, 11)` | Faixa de atenção do sinal (-12 a -2 dB) |
| **Meter Clip** | `#EF4444` | `(239, 68, 68)` | Sobrecarga / Clipping digital (> -2 dB) |
| **Bypass/off** | `#E2E8F0` | `(226, 232, 240)` | Controles desativados, estado offline |
| **Sombra Suave** | `#0F172A` (6% opacity) | `(15, 23, 42)` | Sombras projetadas de painéis (`shadowSoft`) |
| **Highlight** | `#FFFFFF` (60% opacity) | `(255, 255, 255)` | Brilho tridimensional nas bordas superiores |

---

## 2. Dark Mode Palette

| Elemento Visual | Código Hex | Valor RGB (R, G, B) | Aplicação e Significado |
| :--- | :--- | :--- | :--- |
| **Background Principal** | `#0B0F12` | `(11, 15, 18)` | Base escura da janela do plugin |
| **Background Secundário** | `#151D24` | `(21, 29, 36)` | Fundo dos painéis internos de parâmetros |
| **Painel Principal** | `#151D24` | `(21, 29, 36)` | Áreas de agrupamento de controles de áudio |
| **Painel Elevado** | `#1E2933` | `(30, 41, 51)` | Destaque para displays numéricos ou modais |
| **Top Bar** | `#0B0F12` | `(11, 15, 18)` | Barra superior (preset browser, logo) |
| **Borda Sutil** | `#1C2833` | `(28, 40, 51)` | Contornos discretos e linhas divisórias |
| **Texto Principal** | `#F8FAFC` | `(248, 250, 252)` | Nomes de parâmetros, títulos principais |
| **Texto Secundário** | `#94A3B8` | `(148, 163, 184)` | Unidades de medida, subrótulos de status |
| **Texto Desativado** | `#475569` | `(71, 85, 105)` | Parâmetros bypassados ou indisponíveis |
| **Acento Principal** | `#22D3EE` | `(34, 211, 238)` | Knob ativo, indicadores de seleção principal |
| **Acento Secundário** | `#2DD4BF` | `(45, 212, 191)` | Botões ativos, seletores A/B secundários |
| **Glow Ativo** | `#22D3EE` (35% opacity) | `(34, 211, 238)` | Aura de foco luminoso sob knobs e seletores |
| **Meter Safe** | `#34D399` | `(52, 211, 153)` | Níveis saudáveis de sinal (< -12 dB) |
| **Meter Warning** | `#FBBF24` | `(251, 191, 36)` | Faixa de atenção do sinal (-12 a -2 dB) |
| **Meter Clip** | `#F87171` | `(248, 113, 113)` | Sobrecarga / Clipping digital (> -2 dB) |
| **Bypass/off** | `#1E2933` | `(30, 41, 51)` | Controles desativados, estado offline |
| **Sombra Suave** | `#000000` (45% opacity) | `(0, 0, 0)` | Sombras projetadas de painéis (`shadowSoft`) |
| **Highlight** | `#FFFFFF` (10% opacity) | `(255, 255, 255)` | Brilho tridimensional nas bordas superiores |

---

## 3. Aplicação do Contraste e Acessibilidade

Para garantir uma interface acessível e legível em ambos os temas:
*   **Controles Críticos**: O contraste entre o texto do valor atual (ex: `-3.5 dB`) e a cor do fundo do painel deve exceder `4.5:1` em ambos os modos.
*   **Bypass**: Ao acionar o botão global bypass do plugin, os painéis que representam os processadores inativos entram no estado de **Opacidade Reduzida (40%)**, alterando os rótulos de texto de `Texto Principal` para `Texto Desativado` e enfraquecendo os glows ativos, o que garante que o usuário saiba imediatamente que o processador não está afetando o sinal de áudio.
