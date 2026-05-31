# LUMINAR Presence & Smooth Guard Audibility Calibration (Fase 18.3C)

Este documento resume as modificações matemáticas, o processo de compilação, a execução dos testes e o roteiro de instalação manual do plugin **LUMINAR** após a calibração de audibilidade realizada na Fase 18.3C.

---

## 1. Alterações Realizadas e Justificativa DSP

### Presença (Presence Stage)
* **Objetivo**: Aumentar a utilidade e audibilidade do knob de Presença a partir de 40-50% mantendo total transparência em 0%.
* **Alterações**:
  * **Ganho base (`presenceWetGain`)**: Aumentado de `2.6f` para `3.2f` (+23% no ganho do sinal wet).
  * **Curva de Mix (`mapPresenceMix`)**: Alterada de `0.42f * pow(presence, 0.78f)` para `0.55f * pow(presence, 0.60f)`.
    * A potência de `0.60` lineariza o comportamento no meio do curso. Em 50% do knob, a contribuição total wet sobe em cerca de **83%** em relação à calibração anterior.
  * **Drive Máximo (`mapPresenceDrive`)**: Elevado de `1.20f + presence * 2.30f` para `1.20f + presence * 2.80f` (máximo de `4.00`), adicionando saturação musical sutil nos limites superiores.

### Smooth Guard
* **Objetivo**: Tornar o detector de sibilância/aspereza funcional (atuando como um de-esser rápido a 100% e protetor musical a 50%).
* **Alterações**:
  * **Ganho Pré-Detector (Sidechain)**: Multiplicado o sinal retificado por `2.5f` em `processSmoothGuard` antes de computar o envelope follower, garantindo que o sinal de alta frequência cruze o threshold definido.
  * **Constantes de Tempo**: Ataque ajustado de `10 ms` para `4 ms` e Release ajustado de `130 ms` para `90 ms` para captura ágil dos transientes de sibilância.
  * **Redução Máxima**: Elevada de `5.0 dB` para `7.0 dB` em `mapSmoothMaxReductionDb`, garantindo que o ganho final atinja a atenuação máxima permitida pelo limitador (`-6 dB` ou ganho de `0.50f` no clamp) em passagens muito estridentes.

---

## 2. Linhas Alteradas no Código

As alterações foram feitas exclusivamente em [AirProcessor.cpp](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/02_AIR/DSP/AirProcessor.cpp):

```diff
-        smoothAttackCoeff = std::exp (-1.0f / static_cast<float> (sampleRate * 0.010));
-        smoothReleaseCoeff = std::exp (-1.0f / static_cast<float> (sampleRate * 0.130));
+        smoothAttackCoeff = std::exp (-1.0f / static_cast<float> (sampleRate * 0.004));
+        smoothReleaseCoeff = std::exp (-1.0f / static_cast<float> (sampleRate * 0.090));
```

```diff
-        constexpr float presenceWetGain = 2.6f;
+        constexpr float presenceWetGain = 3.2f;
```

```diff
-        auto harshBand = std::abs (smoothGuardFilters[index].processSample (input));
+        auto harshBand = std::abs (smoothGuardFilters[index].processSample (input)) * 2.5f;
```

```diff
-        return 1.20f + presence * 2.30f;
+        return 1.20f + presence * 2.80f;
```

```diff
-        return 0.42f * std::pow (presence, 0.78f);
+        return 0.55f * std::pow (presence, 0.60f);
```

```diff
-        return 5.0f * std::pow (smooth, 0.85f);
+        return 7.0f * std::pow (smooth, 0.85f);
```

---

## 3. Resultados de Compilação e Testes

* **Build**: Sucesso total na compilação do plugin e standalone via CMake (`cmake --build build`).
* **Testes Automatizados (`AUREQ_FilterTests`)**:
  * Executado com `./build/tests/AUREQ_FilterTests`.
  * **Status**: `PASS` (todos os testes DSP e estabilidade do filtro concluídos com sucesso).

---

## 4. Caminho dos Artefatos Gerados

* **VST3 Plugin**:
  `/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3`
* **Standalone App**:
  `/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/Standalone/LUMINAR.app`

---

## 5. Comandos de Instalação Manual no macOS (VST3)

Use o bloco de comandos abaixo no seu terminal para copiar o plugin compilado diretamente para a pasta de Plug-ins do sistema e limpar a quarentena do macOS. Isso permitirá testar o plugin calibrado imediatamente no FL Studio ou outra DAW de sua escolha:

```bash
# Remover versão antiga do VST3
sudo rm -rf "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"

# Copiar a nova versão calibrada
sudo cp -R "/Users/sidyziin/Documents/VST - Project/build/plugins/02_AIR/LUMINAR_artefacts/VST3/LUMINAR.vst3" "/Library/Audio/Plug-Ins/VST3/"

# Remover atributos de quarentena do macOS
sudo xattr -rd com.apple.quarantine "/Library/Audio/Plug-Ins/VST3/LUMINAR.vst3"
```
