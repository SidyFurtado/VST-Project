# Plano de Implementação: AUREQ

Este documento estabelece o roadmap de desenvolvimento técnico dividido em fases consecutivas para a construção do **AUREQ**, o equalizador paramétrico flagship da **ASTRA Audio Suite** no monorepo **VST - Project**.

> [!IMPORTANT]
> A direção visual premium atual foi aprovada e deve ser preservada nas próximas fases.

---

## Fases de Implementação Detalhadas

### Fase 5.0: Integrar JUCE e Validar Build [CONCLUÍDA]
*   **Objetivo**: Garantir que o pipeline de compilação via CMake + JUCE esteja operacional e que os alvos sejam gerados corretamente com o menor esqueleto funcional de plugin.
*   **Ações**:
    *   [x] Configurar o link de compilação no CMakeLists.txt para incluir o framework JUCE e os módulos necessários (`juce_audio_processors`, `juce_audio_utils`, `juce_dsp`, `juce_gui_basics`).
    *   [x] Criar arquivos de cabeçalho e código para `PluginProcessor` e `PluginEditor`.
    *   [x] Configurar o bloco de áudio principal `processBlock` para operar em modo pass-through (receber buffers e encaminhá-los para a saída sem alterações).
    *   [x] Executar compilação inicial e validar que o monorepo gera os alvos Standalone e VST3 vazios sem erros de linker.
    *   [x] Registrar a versão 0.1 do plugin no CMake.

### Fase 5.1: Criar APVTS e Mapeamento Inicial de Parâmetros [CONCLUÍDA]
*   **Objetivo**: Mapear toda a árvore de parâmetros e garantir salvamento e carregamento de estado estáveis.
*   **Ações**:
    *   [x] Instanciar a classe `juce::AudioProcessorValueTreeState` (APVTS) no `PluginProcessor`.
    *   [x] Declarar os parâmetros globais (`Input Gain`, `Output Gain`, `Bypass Global`, `Theme Mode`) e os parâmetros específicos das 8 bandas iniciais (Frequência, Ganho, Q, Tipo e Bypass por banda, prefixados por ID).
    *   [x] Implementar a serialização XML em `getStateInformation` e `setStateInformation` para persistência na DAW.
    *   [x] Atualizar a UI temporária para exibir "APVTS Ready" e mostrar dinamicamente o número total de parâmetros.

### Fase 5.2: Parameter Access Layer [CONCLUÍDA]
*   **Objetivo**: Criar uma camada centralizada para IDs, nomes, defaults e acesso seguro aos parâmetros do AUREQ.
*   **Ações**:
    *   [x] Criar arquivos de cabeçalho e implementação para `ParameterIDs`.
    *   [x] Integrar `ParameterIDs` no CMakeLists.txt do plugin.
    *   [x] Atualizar a inicialização do layout de parâmetros do APVTS para utilizar funções utilitárias em `AUREQ::Params`.
    *   [x] Garantir preservação de compatibilidade dos IDs antigos e manutenção do total de 52 parâmetros.

### Fase 5.3: Criar Motor de Bandas (8 Bandas Iniciais) [CONCLUÍDA]
*   **Objetivo**: Construir a infraestrutura interna de gerenciamento de canais e cascatas de filtros.
*   **Ações**:
    *   [x] Criar a classe de processamento `EQProcessorCore` e o invólucro de banda `EQBand`.
    *   [x] Instanciar buffers internos para processamento separado sample-by-sample nos canais esquerdo e direito.
    *   [x] Garantir alocação estática de memória em `prepareToPlay` baseada na taxa de amostragem informada pelo host.

### Fase 5.4: Gain Stage global seguro [CONCLUÍDA]
*   **Objetivo**: Implementar o primeiro DSP real do AUREQ de forma pequena, segura e testável: Input Gain, Output Gain e Global Bypass.
*   **Ações**:
    *   [x] Implementar Global Bypass em `EQProcessorCore::processBlock` (retorno imediato sem alterar buffer).
    *   [x] Converter dB para linear (`juce::Decibels::decibelsToGain`) e aplicar ganho combinado `totalGain` ao buffer.
    *   [x] Adicionar proteção de segurança numérica contra NaN, infinito e ganhos negativos.

### Fase 5.5: Biquad Foundation + primeira banda Bell [CONCLUÍDA]
*   **Objetivo**: Implementar a fundação matemática de filtros biquad IIR e ativar somente a primeira banda como Bell/Peaking EQ básico.
*   **Ações**:
    *   [x] Implementar a classe `BiquadFilter` com a matemática dos filtros biquad da topologia Transposta Direta II.
    *   [x] Codificar a primeira banda Bell baseando-se no Cookbook RBJ.
    *   [x] Aplicar a proteção de segurança numérica contra NaNs/infinitos e clamps de segurança.
    *   ### Fase 5.6: Parameter Smoothing para a Band 1 [CONCLUÍDA]
*   **Objetivo**: Adicionar suavização de parâmetros para frequência, ganho e Q da Band 1, evitando zipper noise e mudanças bruscas de coeficientes durante automação.
*   **Ações**:
    *   [x] Criar a classe de processamento `ParameterSmoother` com suporte a smoothing exponencial.
    *   [x] Adicionar o método `getNextBlockValue(numSamples)` para avanço de rampa por bloco com complexidade $O(1)$.
    *   [x] Integrar os smoothers de frequência ($30\text{ ms}$), ganho ($20\text{ ms}$) e Q ($20\text{ ms}$) no `EQBand`.
    *   [x] Implementar atualização dos coeficientes do biquad uma vez por bloco de áudio.
    *   [x] Garantir clamps e proteção contra NaN/infinitos nas transições de parâmetros.

### Fase 5.7: Ativar Bell/Peaking EQ nas 8 bandas iniciais [CONCLUÍDA]
*   **Objetivo**: Expandir o processamento Bell/Peaking EQ da Band 1 para todas as 8 bandas em cascata sequencial.
*   **Ações**:
    *   [x] Atualizar `EQProcessorCore::processBlock` para iterar e processar todas as 8 bandas sequencialmente.
    *   [x] Confirmar que cada banda possui seus próprios smoothers de frequência, ganho e Q trabalhando de forma independente.
    *   [x] Validar que o comportamento padrão (Band 1 ativa por padrão, Bands 2–8 desativadas por padrão) e segurança numérica de sinais são preservados.

### Fase 5.8: Criar UI Inicial com Gráfico Vazio e Temas [CONCLUÍDA]
*   **Objetivo**: Construir a casca gráfica e a comutação de temas baseada no Design System.
*   **Ações**:
    *   [x] Configurar o tamanho da janela do editor em `680px x 420px`.
    *   [x] Implementar as divisões do esqueleto visual: Top Bar, Área do Display, Área de Controles e Rodapé.
    *   [x] Integrar o gerenciamento de temas (`ThemeManager`), configurando os componentes para reagir ao token de cor do tema ativo (Light/Dark Mode).
    *   [x] Desenhar a moldura do display gráfico cartesiano vazio com as reguas de frequências (Hz) e amplitude (dB).

### Fase 5.8.1: Refinamento Visual Liquid Glass [CONCLUÍDA]
*   **Objetivo**: Refinar visualmente a UI estática inicial de AUREQ para atingir o visual "liquid glass" premium e a identidade espacial da ASTRA Audio Suite.
*   **Ações**:
    *   [x] Atualizar Theme com cores profundas de "espaço" no Dark Mode e tons de gelo translúcidos no Light Mode.
    *   [x] Criar helpers de desenho de painéis de vidro (`drawGlassPanel`), sombras macias (`drawSoftShadow`) e badges/pills (`drawPill`).
    *   [x] Redesenhar o cabeçalho (Top Bar) como uma placa de vidro flutuante com logo em tracking e badge pill.
    *   [x] Melhorar o EQGraphView com gradiente sutil, simulated inner shadow, brilho aurora de fundo e linha de 0 dB com glow multi-camadas.

### Fase 5.8.2: Redirecionamento Estético Premium [CONCLUÍDA]
*   **Objetivo**: Redirecionar e aprimorar a estética do AUREQ para atingir um visual premium de nível comercial ("produto caro").
*   **Ações**:
    *   [x] Refinar as cores do tema com maior transparência nos painéis e tons de Dark Mode mais ricos.
    *   [x] Implementar fundo com gradiente vertical atmosférico e múltiplas auroras/nebulas espaciais.
    *   [x] Redesenhar o cabeçalho com placa flutuante de vidro, sombra projetada e linha divisória de vidro.
    *   [x] Converter os badges da Top Bar em pills elegantes com LEDs glowing de status embutidos.
    *   [x] Aumentar os cantos do Graph Card para 12px com sombra externa extra macia e chanfro de vidro de dupla camada.
    *   [x] Tornar a grade logarítmica de frequência e linear de ganho muito discreta e elegante.
    *   [x] Intensificar a linha central de 0 dB com glow de 4 camadas simulando laser brilhante.
    *   [x] Simplificar o rodapé removendo elementos técnicos desnecessários para torná-lo editorial.

### Fase 5.9: Renderizar Curva de EQ Teórica [CONCLUÍDA]
*   **Objetivo**: Desenhar no EQGraphView uma curva visual representando a resposta de magnitude combinada das 8 bandas Bell/Peaking ativas, usando os parâmetros lidos do APVTS.
*   **Ações**:
    *   [x] Criar o modelo visual de banda `VisualEQBand` na interface gráfica (UI).
    *   [x] Implementar a passagem das 8 bandas da UI do editor para o `EQGraphView`.
    *   [x] Calcular a curva teórica visual aproximada em escala log2 no `EQGraphView`.
    *   [x] Desenhar a curva no `EQGraphView` utilizando a cor do acento com stroke suave e glow discreto (respeitando Light/Dark Mode).
    *   [x] Implementar atualização periódica automática dos parâmetros via `juce::Timer` no `PluginEditor` a 30 Hz.
    *   [x] Garantir que o DSP e filtros originais não sofram alterações e que não haja qualquer interatividade (drag, handles, etc.) nesta fase.


