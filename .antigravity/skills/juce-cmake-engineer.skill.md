# Skill: Engenharia de JUCE e CMake (JUCE & CMake Engineer)

Esta skill é voltada para a gestão do sistema de build (CMake), integração de bibliotecas do framework JUCE e estruturação dos alvos binários do monorepo **VST - Project**.

---

## 1. Responsabilidades Principais
- **CMake Incremental e Organizado**: Escrever diretivas CMake claras, declarativas e modulares, divididas coerentemente entre os escopos raiz, `shared/` e diretórios específicos de plugins.
- **Build-Driven Development**: Garantir que as alterações de compilação sejam testadas passo a passo, assegurando que o projeto compile em sua forma mínima antes de acrescentar complexidade lógica.
- **Gerenciamento de Alvos**: Controlar com precisão os alvos do CMake:
  - `shared_code`: Biblioteca estática que agrupa utilitários comuns.
  - `AUREQ`: Plugin principal do equalizador flagship.
- **Preservação Funcional**: Evitar reescrever código de build ou redefinir propriedades de compilador já validadas sem necessidade documentada.

---

## 2. Regras Operacionais
- **Compilação Contínua**: O build do plugin e do código compartilhado deve ser testado e aprovado com sucesso antes de avançar para novas fases de implementação.
- **Isolamento de Plugin**: Configurar as definições de inclusão e linkedição do CMake para que o desenvolvimento de um plugin não interfira na compilação ou na visibilidade de diretórios de outros plugins.
- **Foco de Formato**: Manter apenas o formato **VST3** e Standalone ativados durante o desenvolvimento e a depuração. Não ative alvos adicionais como AU ou AAX, nem configure geradores de instaladores nas fases de construção do MVP.
- **Sem Grandes Refatorações no Build**: Durante a depuração de erros de compilação ou linker, evite realizar grandes modificações arquiteturais no código C++. Isole o problema no CMake ou corrija cabeçalhos faltantes com precisão cirúrgica.
- **Transparência em Falhas**: Registrar em logs e relatórios todos os comandos exatos de build que foram tentados, capturando os erros completos do compilador para permitir diagnósticos claros.
