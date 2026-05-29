# Phase 13.2 - UTF-8 Accent Support Fix Summary

## Status

Concluída.

## Causa Provável

`Localization.cpp` estava salvo como UTF-8, mas as strings acentuadas eram retornadas como literais `const char*` simples. A validação visual indicou mojibake na UI, então a causa provável é ambiguidade de conversão entre literal C++ UTF-8 e `juce::String` em algum ponto do pipeline de build/renderização.

## Estratégia Aplicada

Foi criado um helper privado em `Localization.cpp`:

```cpp
static juce::String utf8 (const char* text)
{
    return juce::String (juce::CharPointer_UTF8 (text));
}
```

As strings PT-BR com acentos agora passam por esse helper, mantendo os acentos corretos no código e criando `juce::String` explicitamente a partir de UTF-8.

## Strings Blindadas

- `Gêneros`
- `Presets do Usuário`
- `Sem Presets do Usuário`
- `Atualizar Presets do Usuário`
- `Apagar Preset do Usuário...`
- `A pasta de presets do usuário não pode ser aberta.`
- `O preset não pode ser salvo. Tente outro nome.`
- `Digite um nome para este preset do usuário.`
- `O preset do usuário não pode ser movido para a Lixeira.`
- `Português (Brasil)`
- `SAÍDA`
- `SELECIONE UMA BANDA NO GRÁFICO`
- `Estéreo`
- `Percussão`
- `Violão`

## Arquivos Alterados

- `plugins/01_EQ/UI/Localization.cpp`
- `plugins/01_EQ/UI/PluginEditor.cpp`
- `plugins/01_EQ/Docs/implementation-plan.md`

## Arquivos Criados

- `docs/workflow/phase-13-2-utf8-accent-support-fix-summary.md`

## Segurança

Não foram alterados APVTS layout, IDs de parâmetros, DSP, analyzer FFT, valores musicais de presets, nomes de factory presets, nomes de user presets, CMake, `PresetManager`, `PluginProcessor`, Livros/PDFs ou APIs de user presets.

## Ajuste Visual Adicional

A captura do Standalone mostrou que o separador UTF-8 do footer `ASTRA Audio Suite · v0.1.0` ainda aparecia como `Â·`. Esse literal hardcoded em `PluginEditor.cpp` também foi convertido com `juce::CharPointer_UTF8`, sem alterar layout ou copy.

## Validação

- `cmake --build build`
- `./build/tests/AUREQ_FilterTests`

## Validação Manual Necessária

Confirmar no Standalone:

- `Português (Brasil)`
- `Gêneros`
- `Presets do Usuário`
- `FREQ`
- `GRÁFICO`
- `Estéreo`
- `Percussão`
- `Violão`
- Dialogs de salvar/deletar user preset
- Troca para English
- Persistência ao fechar/reabrir
