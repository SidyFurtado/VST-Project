# LUMINAR Smooth Guard Aggressive Calibration (Fase 18.3D)

Este documento resume as modificações matemáticas, o processo de compilação, a execução dos testes e o roteiro de instalação manual do plugin **LUMINAR** após a calibração agressiva do Smooth Guard realizada na Fase 18.3D.

---

## 1. Alterações Realizadas e Justificativa DSP

### Smooth Guard Aggressive Calibration
* **Objetivo**: Tornar o Smooth Guard um processador óbvio e agressivo quando configurado em 100% (comportamento de de-esser rápido para controle drástico de aspereza e sibilância), mantendo a sutilidade e musicalidade em 50% e total transparência em 0%.
* **Alterações**:
  * **Ganho Pré-Detector (Sidechain)**: Aumentado o multiplicador do sinal retificado de `2.5f` para `10.0f` (+300% na amplitude direcionada ao envelope follower). Isso garante que transientes e picos de sibilância cruzem facilmente o limiar matemático e acionem a compressão.
  * **Redução Máxima (`mapSmoothMaxReductionDb`)**: Ajustada a escala do teto de redução máxima de `7.0 dB` para `15.0 dB` em `15.0f * std::pow (smooth, 0.85f)`.
    * A 50%, a redução teórica máxima é de `8.3 dB` (resposta musical presente).
    * A 100%, a redução teórica máxima é de `15.0 dB` (resposta de compressão pesada).
  * **Gain Reduction Clamp (Piso Rígido)**: Reduzido o piso de atenuação em `processSmoothGuard` de `0.50f` (-6 dB) para `0.18f` (aproximadamente -15 dB) para permitir que a redução de ganho calculada atinja o teto de -15 dB de forma livre.

---

## 2. Linhas Alteradas no Código

As alterações foram feitas em [AirProcessor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/02_AIR/DSP/AirProcessor.cpp):

```diff
-        auto harshBand = std::abs (smoothGuardFilters[index].processSample (input)) * 2.5f;
+        auto harshBand = std::abs (smoothGuardFilters[index].processSample (input)) * 10.0f;
```

```diff
-        return std::clamp (std::isfinite (gain) ? gain : 1.0f, 0.50f, 1.0f);
+        return std::clamp (std::isfinite (gain) ? gain : 1.0f, 0.18f, 1.0f);
```

```diff
-        return 7.0f * std::pow (smooth, 0.85f);
+        return 15.0f * std::pow (smooth, 0.85f);
```

---

## 3. Resultados de Compilação e Testes

* **Build**: Compilado com sucesso via CMake (`cmake --build build`) gerando os artefatos `LUMINAR_VST3` e `LUMINAR_Standalone`.
* **Testes Automatizados (`AUREQ_FilterTests`)**:
  * Executado com `./build/tests/AUREQ_FilterTests`.
  * **Status**: `PASS` (100% dos testes validados).

---

## 4. Caminho dos Artefatos Gerados

* **VST3 Plugin**:
  `/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
* **Standalone App**:
  `/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## 5. Comandos de Instalação Manual no macOS (VST3)

Para testar o plugin atualizado com calibração agressiva na DAW (como o FL Studio), execute o bloco abaixo no terminal:

```bash
# Remover versão antiga do VST3
sudo rm -rf "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"

# Copiar a nova versão calibrada
sudo cp -R "/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3" "/Library/Audio/Plug-Ins/VST3/"

# Limpar atributos de quarentena do macOS
sudo xattr -rd com.apple.quarantine "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"
```
