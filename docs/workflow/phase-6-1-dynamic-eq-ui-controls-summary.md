# Resumo de Validação Técnica: Fase 6.1 — Dynamic EQ UI Controls

Este documento registra a especificação, implementação e validação dos controles visuais e feedbacks gráficos do recurso de equalização dinâmica (**Dynamic EQ**) na interface gráfica do equalizador **AUREQ**, integrante da **ASTRA Audio Suite**.

---

## 1. Objetivo da Fase

Adicionar a interface de usuário (UI) básica para manipulação dos parâmetros de equalização dinâmica da banda ativa do equalizador paramétrico, de modo reativo, robusto e preservando a integridade estética premium ("liquid glass") aprovada. A alteração deve ler e gravar os parâmetros do APVTS já configurados em fases anteriores, com gerenciamento seguro das associações de controles (attachments) ao trocar de banda selecionada, sem modificar os parâmetros do APVTS e sem alterar o algoritmo de DSP já validado.

---

## 2. Controles Adicionados & Layout do Selected Band Card

Para acomodar os controles de dinâmica sem poluir visualmente nem apertar a interface:
1. **Redimensionamento do Selected Band Card**: O card de banda ativa foi expandido de `460px` para `530px` de largura, com posicionamento `[X = 210, Y = 408, W = 530, H = 104]`.
2. **Knobs Estáticos Compactos**: Os knobs de frequência, ganho e Q foram discretamente redimensionados para `60px` de largura, ocupando as posições `X = 220, 285, 350`.
3. **Divisor Vertical**: Uma linha divisora vertical translúcida foi inserida na coordenada `X = 418` (`Y = 414` a `506`) para delimitar claramente as seções estática e dinâmica da banda.
4. **Controles de Dynamic EQ**:
   - **Botão DYN**: Toggle pill no topo direito da seção dinâmica em `[X = 615, Y = 414, W = 45, H = 20]`.
   - **Range**: Knob compacto em `[X = 425, Y = 440, W = 55, H = 66]`.
   - **Threshold**: Knob compacto em `[X = 485, Y = 440, W = 55, H = 66]`.
   - **Attack**: Knob compacto em `[X = 545, Y = 440, W = 55, H = 66]`.
   - **Release**: Knob compacto em `[X = 605, Y = 440, W = 55, H = 66]`.
5. **Botões de Ação da Banda**: Bypass e Remove da banda, bem como a comutação de slopes (12/24/48 dB/oct) para Low Cut e High Cut, foram posicionados simetricamente no canto direito do card (`X = 670` a `730`).
6. **Controles Globais**: O botão global `Bypass` e o slider de `Output Gain` foram sutilmente deslocados para a direita (`X = 750` e `X = 816` respectivamente) para manter um respiro visual de `10px` entre o card de banda ativa e a borda.

---

## 3. Conexões APVTS e Funcionamento dos Attachments Dinâmicos

Os controles visuais são conectados aos parâmetros APVTS por meio de `juce::AudioProcessorValueTreeState::SliderAttachment` e `ButtonAttachment`.
Como o painel inferior reflete apenas a banda ativa, esses attachments são **re-associados em tempo de execução** sempre que `selectedBandIndex` muda:
- **Destruição Segura**: Antes de qualquer re-associação, os ponteiros inteligentes (`std::unique_ptr`) dos attachments antigos são explicitamente resetados (`.reset()`) para liberar os listeners e impedir referências inválidas (*dangling pointers*).
- **Associação Precisa**: Se `selectedBandIndex` for válido (`0 a 7`), novos attachments são criados apontando para os IDs parametrizados:
  - `bandNDynamicEnabled` -> `dynToggleAttachment`
  - `bandNDynamicRange` -> `dynRangeAttachment`
  - `bandNDynamicThreshold` -> `dynThresholdAttachment`
  - `bandNDynamicAttack` -> `dynAttackAttachment`
  - `bandNDynamicRelease` -> `dynReleaseAttachment`
- **Estado de Sem Seleção**: Se `selectedBandIndex == -1` (nenhuma banda ativa), todos os novos controles dinâmicos são colocados em estado inativo (`setEnabled (false)`) e opacidade reduzida (`setAlpha (0.35f)`).

---

## 4. Relevância por Tipo de Filtro