### Fase 5.10: Handles Visuais Não Interativos [CONCLUÍDA]
*   **Objetivo**: Desenhar os handles visuais das 8 bandas no gráfico do EQ, posicionados por frequência e ganho, mas sem permitir interação.
*   **Ações**:
    *   [x] Desenhar o círculo principal do handle com a cor accent para cada banda habilitada e ativa.
    *   [x] Adicionar anel externo translúcido, glow sutil e sombra suave nos handles das bandas.
    *   [x] Exibir o número correspondente da banda (1-8) centrado no círculo com alto contraste.
    *   [x] Renderizar bandas bypassadas com baixa opacidade e sem glow.
    *   [x] Manter as bandas desativadas invisíveis e as bandas habilitadas não-Bell como pontos altamente discretos.
    *   [x] Garantir que não haja qualquer interatividade por mouse (drag, clicks, wheel) ou hit testing.

### Fase 5.11: Interação Básica dos Handles [CONCLUÍDA]
*   **Objetivo**: Permitir que o usuário arraste os handles das bandas no gráfico para alterar frequência e ganho via APVTS.
*   **Ações**:
    *   [x] Implementar hit-testing com raio de 12px para selecionar handles ativos e Bell.
    *   [x] Capturar gestos de mouseDown, mouseDrag e mouseUp no EQGraphView.
    *   [x] Desenhar a conversão de coordenadas Hz/dB de e para pixels de forma precisa e com clamps.
    *   [x] Criar callbacks de drag do EQGraphView e vinculá-los aos parâmetros no PluginEditor.
    *   [x] Integrar beginChangeGesture(), setValueNotifyingHost() e endChangeGesture() de forma segura e thread-safe.
    *   [x] Intensificar o tamanho e o glow do handle ativamente arrastado para feedback visual.

### Fase 5.12: Controle de Q pelo Mouse Wheel [CONCLUÍDA]
*   **Objetivo**: Permitir que o usuário altere o Q da banda selecionada usando mouse wheel sobre o handle da banda.
*   **Ações**:
    *   [x] Inicializar selectedBandIndex como -1 para evitar alteração acidental precoce na Band 1.
    *   [x] Atualizar mouseDown para salvar selectedBandIndex no clique em handles.
    *   [x] Implementar mouseWheelMove com hit-test de hover (raio de 15px) e fallback para selectedBandIndex.
    *   [x] Aplicar fórmula de escala multiplicativa suave de Q clampada entre 0.1 e 18.0.
    *   [x] Vincular o callback onBandQChanged no PluginEditor para atualizar os parâmetros no APVTS.
    *   [x] Implementar auras de largura de banda e anéis de foco visual para handles selecionados.

### Fase 5.13.1: Redesenho Visual Premium (Aesthetic Refinement) [CONCLUÍDA]
*   **Objetivo**: Redesenhar a interface do AUREQ com efeitos premium de materialidade de vidro, profundidade e sombras macias, sem afetar o comportamento de DSP ou APVTS.
*   **Ações**:
    *   [x] Expandir as cores do tema em `Theme.h`/`Theme.cpp` com suporte a gradiente de fundo em 3 pontos no Dark Mode.
    *   [x] Reduzir a opacidade dos painéis do topo e do gráfico para permitir transparência e efeito glass.
    *   [x] Incrementar a margem do plugin de 20px para 24px para melhor composição e respiro visual.
    *   [x] Atualizar `drawSoftShadow` com translação vertical descendente (simulação de elevação) e decaimento quadrático de opacidade para sombras mais macias.
    *   [x] Atualizar `drawGlassPanel` e a área do gráfico com um traçado de highlight na borda superior (specular light path) e bisel interno duplo.
    *   [x] Mudar a grade logarítmica/linear para linhas extremamente finas (0.5px) com opacidade sutil, simplificando o glow de 0 dB para um traço minimalista de acento.
    *   [x] Refinar a curva de EQ com duas camadas de brilho óptico sob a linha principal.
    *   [x] Redesenhar as alças (handles) das bandas para círculos maiores (8px de raio) com gradiente 3D esférico, ponto reflexivo de luz, e Q-aura tracejada suave.
    *   [x] Redesenhar o painel flutuante (HUD) para tamanho 140x76 com respiro aprimorado, fontes otimizadas, e indicador LED de status.

### Fase 5.13.2: Redesenho de Produto Comercial Premium [CONCLUÍDA]
*   **Objetivo**: Transformar o AUREQ de um gráfico simples de EQ em um produto comercial completo com botões, knobs, preset browser e painel inferior dinâmico, sem alterar o DSP ou APVTS.
*   **Ações**:
    *   [x] Expandir as dimensões da janela para 900x560.
    *   [x] Implementar o Top Header (72px) com logo AUREQ, Preset Browser com botões prev/next, e botões A/B, Reset, Settings, Theme e Bypass.
    *   [x] Implementar o Bottom Control Panel (128px) com knobs rotativos de design premium para Input/Output Gain e controles dinâmicos para a banda ativa (Freq, Gain, Q).
    *   [x] Implementar o LookAndFeel customizado `AureqLookAndFeel` para renderizar botões e knobs rotativos com halos de luz e valores textuais formatados em tempo real.
    *   [x] Habilitar re-associação dinâmica das conexões (Attachments) do APVTS para que os botões e knobs do painel controlem a banda atualmente selecionada no display gráfico.
    *   [x] Vincular os botões globais e a funcionalidade de Reset diretamente aos parâmetros do APVTS.

### Fase 5.13.3: Polimento Visual (Fresh Air & Glass Refinement) [CONCLUÍDA]
*   **Objetivo**: Clarear e suavizar a interface com efeito glass translúcido, iluminação sutil e alinhamento geométrico refinado, inspirando-se em Fresh Air e Pro-Q.
*   **Ações**:
    *   [x] Ajustar as cores do Dark Mode para tons de azul cósmico mais leves e translúcidos.
    *   [x] Aplicar AureqLookAndFeel a todos os botões do topo para renderizarem como pills de vidro.
    *   [x] Desenhar aro externo, track luminoso fino (2.2px) e especular highlight nos knobs rotativos.
    *   [x] Posicionar os botões e knobs do painel de forma geométrica e simétrica.
    *   [x] Criar uma placa de vidro interna (card) para isolar os controles da banda selecionada.
    *   [x] Suavizar a grade de visualização e aplicar bloom com triplo glow ótico na curva de EQ.


### Fase 5.14: Adicionar Analisador FFT Simples [CONCLUÍDA]
*   **Objetivo**: Exibir o sinal espectral pós-EQ no fundo do display gráfico, com pipeline de análise FFT assíncrona e totalmente isolada da thread de áudio.
*   **Ações**:
    *   [x] Criar `AnalyzerFIFO` (lock-free ring buffer, `juce::AbstractFifo`, 8192 floats pré-alocados) em `Source/` para transporte audio→UI sem locks nem alocação.
    *   [x] Criar `AnalyzerProcessor` com FFT 2048pts (`juce::dsp::FFT`), janelamento Hann pré-calculado, conversão de magnitude para dBFS e ballistics de decaimento temporal (0.78/frame, 50% overlap).
    *   [x] Adicionar push do canal L pós-EQ para `analyzerFifo` no final do `processBlock` (4 linhas, sem alterar DSP).
    *   [x] Drenar a FIFO e atualizar `EQGraphView` com os bins espectrais via `timerCallback` a 30 Hz.
    *   [x] Renderizar o espectro como gradiente translúcido cyan/mint no `EQGraphView::paint()`, antes da curva de EQ, preservando a identidade visual premium aprovada.
    *   [x] Registrar os 4 novos arquivos no `CMakeLists.txt` e validar build limpo.

### Fase 8.0: Preset Manager & Factory Presets [CONCLUÍDA]
*   **Objetivo**: Implementar PresetManager com 8 presets de fábrica embutidos em código e conectá-los ao Preset Browser da Top Bar.
*   **Ações**:
    *   [x] Criar `PresetManager` e definir 8 presets em memória (Default Flat, Vocal Presence, Drum Punch, Master Air, Low Cut Cleanup, Dynamic Vocal Control, Mid Focus, Side Air).
    *   [x] Resetar bandas para estado flat padrão antes de aplicar parâmetros específicos de cada preset.
    *   [x] Converter valores e aplicá-los ao APVTS via `setValueNotifyingHost` de forma segura.
    *   [x] Integrar `PresetManager` como membro de `AureqAudioProcessor`.
    *   [x] Conectar botões previous/next e `presetLabel` na UI do PluginEditor para navegação cíclica.
    *   [x] Sincronizar estado do presetLabel periodicamente no timer e disparar repaints/recarga de controles na UI.

### Fase 8.0.1: Factory Preset Musical Validation [CONCLUÍDA]
*   **Objetivo**: Validar musicalmente os 8 presets de fábrica e corrigir apenas valores incoerentes com a intenção sonora de cada preset.
*   **Ações**:
    *   [x] Revisar nomes, filtros, channel modes, ranges dinâmicos, ganhos, slopes e estado seguro das bandas não usadas.
    *   [x] Corrigir `Dynamic Vocal Control` para usar ranges dinâmicos negativos em 2.8 kHz e 6.5 kHz, reduzindo aspereza e sibilância.
    *   [x] Moderar `Drum Punch`, `Master Air` e `Side Air` para evitar grave/brilho excessivos.
    *   [x] Corrigir o range do parâmetro existente `bandNDynamicRange` para aceitar `-24..24 dB`, sem criar novos parâmetros, sem alterar IDs e mantendo a contagem de 108 parâmetros.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests` com sucesso.

### Fase 8.1: Preset Browser Dropdown [CONCLUÍDA]
*   **Objetivo**: Transformar o label central de preset em um browser dropdown simples para seleção direta dos 8 presets de fábrica.
*   **Ações**:
    *   [x] Usar `juce::PopupMenu` ao clicar em `presetLabel`, mantendo a Top Bar e suas dimensões principais intactas.
    *   [x] Listar os 8 presets vindos do `PresetManager` e marcar o preset atual no menu.
    *   [x] Carregar o preset selecionado via helper único `loadPresetAndRefresh()`, atualizando label, controles da banda selecionada e repaint.
    *   [x] Preservar previous/next reutilizando o mesmo fluxo de carregamento e atualização.
    *   [x] Aplicar estilização mínima compatível com a UI premium aprovada usando o LookAndFeel existente.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests` com sucesso.

