# Resumo de Validação Técnica: Fase 5.11 — Interação Básica dos Handles

Este documento resume as implementações realizadas na **Fase 5.11** do equalizador **AUREQ**, parte da **ASTRA Audio Suite**, incluindo o suporte a hit-testing e arraste básico (drag) dos handles visuais das bandas para modificação direta de frequência e ganho no APVTS.

---

## 1. Objetivo da Fase
O objetivo principal desta fase foi habilitar a interatividade do usuário com o gráfico de EQ, permitindo que ele clique e arraste os nós das bandas de Bell diretamente no display para controlar as variáveis de Frequência (X) e Ganho (Y) de forma integrada e suave.

---

## 2. Como o Hit Test Funciona
No método `mouseDown` de [EQGraphView](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/EQGraphView.cpp):
- É percorrido o array das 8 bandas.
- Apenas as bandas que estão **habilitadas** (`enabled == true`), **não-bypassadas** (`bypassed == false`) e são do tipo **Bell** (`type == 0`) são consideradas ativas e elegíveis para interação.
- A distância Euclidiana entre a posição do ponteiro do mouse (`event.position`) e o centro de pixel de cada handle $(x, y)$ é calculada com uma fórmula de distância manual robusta:
  $$\text{dist} = \sqrt{(x_{\text{mouse}} - x_{\text{handle}})^2 + (y_{\text{mouse}} - y_{\text{handle}})^2}$$
- Se a distância for menor ou igual a **12 pixels** (raio seguro para dispositivos touch e cliques de mouse), a banda correspondente é selecionada e seu índice é salvo na variável `activeBandDragIndex`.

---

## 3. Como o Arraste Altera Frequência e Ganho
Durante o movimento de arraste (`mouseDrag`):
- As coordenadas do mouse são capturadas e convertidas de volta a valores de áudio físicos por meio de métodos de conveniência integrados:
  - **Frequência (X)**: Convertida por `xToFrequency(float x)` usando a reversão da escala logarítmica e limitada com `juce::jlimit` entre **20 Hz e 20000 Hz**.
  - **Ganho (Y)**: Convertido por `yToGain(float y)` mapeando linearmente a calha do plot e limitado com `juce::jlimit` entre **-24 dB e +24 dB**.
- O estado local de `bands` dentro da UI é atualizado instantaneamente para redesenho imediato a 60fps (sem lag visual), e em seguida é disparado o callback `onBandDragged` com os novos valores para propagação externa.

---

## 4. Atualização do APVTS e Host Gestures
O [PluginEditor](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/PluginEditor.cpp) captura os callbacks dinâmicos do [EQGraphView](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/EQGraphView.h) e interage de forma thread-safe e direta com o APVTS:
1. **Início do Arraste (`onDragStart`)**: Obtém os ponteiros dos parâmetros no APVTS e chama `beginChangeGesture()` para ambos (Frequency e Gain), informando a DAW para iniciar a gravação de automação.
2. **Durante o Arraste (`onBandDragged`)**: Converte os valores contínuos reais (Hz, dB) para seus equivalentes normalizados de $0.0$ a $1.0$ usando a função `convertTo0to1(...)` de cada parâmetro correspondente, aplicando-os em seguida via `setValueNotifyingHost(...)`.
3. **Fim do Arraste (`onDragEnd`)**: Finaliza chamando `endChangeGesture()`, fechando de forma limpa a gravação de automação no host.

*Nota: Se algum parâmetro não for localizado, a rotina falha de forma segura sem travar o plugin.*

---

## 5. Feedback Visual do Drag
Durante a interação, o handle sob arraste ganha destaque visual imediato:
- O raio do círculo interno aumenta ligeiramente de **6px para 7px**.
- O raio da aura/anel externo aumenta de **9px para 11px**.
- O raio de difusão de glow traseiro aumenta de **14px para 18px** com a opacidade ampliada, dando excelente indicação visual de foco luminoso ativo.

---

## 6. Limitações Conhecidas e Escopo Estrito
- **Q Não Editável**: O fator Q não é alterado pelo arraste de mouse nesta fase. A movimentação do scroll de mouse wheel foi explicitamente omitida para preservação do escopo.
- **Sem Analyzer FFT**: Nenhum sinal espectral pós ou pré-EQ é renderizado em segundo plano.
- **DSP Preservado**: Nenhuma alteração foi realizada nos componentes matemáticos ou de áudio digital.

---

## 7. Resultado do Build e Caminhos
A compilação foi validada com 100% de sucesso.

- **Caminho do Plugin VST3**:
  `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

- **Caminho do Standalone Executável (Mac App)**:
  `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

---

## 8. Próximo Passo Recomendado
Avançar para a **Fase 5.12: Adicionar Analisador FFT Simples** (com cópia de buffers via lock-free FIFO na thread de tempo real e janelamento Hann assíncrono), ou trabalhar no **Ajuste de Q por scroll de mouse** sobre os handles de banda ativos.
