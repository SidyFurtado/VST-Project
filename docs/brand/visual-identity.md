# Identidade Visual da Marca

Este documento estabelece as diretrizes de marca e a personalidade visual da linha de plugins da **ASTRA Audio Suite** (desenvolvida dentro do monorepo **VST - Project**). Ele serve de base para guiar o design de interface de todos os processadores da coleção (AUREQ, GRAVITY COMP, NEBULA SAT, EVENT LIMITER, ORBIT IMAGER, LUMEN EXCITER e PULSAR GATE), garantindo que o usuário reconheça imediatamente a assinatura de qualidade profissional do projeto.

---

## 1. Personalidade Visual da Marca

Os plugins da **ASTRA Audio Suite** não buscam emular hardware vintage enferrujado, válvulas brilhantes falsas ou parafusos metálicos 3D na tela. A nossa identidade é focada na era moderna, digital e técnica. A marca comunica:
*   **Precisão**: Ferramentas cirúrgicas para engenheiros de áudio exigentes.
*   **Clareza**: Interfaces sem distrações desnecessárias; foco total na visualização de dados.
*   **Leveza**: Layouts limpos, arejados e respiráveis (estilo *Fresh Air*).
*   **Confiança**: Interface robusta, previsível e estável que inspira segurança.
*   **Controle**: Ergonomia refinada para ajustes finos sem complexidade excessiva.
*   **Qualidade Profissional**: Acabamento estético de primeira linha com micro-detalhes premium.

---

## 2. Princípios de Design

*   **Espaço Negativo Generoso**: O respiro visual é tão importante quanto o processamento de áudio. Seções e botões devem possuir margens amplas para evitar poluição visual e sensação de aperto na tela.
*   **Minimalismo Funcional**: Reduzir a quantidade de controles exibidos simultaneamente. Se um parâmetro puder ser automatizado ou ajustado de forma inteligente nos bastidores pelo DSP (sem comprometer a criatividade), evite colocar um knob extra na tela.
*   **Hierarquia de Destaque**: O principal elemento do plugin (por exemplo, a curva de EQ no AUREQ ou o medidor de Gain Reduction no GRAVITY COMP) deve comandar a atenção central do layout, com tamanho maior e cores atrativas.
*   **Feedback Visual Imediato**: O usuário deve saber o que está acontecendo com o áudio apenas olhando para a interface, através de meters precisos e glows discretos.

---

## 3. Suporte Obrigatório a Temas (Light e Dark Mode)

Todos os plugins da marca devem suportar nativamente e de forma robusta o **Light Mode** e o **Dark Mode**. O usuário poderá selecionar o tema de sua preferência no menu de configurações do cabeçalho.

### O que deve permanecer IGUAL entre os temas:
*   **Layout Físico**: A posição absoluta dos botões, knobs, barras de sinal e displays de valores nunca muda ao alternar de tema.
*   **Espaçamento e Grid**: Todas as margens, preenchimentos (`padding`) e lacunas (`gap`) baseadas em tokens permanecem idênticos.
*   **Tipografia**: As famílias de fontes (`Inter`, `Roboto Mono`), seus tamanhos e seus respectivos pesos (`Bold`, `Regular`) não se alteram.
*   **Componentes e Elementos**: A lógica estrutural dos componentes (ex: o fato de um knob possuir um arco circular de preenchimento) é a mesma.

### O que PODE MUDAR entre os temas:
*   **Cores de Superfície**: Fundos e painéis alteram drasticamente suas matizes (branco/azul-claro gelo no Light vs grafite/azul-petróleo escuro no Dark).
*   **Cores de Texto**: Rótulos e valores invertem de escuro (Slate 900) para claro (Slate 50).
*   **Intensidade das Sombras**: O Light Mode usa sombras projetadas muito suaves e sutis (com opacidade baixa). O Dark Mode usa sombras com maior opacidade para criar contraste sobre superfícies profundas.
*   **Intensidade de Glows**: O Dark Mode faz uso mais evidente de halos de luz brilhantes sob acentos ativos (ex: Cyan neon) para destacar seletores. No Light Mode, estes glows são atenuados para evitar borrões na tela clara.

---

## 4. Consistência na Coleção de Plugins

Para garantir que a linha inteira de plugins pareça fazer parte do mesmo ecossistema:
1.  **Cabeçalho Padronizado**: Todos os plugins compartilham do mesmo topo (Logo da ASTRA Audio Suite alinhado à esquerda, seletor de Presets no centro, botão A/B e alternador de tema à direita).
2.  **Mesmo Design de Knobs e Botões**: O mesmo estilo de knobs arredondados e botões minimalistas deve ser importado em todos os layouts.
3.  **Comportamento dos Medidores**: A escala e a velocidade de resposta de meters de entrada, saída e redução de ganho devem ter a mesma assinatura visual (decaimento, tempo de pico e cores).

---

## 5. Regras para Evitar Poluição Visual

*   ❌ **Sem Knobs em excesso**: Não crie mais do que 6 knobs principais por tela (exceto no AUREQ que usa painel flutuante de controle dinâmico de banda para evitar acúmulo de knobs).
*   ❌ **Sem Skewmorfismo Pesado**: Não imitar texturas de couro, madeira ou metal texturizado.
*   ❌ **Sem Fontes Decorativas**: Usar apenas a fonte limpa e técnica definida no sistema de tokens.
*   ❌ **Sem Efeitos de Pisca-Pisca**: Não usar animações dinâmicas excessivas que possam distrair o usuário durante o trabalho de mixagem.