Para garantir coerência de áudio e segurança numérica:
- **Tipos Suportados**: A equalização dinâmica opera exclusivamente nos tipos **Bell**, **Low Shelf** e **High Shelf**.
- **Tipos Não Suportados**: Para os filtros **Low Cut**, **High Cut**, **Notch** e **Band Pass**, a UI desabilita visualmente todos os controles dinâmicos (`dynToggleButton` e knobs de dinâmica) e diminui sua opacidade para `0.35f` no callback periódico (`timerCallback()`). O botão DYN comunica o estado de indisponibilidade indicando `N/A`.
- O motor de DSP preserva seu comportamento e ignora qualquer valor dinâmico se o filtro estiver configurado para um tipo não suportado.

---

## 5. Feedbacks Visuais no Gráfico & HUD Dinâmico

1. **Badge DYN no Handle**: Se a banda selecionada estiver com equalização dinâmica ativada (`dynamicEnabled == true`) em um tipo de filtro suportado, um pequeno distintivo de vidro translúcido contendo o texto **DYN** é desenhado centralizado `12px` acima do círculo do handle.
2. **Halo / Indicador de Range**: Desenha-se uma linha vertical tracejada sutil na cor acento, estendendo-se do centro do handle até a amplitude correspondente ao ganho estático somado ao range dinâmico máximo (`gainDb + dynamicRange`), finalizando com um pequeno círculo delimitador de limite.
3. **Floating HUD Expandido**:
   - Dimensões atualizadas para `160px` de largura e `92px` de altura para acomodar as informações adicionais sem amontoar.
   - Inclusão de uma segunda linha contendo:
     - **DYN**: Exibe `ON` ou `OFF` para tipos compatíveis, e `N/A` para tipos incompatíveis.
     - **RANGE**: Exibe o ganho máximo dinâmico (ex: `-6.0 dB`) ou `-` quando inativo.

---

## 6. Resultados de Compilação & Execução de Testes

Os binários do equalizador e os testes matemáticos foram executados com sucesso no macOS.

### A. Compilação
- **Comando**: `cmake --build build`
- **Status**: Compilação limpa concluída com sucesso (0 erros).

### B. Execução de Testes Unitários de Regressão
- **Comando**: `./build/tests/AUREQ_FilterTests`
- **Resultado**: Todos os testes matemáticos e de estabilidade (incluindo o sub-suite de estabilidade dinâmica) passaram sem nenhuma falha.
```
========================================================
         AUREQ - DSP FILTER VALIDATION TESTS            
========================================================

[SUITE] Running Biquad Coefficient Tests...
[PASS] Biquad Coefficient Tests completed successfully.

[SUITE] Running EQBand Block Processing Tests...
[PASS] EQBand Block Processing Tests completed successfully.

[SUITE] Running Filter Stability and Safety Tests...
[SUB-SUITE] Running EQBand Dynamic EQ stability sub-suite...
[PASS] Filter Stability and Safety Tests completed successfully.

========================================================
      SUCCESS: All AUREQ DSP Filter Tests Passed!      
========================================================
```

---

## 7. Conformidade das Regras Estritas

- **APVTS Não Alterado**: A contagem de parâmetros permanece exatamente em 108. Nenhuma declaração ou ID de parâmetro foi adicionada, modificada ou removida.
- **DSP Core Não Alterado**: O processamento de áudio em tempo real implementado na Fase 6.0 foi 100% preservado e permaneceu intacto.
- **Visual Premium Preservado**: A materialidade "liquid glass" translúcida, glow orbital discreto e alinhamentos geométricos seguem estritamente as diretrizes visuais aprovadas.

---

## 8. Caminho dos Binários Gerados

- **VST3**: [AUREQ.vst3](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3)
- **Standalone**: [AUREQ.app](file:///Users/sidyziin/Documents/VST%20-%20Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app)
- **Executável de Teste**: [AUREQ_FilterTests](file:///Users/sidyziin/Documents/VST%20-%20Project/build/tests/AUREQ_FilterTests)

---

## 9. Próximo Passo Recomendado

Prosseguir para o desenvolvimento da **Fase 6.2: Recursos Avançados de Workflow**, implementando o display de Gain Reduction dinâmica no Selected Band Card e no display cartesiano em tempo real, ou iniciar o sistema de preset browser definitivo do plugin.