### Fase 8.2: Factory Preset Library + Categorized Preset Browser [CONCLUÍDA]
*   **Objetivo**: Expandir os presets de fábrica para uma biblioteca categorizada e organizar o dropdown em submenus musicais.
*   **Ações**:
    *   [x] Expandir `PresetManager::Preset` com `category`, `subcategory` e `tags`, mantendo `PresetParameter` e APIs legadas compatíveis.
    *   [x] Adicionar getters seguros para categoria, subcategoria e tags.
    *   [x] Criar 55 presets de fábrica em código, com `Default Flat` como primeiro item de acesso direto.
    *   [x] Atualizar o `juce::PopupMenu` para submenus por categoria e submenus de gênero dentro de `Genres`.
    *   [x] Preservar previous/next navegando linearmente por todos os presets e mantendo `presetLabel` apenas com o nome.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests` com sucesso.

### Fase 8.3A: Mix/Master Preset Research Pass [CONCLUÍDA]
*   **Objetivo**: Criar uma referência técnica interna para orientar a futura expansão de factory presets do AUREQ em navegação `Gênero > Instrumento/Fonte > Presets`, sem implementar presets ou alterar código.
*   **Ações**:
    *   [x] Consultar as skills obrigatórias, documentação de DSP, fases anteriores de Dynamic EQ, Channel Mode, slopes, APVTS e validação musical de presets.
    *   [x] Consultar `PresetManager.h`, `PresetManager.cpp`, o plano de implementação e os materiais de referência em `Livros/` somente em modo leitura.
    *   [x] Criar `docs/workflow/aureq-preset-engineering-reference.md` com princípios de EQ musical, regiões de frequência, guias por fonte e gênero, regras de preset AUREQ e proposta de biblioteca.
    *   [x] Preservar integralmente código, APVTS, DSP, analyzer FFT, UI, PDFs e a pasta `Livros/`.
    *   [x] Omitir build por se tratar de uma fase exclusivamente documental.

### Fase 8.3B: Genre-First Preset Candidate Blueprint [CONCLUÍDA]
*   **Objetivo**: Transformar a referência técnica da Fase 8.3A em um blueprint priorizado de presets candidatos organizados por `Gênero > Instrumento/Fonte > Presets`, sem implementar presets no `PresetManager`.
*   **Ações**:
    *   [x] Consultar as skills obrigatórias, a referência técnica da Fase 8.3A, os resumos da biblioteca atual, `PresetManager.h`, `PresetManager.cpp` e este plano de implementação.
    *   [x] Criar `docs/workflow/aureq-genre-first-preset-candidate-blueprint.md` com 111 presets candidatos cobrindo Forro, Trap, Hip-Hop, Funk BR, Rock, Sertanejo, Pop, Gospel e Electronic.
    *   [x] Documentar para cada candidato a fonte, intenção musical, tipo de processamento, bandas sugeridas em linguagem humana, risco musical e observação de segurança.
    *   [x] Incluir presets prioritários para primeira implementação, presets que exigem validação auditiva, nomes finais recomendados e orientação de mapeamento futuro para `PresetManager`.
    *   [x] Preservar integralmente código, APVTS, DSP, analyzer FFT, UI, CMake, PDFs e a pasta `Livros/`.
    *   [x] Omitir build por se tratar de uma fase exclusivamente documental.

### Fase 8.3C: Genre-First Preset Library Batch 1 [CONCLUÍDA]
*   **Objetivo**: Implementar uma primeira leva controlada de factory presets genre-first, substituindo a seção `Genres` antiga por uma estrutura navegável `Genres > Gênero > Fonte > Presets`.
*   **Ações**:
    *   [x] Substituir os 11 presets antigos da seção `Genres` por 36 presets genre-first do Batch 1, mantendo `Default Flat` no topo.
    *   [x] Preservar os presets técnicos gerais existentes em `Vocals`, `Drums`, `Bass / 808`, `Instruments`, `Mix Bus / Master`, `Utility / Cleanup` e `Dynamic EQ`.
    *   [x] Usar `category = "Genres"`, `subcategory = nome do gênero` e tags `source:*` para mapear fonte/instrumento sem alterar o struct `Preset`.
    *   [x] Organizar o preset browser em três níveis para `Genres`, extraindo a fonte a partir das tags `source:*`.
    *   [x] Trocar o mapeamento frágil `presetIndex + 1` por um mapa local de IDs de menu para índices reais de factory presets.
    *   [x] Preservar user presets, save/load/delete/refresh/reveal, previous/next linear e `presetLabel`.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, CMake, PDFs e a pasta `Livros/`.

### Fase 8.3D: Preset Browser Genre-First UX Fix [CONCLUÍDA]
*   **Objetivo**: Corrigir cirurgicamente a hierarquia visual do preset browser para priorizar a navegação por gênero.
*   **Ações**:
    *   [x] Manter `Save Current Preset...` e `Default Flat` no topo do menu principal.
    *   [x] Mover `Genres` para ser o primeiro grupo musical após `Default Flat`.
    *   [x] Preservar a estrutura `Genres > Gênero > Fonte > Preset`.
    *   [x] Mover as categorias antigas para `By Source`, mantendo `Vocals`, `Drums`, `Bass / 808`, `Instruments`, `Mix Bus / Master`, `Dynamic EQ` e `Utility / Cleanup`.
    *   [x] Preservar user presets, previous/next linear e a lista de presets existente.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, CMake, PDFs e a pasta `Livros/`.

### Fase 8.4: Engineering-Grade Preset Library Expansion [CONCLUÍDA]
*   **Objetivo**: Expandir a biblioteca de factory presets para uma navegação genre-first com presets orientados a problemas reais de mixagem e masterização.
*   **Ações**:
    *   [x] Expandir a biblioteca para 148 factory presets.
    *   [x] Definir `Genres` como núcleo da biblioteca, com 126 presets genre-first e 14 presets por gênero.
    *   [x] Reduzir `By Source` para uma toolbox técnica genérica de 22 presets contando `Default Flat`.
    *   [x] Remover duplicação runtime entre presets genre-first e presets técnicos genéricos.
    *   [x] Atualizar a ordem/rotulagem das fontes no browser para cobrir `Sample`, `Snare / Clap`, `Lead / Synth`, `Pad`, `Master` e demais fontes novas.
    *   [x] Preservar user presets, previous/next linear, `Default Flat` no topo e a hierarquia `Genres > Gênero > Fonte > Preset`.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, CMake, PDFs e a pasta `Livros/`.

### Fase 13.0: Sistema de Localização PT-BR / EN [CONCLUÍDA]
*   **Objetivo**: Centralizar textos da interface do AUREQ e permitir alternar entre Português do Brasil e English sem alterar parâmetros, DSP ou presets.
*   **Ações**:
    *   [x] Criar `UI/Localization.h` e `UI/Localization.cpp` com `AureqLanguage`, `TextKey` e helpers `getText`, `getCategoryText`, `getSourceText`, `getFilterTypeText` e `getChannelModeText`.
    *   [x] Definir Português do Brasil como idioma padrão e adicionar troca de idioma em `Config. > Idioma`.
    *   [x] Atualizar Top Bar, Settings, preset browser, User Presets actions, A/B menu, Bottom Panel e floating HUD para consumir strings localizadas.
    *   [x] Persistir a preferência visual de idioma em `juce::PropertiesFile`, sem APVTS e sem serializar no estado do plugin.
    *   [x] Manter nomes de factory presets, nomes de user presets, IDs internos, APVTS layout, DSP e analyzer inalterados.
    *   [x] Registrar `Localization.cpp` no CMake local do plugin.

### Fase 13.1: Localization Visual QA / Copy Polish [CONCLUÍDA]
*   **Objetivo**: Revisar visualmente e polir a cópia PT-BR/EN da camada de localização, mantendo a UI, presets, APVTS, DSP e analyzer intactos.
*   **Ações**:
    *   [x] Corrigir termos PT-BR de menus e diálogos com acentos naturais, como `Gêneros`, `Usuário`, `Português (Brasil)`, `Gráfico`, `Estéreo`, `Percussão` e `Violão`.
    *   [x] Encurtar labels compactos para preservar layout, incluindo `Reset`, `FREQ`, `Q`, `CH:` e `AUREQ Equalizador`.
    *   [x] Manter termos técnicos reconhecíveis onde são mais claros para produtores, como `Bell`, `Shelf Grave`, `Shelf Agudo`, `Notch`, `Bypass`, `DYN`, `Mid`, `Side`, `Mix Bus`, `Master`, `Kick`, `Hi-Hat` e `Synth`.
    *   [x] Preservar nomes de factory presets e user presets sem tradução.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, valores musicais de presets, persistência de idioma e mapeamento de IDs dos menus.

### Fase 13.2: UTF-8 Accent Support Fix [CONCLUÍDA]
*   **Objetivo**: Corrigir a renderização de acentos PT-BR na UI sem remover acentos nem alterar parâmetros, presets, DSP, analyzer ou arquitetura de UI.
*   **Ações**:
    *   [x] Confirmar que `Localization.cpp` está salvo como UTF-8.
    *   [x] Criar helper privado `utf8(const char*)` em `Localization.cpp` usando `juce::CharPointer_UTF8`.
    *   [x] Blindar strings PT-BR acentuadas como `Gêneros`, `Presets do Usuário`, `Português (Brasil)`, `SAÍDA`, `GRÁFICO`, `Estéreo`, `Percussão` e `Violão`.
    *   [x] Blindar o separador UTF-8 do footer `ASTRA Audio Suite · v0.1.0` após validação visual apontar mojibake `Â·`.
    *   [x] Não alterar CMake, fontes, presets, APVTS, IDs, DSP, analyzer FFT, `PresetManager`, `PluginProcessor` ou APIs de user presets.

### Fase 14.0: Release Candidate / Plugin Metadata Polish [CONCLUÍDA]
*   **Objetivo**: Polir metadados de release candidate do AUREQ e remover o warning antigo de Bundle ID com espaço, sem alterar áudio, UI, presets ou parâmetros.
*   **Ações**:
    *   [x] Atualizar o `VERSION` binário do plugin para `0.9.0`, mantendo o rótulo de release documentado como `0.9.0-rc1`.
    *   [x] Definir `BUNDLE_ID "br.com.sidyfurtado.aureq"` explicitamente para evitar o fallback inválido `com.ASTRA Audio Suite.AUREQ`.
    *   [x] Atualizar `COMPANY_NAME` para `ASTRA Audio`.
    *   [x] Preservar `PRODUCT_NAME "AUREQ"`, `PLUGIN_CODE Aurq` e `MANUFACTURER_CODE AStr`.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, presets, UI, `PresetManager`, `PluginProcessor`, CMake raiz, PDFs e a pasta `Livros/`.

### Fase 14.1: Release Candidate Manual QA & DAW Validation [CONCLUÍDA — VALIDAÇÃO HUMANA PENDENTE]
*   **Objetivo**: Executar QA de Release Candidate do AUREQ 0.9.0-rc1 cobrindo build, testes, artefatos, Standalone e checklist RC completo, sem criar features novas, alterar APVTS, IDs, DSP, analyzer FFT, presets ou UI.
*   **Ações**:
    *   [x] Confirmar `cmake --build build` sem erros (targets: shared_code, AUREQ, AUREQ_Standalone, AUREQ_VST3, AUREQ_FilterTests).
    *   [x] Confirmar `./build/tests/AUREQ_FilterTests` com 100% de sucesso (3 suites, 0 falhas).
    *   [x] Confirmar existência do artefato VST3: `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`.
    *   [x] Confirmar existência do artefato Standalone: `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`.
    *   [x] Verificar metadados dos binários via plist: `CFBundleIdentifier = br.com.sidyfurtado.aureq`, `CFBundleName = AUREQ`, versão `0.9.0` — ambos os binários.
    *   [x] Confirmar abertura do Standalone sem crash (processo PID 12225 confirmado).
    *   [x] Verificar disponibilidade de DAW no ambiente — nenhuma encontrada, Bloco K marcado como `[BLOCKED]`.
    *   [x] Criar checklist RC completo em `docs/workflow/aureq-rc-manual-qa-checklist.md` (Blocos A–L, 112 itens).
    *   [x] Criar `docs/workflow/phase-14-1-release-candidate-manual-qa-summary.md`.
    *   [x] Registrar 11 itens `[PASS]`, 13 `[PARTIAL]`, 79 `[PENDING]` (sessão humana), 9 `[BLOCKED]` (sem DAW), 0 `[FAIL]`.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, presets, UI, `PresetManager`, `PluginProcessor`, CMake, PDFs e a pasta `Livros/`.

### Fase 14.2: Standalone Human QA Sign-off [CONCLUÍDA]
*   **Objetivo**: Registrar formalmente a aprovação humana do Standalone AUREQ 0.9.0-rc1 após sessão de validação manual pelo usuário, sem alterar código, APVTS, DSP, analyzer FFT, presets ou UI.
*   **Ações**:
    *   [x] Receber relato do usuário de que os fluxos principais do Standalone aparentam funcionar corretamente.
    *   [x] Registrar aprovação dos Blocos A–J e L do checklist RC (B–J via sessão humana; A e L parcialmente via automação na Fase 14.1).
    *   [x] Confirmar ausência de bugs bloqueantes ou crashes reportados.
    *   [x] Manter Bloco K (DAW Validation) como pendente — nenhuma DAW disponível no ambiente.
    *   [x] Declarar RC interno AUREQ 0.9.0-rc1 aprovado para próxima etapa de empacotamento.
    *   [x] Criar `docs/workflow/phase-14-2-standalone-human-qa-signoff-summary.md`.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, presets, UI, `PresetManager`, `PluginProcessor`, CMake, PDFs e a pasta `Livros/`.

### Fase 15.0: Packaging / Distribution Prep [CONCLUÍDA]
*   **Objetivo**: Criar infraestrutura de staging de distribuição para o AUREQ 0.9.0-rc1, organizando artefatos em `release/AUREQ-0.9.0-rc1/` e criando documentação de release, sem criar instaladores, sem assinar código e sem instalar em pastas do sistema.
*   **Ações**:
    *   [x] Confirmar `cmake --build build` e `AUREQ_FilterTests` aprovados antes do staging.
    *   [x] Criar estrutura de pastas `release/AUREQ-0.9.0-rc1/macOS/VST3/`, `macOS/Standalone/`, `Windows/VST3/` e `Docs/`.
    *   [x] Copiar `AUREQ.vst3` de `build/...` para `release/AUREQ-0.9.0-rc1/macOS/VST3/`.
    *   [x] Copiar `AUREQ.app` de `build/...` para `release/AUREQ-0.9.0-rc1/macOS/Standalone/`.
    *   [x] Criar `release/AUREQ-0.9.0-rc1/Windows/VST3/.gitkeep` (placeholder — build Windows não disponível).
    *   [x] Criar `docs/release/AUREQ-0.9.0-rc1-release-notes.md` com features, changelog, limitações e requisitos.
    *   [x] Criar `docs/release/AUREQ-macos-installation-guide.md` com caminhos de instalação e instrução sobre Gatekeeper.
    *   [x] Criar `docs/release/AUREQ-windows-installation-guide-draft.md` (guia preliminar, build Windows ainda não disponível).
    *   [x] Criar `docs/release/AUREQ-release-checklist.md` com 9 blocos cobrindo build, metadados, staging, signing, instaladores macOS e Windows, e distribuição pública.
    *   [x] Adotar Opção B para `.gitignore`: binários de staging não commitados; estrutura preservada via `.gitkeep`.
    *   [x] Preservar APVTS layout/IDs, DSP, analyzer FFT, presets, UI, código do plugin, CMake raiz, CMake do plugin, PDFs e a pasta `Livros/`.

### Fase 15.1: Icon & Installer Visual Identity System [CONCLUÍDA]
*   **Objetivo**: Criar a identidade visual base (ícones e gráficos de instalador) para a distribuição do AUREQ e da ASTRA Audio Suite, usando SVGs editáveis e documentando especificações, sem alterar binários, bundles, CMake ou instaladores reais.
*   **Ações**:
    *   [x] Criar `assets/brand/aureq-plugin-icon.svg` (Conceito "EQ Node" / Cosmic Navy + Cyan).
    *   [x] Criar `assets/brand/astra-suite-icon.svg` (Conceito "Orbital Constellation" / Cosmic Navy profundo + Cyan/Mint).
    *   [x] Gerar e renomear PNG previews de ícones em `assets/brand/previews/` via macOS `qlmanage`.
    *   [x] Criar `assets/installer/aureq-installer-header.svg` (740x90) e background `aureq-installer-background.svg` (740x540).
    *   [x] Gerar e renomear PNG previews de instalador em `assets/installer/previews/` via macOS `qlmanage`.
    *   [x] Criar `docs/brand/aureq-distribution-visual-identity.md` com especificações visuais de marca e plano de exportação.
    *   [x] Criar `docs/workflow/phase-15-1-icon-installer-visual-identity-summary.md`.
    *   [x] Confirmar que SVGs funcionam como fonte nativa e versionável.
    *   [x] Preservar integralmente bundles, Info.plist, CMake, código, APVTS, DSP e UI. Nenhuma pasta de sistema tocada.

### Fase 15.2A: macOS Icon Export & Bundle Icon Integration [CONCLUÍDA]
*   **Objetivo**: Exportar o ícone visual nativo da Apple (`.icns`) via `sips` e `iconutil`, integrá-lo de forma segura e nativa no build do Standalone macOS via JUCE CMake (`ICON_BIG`), e estabelecer formalmente os caminhos padrão dos futuros instaladores na documentação.
*   **Ações**:
    *   [x] Criar diretório `assets/brand/macOS/AUREQ.iconset`.
    *   [x] Gerar iconset com 10 resoluções via `sips` a partir do `aureq-plugin-icon-preview-1024.png`.
    *   [x] Exportar e validar pacote `assets/brand/macOS/AUREQ.icns` via `iconutil`.
    *   [x] Adicionar diretiva `ICON_BIG` apontando para o preview do ícone no arquivo `plugins/01_EQ/CMakeLists.txt`.
    *   [x] Atualizar guia de instalação macOS garantindo instalação futura forçada em `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
    *   [x] Atualizar draft de instalação Windows garantindo instalação futura forçada em `C:\Program Files\Common Files\VST3\AUREQ.vst3`.
    *   [x] Compilar o plugin com sucesso (`cmake --build build`).
    *   [x] Testes de regressão DSP executados com sucesso (`AUREQ_FilterTests`).
    *   [x] Validar que o `AppIcon.icns` foi injetado pelo JUCE no app Standalone.
    *   [x] Preservar completamente código C++, DSP, APVTS, Analyzer, Presets, e UI interna. Nenhuma instalação local realizada fora do workspace.

