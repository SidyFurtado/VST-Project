# Phase 14.0 - Release Candidate / Plugin Metadata Polish Summary

## Status

Concluída.

## Scope

Esta fase alterou apenas metadados do alvo JUCE/CMake do AUREQ para preparar um release candidate e corrigir o warning antigo de Bundle ID com espaço. Não houve alteração de APVTS, IDs de parâmetros, DSP, analyzer FFT, presets, UI, `PresetManager`, `PluginProcessor`, CMake raiz, Livros/PDFs ou APIs de user presets.

## Metadados Antigos

```cmake
PRODUCT_NAME "AUREQ"
VERSION "0.1.0"
COMPANY_NAME "ASTRA Audio Suite"
PLUGIN_CODE Aurq
MANUFACTURER_CODE AStr
```

Sem `BUNDLE_ID` explícito, o JUCE gerava o fallback inválido:

```text
com.ASTRA Audio Suite.AUREQ
```

## Metadados Novos

```cmake
PRODUCT_NAME "AUREQ"
VERSION "0.9.0"
BUNDLE_ID "br.com.sidyfurtado.aureq"
COMPANY_NAME "ASTRA Audio"
PLUGIN_CODE Aurq
MANUFACTURER_CODE AStr
```

## Release Label

- Versão binária: `0.9.0`
- Label de release candidate: `0.9.0-rc1`

O `VERSION` do JUCE/CMake foi mantido numérico para reduzir risco de incompatibilidade com hosts.

## Validação

- `cmake --build build`
- `./build/tests/AUREQ_FilterTests`
- Conferência de geração dos artefatos VST3 e Standalone.
- Abertura do Standalone para smoke test.

## Artefatos Esperados

- `build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- `build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
