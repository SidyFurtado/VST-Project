# Resumo de Validação Técnica: Fase 5.12 — Controle de Q pelo Mouse Wheel

Este documento resume as implementações realizadas na **Fase 5.12** do equalizador **AUREQ**, parte da **ASTRA Audio Suite**, incluindo a seleção visual persistente das bandas de equalização e a manipulação do fator Q (largura de banda) por meio da roda do mouse (mouse wheel).

---

## 1. Objetivo da Fase
O objetivo desta fase foi permitir que o usuário altere o fator Q da banda selecionada ou da banda sob o cursor do mouse utilizando a roda de rolagem do mouse (mouse wheel), proporcionando controle total sobre a largura do filtro de forma rápida e intuitiva.

---

## 2. Seleção Visual da Banda
- Adicionada a variável privada `selectedBandIndex` no [EQGraphView](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/EQGraphView.h), inicializada como `-1` por padrão para evitar alterações acidentais na Band 1 antes de qualquer interação do usuário.
- O método `mouseDown` de [EQGraphView](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/EQGraphView.cpp) foi atualizado para que cliques bem-sucedidos em handles definam tanto o arraste (`activeBandDragIndex`) quanto a seleção permanente (`selectedBandIndex`).

---

## 3. Controle de Q via Mouse Wheel
Implementado o método [mouseWheelMove](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/EQGraphView.cpp#L421) no [EQGraphView](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/EQGraphView.cpp):
1. **Hit-Testing de Hover**: O método verifica se o mouse está sobre a área de algum handle de banda ativo (utilizando um raio de **15 pixels** para facilidade de rolagem). Se sim, este handle torna-se o alvo e o índice selecionado é atualizado.
2. **Fallback por Seleção**: Se o mouse estiver rolando em uma área vazia do gráfico mas houver uma banda selecionada (`selectedBandIndex != -1`), a rolagem é direcionada à banda selecionada.
3. **Escala Multiplicativa**: A mudança do fator Q é realizada de forma exponencial/multiplicativa suave:
   $$Q_{\text{novo}} = Q_{\text{atual}} \times 1.15^{(\text{deltaY} \times 5)}$$
   Isso permite que a sensibilidade de rolagem seja perfeitamente balanceada tanto para valores pequenos quanto grandes de Q.
4. **Clamp Estrito**: O valor resultante é limitado rigorosamente entre **0.1** (largura de banda máxima) e **18.0** (filtro extremamente estreito).

---

## 4. Atualização do APVTS
O [PluginEditor](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/UI/PluginEditor.cpp) escuta o callback `onBandQChanged` e repassa a alteração de forma thread-safe para o APVTS:
- Obtém o parâmetro correspondente `bandNQ` do APVTS.
- Envolve a alteração com `beginChangeGesture()` e `endChangeGesture()`.
- Normaliza o valor chamando `convertTo0to1(...)` e atualiza-o através de `setValueNotifyingHost(...)`.

---

## 5. Feedback Visual de Q e Seleção
Quando uma banda está selecionada (`selectedBandIndex == i`):
- Um **anel de foco sutil** (`g.drawEllipse`) é renderizado externamente ao redor do handle correspondente.
- Um **indicador de largura de banda** (aura concêntrica tracejada com raio inversamente proporcional à raiz quadrada de Q) é desenhado ao redor do nó:
  $$\text{raio}_{\text{aura}} = \frac{35.0}{\sqrt{Q}}$$
  Isso faz com que filtros largos (Q baixo) tenham uma grande aura sombreada e tracejada, enquanto filtros cirúrgicos (Q alto) tenham uma aura extremamente colada ao centro, simulando perfeitamente o comportamento físico acústico na tela.

---

## 6. Confirmações de Segurança e Escopo
- **Drag Preservado**: O arraste horizontal (frequência) e vertical (ganho) de handles de banda continua perfeitamente operacional.
- **DSP Intocado**: O motor DSP e biquads matemáticos não sofreram nenhuma alteração.
- **Sem FFT/Analyzer**: A exibição espectral FFT permanece desativada nesta fase.

---

## 7. Resultado do Build e Caminhos
A compilação foi concluída com 100% de sucesso.

- **Caminho do Plugin VST3**:
  `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

- **Caminho do Standalone Executável (Mac App)**:
  `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

---

## 8. Próximo Passo Recomendado
Avançar para a **Fase 5.13: Adicionar Analisador FFT Simples** no fundo do display gráfico ou criar a interface de painel inteligente flutuante contextual para a banda selecionada.
