# Resumo de Validação Técnica: Fase 5.18 — Cut Slopes

Este documento registra a implementação e validação da **Fase 5.18** do equalizador **AUREQ**, parte da **ASTRA Audio Suite**, que adiciona suporte a múltiplos slopes (12, 24 e 48 dB/oct) para os filtros Low Cut (High-Pass) e High Cut (Low-Pass).

---

## 1. Objetivo da Fase

Permitir slopes ajustáveis de **12 dB/oct**, **24 dB/oct** e **48 dB/oct** para os filtros de corte (Low Cut e High Cut). A infraestrutura DSP de cascata de biquads foi implementada de forma a suportar o processamento sample-by-sample com alocação zero no loop de áudio. A interface gráfica (UI) foi equipada com botões seletores e a curva visual do equalizador foi atualizada para refletir a inclinação em tempo real.

---

## 2. Como os Slopes foram Tratados (Sem Alterar o APVTS)

Conforme as restrições estritas de não alterar a estrutura do APVTS nem criar novos parâmetros nesta fase (para garantir total compatibilidade com sessões do host DAW e presets de fábrica):
- **Estado Interno Seguro**: O estado de slope de cada uma das 8 bandas é gerenciado em um array thread-safe `std::array<std::atomic<int>, 8> bandSlopes` de precisão atômica na classe do processador `AureqAudioProcessor`.
- **Comunicação Atômica Sem Locks**: O editor gráfico lê e altera esses slopes usando os métodos thread-safe `getBandSlope(bandIndex)` e `setBandSlope(bandIndex, slope)`. Nenhuma trava de mutex ou alocação dinâmica é realizada no loop de processamento.
- **Temporariedade de Sessão**: Esta implementação é considerada um estado temporário de sessão. Como os slopes não residem em parâmetros persistentes do APVTS, os slopes redefinem para o padrão de 12 dB/oct ao reiniciar o plugin ou abrir um novo projeto. A persistência total via APVTS será abordada em fase futura dedicada.
- **Isolamento de Instâncias**: Diferente de abordagens globais/estáticas frágeis, o estado é armazenado de forma estrita em cada instância individual do plugin, garantindo total isolamento e permitindo que múltiplas instâncias rodem simultaneamente na DAW sem interferência mútua.

---

## 3. Suporte DSP para Cascata de Biquads

Para atingir as inclinações desejadas sem alocar memória no `processBlock` em tempo real:
- Cada banda em `EQBand` contém um filtro principal `filter` e um array estático de biquads extras `std::array<BiquadFilter, 3> extraCutFilters` (fornecendo um máximo de 4 estágios no total).
- Durante o `prepare(sampleRate, numChannels)` e `reset()`, todos os filtros extras são inicializados e limpos em conformidade numérica (prevenção de denormais e NaNs).
- Ao atualizar coeficientes:
  - **12 dB/oct** usa 1 biquad (estágio principal).
  - **24 dB/oct** usa 2 biquads em cascata (estágio principal + 1 extra).
  - **48 dB/oct** usa 4 biquads em cascata (estágio principal + 3 extras).
- Para filtros de tipo não-corte (Bell, Shelves, Notch, Band Pass), apenas o biquad principal é processado.
- Os filtros extras permanecem completamente inertes (sem processar amostras) quando o tipo da banda não é Low Cut ou High Cut.

---

## 4. UI e Curva Visual

### Seletor de Slope
- No bottom panel (Selected Band Card), adicionou-se uma linha horizontal de 3 botões pill estilo vidro ("12", "24", "48") posicionados na coordenada `X = 576 + i * 28`, `Y = bottomY + 76`, `W = 24`, `H = 20`.
- Esses botões são exibidos **apenas** quando o tipo de filtro selecionado for Low Cut (LC) ou High Cut (HC), e ficam ocultos nos demais tipos.
- O botão ativo é destacado com contorno de vidro com preenchimento em ciano/mint reativo ao tema do plugin.

### Curva Visual Aproximada
- No `EQGraphView`, o cálculo da curva teórica foi adaptado para multiplicar a atenuação logarítmica (em dB) pelo número de estágios correspondentes ao slope selecionado (1x para 12dB, 2x for 24dB, 4x para 48dB). A curva combinada reflete com exatidão a atenuação acentuada.

### Floating HUD
- O HUD flutuante indica o slope selecionado em tempo real no campo de tipo (ex: `Type: Low Cut (24dB)` ou `Type: High Cut (48dB)`), garantindo clareza total.

---

## 5. Limitações Conhecidas

- **Sem Automação/Presets**: Por não usar parâmetros APVTS dedicados a esta fase, a automação de DAW e a gravação de presets não persistirão o estado dos slopes (voltando para 12 dB/oct). Esta persistência será implementada em fase futura dedicada.
- **Resonância de Q**: O fator Q fornecido pelo usuário é cascateado igualmente entre todas as etapas do filtro.

---

## 6. Confirmação de Segurança de Regras

- **APVTS Intocado**: O layout de parâmetros do APVTS e o total de 52 parâmetros foram integralmente preservados.
- **Mapeamento de Tipos Preservado**: Bell, Shelves, Notch e Band Pass continuam funcionando.
- **Dynamic EQ, Mid/Side, Linear Phase**: Não foram alterados ou implementados.
- **Sem SharedSlopeRegistry**: Não foi criado nenhum estado estático/global de compartilhamento.
- **Sem Erros de Build**: A compilação passou com sucesso.

---

## 7. Resultado do Build e Binários Gerados

A compilação do monorepo foi executada via CMake:
```bash
cmake --build build
[100%] Built target AUREQ_VST3
```

### Caminhos dos Binários
- **VST3**: `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- **Standalone**: `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
