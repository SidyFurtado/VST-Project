# Phase 19.4.1 - GRAVITY UTF-8 Preset Menu Hotfix Summary

Date: 2026-06-02

## Summary

Fixed mojibake in GRAVITY preset menu strings that use Brazilian Portuguese accents by forcing affected static C++ strings through JUCE UTF-8 conversion.

## Changes

- Added local `utf8()` helper in `PluginEditor.cpp`.
- Updated accented submenu labels:
  - `Gêneros`
  - `Presets do Usuário`
  - `Forró`
- Updated factory preset menu item rendering so accented preset names such as `Tamborzão`, `Mandelão`, and `Violão` are converted explicitly as UTF-8.

## Scope Guard

- No DSP changes.
- No layout changes.
- No button coordinate changes.
- No APVTS parameter changes.

## Build Validation

Build was intentionally not run in this step. Awaiting user command:

```bash
cmake --build build --config Release
```
