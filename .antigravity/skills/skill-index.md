# Índice de Skills Internas do Antigravity

Este diretório contém os perfis de competências e instruções operacionais recomendadas para agentes de inteligência artificial (como Antigravity) atuando no monorepo **VST - Project** da **ASTRA Audio Suite**. 

Antes de iniciar qualquer tarefa importante no repositório, você deve consultar as instruções específicas da skill aplicável para garantir conformidade e qualidade técnica.

---

## Skills Disponíveis

### 1. [Arquitetura de Projeto](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/project-architect.skill.md) (`project-architect.skill.md`)
- **Quando usar**: Sempre que for necessário criar novos diretórios, planejar novos plugins, organizar a árvore de arquivos, realizar modificações estruturais ou analisar a divisão de dependências entre componentes do monorepo.

### 2. [Engenharia de DSP](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/dsp-engineer.skill.md) (`dsp-engineer.skill.md`)
- **Quando usar**: Durante a fase de codificação de áudio digital, processamento de sinais, criação de filtros IIR/FIR, cálculo de coeficientes FFT, implementação de interpolação/suavização de parâmetros e garantia de estabilidade numérica contra estouros (NaNs/denormais).

### 3. [Engenharia JUCE & CMake](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/juce-cmake-engineer.skill.md) (`juce-cmake-engineer.skill.md`)
- **Quando usar**: Sempre que for necessário alterar arquivos de configuração CMakeLists, configurar o link com bibliotecas do JUCE, depurar problemas de compilação, resolver conflitos de cabeçalhos de plataforma ou configurar novos alvos de plugins.

### 4. [Design System & Interface Gráfica](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/ui-design-system.skill.md) (`ui-design-system.skill.md`)
- **Quando usar**: Durante o planejamento, prototipagem ou implementação da interface gráfica (UI/UX) do plugin, aplicação de cores de tema, alinhamento visual de componentes e uso dos Theme Tokens.

### 5. [Estratégia de Produto](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/plugin-product-strategist.skill.md) (`plugin-product-strategist.skill.md`)
- **Quando usar**: Para o alinhamento de roadmaps, definição de escopo incremental (MVP, v0.2, v1.0), benchmark de concorrência ou mapeamento das especificações comerciais da suíte ASTRA.

### 6. [Garantia de Qualidade & Validação](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/qa-build-validator.skill.md) (`qa-build-validator.skill.md`)
- **Quando usar**: Na fase de testes de regressão, execução de scripts de validação, depuração de erros de compilação pós-build ou escrita de testes unitários para DSP.

### 7. [Guardião de Segurança](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/safety-guardian.skill.md) (`safety-guardian.skill.md`)
- **Quando usar**: **Obrigatório** antes de executar quaisquer comandos de terminal potencialmente sensíveis, exclusões ou modificações em lote no sistema de arquivos, bem como no manuseio de arquivos Markdown externos não-confiáveis.
