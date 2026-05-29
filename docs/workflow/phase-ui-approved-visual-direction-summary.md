# Resumo de Validação Técnica: Direção Visual Aprovada (AUREQ)

Este documento registra formalmente a direção visual oficial e aprovada do equalizador **AUREQ**, parte da **ASTRA Audio Suite**, estabelecendo a linguagem estética e a estrutura de layout que devem ser preservadas nas próximas fases de desenvolvimento.

---

## 1. Linguagem Estética Oficial

O AUREQ segue uma identidade visual premium comercial caracterizada pelo conceito **Premium Glass & Cosmic Navy**:
- **Paleta de Cores**: Fundo com degradê espacial profundo baseado em azul-navy cósmico e antracite leve (`backgroundTop`, `backgroundMiddle`, `backgroundBase`), evitando fundos pretos chapados ou cinzas sem saturação.
- **Transparência e Reflexos (Glassmorphism)**: Painéis com opacidade entre `0.22f` e `0.35f` que revelam auroras coloridas em gradiente ciano/mint ao fundo. Os painéis usam chanfros de bisel duplo e linhas superiores de refração da luz em branco translúcido.
- **Acentos e Iluminação**: Uso de Cyan (`accent`) e Mint (`accentSoft`) com dispersão suave de luz (bloom ótico) para realçar elementos focados, a curva de EQ e o status de controles ativos.

---

## 2. Layout Base Aprovado

A estrutura da interface é dividida em seções bem espaçadas que mantêm as dimensões fixas de **900 x 560** pixels:

1. **Header Premium (Top Bar)**:
   - Identidade visual ASTRA/AUREQ com tipografia elegante.
   - Preset Browser integrado com setas cíclicas e visor central.
   - Botões utilitários dispostos como cápsulas de vidro (*glass pills*) com LEDs e texto em Inter Bold.
2. **Graph Card (Hero Display)**:
   - Curva teórica de magnitude com bloom de triplo glow ótico.
   - Grade logarítmica de frequência e linear de ganho com linhas discretas a `0.04f` de opacidade.
   - Handles de banda em 3D esférico ciano com auras de Q e halos de foco.
   - Floating HUD com status de bypass (LED) e valores textuais.
3. **Bottom Control Panel (Painel Inferior)**:
   - Alinhamento vertical rigoroso para Knobs (`Y = bottomY + 30`, `height = 72`) e Toggles (`Y = bottomY + 54`, `height = 24`).
   - Knobs customizados premium com aro externo protetor, Dial 3D com destaque esférico superior, indicador glowing de valor e value text em `Roboto Mono`.
   - Card interno translúcido (`selectedBandCard`) para isolar visualmente os controles de Frequência, Ganho e Q da banda focada.

---

## 3. Diretrizes para Próximas Fases

- **Preservação da Identidade**: Futuras evoluções do plugin (como a adição de analisador FFT, novos recursos de workflow, Undo/Redo ou presets) devem obrigatoriamente manter a paleta de cores, opacidade dos painéis e design LookAndFeel dos controles.
- **Refinamentos, não Redesign**: Alterações visuais futuras devem ser tratadas como refinamentos de detalhes (polimento de cores, tempos de animação, suavidade gráfica) e nunca como um redesenho de layout ou mudança estrutural do esquema aprovado.
