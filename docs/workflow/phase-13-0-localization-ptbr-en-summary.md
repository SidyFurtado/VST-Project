# Phase 13.0 - Localization PT-BR / EN Summary

## Status

Concluida.

## Scope

Phase 13.0 added a centralized localization layer for the AUREQ interface with Brazilian Portuguese as the default language and English as an alternate visual preference. The work is UI-text only: no APVTS parameters, parameter IDs, DSP, analyzer FFT, preset values, factory preset names, user preset names, or user preset APIs were changed.

## Architecture

- Added `plugins/01_EQ/UI/Localization.h`.
- Added `plugins/01_EQ/UI/Localization.cpp`.
- Introduced `AureqLanguage` with `PortugueseBR` and `English`.
- Introduced `TextKey` for interface strings.
- Centralized translations through `getText(TextKey, AureqLanguage)`.
- Added helper text mapping for filter types, channel modes, preset categories, and genre source labels.
- Registered `Localization.cpp` in the plugin-local `plugins/01_EQ/CMakeLists.txt`.

## Runtime Behavior

- `AureqAudioProcessorEditor` owns `currentLanguage`.
- The default language is PT-BR.
- The language can be changed from the Settings menu:
  - `Config. > Idioma > Portugues (Brasil)`
  - `Config. > Idioma > English`
- Changing language updates visible editor text immediately through `applyLocalizedTexts()`.
- Popup menus are translated when opened.
- `EQGraphView` receives the selected language through `setLanguage()`.

## Persistence

The language preference is stored with `juce::PropertiesFile` as a local application preference. It is intentionally not stored in APVTS and is not included in plugin state serialization.

## Translated Areas

- Top Bar buttons.
- Settings menu.
- Preset browser category labels.
- User Presets actions and dialogs.
- A/B context menu.
- Undo/Redo labels.
- Bottom Panel labels.
- EQGraphView floating HUD.
- Filter type and channel/source display labels.

## Preserved Technical Terms

- `Bypass`, `A/B`, `DYN`, `Mid`, `Side`, `Mix Bus`, `Master`, `Kick`, `Hi-Hat`, `Synth`, `Lead`, `Pad`, and button abbreviations `LC`, `HC`, `LS`, `HS`, `NT`, `BP` remain compact where that is clearer for audio users or safer for the current layout.
- Units such as `Hz`, `kHz`, `dB`, and `ms` were not translated.
- Factory preset names and user preset names were not translated.

## Safety Notes

- The LookAndFeel no longer depends on visible English strings to identify Reset, Theme, or Bypass buttons. It uses stable component IDs instead.
- Language changes do not capture undo checkpoints.
- User preset load/save/delete/refresh/reveal behavior was preserved.
- The preset browser ID mapping was not changed.