### Fase 15.2B: macOS Plugin-Only PKG Installer [CONCLUÍDA]
*   **Objetivo**: Criar o primeiro instalador real (`.pkg`) para macOS, focado apenas no VST3, garantindo instalação no caminho padrão do sistema (`/Library/Audio/Plug-Ins/VST3/`) sem intervenção do usuário, mantendo segurança e validação não-destrutiva.
*   **Ações**:
    *   [x] Criar estrutura de payload simulando a raiz do sistema macOS (`Library/Audio/Plug-Ins/VST3/AUREQ.vst3`).
    *   [x] Gerar pacote de componente (`AUREQ-VST3-component.pkg`) usando `pkgbuild` apontando para `/` como install-location.
    *   [x] Criar recursos visuais (`welcome.html`, `license.html` e `background.png`) e um `Distribution.xml` bloqueando caminhos arbitrários.
    *   [x] Sintetizar o instalador final (`AUREQ-0.9.0-rc1-macOS-plugin-only.pkg`) usando `productbuild`.
    *   [x] Validar o conteúdo do pacote sem instalá-lo no sistema usando `pkgutil --expand` e inspecionando o Bill of Materials (`lsbom`).
    *   [x] Confirmar zero modificação nos diretórios reais de sistema (`/Library/` e `/Applications/`).
    *   [x] Preservar DSP, C++, e APVTS através da rotina de testes do `AUREQ_FilterTests`.

