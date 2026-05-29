# Estilo e Comportamento de Medidores (Meter Style)

Os **Medidores (Meters)** fornecem feedback visual preciso e em tempo real sobre a amplitude e a atenuação de sinais de áudio. Este documento especifica as diretrizes de estilo dos meters para manter o layout elegante, discreto e focado na clareza do sinal.

---

## 1. Tipos de Medidores Requeridos

### A. Input / Output Meters (Medidores de Entrada e Saída)
*   **Finalidade**: Exibir o sinal que chega ao plugin e o sinal processado que sai dele.
*   **Estrutura**: Geralmente dispostos como barras verticais duplas (canal esquerdo e canal direito) nos cantos externos da interface ou agrupados de forma compacta na seção de saída.
*   **Comportamento Dual**: Exibição combinada de **Pico (Peak)** e **RMS (Média)** na mesma barra:
    *   **RMS**: Barra contínua de cor sólida indicando a energia média percebida do áudio.
    *   **Pico (Peak)**: Um segmento flutuante fino (de 2px de altura) posicionado acima do nível RMS, que indica o valor máximo instantâneo e decai lentamente com uma curva de resposta exponencial.

### B. Gain Reduction Meter (Medidor de Redução de Ganho)
*   **Finalidade**: Exibir a quantidade de compressão ou atenuação de sinal aplicada em decibéis (dB).
*   **Estrutura**: Barra vertical simples posicionada de cabeça para baixo ou barra horizontal que se preenche a partir da direita para a esquerda.
*   **Comportamento**: Como o ganho é reduzido, o medidor se move de `0 dB` (sem redução) para valores negativos (ex: `-3 dB`, `-6 dB`, `-12 dB`). A cor é tipicamente uma versão monocromática discreta ou a cor secundária ativa, caindo rapidamente ao atenuar e retornando ao repouso suavemente.

### C. Analisador do EQ (Spectrum Analyzer)
*   **Finalidade**: Exibir a FFT de entrada e saída sobreposta à curva de equalização.
*   **Aparência**: Um grid discreto com linhas semi-transparentes em tom de borda sutil, com curvas preenchidas de forma translúcida em cyan (saída) e cinza-azulada (entrada).

---

## 2. Cores de Status e Níveis de Sinal

As cores dos medidores permanecem idênticas em ambos os temas para manter a interpretação das medições intuitiva:

| Nível de Sinal | Cor de Preenchimento | Light Mode Hex | Dark Mode Hex | Significado Técnico |
| :--- | :--- | :--- | :--- | :--- |
| **Nível Seguro (Safe)** | Mint / Verde | `#10B981` | `#34D399` | Sinais normais abaixo de `-12 dB` |
| **Nível de Atenção** | Ambar / Laranja | `#F59E0B` | `#FBBF24` | Transição de dinâmica entre `-12 dB` e `-2 dB` |
| **Clipping / Sobrecarga** | Vermelho elétrico | `#EF4444` | `#F87171` | Sinal acima de `-2 dB` ou atingindo `0 dBFS` |

---

## 3. Indicador de Clipping (Clipping Indicator)

*   **Comportamento**: Uma pequena luz quadrada ou circular com cantos ligeiramente arredondados no topo do medidor de Input/Output.
*   **Estado Inativo**: Cor de calha em bypass.
*   **Estado Ativo**: Acende em vermelho elétrico instantaneamente ao detectar que qualquer amostra de áudio atingiu ou ultrapassou `0 dBFS` (ou ao detectar clipping inter-amostra, se implementado).
*   **Decaimento**: O indicador vermelho permanece aceso por pelo menos 3 segundos após o último pico de clip para dar tempo ao usuário de notar a sobrecarga. Um clique com o botão esquerdo do mouse sobre o indicador redefine o estado de clip para apagado.

---

## 4. Visual por Tema

### Light Mode
*   **Calha de Fundo (Track)**: `#E2E8F0` (Fundo cinza claro que integra o medidor no painel branco).
*   **Escala de dB**: Texto em cinza-escuro `#475569` posicionado externamente, em fonte `Roboto Mono` de 9px.
*   **Sombras**: Uma leve ranhura interna (`inner shadow`) simulada para dar sensação de que o medidor está cavado na superfície do painel.

### Dark Mode
*   **Calha de Fundo (Track)**: `#151D24` (Integra-se no fundo grafite).
*   **Escala de dB**: Texto em cinza-claro `#94A3B8` posicionado de forma compacta.
*   **Glow**: O sinal ativo de pico e clip projeta uma leve luminosidade lateral na calha interna do meter.

---

## 5. Regras Específicas por Tipo de Plugin

1.  **Equalizador (EQ)**: Não usa meters verticais gigantes. Em vez disso, a tela gráfica de FFT no centro é o principal medidor, com pequenos meters compactos de Input/Output no rodapé ou nos cantos.
2.  **Compressor**: O destaque central é o **Gain Reduction Meter**, posicionado de forma proeminente ao centro ou entre os knobs de controle principais de Threshold e Ratio. Os medidores de I/O devem ser discretos.
3.  **Saturador (Saturator)**: Apresenta um medidor de entrada e um medidor de saída emparelhados de tamanho médio, enfatizando o quanto o sinal está sendo empurrado (Drive) para a zona de saturação (zona de transição laranja).
4.  **Limitador (Limiter)**: Requer meters de Input/Output grandes e ultra-precisos, com detecção de True Peak e indicação de Gain Reduction clara lado a lado, pois o Limiter é o guardião final contra clipping na mixagem.

---

## 6. Como Evitar Poluição Visual nos Medidores

*   ❌ **Sem divisões pretas grossas**: Não divida o medidor em dezenas de pequenos blocos retangulares separados por traços pretos agressivos (estilo anos 80). Utilize uma barra de cor contínua e suave.
*   ❌ **Sem escalas sobrecarregadas**: Coloque marcações de dB apenas nos pontos essenciais: `+6, 0, -3, -6, -12, -18, -30, -60`.
*   ❌ **Sem arcos superiores desnecessários**: O arco superior clássico analógico (VU meter de agulha) não se encaixa na nossa estética moderna. Todos os nossos medidores de sinal são lineares e verticais/horizontais.
*   ✔️ **Decaimento Suave**: A resposta dinâmica do desenho em C++ deve seguir balísticas precisas do padrão de medição digital (Ex: decaimento de pico de 20 dB por segundo).
