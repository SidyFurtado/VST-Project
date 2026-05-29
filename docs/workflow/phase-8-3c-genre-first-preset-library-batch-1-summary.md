# Resumo Técnico: Fase 8.3C — Genre-First Preset Library Batch 1

Este documento registra a implementação do primeiro lote controlado de presets de fábrica genre-first do AUREQ.

## 1. Objetivo da Fase

Implementar uma leva inicial de presets organizados como:

```text
Genres >
    Gênero >
        Instrumento/Fonte >
            Presets
```

A fase substituiu a seção `Genres` antiga em vez de adicionar novos presets por cima dela. `Default Flat` continua no topo do menu e os user presets permanecem separados.

## 2. Escopo Preservado

- APVTS layout/IDs: não alterado.
- DSP: não alterado.
- Analyzer FFT: não alterado.
- CMake: não alterado.
- Pasta `Livros/`: não alterada.
- PDFs: não lidos, movidos, editados, copiados ou convertidos nesta fase.
- User presets: save/load/delete/refresh/reveal preservados.
- Arquivos externos de preset: não criados.
- Busca, favoritos e tags visuais: não implementados.

## 3. Biblioteca Final

Quantidade final de factory presets: **80**.

Mudança na seção `Genres`:

- Presets `Genres` antigos substituídos: **11**.
- Presets genre-first implementados no Batch 1: **36**.
- Crescimento líquido da biblioteca: **+25 presets**.

Como referência, a biblioteca anterior tinha 55 factory presets. Após substituir 11 presets antigos por 36 novos, o total passou para 80.

## 4. Presets Implementados

### Forro

- Vocal: `Forro Vocal Front Clarity`, `Forro Vocal De-Harsh Safe`
- Accordion: `Forro Accordion Presence`
- Zabumba: `Forro Zabumba Low Control`

### Trap

- Vocal: `Trap Vocal Cut Through`, `Trap Vocal De-Esser`
- 808: `Trap 808 Tight Mono Focus`
- Hi-Hat: `Trap Hi-Hat Softener`

### Hip-Hop

- Rap Vocal: `Hip-Hop Rap Vocal Forward`, `Hip-Hop Rap Vocal Low Cleanup`
- Snare: `Hip-Hop Snare Crack`
- Beat Bus: `Hip-Hop Beat Bus Glue`

### Funk BR

- Vocal: `Funk BR Vocal Clarity`
- Bass / 808: `Funk BR Bass Mono Lock`
- Kick: `Funk BR Kick Punch`
- Percussion: `Funk BR Percussion Brightness`

### Rock

- Vocal: `Rock Vocal Presence`
- Electric Guitar: `Rock Electric Guitar Fizz Tame`
- Snare: `Rock Snare Body`
- Drum Bus: `Rock Drum Bus Punch`

### Sertanejo

- Vocal: `Sertanejo Vocal Silk Air`, `Sertanejo Vocal Warmth`
- Acoustic Guitar: `Sertanejo Acoustic Guitar Clarity`
- Viola: `Sertanejo Viola Presence`

### Pop

- Vocal: `Pop Vocal Gloss Control`, `Pop Vocal Air`
- Bass: `Pop Bass Tight`
- Mix Bus: `Pop Mix Gentle Width`

### Gospel

- Lead Vocal: `Gospel Lead Vocal Lift Safe`
- Choir: `Gospel Choir Air`
- Piano / Keys: `Gospel Piano Clarity`
- Mix Bus: `Gospel Mix Clean Lift`

### Electronic

- Kick: `Electronic Kick Tight Punch`
- Bass: `Electronic Bass Tight Low`
- Synth: `Electronic Synth Presence`
- Mix Bus: `Electronic Mix Side Low Clean`

## 5. Metadata

O struct `Preset` foi mantido como estava:

```text
name
category
subcategory
tags
parameters
```

Para presets genre-first:

- `category = "Genres"`
- `subcategory = nome do gênero`
- `tags` inclui uma entrada `source:*`

Exemplos:

- `source:vocal`
- `source:808`
- `source:electric guitar`
- `source:mix bus`

## 6. Menu Genres

O browser agora monta a seção `Genres` em três níveis:

```text
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
        Vocal >
        Bass / 808 >
        Kick >
        Percussion >
    Rock >
        Vocal >
        Electric Guitar >
        Snare >
        Drum Bus >
    Sertanejo >
        Vocal >
        Acoustic Guitar >
        Viola >
    Pop >
        Vocal >
        Bass >
        Mix Bus >
    Gospel >
        Lead Vocal >
        Choir >
        Piano / Keys >
        Mix Bus >
    Electronic >
        Kick >
        Bass >
        Synth >
        Mix Bus >
```

## 7. Extração de Source

O `source` é extraído em `showPresetBrowserMenu()` lendo as tags de cada preset e procurando a primeira tag que começa com `source:`.

Depois disso:

- O trecho após `source:` é usado para agrupar presets por fonte.
- A comparação é case-insensitive.
- O rótulo visual do submenu é formatado por uma pequena tabela local, por exemplo `hi-hat` vira `Hi-Hat`.

## 8. ID Mapping

O browser deixou de depender de `presetIndex + 1`.

Foi criado um mapa local:

```text
menu item ID -> índice real do factory preset
```

Esse mapa é preenchido enquanto os itens são adicionados ao menu e é capturado pelo callback assíncrono. Assim, presets em submenus de terceiro nível carregam o índice correto.

Os IDs de user preset continuam em faixas altas separadas:

- `10000`: Save Current Preset
- `20000+`: carregar user presets
- `29000`: Refresh User Presets
- `29001`: Reveal Presets Folder
- `30000+`: deletar user presets

## 9. Cuidados Musicais

- Mix bus usa ganhos pequenos e evita movimentos agressivos.
- Dynamic EQ negativo foi usado para controle de sibilância, harshness e low-end instável.
- Dynamic EQ positivo não foi usado de forma ampla; os realces continuam conservadores.
- Side foi usado para limpeza/largura, evitando reforço grave lateral.
- Q estreito foi reservado para controle corretivo.
- Slopes fortes foram usados somente onde a intenção é limpeza clara.

## 10. Validação

Build executado:

```bash
cmake --build build
```

Resultado:

```text
[100%] Built target AUREQ_FilterTests
```

Status: **aprovado**.

Testes executados:

```bash
./build/tests/AUREQ_FilterTests
```

Resultado:

```text
[PASS] Biquad Coefficient Tests completed successfully.
[PASS] EQBand Block Processing Tests completed successfully.
[PASS] Filter Stability and Safety Tests completed successfully.
SUCCESS: All AUREQ DSP Filter Tests Passed!
```

Status: **aprovado**.

## 11. Caminhos dos Artefatos

- VST3: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- Standalone: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
- Testes: `/Users/sidyziin/Documents/VST - Project/build/tests/AUREQ_FilterTests`

## 12. Próximo Passo Recomendado

Executar validação manual no plugin:

- abrir o preset browser;
- confirmar `Default Flat` no topo;
- navegar por `Genres > Forro > Vocal`;
- navegar por `Genres > Trap > 808`;
- navegar por `Genres > Rock > Electric Guitar`;
- navegar por `Genres > Gospel > Choir`;
- carregar presets genre-first e conferir `presetLabel`;
- confirmar user presets;
- confirmar previous/next linear.

Depois da validação auditiva, avançar para um Batch 2 menor, priorizando lacunas reais percebidas no uso.
