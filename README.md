# VST - Project

Este é o repositório principal (monorepo) dedicado ao desenvolvimento de múltiplos plugins VST de mixagem e masterização com uma identidade visual premium e unificada.

A arquitetura do projeto foi planejada para permitir o desenvolvimento modular de cada plugin individualmente, compartilhando componentes e utilitários comuns a fim de manter consistência e eficiência.

## Estrutura do Projeto

*   **`docs/`**: Documentação de arquitetura, identidade visual (`brand`), guias de workflow e referências técnicas.
*   **`design-system/`**: Componentes e padrões visuais compartilhados (cores, tipografia, botões, knobs, meters, etc.).
*   **`shared/`**: Código compartilhado entre os plugins, incluindo utilitários de DSP, gerenciamento de estado, presets e componentes de GUI.
*   **`plugins/`**: Diretórios contendo cada plugin individual do monorepo:
    *   `01_EQ`: Equalizador paramétrico.
    *   `02_Compressor`: Compressor dinâmico.
    *   `03_Saturation`: Plugin de saturação harmônica.
    *   `04_Limiter`: Limitador de pico (Limiter).
*   **`assets/`**: Recursos de design estáticos como fontes, ícones, logotipos e texturas.
*   **`tests/`**: Testes globais e de validação dos plugins.
*   **`builds/`**: Artefatos gerados pelo processo de compilação.
*   **`tools/`**: Scripts de suporte e exportação.

## Pasta de Referência Técnica (Livros)

O monorepo conta com uma pasta dedicada `Livros/` contendo livros e PDFs de referência fundamentais sobre Processamento Digital de Sinais (DSP), áudio digital, filtros, dinâmica e desenvolvimento de plugins. Estes materiais são cruciais para guiar a implementação de qualquer algoritmo de DSP neste projeto.

---

*Nota: Este projeto está em fase inicial de estruturação organizacional.*