### Fase 15.2C: macOS PKG Local Install QA / Uninstall Plan [CONCLUÍDA]
*   **Objetivo**: Executar o primeiro teste prático do instalador gerado na máquina hospedeira de forma isolada, validar o sucesso físico no disco e no registro de software da Apple, e fornecer os meios para posterior desinstalação controlada.
*   **Ações**:
    *   [x] Validar de antemão a não existência de `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
    *   [x] Instalar com privilégios de superusuário: `sudo installer -pkg ... -target /`.
    *   [x] Confirmar que a cópia tomou posse via `root:wheel` atestando padrão da indústria para diretórios globais do macOS (`ls -ld`).
    *   [x] Validar a integridade do banco de dados do sistema via `pkgutil --pkg-info br.com.sidyfurtado.aureq.vst3`.
    *   [x] Manter o arquivo ativamente na máquina para que o usuário possa proceder de imediato com validação humana em DAWs (Reaper, Ableton).
    *   [x] Criar e documentar a política rigorosa de Uninstall (remoção de arquivo + `pkgutil --forget`).

### Fase 15.2C-R1: macOS PKG Local Install QA Recheck [BLOQUEADA]
*   **Objetivo**: Reexecutar em 2026-05-28 o QA local do PKG plugin-only, criando backup prévio se `AUREQ.vst3` já existisse e mantendo o VST3 instalado para validação manual em DAW.
*   **Ações**:
    *   [x] Confirmar que `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3` já existia antes da tentativa.
    *   [x] Criar backup em `backups/local-install-qa/AUREQ.vst3.backup-20260528-093811`.
    *   [!] Tentativa de `sudo installer -pkg "installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg" -target /` bloqueada por autenticação sudo indisponível na sessão.
    *   [x] Capturar estado observado via `ls -ld`, `pkgutil --pkg-info` e `pkgutil --files`; o recibo existente tem `install-time` de 2026-05-27 18:42:10 -03, indicando instalação anterior.
    *   [x] Documentar uninstall e restore sem executá-los.
    *   [x] Preservar APVTS, DSP, analyzer FFT, presets, UI interna, CMake, PKG gerado, livros e PDFs.

### Fase 15.2D: DAW Validation Manual Documentation [CONCLUÍDA]
*   **Objetivo**: Criar documentação operacional para validação manual do AUREQ em DAWs usando o VST3 atualmente instalado em `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`, sem executar build e sem alterar código, CMake, APVTS, DSP, analyzer, presets, UI, PKG, livros ou PDFs.
*   **Ações**:
    *   [x] Documentar que a Fase 15.2C-R1 ficou bloqueada por autenticação sudo indisponível na sessão.
    *   [x] Documentar que o `AUREQ.vst3` já estava instalado e que existe receipt `br.com.sidyfurtado.aureq.vst3` version `0.9.0`.
    *   [x] Criar checklist manual com status `PASS`, `FAIL`, `PARTIAL` e `BLOCKED`.
    *   [x] Cobrir Ableton Live, Reaper e qualquer DAW disponível.
    *   [x] Cobrir rescan, identificação como VST3, carregamento em track, UI, áudio, preset browser, user presets, A/B, Undo/Redo, Dynamic EQ, Channel Mode, salvar/reabrir sessão e observação de crash/logs.
    *   [x] Documentar que a validação em DAW pode seguir com o plugin atualmente instalado.
    *   [x] Documentar que a validação do PKG novo deve ser repetida manualmente no Terminal com senha administrativa.
    *   [x] Criar `docs/workflow/phase-15-2d-daw-validation-manual-checklist.md`.
    *   [x] Criar `docs/workflow/phase-15-2d-daw-validation-summary.md`.

### Fase 15.2G: macOS Branded DMG Installer Experience [CONCLUÍDA]
*   **Objetivo**: Criar uma embalagem visual premium em `.dmg` para o instalador plugin-only do AUREQ, mantendo o `.pkg` como motor real de instalação e sem instalar nada no sistema.
*   **Ações**:
    *   [x] Criar estrutura `installer/macOS/dmg/` com `staging/`, `scripts/` e `output/`.
    *   [x] Copiar o PKG existente para `installer/macOS/dmg/staging/AUREQ Installer.pkg`, sem alterar o PKG fonte.
    *   [x] Incluir README bilingue PT-BR/EN explicando o fluxo de instalação e o destino `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
    *   [x] Incluir `AUREQ.icns` e background premium AUREQ/ASTRA em `.background/aureq-dmg-background.png`.
    *   [x] Criar `installer/macOS/dmg/scripts/create-dmg.sh` usando `hdiutil` como ferramenta principal e AppleScript/Finder polish como best effort.
    *   [x] Gerar DMG final read-only/compressed em `installer/macOS/dmg/output/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg`.
    *   [x] Validar o DMG por montagem, inspeção de arquivos e desmontagem via `hdiutil`, sem executar o PKG.
    *   [x] Preservar APVTS, IDs, DSP, analyzer FFT, presets, UI interna, CMake, código C++, PKG fonte, livros e PDFs.

### Fase 15.2H: macOS Release Hardening & DAW Validation Pass [CONCLUÍDA]
*   **Objetivo**: Consolidar o estado de release macOS antes de avançar para Windows, cobrindo instalação, duplicatas, metadata, Gatekeeper, DMG UX e checklist real de validação em DAWs, sem alterar código ou executar instaladores.
*   **Ações**:
    *   [x] Confirmar que existe apenas `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3` e que nao ha duplicatas fisicas `AUREQ.vst3` ou `AUREQ_2.vst3` em `~/Library`.
    *   [x] Confirmar `Info.plist`: `CFBundleIdentifier = br.com.sidyfurtado.aureq`, nome `AUREQ` e versao `0.9.0`.
    *   [x] Confirmar `moduleinfo.json`: vendor `ASTRA Audio`.
    *   [x] Confirmar que ocorrencia de `Apple` e apenas DTD padrao do plist, nao vendor.
    *   [x] Registrar Gatekeeper como `BLOCKED` ate Apple Developer ID/notarizacao, com assinatura atual ad-hoc e `TeamIdentifier=not set`.
    *   [x] Validar DMG por montagem/inspecao/desmontagem sem executar o PKG; DMG funcional, polish Finder ainda parcial.
    *   [x] Criar checklist macOS DAW com foco em FL Studio, Ableton Live, Reaper e hosts futuros.
    *   [x] Documentar que Logic Pro e validacao futura via AU/wrapper, nao falha VST3.
    *   [x] Preservar APVTS, IDs, DSP, analyzer FFT, presets, UI interna, CMake, código C++, PKG, plugin binary, livros e PDFs.

