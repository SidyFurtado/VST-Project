# Regras e Fluxo de Trabalho do Projeto

Para garantir um desenvolvimento robusto, livre de bugs e alinhado com as melhores práticas de DSP e UI de áudio, o ciclo de vida deste monorepo foi estruturado em **7 fases consecutivas**. Cada fase deve ser concluída e validada antes de avançar para a próxima.

## Fases de Desenvolvimento

### Fase 1: Estrutura Inicial do Monorepo
*   **Objetivo**: Organizar a árvore de diretórios raiz, os diretórios específicos de cada plugin e os arquivos base de configuração/documentação.
*   **Regras**:
    *   Nenhum código funcional C++/DSP ou UI deve ser escrito.
    *   Nenhuma dependência externa deve ser instalada.
    *   A pasta `Livros/` contendo os PDFs originais de referência deve permanecer intocada.

### Fase 2: Leitura e Resumo Técnico dos Livros
*   **Objetivo**: Estudo aprofundado dos materiais de DSP localizados na pasta `Livros/`.
*   **Regras**:
    *   Criação de notas técnicas detalhadas em `docs/dsp-notes/`.
    *   Mapeamento de equações de filtros digitais, curvas de compressores e algoritmos de FFT baseados na teoria dos livros.

### Fase 3: Especificação do Design System
*   **Objetivo**: Mapeamento completo da interface visual e criação de assets gráficos.
*   **Regras**:
    *   Definir cores, tipografias, layouts de knobs, meters, botões e mockups visuais nas subpastas correspondentes de `design-system/`.
    *   Criar assets (ícones, logos, texturas) em `assets/`.

### Fase 4: Setup do Ambiente (JUCE/CMake) e Planejamento Flagship
*   **Objetivo**: Configuração da pipeline de compilação dos plugins e especificação detalhada da linha comercial.
*   **Regras**:
    *   Configuração do CMake para gerenciar o monorepo C++ e integração do JUCE Framework.
    *   Criação do esqueleto de compilação comum a fim de gerar binários VST3, AU e Standalone.
    *   Fase 4.2: Transformar a especificação do plugin `01_EQ` para um equalizador flagship denominado **AUREQ**, parte da **ASTRA Audio Suite**.

### Fase 5: Desenvolvimento do Primeiro Plugin (01_EQ - AUREQ)
*   **Objetivo**: Implementação do equalizador flagship AUREQ de 8 a 24 bandas.
*   **Regras**:
    *   O código de DSP deve ser escrito de forma isolada do código da GUI.
    *   Focar no processamento sample-by-sample estável e eficiente.

### Fase 6: Testes, Preset-Tools e Validação
*   **Objetivo**: Garantir que o plugin de equalização esteja estável e calibrado.
*   **Regras**:
    *   Criação de testes unitários para o motor de DSP (`tests/dsp/`).
    *   Validação em DAWs hosts e desenvolvimento de presets básicos (`shared/presets/`).

### Fase 7: Próximos Plugins (Gravity Comp, Nebula Sat, Event Limiter, etc.)
*   **Objetivo**: Iniciar o desenvolvimento sequencial dos demais plugins (`02_Compressor` - GRAVITY COMP, `03_Saturation` - NEBULA SAT, `04_Limiter` - EVENT LIMITER, etc.).
*   **Regras**:
    *   Aproveitar as classes base e o design system criados nos plugins anteriores.
    *   Trabalhar em apenas um plugin por vez.

---

## Regras Visuais Obrigatórias

1. **Uso Estrito de Design Tokens**: Todos os plugins devem consumir as especificações visuais de cores, tipografias, espaçamentos, raios e sombras exclusivamente por meio do sistema de tokens definido em `design-system/tokens/theme-tokens.md`.
2. **Suporte Obrigatório a Temas**: Todos os plugins desenvolvidos no monorepo devem suportar nativamente e de forma idêntica os modos **Light Mode** e **Dark Mode**.
3. **Reaproveitamento de Componentes Visuais**: Todo componente de UI (ex: knobs, botões, meters) deve ser desenhado a partir de classes base compartilhadas em `shared/gui/`, evitando reescrever código de desenho gráfico individualmente para cada plugin.
4. **Documentação Prévia de Componentes**: Qualquer novo componente de interface deve ter sua especificação visual documentada e incorporada ao design system antes de iniciada sua implementação em código C++/JUCE.
5. **Não Criar UI Isolada Sem Justificativa**: A menos que um plugin exija um controle extremamente específico não coberto pelo sistema de design, nenhum comportamento visual proprietário e desalinhado com a coleção deve ser implementado.
6. **Validação Estética Prévia**: O design visual de novos painéis e arranjos de knobs deve ser validado conceitualmente antes da codificação da interface gráfica.
7. **Evitar Excesso de Controles**: Restringir controles visíveis simultâneos para evitar sobrecarga de informação (poluição visual). AUREQ utiliza painel flutuante de controle dinâmico da banda selecionada para otimização espacial.
8. **Priorizar Respiro e Espaço Negativo**: Manter margens generosas e vácuo visual em pelo menos 35% da tela para assegurar leveza e hierarquia limpa.
9. **Preservação de Tema**: A escolha de tema do usuário (Light/Dark) deve ser salva no estado interno do plugin e preservada entre sessões da DAW e novas instâncias do processador.

