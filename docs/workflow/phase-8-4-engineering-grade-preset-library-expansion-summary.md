# Resumo Técnico: Fase 8.4 — Engineering-Grade Preset Library Expansion

Esta fase expandiu a biblioteca de factory presets do AUREQ para uma abordagem genre-first orientada a problemas reais de mixagem.

## Escopo

- Factory presets continuam embutidos em `PresetManager.cpp`.
- `Default Flat` continua no topo.
- Browser preservado como `Genres > Genero > Fonte > Preset`, seguido de `By Source` e `User Presets`.
- APVTS layout/IDs, DSP, analyzer FFT, CMake, Livros/PDFs e user presets nao foram alterados.

## Quantidade Final

- Factory presets totais: **148**
- Presets `Genres`: **126**
- Presets `By Source`/toolbox tecnica: **22**, contando `Default Flat`
- Generos cobertos: **9**
- Presets por genero: **14**

## Generos E Fontes Cobertas

### Forro

Vocal, Backing Vocal, Accordion, Zabumba, Percussion, Bass, Mix Bus.

### Trap

Vocal, Adlibs, 808, Kick, Snare / Clap, Hi-Hat, Synth, Mix Bus.

### Hip-Hop

Rap Vocal, Beat Bus, Kick, Snare, Bass, Sample, Mix Bus.

### Funk BR

Vocal, Beat, Kick, Bass / 808, Percussion, Lead / Synth, Mix Bus.

### Rock

Vocal, Electric Guitar, Acoustic Guitar, Bass, Kick, Snare, Drum Bus, Mix Bus.

### Sertanejo

Vocal, Backing Vocal, Acoustic Guitar, Viola, Bass, Percussion, Mix Bus.

### Pop

Vocal, Backing Vocal, Synth, Drums, Bass, Guitar, Mix Bus.

### Gospel

Lead Vocal, Choir, Piano / Keys, Guitar, Bass, Drums, Mix Bus.

### Electronic

Kick, Bass, Synth, Lead, Pad, Percussion, Mix Bus, Master.

## Toolbox By Source

`By Source` foi reduzido para presets genericos de engenharia:

- Vocals: Vocal Presence Safe, Vocal De-Esser Dynamic, Vocal Low-Mid Cleanup, Backing Vocal Tuck
- Drums: Kick Punch Toolkit, Snare Body Toolkit, Drum Bus Punch Safe
- Bass / 808: Bass Tight Low, Bass Side Cleanup, 808 Note Tamer Dynamic
- Instruments: Acoustic Guitar Clarity, Electric Guitar Fizz Control, Piano Keys Clarity
- Mix Bus / Master: Master Air Safe, Mix Bus Gentle Lift, Stereo Side Low Cleanup
- Dynamic EQ: Dynamic De-Harsh 3k, Dynamic Sibilance 6k, Dynamic Low-Mid Control
- Utility / Cleanup: Low Cut Cleanup, Sub Rumble Clean

Presets com identidade de genero foram movidos para `Genres`, evitando duplicacao direta entre a biblioteca principal e a toolbox.

## Estrategia Musical

- Mix bus/master usam boosts pequenos, normalmente entre `+0.5` e `+1.5 dB`.
- Fontes individuais usam boosts conservadores, normalmente ate `+2.5 dB`.
- Cortes corretivos ficam mais fortes quando o nome indica problema claro, como mud, boxiness, harshness ou rumble.
- Dynamic EQ negativo foi usado para sibilancia, harshness, mud e instabilidade de low-end.
- Side foi usado somente para ar, largura e limpeza lateral.
- Low-end em Side foi limpo, nao reforcado.
- `48 dB/oct` ficou reservado para cleanup forte de rumble/side low cleanup.
- Q largo foi usado para shaping musical; Q mais estreito ficou em controle corretivo.

## Browser

`PluginEditor.cpp` foi atualizado somente para a ordem/rotulagem das fontes novas no submenu `Genres`.

Fontes novas/confirmadas:

- Snare / Clap
- Beat
- Drums
- Sample
- Guitar
- Lead
- Lead / Synth
- Pad
- Master

## Validacao

Build:

```bash
cmake --build build
```

Status: **aprovado**.

Testes:

```bash
./build/tests/AUREQ_FilterTests
```

Status: **aprovado**.

Resultado:

```text
SUCCESS: All AUREQ DSP Filter Tests Passed!
```

## Artefatos

- VST3: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- Standalone: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`

## Validacao Manual Recomendada

- `Genres > Trap > Vocal`
- `Genres > Trap > 808`
- `Genres > Forro > Accordion`
- `Genres > Rock > Electric Guitar`
- `Genres > Gospel > Choir`
- `Genres > Electronic > Master`
- `By Source` com toolbox tecnica
- `User Presets`
- carregar presets e confirmar `presetLabel`
- confirmar Previous/Next linear
- confirmar Save/Load/Delete/Refresh/Reveal User Presets