### Fase 15.2I: FL Studio macOS Validation Sign-off / Cache Note [CONCLUÍDA]
*   **Objetivo**: Registrar o primeiro sign-off manual do AUREQ no FL Studio macOS e documentar o `AUREQ_2` remanescente como provavel cache/entrada antiga da DAW, sem limpar cache, remover arquivos ou alterar artefatos.
*   **Ações**:
    *   [x] Registrar que o FL Studio detectou o AUREQ principal como VST3.
    *   [x] Registrar que o AUREQ abriu dentro do FL Studio e a UI carregou corretamente.
    *   [x] Registrar vendor principal correto: `ASTRA Audio`.
    *   [x] Confirmar que a duplicata fisica user-level foi resolvida e que o plugin fisico instalado existe somente em `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
    *   [x] Documentar `AUREQ_2` como provavel cache/entrada antiga do FL Studio, com vendor antigo `ASTRA Audio Suite`.
    *   [x] Recomendar manter favorito apenas no AUREQ correto e remover/desfavoritar `AUREQ_2` no Plugin Manager.
    *   [x] Manter Gatekeeper como `BLOCKED` ate Apple Developer ID + notarizacao.
    *   [x] Reafirmar macOS como prioridade antes do Windows.
    *   [x] Preservar codigo, CMake, APVTS, IDs, DSP, analyzer FFT, presets, UI interna, PKG, DMG, livros e PDFs.

### Fase 15.2J: FL Studio macOS Musical QA Pass [PREPARADA]
*   **Objetivo**: Preparar o passe de QA musical real do AUREQ no FL Studio macOS, cobrindo uso em mixer track, audio, filtros, slopes, Channel Mode, Dynamic EQ, presets, A/B, Undo/Redo, bypass, analyzer, salvar/reabrir projeto e estabilidade, sem feature nova.
*   **Ações**:
    *   [x] Criar `docs/workflow/aureq-fl-studio-musical-qa-checklist.md` com status `PASS`, `PARTIAL`, `FAIL` e `BLOCKED`.
    *   [x] Cobrir insercao em mixer track e passagem de audio limpo.
    *   [x] Cobrir Input/Output Gain.
    *   [x] Cobrir filtros Bell, LC, HC, LS, HS, NT e BP.
    *   [x] Cobrir slopes 12/24/48.
    *   [x] Cobrir Channel Mode ST, M, S, L e R.
    *   [x] Cobrir Dynamic EQ: Range, Threshold, Attack e Release.
    *   [x] Cobrir Preset Browser: `Genres > Trap > 808`, `Genres > Forro > Vocal`, `Genres > Rock > Electric Guitar`, `By Source` e `User Presets`.
    *   [x] Cobrir A/B, Undo/Redo, salvar/reabrir projeto FL Studio, bypass, analyzer visual e estabilidade sem crash.
    *   [x] Registrar campos para observacoes musicais sobre presets agressivos, curvas estranhas, ganho alto, nomes confusos e comportamentos inesperados.
    *   [x] Manter resultado final como `BLOCKED` ate validacao humana.
    *   [x] Preservar codigo, CMake, APVTS, IDs, DSP, analyzer FFT, presets, UI interna, PKG, DMG, livros e PDFs.

### Fase 16.0: Real-Time Spectrum Overlay Visualizer [CONCLUÍDA]
*   **Objetivo**: Polir o analyzer FFT ja existente como overlay visual em tempo real dentro do `EQGraphView`, deixando-o mais perceptivel e musical sem alterar audio, APVTS, IDs, presets ou DSP principal.
*   **Ações**:
    *   [x] Confirmar pipeline existente `processBlock -> AnalyzerFIFO -> timerCallback -> AnalyzerProcessor -> EQGraphView`.
    *   [x] Corrigir ballistics visual em dB, substituindo multiplicacao de dB negativo por decay via subtracao com clamp para `-90 dBFS`.
    *   [x] Aumentar a legibilidade do overlay com fill cyan/mint translúcido e stroke superior suave.
    *   [x] Manter spectrum atras da curva de EQ e dos handles.
    *   [x] Preservar escala logaritmica 20 Hz - 20 kHz e range visual aproximado `-90 dB` a `0 dB`.
    *   [x] Nao criar parametros, botoes, modos Pre/Post/Off ou alteracoes de automacao/estado.
    *   [x] Validar build com `cmake --build build`.
    *   [x] Validar testes DSP com `./build/tests/AUREQ_FilterTests`.
    *   [x] Preservar APVTS, IDs, presets, DSP musical principal, audio processado, CMake, installers, PKG/DMG, livros e PDFs.

### Fase 16.2: Top Bar Layout Polish [CONCLUÍDA]
*   **Objetivo**: Corrigir apenas spacing/layout da Top Bar para evitar colisao visual entre preset browser, `next`, A/B, Reset e botoes da direita, preservando logica e estetica premium.
*   **Ações**:
    *   [x] Identificar layout hardcoded em `AureqAudioProcessorEditor::resized()`.
    *   [x] Calcular o grupo direito de tras para frente com gaps fixos de `8px`.
    *   [x] Definir larguras fixas seguras para previous/next, A/B, Reset, Settings, Theme e Bypass.
    *   [x] Calcular area segura para o preset browser entre branding esquerdo e grupo direito.
    *   [x] Permitir que `presetLabel` encolha primeiro em caso de espaco apertado.
    *   [x] Preservar callbacks e logica de preset browser, previous/next, A/B, Reset, Settings, Theme e Bypass.
    *   [x] Validar build com `cmake --build build`.
    *   [x] Validar testes DSP com `./build/tests/AUREQ_FilterTests`.
    *   [x] Preservar APVTS, IDs, DSP, analyzer FFT, presets, user presets, CMake, installers, PKG/DMG, livros e PDFs.

### Fase 16.3: Refresh macOS Beta Installer With Latest Build [CONCLUÍDA]
*   **Objetivo**: Regenerar os artefatos beta macOS (`.pkg` e `.dmg`) com o VST3 mais recente, sem alterar codigo, APVTS, IDs, DSP, analyzer, presets, UI ou CMake, e sem instalar nada no sistema.
*   **Ações**:
    *   [x] Validar build com `cmake --build build`.
    *   [x] Validar testes DSP com `./build/tests/AUREQ_FilterTests`.
    *   [x] Confirmar VST3 buildado em `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`.
    *   [x] Atualizar payload local em `installer/macOS/plugin-only/payload/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
    *   [x] Regenerar `installer/macOS/plugin-only/output/AUREQ-VST3-component.pkg`.
    *   [x] Regenerar `installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg`.
    *   [x] Inspecionar o PKG de forma nao destrutiva com `pkgutil --expand` e `lsbom`.
    *   [x] Regenerar DMG via `installer/macOS/dmg/scripts/create-dmg.sh`.
    *   [x] Montar/desmontar o DMG com `hdiutil` e confirmar conteudo esperado.
    *   [x] Confirmar que o PKG nao foi executado, nada foi instalado, nenhum `sudo` foi usado e o beta permanece unsigned/not notarized.

### Fase 16.4: Private Beta Tester Package & Feedback Kit [CONCLUÍDA]
*   **Objetivo**: Criar documentacao de envio para testers privados do AUREQ 0.9.0-rc1 macOS beta, sem alterar artefatos tecnicos.
*   **Ações**:
    *   [x] Criar `docs/beta/AUREQ-0.9.0-rc1-private-beta-instructions.md`.
    *   [x] Criar `docs/beta/AUREQ-0.9.0-rc1-tester-feedback-form.md`.
    *   [x] Criar `docs/beta/AUREQ-0.9.0-rc1-known-limitations.md`.
    *   [x] Documentar instalacao via DMG/PKG e destino `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
    *   [x] Documentar aviso de beta privada unsigned/not notarized e dependencia futura de Apple Developer ID + notarizacao.
    *   [x] Cobrir DAWs para teste: FL Studio, Ableton Live, Reaper, Studio One, Cubase e Bitwig.
    *   [x] Documentar que Logic Pro nao e alvo VST3 direto.
    *   [x] Criar formulario de feedback cobrindo DAW, macOS, CPU, deteccao VST3, UI, audio, spectrum, presets, user presets, A/B, Undo/Redo, Dynamic EQ, Channel Mode, salvar/reabrir projeto, crashes e dificuldade de instalacao.
    *   [x] Preservar codigo, CMake, APVTS, DSP, analyzer, presets, UI, PKG/DMG, livros e PDFs.

### Fase 17.0: Windows Plugin-Only Installer Plan [CONCLUÍDA]
*   **Objetivo**: Preparar a estrutura inicial do instalador Windows VST3-only do AUREQ 0.9.0-rc1 com Inno Setup, sem gerar `.exe` final e sem usar artefato macOS como payload Windows.
*   **Ações**:
    *   [x] Criar estrutura `installer/Windows/plugin-only/` com `scripts/`, `resources/`, `staging/` e `output/`.
    *   [x] Criar `installer/Windows/plugin-only/scripts/AUREQ-plugin-only.iss`.
    *   [x] Configurar no `.iss`: `AppName=AUREQ`, `AppVersion=0.9.0`, `AppPublisher=ASTRA Audio`, `DefaultDirName={commoncf}\VST3`, `DisableDirPage=yes`, `OutputDir=..\output` e `OutputBaseFilename=AUREQ-0.9.0-rc1-Windows-plugin-only-setup`.
    *   [x] Prever `SetupIconFile=..\resources\AUREQ.ico` somente se o `.ico` existir.
    *   [x] Prever `Source: "..\staging\VST3\AUREQ.vst3"` e `DestDir: "{commoncf}\VST3"`.
    *   [x] Documentar destino obrigatorio `C:\Program Files\Common Files\VST3\AUREQ.vst3`.
    *   [x] Criar `installer/Windows/plugin-only/resources/README-Windows.txt`.
    *   [x] Criar `docs/beta/AUREQ-0.9.0-rc1-windows-beta-instructions.md`.
    *   [x] Criar `docs/workflow/phase-17-0-windows-plugin-only-installer-plan-summary.md`.
    *   [x] Manter `staging/VST3/` sem payload ate existir build Windows real.
    *   [x] Confirmar que nao foi gerado `.exe` final, nada foi instalado em Program Files, nenhum Wine/VM/CI foi usado e nenhum code signing foi executado.
    *   [x] Documentar bloqueios: Windows real/VM aprovada/CI runner Windows, Inno Setup real, payload Windows e code signing Windows.
    *   [x] Preservar codigo C++, CMake, APVTS, IDs, DSP, analyzer, presets, UI interna, macOS PKG/DMG, livros e PDFs.

### Fase 17.1: GitHub Actions Windows Build + Inno Setup Installer Pipeline [CONCLUÍDA]
*   **Objetivo**: Criar um workflow no GitHub Actions para compilar o AUREQ (VST3) no Windows e gerar o instalador plugin-only unsigned via Inno Setup automaticamente.
*   **Ações**:
    *   [x] Criar o ícone oficial `AUREQ.ico` em `installer/Windows/plugin-only/resources/` a partir do PNG oficial usando script Python/Pillow local.
    *   [x] Criar o workflow do GitHub Actions em `.github/workflows/windows-build-aureq.yml` rodando em `windows-latest`.
    *   [x] Configurar o workflow para instalar o Inno Setup via Chocolatey.
    *   [x] Configurar o build em CMake Release x64 com compilador MSVC do VS 2022.
    *   [x] Configurar execução automática dos testes unitários do DSP (`AUREQ_FilterTests.exe`).
    *   [x] Implementar script em PowerShell no runner para localizar o `AUREQ.vst3` e copiá-lo para staging.
    *   [x] Acionar a compilação do Inno Setup (`iscc`) gerando o instalador `.exe`.
    *   [x] Configurar upload dos artefatos (VST3 puro e Instalador executável).
    *   [x] Atualizar o guia de instalação do Windows draft e o checklist de release do repositório.
    *   [x] Criar sumário detalhado em `docs/workflow/phase-17-1-github-actions-windows-build-inno-installer-pipeline-summary.md`.

### Fase 9.0: A/B Compare [CONCLUÍDA]
*   **Objetivo**: Implementar comparação rápida entre dois estados internos de equalização sem alterar APVTS, DSP, analyzer ou persistência de sessão.
*   **Ações**:
    *   [x] Armazenar slots A/B temporários no `PluginProcessor` usando `juce::ValueTree` e snapshots de `apvts.copyState().createCopy()`.
    *   [x] Inicializar o slot A com o estado atual e o slot B como cópia segura do A no primeiro uso.
    *   [x] Salvar o slot ativo antes da troca, alternar o slot ativo e restaurar o outro slot com `apvts.replaceState()`.
    *   [x] Conectar o botão A/B da Top Bar para alternar os slots e indicar o slot ativo como `A` ou `B`.
    *   [x] Atualizar controles da banda selecionada, gráfico e repaint após a troca, preservando previous/next e dropdown no slot ativo.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests` com sucesso.

### Fase 9.1: A/B Copy Actions [CONCLUÍDA]
*   **Objetivo**: Implementar ações avançadas de cópia e reset para os slots A/B sem alterar parâmetros, APVTS ou DSP, disparados via menu contextual (clique direito) no botão A/B da Top Bar.
*   **Ações**:
    *   [x] Adicionar métodos no `PluginProcessor`: `copyABSlotAToB()`, `copyABSlotBToA()`, `copyCurrentABSlotToOther()`, e `resetABSlotBFromCurrent()`.
    *   [x] Chamar `ensureABSlotsInitialized()` e usar `createCopy()` para clonar os estados com segurança sem trocar o slot ativo.
    *   [x] Adicionar listener de mouse em `abBtn` e interceptar cliques com `event.mods.isPopupMenu()` no `mouseUp` do `PluginEditor` para mostrar o menu.
    *   [x] Exibir menu contextual customizado pequeno com `Copy A to B`, `Copy B to A`, `Copy Current to Other`, e `Reset B to Current`.
    *   [x] Impedir que o clique direito dispare também o clique esquerdo (toggle) por meio de um filtro rápido em `abBtn.onClick`.
    *   [x] Validar build com `cmake --build build` e testes com `./build/tests/AUREQ_FilterTests`.

