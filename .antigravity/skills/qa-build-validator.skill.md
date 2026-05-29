# Skill: Garantia de Qualidade e Validação (QA & Build Validator)

Esta skill orienta os processos de validação técnica, testes estruturais e garantia de integridade do monorepo **VST - Project**.

---

## 1. Responsabilidades Principais
- **Validação de Build**: Garantir que as alterações propostas não quebrem a compilação do monorepo a partir de comandos CMake padrão.
- **Validação de Diretórios e Escopo**: Analisar de forma minuciosa se novos arquivos foram criados na pasta correta e se não houve vazamento ou modificações fora de escopo (como na pasta `Livros/`).
- **Registro Detalhado de Erros**: Garantir que relatórios técnicos contenham mensagens completas do compilador ou linker quando houver falhas de build.
- **Auditoria de Configuração**: Certificar que o CMake gere os alvos corretos e configure propriedades importantes (como C++17) de maneira uniforme em todo o monorepo.

---

## 2. Regras Operacionais
- **Transparência Absoluta**: Nunca oculte, minimize ou ignore erros de compilação. Registre a mensagem exata, a causa provável e proponha uma correção detalhada no plano.
- **Bloqueio de Regressão**: Se a etapa mínima de compilação ou configuração falhar (como ausência de ferramentas no PATH), não avance para novos desenvolvimentos de recursos até que o ambiente esteja sanado.
- **Balanço entre "Estrutura" e "Build"**: Separar conceitualmente as conquistas em relatórios de progresso:
  - **Estrutura Criada**: Arquivos de código e CMakeLists declarados e estruturados no repositório.
  - **Build Validado**: O pipeline executado, linkado e os alvos binários finais gerados no disco.
- **Testes Unitários**: Planejar testes de exatidão matemática para filtros IIR e processamento dinâmico em múltiplos sample rates na pasta `tests/`.