---

## Regras Técnicas Obrigatórias

1. **Separação Estrita entre DSP e UI**: Todo o código de processamento matemático de áudio (filtros, DSP) deve ser implementado de forma independente e isolada da interface gráfica (UI). O DSP não deve realizar chamadas para componentes de UI ou gerenciar dados gráficos.
2. **Uso de Componentes Compartilhados**: Classes de DSP comuns (como suavização, filtros base, oversamplers) e de UI comuns (knobs customizados, meters, classes de temas) devem ser centralizadas e mantidas no diretório `shared/` a fim de serem reaproveitadas por múltiplos plugins.
3. **Desenvolvimento Sequencial (Primeiro EQ)**: O primeiro plugin da coleção a ser implementado será o `01_EQ` (AUREQ). O desenvolvimento de novos plugins (como o compressor GRAVITY COMP ou o saturador NEBULA SAT) só deve ser iniciado após o equalizador possuir uma base de código estável, testada e validada.
4. **Governança do Design System**: A implementação visual dos elementos gráficos (cores, fontes, sombras, cantos) deve seguir estritamente as especificações e tokens definidos no Design System.
5. **Consideração de Temas no Início**: O suporte a Light Mode e Dark Mode deve ser planejado e testado desde as etapas iniciais de criação da UI de qualquer plugin, evitando reestruturações tardias de cores.
6. **Organização do Monorepo via CMake**: O CMakeLists.txt raiz e os arquivos de compilação locais das pastas `shared/` e `plugins/` devem ser mantidos limpos e integrados para que todo o monorepo compile a partir de um único ponto.
7. **Isolamento de Diretórios**: Nenhuma pasta externa, arquivos de código temporários ou bibliotecas devem ser adicionados fora dos limites do diretório principal `VST - Project`.
8. **Gestão de Dependências Externas**: Todas as dependências externas e frameworks (como o JUCE) devem ser mantidos e configurados dentro do diretório `external/` ou referenciados no sistema local por meio de caminhos documentados no CMake.
9. **Fases Pequenas e Compiláveis**: Todo recurso novo deve ser implementado em fases pequenas e compiláveis.

---

## Regra Estratégica do Primeiro Plugin

1. **Ambição Flagship**: O primeiro plugin (`01_EQ` - **AUREQ**) não deve ser tratado como um equalizador simples ou básico. Ele deve ser desenvolvido como um produto de ponta, capaz de competir em precisão, workflow e recursos visuais/sonoros com os maiores padrões de excelência do mercado.
2. **Desenvolvimento Incremental**: Para evitar a sobrecarga de tentar implementar todos os recursos inovadores de uma vez e travar o progresso, o desenvolvimento deve ocorrer estritamente por fases pré-definidas (começando pelo MVP funcional 0.1 e evoluindo para equalização dinâmica, processamento spectral e linear phase).
3. **Estabilidade em Primeiro Lugar**: Nenhum recurso avançado (como processamento de fase linear ou equalizador dinâmico) deve ser codificado antes que as funções essenciais (filtros IIR estáveis, suavização de arrasto, interface gráfica reativa com tema Light/Dark e o analisador FFT básico) estejam 100% livres de bugs e testadas contra NaNs/denormais.
4. **Originalidade e Identidade**: Não se deve replicar a interface, o layout, a marca ou o código de plugins proprietários concorrentes (como FabFilter Pro-Q). O benchmark deve servir apenas como padrão de qualidade técnica e flexibilidade funcional, mantendo a originalidade visual centrada no Design System da ASTRA Audio Suite.


