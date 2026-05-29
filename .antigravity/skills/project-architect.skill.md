# Skill: Arquitetura de Projeto (Project Architect)

Esta skill é focada na governança estrutural, na organização de diretórios e no fluxo de desenvolvimento incremental e limpo do monorepo **ASTRA Audio Suite** no **VST - Project**.

---

## 1. Responsabilidades Principais
- **Organização do Repositório**: Garantir que novos arquivos sejam alocados estritamente dentro da estrutura conceitual planejada do monorepo.
- **Isolamento de Diretórios**: Evitar a poluição da raiz do projeto com arquivos e pastas temporários de builds ou logs pessoais.
- **Divisão de Responsabilidades**: Garantir a estrita separação física e lógica entre os componentes principais:
  - `shared/`: Bibliotecas de código reutilizáveis por múltiplos plugins.
  - `plugins/`: Diretório contendo os alvos individuais de plugins (ex: `01_EQ`, `02_Compressor`).
  - `docs/`: Documentos de arquitetura e notas técnicas.
  - `design-system/`: Tokens de temas, mockups e assets conceituais.
  - `assets/`: Imagens, logotipos e fontes embutidos nos plugins.
  - `tests/`: Suíte de testes unitários e de integração de DSP.
  - `external/`: Bibliotecas e frameworks de terceiros (ex: `external/JUCE`).
- **Desenvolvimento Sequencial**: Garantir que o trabalho de desenvolvimento aconteça em um plugin por vez.
- **Integridade da Pasta Livros**: Preservar sob qualquer circunstância a pasta original `Livros/` com toda sua documentação matemática e PDFs de referência de DSP.

---

## 2. Regras Operacionais
- **Planejamento Obrigatório**: Antes de qualquer grande mudança arquitetural ou técnica, crie e apresente um plano de implementação curto (`implementation_plan.md`) para validação humana.
- **Relatório de Modificações**: Ao finalizar qualquer atividade, liste de forma clara os comandos executados, arquivos criados e arquivos modificados.
- **Separação de DSP e UI**: Nunca misture lógica matemática de processamento de áudio (DSP) com renderização de tela (UI).
- **Sem Paralelismo de Plugins**: Não inicie a infraestrutura de um novo plugin da suíte (como o compressor GRAVITY COMP) até que o plugin flagship atual (AUREQ) esteja estável e com build completo validado.
- **Evitar Duplicidade**: Reaproveite ao máximo classes utilitárias, smoothed parameters e assets gráficos centralizando-os em `shared/` em vez de duplicar lógica entre diretórios de plugins.
