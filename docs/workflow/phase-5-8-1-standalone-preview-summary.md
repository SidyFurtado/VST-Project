# Resumo de Validação Técnica: Fase 5.8.1 — Preview Standalone do AUREQ

Este documento resume as implementações realizadas para habilitar a compilação do formato Standalone do equalizador **AUREQ**, permitindo visualização direta e testes de interface de forma autônoma.

---

## 1. Objetivo
Habilitar a geração do executável Standalone do plugin `AUREQ` em conjunto com o binário `VST3` para permitir o preview rápido e testes da interface gráfica sem a necessidade de hospedar o plugin em uma DAW ou utilitários externos.

---

## 2. Skills Consultadas
* [.antigravity/skills/skill-index.md](file:///.antigravity/skills/skill-index.md)
* [.antigravity/skills/safety-guardian.skill.md](file:///.antigravity/skills/safety-guardian.skill.md)
* [.antigravity/skills/juce-cmake-engineer.skill.md](file:///.antigravity/skills/juce-cmake-engineer.skill.md)
* [.antigravity/skills/qa-build-validator.skill.md](file:///.antigravity/skills/qa-build-validator.skill.md)

---

## 3. Arquivos Alterados
* **[CMakeLists.txt](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/CMakeLists.txt)**: A diretiva `FORMATS` do comando `juce_add_plugin(AUREQ ...)` foi alterada de `VST3` para `VST3 Standalone`.

---

## 4. Confirmações de Escopo e Segurança
* **DSP e UI Intocados**: Nenhum arquivo contendo algoritmos matemáticos ou de áudio (pasta `DSP/` ou `PluginProcessor`) ou arquivos da interface (`UI/`) foi alterado. O comportamento sonoro e estético permanece idêntico.
* **Preservação de Parâmetros**: O número de parâmetros permaneceu em exatamente 52 e o APVTS continua operando normalmente.

---

## 5. Resultado do Build e Caminhos

O build foi executado via CMake e finalizou com sucesso de forma limpa.

* **Caminho do Binário VST3 (continua sendo gerado)**:
  `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
* **Caminho do Aplicativo Standalone**:
  `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

---

## 6. Como Abrir o App Standalone no macOS
Você pode abrir o executável diretamente do terminal usando o utilitário `open`:

```bash
open build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app
```