### Fase 10.0: Undo / Redo Básico [CONCLUÍDA]
*   **Objetivo**: Implementar histórico interno temporário de Undo/Redo para estados APVTS completos, sem criar parâmetros, sem alterar IDs, DSP, analyzer FFT, presets externos ou persistência de histórico.
*   **Ações**:
    *   [x] Armazenar `undoStack` e `redoStack` no `PluginProcessor` usando snapshots `juce::ValueTree` criados com `apvts.copyState().createCopy()`.
    *   [x] Limitar o histórico a 32 estados e evitar duplicatas idênticas antes de empilhar checkpoints.
    *   [x] Restaurar Undo/Redo com `apvts.replaceState (state.createCopy())`, movendo o estado atual para a pilha oposta.
    *   [x] Capturar checkpoints somente antes de ações grandes: preset, A/B toggle, tipo de filtro, slope, channel mode, adicionar/remover/criar banda e reset de ganhos.
    *   [x] Expor `Undo` e `Redo` no menu pequeno do botão `Settings`, mantendo a Top Bar sem novos botões grandes.
    *   [x] Manter drag, knobs e mudanças contínuas fora desta fase para evitar histórico por tick/movimento.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.

### Fase 10.1: Gesture Undo [CONCLUÍDA]
*   **Objetivo**: Expandir o histórico básico para gestos contínuos de UI, capturando apenas um checkpoint no início do gesto e evitando snapshots por movimento, timer ou analyzer.
*   **Ações**:
    *   [x] Capturar checkpoint em `EQGraphView::onDragStart` antes de `beginChangeGesture()` para drag de handles (Frequency/Gain).
    *   [x] Manter `onBandDragged` livre de checkpoints para evitar histórico por movimento.
    *   [x] Capturar Q por mouse wheel antes de aplicar o valor, com uma janela curta de agrupamento para evitar spam de snapshots.
    *   [x] Capturar checkpoints em `Slider::onDragStart` para os knobs Frequency, Gain, Q, Range, Threshold, Attack e Release.
    *   [x] Capturar checkpoint do `DYN` toggle via `mouseDown`, antes do `ButtonAttachment` aplicar a alteração.
    *   [x] Preservar Input/Output Gain, Global Bypass, Theme, Band Bypass e atalhos de teclado para fases futuras.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.

### Fase 10.2: Undo/Redo Completion + Keyboard Shortcuts [CONCLUÍDA]
*   **Objetivo**: Completar a cobertura de Undo/Redo nos controles restantes de áudio da UI e adicionar atalhos básicos de teclado sem criar parâmetros, alterar APVTS, IDs, DSP ou analyzer.
*   **Ações**:
    *   [x] Capturar checkpoints em `inputGainSlider.onDragStart` e `outputGainSlider.onDragStart`.
    *   [x] Capturar checkpoint do Global Bypass via `mouseDown`, antes do `ButtonAttachment` aplicar a alteração.
    *   [x] Capturar checkpoint do Band Bypass via `mouseDown`, somente quando há banda selecionada e o botão está habilitado.
    *   [x] Criar helpers internos `performUndoFromUI()` e `performRedoFromUI()` para centralizar restauração e atualização visual.
    *   [x] Adicionar `keyPressed()` com Cmd/Ctrl+Z para Undo, Cmd/Ctrl+Shift+Z e Cmd/Ctrl+Y para Redo.
    *   [x] Habilitar foco de teclado no editor, mantendo o menu Settings como caminho confiável quando a DAW interceptar atalhos.
    *   [x] Manter Theme fora do Undo por ser preferência visual.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.

### Fase 11.0: User Presets / Save Preset [CONCLUÍDA]
*   **Objetivo**: Permitir salvar e carregar presets de usuário em disco sem alterar APVTS layout, IDs, DSP, analyzer FFT ou presets de fábrica.
*   **Ações**:
    *   [x] Manter factory presets em código e adicionar uma lista separada de user presets carregados de disco.
    *   [x] Salvar user presets como XML do APVTS com extensão `.aureqpreset`.
    *   [x] Usar `juce::File::userApplicationDataDirectory/AUREQ/Presets` como diretório de user presets.
    *   [x] Adicionar `refreshUserPresets()`, `getNumUserPresets()`, `getUserPresetName()`, `loadUserPreset()` e `saveCurrentUserPreset()` ao `PresetManager`.
    *   [x] Expor wrappers no `PluginProcessor`, capturando Undo antes de carregar user preset e não capturando Undo ao salvar.
    *   [x] Adicionar `Save Current Preset...` e submenu `User Presets` ao preset browser existente.
    *   [x] Pedir nome via `juce::AlertWindow`, sanitizar nomes inválidos e gerar sufixos incrementais para duplicatas.
    *   [x] Deixar delete, rename, import/export avançado, cloud sync, marketplace, busca, subpastas e favoritos fora desta fase.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.

### Fase 11.1: User Preset Management [CONCLUÍDA]
*   **Objetivo**: Adicionar gerenciamento básico dos presets de usuário existentes sem criar parâmetros, alterar APVTS layout, IDs, DSP, analyzer FFT ou a UI premium aprovada.
*   **Ações**:
    *   [x] Adicionar APIs seguras no `PresetManager` para obter arquivo/diretório oficial, revelar pasta e deletar user preset por índice.
    *   [x] Expor wrappers simples no `PluginProcessor` para revelar a pasta oficial e deletar user preset.
    *   [x] Expandir o submenu `User Presets` com `Refresh User Presets`, `Reveal Presets Folder` e `Delete User Preset...`.
    *   [x] Implementar delete com confirmação e `moveToTrash()`, validando índice, arquivo existente, extensão `.aureqpreset` e diretório pai oficial.
    *   [x] Manter factory presets impossíveis de deletar e preservar o label atual quando o preset deletado era o preset selecionado.
    *   [x] Deixar rename, busca, tags, favoritos, subpastas, import/export avançado, cloud sync e marketplace fora desta fase.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.

### Fase 12.0: Product Readiness / Manual QA Pass [CONCLUÍDA]
*   **Objetivo**: Executar uma passagem de QA e prontidão de produto, priorizando validação, auditoria e documentação de release sem criar features novas, alterar APVTS layout, IDs, DSP, analyzer FFT ou redesenhar a UI aprovada.
*   **Ações**:
    *   [x] Criar checklist manual interno cobrindo presets, user presets, A/B, Undo/Redo, graph handles, Bottom Panel, Top Bar, visual, persistência e regressões.
    *   [x] Auditar os fluxos principais pela implementação atual de `PluginEditor` e `EQGraphView`.
    *   [x] Validar abertura do Standalone gerado em build.
    *   [x] Registrar critérios de aprovação, limitações conhecidas e itens adiados para validação humana/host quando necessário.
    *   [x] Manter correções de código fora da fase porque nenhum bug pequeno e claro foi confirmado nesta execução.
    *   [x] Validar com `cmake --build build` e `./build/tests/AUREQ_FilterTests`.

---

### Fase 5.15 (Executada): Tipos de Filtro DSP — 7 Tipos [CONCLUÍDA]
*   **Objetivo**: Expandir o motor DSP do AUREQ de Bell-only para suporte completo a 7 tipos de filtro usando o parâmetro existente `bandNType`, sem criar novos parâmetros.
*   **Ações**:
    *   [x] Adicionar `setHighPass`, `setLowPass`, `setLowShelf`, `setHighShelf`, `setNotch`, `setBandPass` ao `BiquadFilter` com fórmulas RBJ Audio EQ Cookbook completas.
    *   [x] Adicionar helpers estáticos `sanitizeCommonInputs` e `normaliseCoeffs` para reutilizar proteção NaN/Inf em todos os métodos.
    *   [x] Expandir `EQBand::processBlock` e `updateFilterCoefficients` com `switch(type)` roteando todos os 7 tipos para o método correto.
    *   [x] Expandir a curva visual em `EQGraphView::paint()` com aproximações analíticas para todos os tipos (rolloff, shelf sigmoide, notch, band pass).
    *   [x] Confirmar que APVTS, `PluginProcessor`, `EQProcessorCore`, `ParameterSmoother` e `BiquadFilter::processSample/processBlock` foram preservados intocados.
    *   [x] Build limpo: `[100%] Built target AUREQ_VST3 / AUREQ_Standalone`.

### Fase 5.16: Filter Type UI Selector [CONCLUÍDA]
*   **Objetivo**: Criar interface visual de seleção de tipo de filtro para a banda selecionada, conectando-a ao parâmetro existente `bandNType` do APVTS e atualizando a curva e handles no display.
*   **Ações**:
    *   [x] Adicionar fileira de botões pill na parte superior do painel inferior (seção da banda ativa).
    *   [x] Customizar os botões com LookAndFeel para estilo premium glass com status ativo cyan/mint glow.
    *   [x] Conectar cliques de botões ao APVTS via `setValueNotifyingHost` com mapeamento correto dos 7 tipos de filtro.
    *   [x] Atualizar o Floating HUD para exibir o tipo real ativo do filtro por banda.
    *   [x] Habilitar handles e arraste no EQGraphView para todos os tipos de filtro (não apenas Bell).
    *   [x] Adicionar esmaecimento visual automático para knobs Gain/Q quando forem menos relevantes ao tipo de filtro selecionado.

