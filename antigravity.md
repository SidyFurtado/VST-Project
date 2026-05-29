# Instruções para o Agente Antigravity

Este arquivo contém instruções de comportamento e desenvolvimento permanentes que o agente Antigravity deve seguir estritamente ao trabalhar neste monorepo.

## Regras de Operação

1.  **Escopo de Trabalho**: Trabalhe sempre dentro da pasta raiz `VST - Project`. Não crie, mova ou modifique arquivos fora dela.
2.  **Desenvolvimento Incremental**: Trabalhe em apenas um plugin por vez (ex: finalize o `01_EQ` antes de iniciar o `02_Compressor`).
3.  **Identidade Visual Compartilhada**: Todos os plugins devem adotar a mesma identidade visual estabelecida em `docs/brand/visual-identity.md`.
4.  **Reutilização de Código**: Coloque todos os componentes, classes e funções que possam ser compartilhados entre múltiplos plugins dentro do diretório `shared/`.
5.  **Design System & Assets**: Coloque os elementos visuais comuns (design system, mockups, esquemas de knobs) em `design-system/` e recursos estáticos (fontes, texturas, ícones) em `assets/`.
6.  **Separação de Preocupações (SoC)**: Nunca misture código DSP (processamento de áudio) com código de interface de usuário (UI/GUI). Mantenha-os estritamente separados em suas respectivas pastas (`DSP/` e `UI/`).
7.  **Preservação de Referências**: A pasta `Livros/` e seus respectivos arquivos em PDF de DSP/Áudio devem ser preservados exatamente como estão. Não renomeie, não mova e não apague esta pasta ou seus arquivos.
8.  **Consulta Obrigatória**: Sempre que for implementar algoritmos de DSP, filtros, análise espectral, dinâmica, equalização, compressão, saturação, limitação ou qualquer processamento de áudio, consulte primeiro os materiais de referência presentes na pasta de livros do projeto (`Livros/`).
9.  **Planejamento Prévio**: Apresente sempre um plano detalhado (usando a ferramenta de planejamento se necessário) antes de realizar grandes mudanças estruturais ou de código.
10. **Transparência**: Ao final de cada interação/tarefa concluída, apresente uma lista resumida contendo todos os arquivos que foram alterados ou criados.

## Skills Internas do Projeto

Antes de iniciar qualquer atividade complexa ou atuar em áreas específicas do monorepo, você **deve** consultar o índice de competências localizado em [.antigravity/skills/skill-index.md](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/skill-index.md) e aplicar a skill adequada ao tipo de tarefa correspondente.

> [!IMPORTANT]
> É obrigatório consultar as regras da skill [Guardião de Segurança](file:///Users/sidyziin/Documents/VST%20-%20Project/.antigravity/skills/safety-guardian.skill.md) (`safety-guardian.skill.md`) antes de executar comandos de terminal ou realizar quaisquer operações sensíveis de arquivos (como exclusões ou edições em lote).
