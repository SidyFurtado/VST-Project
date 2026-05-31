# LUMINAR Smooth Guard Precision Calibration (Fase 18.5)

Este documento resume as modificações matemáticas, o processo de compilação, a execução dos testes e o roteiro de instalação manual do plugin **LUMINAR** após a calibração cirúrgica (Surgical Mode) do Smooth Guard realizada na Fase 18.5.

---

## 1. Objetivo e Filosofia DSP

Nesta fase, ativamos o detector do **Smooth Guard** de forma cirúrgica (Surgical Mode) sobre o barramento *wet* (Presence + Texture + Air). O objetivo foi implementar um compressor dinâmico de sibilância robusto que reduza asperezas agudas sem alterar o som base aprovado de Presence, Air ou da Texture Layer.

### Especificação DSP implementada:
1. **Sidechain Filtering**: O sinal passa pelo filtro biquad passa-banda de harshness/sibilância centrado em ~6.5 kHz.
2. **Retificação e Rastreamento**: É obtida a amplitude absoluta do sinal filtrado e atualizado o envelope follower com base nos coeficientes de ataque (10ms) e release (130ms) configurados.
3. **Threshold Interno Fixo**: Definido o limiar em `0.025f` (~ -32 dB) para capturar picos de sibilância típicos em vocais sem necessitar de ganho pré-detector excessivo ou controles externos de threshold.
4. **Curva de Redução Soft-Knee**: Quando o envelope excede o limiar, o excesso é comprimido usando a fórmula soft-knee:
   $$\text{reductionShape} = \frac{\text{excess}}{\text{excess} + 0.050f}$$
   A atenuação máxima em dB é dimensionada de forma proporcional a `smoothAmount`:
   $$\text{reductionDb} = -12.0 \times \text{smoothAmount} \times \text{reductionShape}$$
5. **Clamp Limiter (Piso de Segurança)**: O ganho de atenuação linear é limitado a `0.25f` (máximo de -12 dB de redução de sibilância) para evitar qualquer abafamento severo ou muting indesejado do barramento wet.

---

## 2. Código Implementado (`processSmoothGuard`)

O método `processSmoothGuard` em [AirProcessor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/02_AIR/DSP/AirProcessor.cpp) foi totalmente atualizado para:

```cpp
    float AirProcessor::processSmoothGuard (int channel, float input, float smoothAmount) noexcept
    {
        const auto smooth = std::clamp (smoothAmount, 0.0f, 1.0f);

        if (smooth <= 0.0001f)
            return 1.0f;

        const auto index = static_cast<size_t> (channel);

        // 1. Filtragem do Sidechain (Banda de Sibilância ~6.5 kHz)
        auto filtered = smoothGuardFilters[index].processSample (input);
        if (! std::isfinite (filtered))
            filtered = 0.0f;

        // 2. Retificação e Envelope Follower
        const auto absFiltered = std::abs (filtered);
        auto& envelope = smoothGuardEnvelopes[index];
        const auto coeff = absFiltered > envelope ? smoothAttackCoeff : smoothReleaseCoeff;
        envelope = coeff * envelope + (1.0f - coeff) * absFiltered;

        // 3. Threshold Interno Fixo (~ -32 dB)
        constexpr auto threshold = 0.025f;

        // 4. Cálculo da Atenuação Dinâmica com Curva Soft-Knee Proporcional
        const auto excess = std::max (0.0f, envelope - threshold);
        if (excess <= 0.0f)
            return 1.0f;

        const auto reductionShape = excess / (excess + 0.050f);
        const auto maxReductionDb = 12.0f * smooth;
        const auto reductionDb = -maxReductionDb * reductionShape;
        const auto gain = std::pow (10.0f, reductionDb / 20.0f);

        // 5. Piso de Segurança (Clamp a -12 dB máx)
        return std::clamp (std::isfinite (gain) ? gain : 1.0f, 0.25f, 1.0f);
    }
```

---

## 3. Resultados de Compilação e Testes

* **Build**: Concluído com sucesso via CMake. Todos os artefatos `LUMINAR.vst3` e `LUMINAR.app` foram gerados sem erros.
* **Testes Automatizados (`AUREQ_FilterTests`)**:
  * Executado com `./build/tests/AUREQ_FilterTests`.
  * **Status**: `PASS` (100% dos testes concluídos com sucesso).

---

## 4. Caminho dos Artefatos Gerados

* **VST3 Plugin**:
  `/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
* **Standalone App**:
  `/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## 5. Comandos de Instalação Manual no macOS (VST3)

Use este bloco de comandos no terminal para copiar o plugin compilado e remover atributos de quarentena do macOS:

```bash
# Remover a versão em bypass anterior
sudo rm -rf "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"

# Copiar a nova build com Smooth Guard ativo (Surgical Mode)
sudo cp -R "/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3" "/Library/Audio/Plug-Ins/VST3/"

# Limpar restrições do macOS
sudo xattr -rd com.apple.quarantine "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"
```