### Fase 5.17: Band Activation UI Selector [CONCLUÍDA]
*   **Objetivo**: Adicionar controles visuais de ativação/desativação de bandas via botão e gesto no gráfico de EQ, utilizando os parâmetros de controle existentes do APVTS.
*   **Ações**:
    *   [x] Adicionar o botão "+ Band" no painel inferior para ativar a próxima banda disponível no default da mesma.
    *   [x] Implementar double-click em espaço vazio no gráfico de EQ para ativar e posicionar a próxima banda livre na frequência/ganho clicados.
    *   [x] Adicionar o botão "Remove" no Selected Band Card para desativar a banda ativa e atualizar a seleção.
    *   [x] Permitir que bandas bypassadas permaneçam selecionáveis e arrastáveis no gráfico com visual reduzido.
    *   [x] Garantir que bandas desativadas (disabled) fiquem ocultas, não selecionáveis e não participem da curva ou processamento de áudio.

### Fase 5.18: Cut Slopes [CONCLUÍDA]
*   **Objetivo**: Adicionar suporte a slopes (12, 24 e 48 dB/oct) para Low Cut e High Cut sem alterar o APVTS.
*   **Ações**:
    *   [x] Implementar estado interno por instância `bandSlopes` no `PluginProcessor` e expor getters/setters.
    *   [x] Implementar suporte DSP de cascata de biquads em `EQBand` e `EQProcessorCore`.
    *   [x] Adicionar botões seletores visuais de slope (12/24/48) no painel inferior para Low Cut e High Cut.
    *   [x] Atualizar a curva visual do display gráfico para refletir o slope selecionado.
    *   [x] Atualizar o Floating HUD para exibir o slope ativo ao lado do tipo de filtro.

### Fase 5.19: APVTS Advanced Band Parameters [CONCLUÍDA]
*   **Objetivo**: Adicionar 56 novos parâmetros ao layout do APVTS (7 por banda × 8 bandas), permitindo a persistência do Cut Slope e preparando o suporte a equalização dinâmica e modos de canal.
*   **Ações**:
    *   [x] Declarar e implementar funções auxiliares de IDs em `ParameterIDs` para slope, parâmetros dinâmicos e channel mode.
    *   [x] Registrar os 56 novos parâmetros em `createParameterLayout()` no `PluginProcessor.cpp`.
    *   [x] Integrar a leitura do parâmetro de slope no fluxo atômico de `EQProcessorCore::updateFromAPVTS`.
    *   [x] Atualizar a UI do PluginEditor para que os seletores de slope escrevam e leiam o parâmetro APVTS.
    *   [x] Desativar o estado temporário do slope anterior no processador.

### Fase 5.20: APVTS State Validation [CONCLUÍDA]
*   **Objetivo**: Validar a contagem total de 108 parâmetros, correspondência de IDs legados e novos, correção dos valores padrão e persistência de XML.
*   **Ações**:
    *   [x] Auditar a contagem total de parâmetros no construtor do layout do APVTS (totalizando 108).
    *   [x] Corrigir o valor inicial do Dynamic Range para 0.0 dB (neutro) e do Dynamic Release para 120 ms (musical).
    *   [x] Verificar se o salvamento de presets e restauração de sessões capturam e carregam automaticamente os 108 parâmetros.
    *   [x] Garantir que os recursos inativos de DSP (Dynamic EQ e Channel Mode) permaneçam neutros no áudio.
    *   [x] Compilar o projeto e documentar o status em `phase-5-20-apvts-state-validation-summary.md`.

### Fase 5.21: Testes e Validação [CONCLUÍDA]
*   **Objetivo**: Garantir calibração matemática e estabilidade contra travamentos através de testes unitários isolados em C++.
*   **Ações**:
    *   [x] Criar infraestrutura de testes modular no CMake compilando as classes DSP (`BiquadFilter`, `EQBand`, `ParameterSmoother`) diretamente no executável `AUREQ_FilterTests`.
    *   [x] Escrever suite de testes de coeficientes (`BiquadCoefficientTests.cpp`) validando a ausência de NaNs/Infinitos nos 7 tipos de filtros sob variados sample rates, Qs e ganhos.
    *   [x] Escrever suite de testes de blocos (`EQBandProcessingTests.cpp`) testando buffers mono/estéreo em múltiplos block sizes (64, 512, 2048) e verificando pass-through perfeito para bypass e desativação.
    *   [x] Escrever suite de testes de estabilidade (`FilterStabilityTests.cpp`) submetendo os biquads a estresse com buffers de 8192 samples usando silêncio, impulsos, senoides e ruído branco.
    *   [x] Rodar e validar que 100% dos testes passam sem falhas no macOS.

### Fase 6.0: Dynamic EQ Core [CONCLUÍDA]
*   **Objetivo**: Implementar o motor básico de equalização dinâmica no DSP de forma modular, segura e leve, utilizando os parâmetros de APVTS existentes.
*   **Ações**:
    *   [x] Projetar um seguidor de envelope de pico (peak follower) stereo-linked atuando na entrada de áudio do bloco.
    *   [x] Computar a modulação de ganho dinâmico a partir de um joelho soft-knee exponencial musical baseado no excesso acima do threshold e clampado no range.
    *   [x] Habilitar dinâmica apenas para os tipos Bell, Low Shelf e High Shelf, mantendo os demais tipos estáticos.
    *   [x] Integrar parâmetros dinâmicos na leitura atômica da classe `EQProcessorCore`.
    *   [x] Atualizar as suites de testes unitários de DSP no CMake com cobertura de dinâmica ativa/inativa.

### Fase 6.1: Dynamic EQ UI Controls [CONCLUÍDA]
*   **Objetivo**: Adicionar controles visuais de Dynamic EQ no Selected Band Card e feedbacks gráficos no display e HUD.
*   **Ações**:
    *   [x] Adicionar o botão DYN toggle e knobs compactos para Range, Threshold, Attack e Release no Selected Band Card.
    *   [x] Implementar re-associação dinâmica e thread-safe de attachments por banda selecionada.
    *   [x] Adicionar indicador DYN (badge) e range dinâmico (linha tracejada sutil) no display do equalizador.
    *   [x] Expandir e atualizar o floating HUD com status DYN e Range.
    *   [x] Compilar e rodar testes de regressão com 100% de sucesso.

---

## Fases de Evolução Premium (Roadmap Futuro)

### Fase 6.2: Recursos Avançados de Workflow [MEDIDOR DE GANHO DINÂMICO CONCLUÍDO]
*   [x] Implementar medidores visuais de redução de ganho dinâmico (gain reduction meters) nos cards de banda selecionada e no gráfico principal.
*   [x] Implementar o sistema de comparação rápida A/B de estados de equalização.
*   [ ] Implementar histórico infinito de Undo/Redo na interface visual.
*   [ ] Adicionar recurso de Solo de Banda (band solo) e Copy/Paste de configurações entre bandas.
*   [ ] Refinar o analisador visual para exibir sinais Pré-EQ e Pós-EQ simultaneamente.

### Fase 7.0: Channel Mode Core [CONCLUÍDA]
*   **Objetivo**: Implementar o core DSP inicial de Channel Mode por banda, utilizando o parâmetro APVTS `bandNChannelMode` para comutar entre Stereo, Mid, Side, Left e Right.
*   **Ações**:
    *   [x] Mapear o parâmetro APVTS e passá-lo para cada `EQBand`.
    *   [x] Implementar processamento de Stereo (L/R tradicional).
    *   [x] Implementar processamento Left e Right isolados por canal.
    *   [x] Implementar conversão Mid/Side com processamento individualizado e reconstrução L/R.
    *   [x] Adicionar mono fallback para tratar buffers de 1 canal com segurança.
    *   [x] Atualizar suíte de testes unitários e de estabilidade para validar todos os 5 modos.

### Fase 7.1: Channel Mode UI Selector [CONCLUÍDA]
*   **Objetivo**: Adicionar o seletor visual de Channel Mode no Selected Band Card, conectando-o aos parâmetros APVTS existentes.
*   **Ações**:
    *   [x] Adicionar botões ST, M, S, L, R no Selected Band Card.
    *   [x] Configurar o visual de glass pills para os botões.
    *   [x] Conectar as ações dos botões ao APVTS com begin/endChangeGesture.
    *   [x] Atualizar dinamicamente os botões ao trocar a banda selecionada.
    *   [x] Atualizar o Floating HUD e os handles de banda para indicar o Channel Mode ativo.


### Fase 7.2: Bottom Panel UX Polish [CONCLUÍDA]
*   **Objetivo**: Polir o Bottom Panel / Selected Band Card para melhorar a legibilidade, organização visual e hierarquia premium, sem adicionar funcionalidade nova.
*   **Ações**:
    *   [x] Estruturar o Selected Band Card em grupos lógicos com micro títulos e divisórias.
    *   [x] Reajustar largura e espaçamento dos botões de tipo de filtro (abreviando Notch para NT).
    *   [x] Reajustar largura e espaçamento dos botões de Channel Mode e visor de meter dinâmico.
    *   [x] Adicionar rótulo "SLOPE" de exibição reativa acima dos botões de slope.
    *   [x] Garantir que não há colisões ou overlaps e que o build e testes permaneçam passando.

### Fase 7: Equalização Dinâmica Avançada (Dynamic EQ Refinements)
*   Integrar detector RMS e sidechain externo.
*   Adicionar parâmetro de Ratio de compressão/expansão.

### Fase 8: Dinâmica Espectral (Spectral Dynamics)
*   Desenvolver o motor DSP de análise de bins de frequência de FFT.
*   Implementar redução/expansão de ganho inteligente individualizada por bin espectral para controle de mascaramento de áudio contínuo.

### Fase 9: Fase Linear (Linear Phase)
*   Desenvolver o motor FIR de equalização.
*   Implementar convolução por partição na frequência (Overlap-Save FFT Convolution) para processar grandes kernels de fase linear com consumo de CPU mitigado.
*   Configurar a latência estável do plugin e reportá-la à DAW (Plugin Delay Compensation).

### Fase 10: Workflow Avançado e Conectividade
*   Desenvolver detecção automática de colisão de frequências (Collision Detection) entre instâncias do AUREQ via barramento de rede local/memória compartilhada.
*   Implementar o motor de captura espectral *Spectrum Grab*.
*   Criar algoritmos de correspondência espectral (*EQ Match*).
*   Adicionar suporte nativo a canais de áudio espacial/surround (Dolby Atmos).
