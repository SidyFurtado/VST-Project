# Skill: Design System e Interface Gráfica (UI & Design System)

Esta skill orienta a concepção estética, a experiência de uso (UX) e a codificação da interface gráfica (UI) de toda a suíte de efeitos **ASTRA**.

---

## 1. Responsabilidades Principais
- **Fidelidade aos Tokens de Design**: Consumir rigorosamente as tabelas de cores, fontes, raios, sombras e espaçamentos definidas no diretório `design-system/tokens/`.
- **Suporte Nativo a Temas**: Garantir a coexistência idêntica e suave dos modos **Light Mode** e **Dark Mode** em toda a interface.
- **Aestética Premium e Moderna**: Adotar visual limpo, espaçado e elegante baseado em astronomia (com cores profundas, gradientes orbitais sutis e pequenos brilhos discretos/glow em pontos de atenção).
- **Legibilidade e Organização**: Priorizar hierarquia tipográfica forte, mantendo pelo menos 35% da tela em espaço negativo (respiro visual) para prevenir fadiga cognitiva.
- **Evitar Poluição Visual**: Restringir knobs vintage realistas e texturas de madeira ou metal rústico. A estética deve ser minimalista, plana (flat) com gradientes modernos e micro-interações fluidas.

---

## 2. Regras Operacionais
- **Uso de Componentes Compartilhados**: Centralizar classes customizadas de componentes gráficos (como knobs de controle, sliders, botões e meters) em `shared/gui/`. Não repita lógica de desenho específica em diretórios isolados de plugins para elementos que podem ser reaproveitados.
- **Identidade Própria**: Não copie o design e a disposição física de concorrentes consagrados (como o FabFilter Pro-Q). Use referências apenas como benchmark de qualidade técnica e fluxos de uso.
- **Theme Tokens como Fonte de Verdade**: As cores, bordas e glows em código JUCE/C++ devem estar diretamente mapeadas às definições constantes da classe de controle de tema, que consome as definições do Design System.
- **Consistência na Coleção**: Garantir que as interfaces dos plugins seguintes sigam exatamente a mesma linguagem de design estabelecida para o **AUREQ**, promovendo a integridade de marca da **ASTRA Audio Suite** que engloba:
  - `AUREQ` (Flagship modern parametric equalizer)
  - `GRAVITY COMP` (Modern compressor)
  - `NEBULA SAT` (Advanced saturation module)
  - `EVENT LIMITER` (Peak limiter)
  - `ORBIT IMAGER` (Spatial processor)
  - `LUMEN EXCITER` (Harmonic exciter)
  - `PULSAR GATE` (Dynamic gate)
- **Micro-interações Reativas**: Incorporar transições suaves de hover, feedback visual durante o arraste e indicação clara de estados ativados/desativados.
