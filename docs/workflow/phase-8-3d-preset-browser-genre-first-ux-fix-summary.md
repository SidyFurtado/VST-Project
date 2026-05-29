# Resumo Técnico: Fase 8.3D — Preset Browser Genre-First UX Fix

Este documento registra a correção cirúrgica da hierarquia visual do preset browser do AUREQ.

## 1. Causa do Problema

A Fase 8.3C implementou os presets genre-first e criou a estrutura interna `Genres > Gênero > Fonte > Preset`, mas o menu principal ainda apresentava primeiro as categorias antigas:

```text
Vocals
Drums
Bass / 808
Instruments
Mix Bus / Master
Genres
```

Isso fazia a navegação parecer source-first, mesmo com a seção `Genres` pronta.

## 2. Correção Aplicada

O `showPresetBrowserMenu()` foi reorganizado para tornar `Genres` o primeiro fluxo musical após `Default Flat`.

Estrutura nova:

```text
Save Current Preset...
Default Flat

Genres >
    Forro >
        Vocal >
        Accordion >
        Zabumba >
    Trap >
        Vocal >
        808 >
        Hi-Hat >
    Hip-Hop >
        Rap Vocal >
        Snare >
        Beat Bus >
    Funk BR >
    Rock >
    Sertanejo >
    Pop >
    Gospel >
    Electronic >

By Source >
    Vocals >
    Drums >
    Bass / 808 >
    Instruments >
    Mix Bus / Master >
    Dynamic EQ >
    Utility / Cleanup >

User Presets >
```

## 3. Preservações

- Lista de presets: não alterada.
- Presets musicais: não alterados.
- APVTS layout/IDs: não alterado.
- DSP: não alterado.
- Analyzer FFT: não alterado.
- User Presets: save/load/delete/refresh/reveal preservados.
- Previous/Next: navegação linear preservada.
- `PresetManager.cpp`: não alterado nesta fase.
- Pasta `Livros/` e PDFs: não alterados.

## 4. Detalhes Técnicos

As categorias antigas foram agrupadas em um novo `juce::PopupMenu` local chamado `By Source`.

A função local que adiciona menus de categoria passou a receber o menu pai como argumento, permitindo reutilizar a mesma lógica dentro de `By Source` sem duplicar código nem alterar o `PresetManager`.

O menu `Genres` continua usando:

- `category = "Genres"`
- `subcategory = nome do gênero`
- tag `source:*` para montar o terceiro nível
- mapa local de IDs para carregar o índice correto do factory preset

## 5. Validação

Build executado:

```bash
cmake --build build
```

Status: **aprovado**.

Testes executados:

```bash
./build/tests/AUREQ_FilterTests
```

Status: **aprovado**.

## 6. Caminhos dos Artefatos

- VST3: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- Standalone: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

## 7. Próximo Passo Recomendado

Executar a validação manual no Standalone:

- abrir o preset browser;
- confirmar `Genres` antes de `By Source`;
- abrir `Genres > Trap > 808`;
- abrir `Genres > Forro > Vocal`;
- abrir `Genres > Rock > Electric Guitar`;
- abrir `By Source > Vocals`;
- confirmar User Presets;
- carregar presets e verificar `presetLabel`;
- confirmar Previous/Next linear.
