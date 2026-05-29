# Resumo da Fase 5.5: Biquad Foundation + primeira banda Bell

Este documento resume as implementações matemáticas e integrações feitas durante a Fase 5.5, estabelecendo a base para o processamento de sinal IIR do equalizador **AUREQ**.

---

## 1. Objetivo da Fase
Implementar a fundação matemática de filtros digitais recursivos IIR de segunda ordem (Biquads) e habilitar a primeira banda do equalizador para operar como um filtro Bell/Peaking EQ básico, mantendo o código seguro e o build passando.

---

## 2. Detalhes do Filtro Implementado

### A. Topologia
Utilizamos a topologia **Direct Form II Transposed (DF2T)**, recomendada por sua estabilidade numérica e eficiência sob ponto flutuante:
- **Equação de Diferenças**:
  $$y[n] = b_0 x[n] + z_1[n-1]$$
  $$z_1[n] = b_1 x[n] - a_1 y[n] + z_2[n-1]$$
  $$z_2[n] = b_2 x[n] - a_2 y[n]$$
- **Estados por Canal**: Os registradores de estados `z1` e `z2` são alocados no método `prepare` usando `std::vector<double>` para suportar o processamento independente de múltiplos canais (como estéreo).
- **Sem alocação no loop de áudio**: O vetor de estados é pré-dimensionado no método `prepare` baseado no número de canais reportados, eliminando qualquer chamada de alocação de memória dinâmica (`new`/`malloc`) em tempo real.

### B. Fórmula matemática do Bell/Peaking EQ
A computação de coeficientes baseia-se no clássico **Audio EQ Cookbook de Robert Bristow-Johnson (RBJ)**:
- **Variáveis intermediárias**:
  $$\omega_0 = 2 \pi \frac{f_0}{f_s}$$
  $$\alpha = \frac{\sin(\omega_0)}{2 Q}$$
  $$A = 10^{\text{gainDb}/40}$$
- **Coeficientes Brutos**:
  $$b_0 = 1 + \alpha A$$
  $$b_1 = -2 \cos(\omega_0)$$
  $$b_2 = 1 - \alpha A$$
  $$a_0 = 1 + \frac{\alpha}{A}$$
  $$a_1 = -2 \cos(\omega_0)$$
  $$a_2 = 1 - \frac{\alpha}{A}$$
- **Normalização**: Todos os coeficientes finais são normalizados por $a_0$ (ex: $b_0 \leftarrow b_0 / a_0$).

---

## 3. Segurança Numérica Implementada

- **Limites de Parâmetro**:
  - A frequência central $f_0$ é restrita por software entre $20\text{ Hz}$ e a menor frequência entre $20000\text{ Hz}$ ou $95\%$ do limite de Nyquist ($\text{sampleRate} \times 0.5 \times 0.95$). Isso evita comportamento instável próximo do limite superior.
  - O fator de qualidade $Q$ é restrito entre $0.1$ e $18.0$.
- **Tratamento de Frequência de Amostragem**: Se `sampleRate <= 0.0`, os coeficientes do filtro voltam para o modo pass-through (filtro de identidade: $b_0 = 1$, todos os outros coeficientes são zero).
- **Proteção contra NaNs e Infinitos**:
  - Evitamos divisões por zero tratando casos em que $|a_0| < 10^{-12}$.
  - Se qualquer coeficiente normalizado resultante for inválido (`isnan` ou `isinf`), o filtro volta automaticamente para o modo bypass (identidade).
  - No motor principal `EQProcessorCore::processBlock`, adicionamos uma varredura sanitária pós-processamento: qualquer amostra que resulte em `isnan` ou `isinf` é imediatamente substituída por `0.0f` para proteger o monitoramento do usuário.

---

## 4. Escopo Restrito de Processamento

- **Banda Única Ativa**: Apenas a **Band 1** (índice 0) está integrada e executa o processamento do filtro Bell.
- **Bypass de Outras Bandas**: As bandas 2 a 8 não processam nenhum áudio, mesmo se os parâmetros forem modificados via APVTS.
- **Tipos de Filtros Limitados**: Somente o tipo `FilterType::Bell` processa áudio. Os outros tipos (Low Cut, High Cut, Shelving, etc.) não executam nenhuma operação DSP nessa fase.
- **Estruturas Globais Preservadas**: O controle de volume global (`inputGain` e `outputGain`) e o `globalBypass` continuam operacionais e integrados na pipeline de áudio.

---

## 5. Resultado do Build e Caminhos

- **Status da Compilação**: 100% aprovado.
- **Caminho do Binário VST3 Gerado**:
  `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`

---

## 6. Próximo Passo Recomendado
Avançar para a **Fase 5.6**, onde iniciaremos a construção da casca visual da interface gráfica (UI inicial de 680x420 pixels) com o gerenciamento de temas (Light/Dark Mode) e a moldura vazia do gráfico cartesiano.
