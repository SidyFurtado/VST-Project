# GRAVITY Implementation Plan

## Phase 19.3 - UI/UX Reconstruction and R-Comp Layout Alignment

Status: edited, awaiting user-triggered build validation.

### Scope

- Reconstruct the GRAVITY editor around a strict 800px-wide, three-column workhorse compressor layout.
- Preserve the existing APVTS parameter IDs and attachments.
- Preserve the existing DSP path and metering timer.
- Keep ASTRA Dark Glass styling with Crimson accent `#E84040`.

### UI Architecture

- Column 1: `gravity_thresh` vertical fader with Input VU on the left and inverted Gain Reduction VU on the right.
- Column 2: central Transfer Curve display above the `gravity_ratio` vertical fader.
- Column 3: `gravity_gain` vertical fader with Output VU beside it.
- Footer: compact horizontal `gravity_attack` and `gravity_release` controls plus `gravity_mode` and `gravity_arc` state buttons.

### Transfer Curve

- Reads existing `gravity_thresh` and `gravity_ratio` values from APVTS raw parameter pointers.
- Draws a dark glass graph panel, subtle grid, dry diagonal reference line, threshold guide, and Crimson transfer curve.
- Uses a 6 dB soft-knee model matching the compressor concept:
  - Below knee: unity.
  - Inside knee: quadratic blend.
  - Above knee: compressed slope based on ratio.

### Metering

- Timer remains at 30 FPS.
- Input and Output meters remain bottom-up VU bars.
- Gain Reduction meter now fills top-down using Amber `#F6C90E`.
- Gain Reduction peak hold was adjusted for negative gain-reduction values.

### Validation

Required after user approval:

```bash
cmake --build build --config Release
```

## Phase 23.5A - AU Format Validation / Manufacturer Code Fix

Status: completed.

### Scope

- Replace `MANUFACTURER_CODE AStr` with `PLUGIN_MANUFACTURER_CODE AStr` in `juce_add_plugin(GRAVITY)`.
- Preserve `PRODUCT_NAME "GRAVITY"`, `PLUGIN_CODE Grvt`, `BUNDLE_ID "br.com.sidyfurtado.gravity"`, version, DSP, APVTS, presets, UI, and installers.
- Rebuild artifacts and confirm the generated AU uses manufacturer `AStr` instead of JUCE fallback `Manu`.
- Validate with `cmake --build build` and `./build/tests/AUREQ_FilterTests`.

## Phase 23.5C - macOS AU-First Suite PKG Generation

Status: completed.

### Scope

- Copy `GRAVITY.component` into `installer/macOS/au-first-suite/payload/Library/Audio/Plug-Ins/Components/`.
- Generate `ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU-component.pkg` and `ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`.
- Validate non-destructively with `pkgutil --expand`, `lsbom`, and `pkgutil --check-signature`.
- Confirm the package is AU-only, with no VST3, no Standalone, no `/Applications` install destination, and no `/Library/Audio/Plug-Ins/VST3` payload.
- Do not run the installer, do not install to `/Library`, do not use `sudo`, and do not run `auval` in this phase.

## Phase 23.5D-R - AU Duplicate Cleanup & AudioUnit Cache Refresh

Status: completed.

### Scope

- Backup legacy components in `backups/au-user-duplicates-cleanup/`.
- Remove legacy user duplicate component `~/Library/Audio/Plug-Ins/Components/GRAVITY.component`.
- Delete `~/Library/Caches/AudioUnitCache/com.apple.audiounits.cache`.
- Restart `AudioComponentRegistrar`.
- Confirm successful validation of `/Library` component with `auval` under manufacturer `AStr`.

## Phase 23.5E - Download Page / Beta Docs Update

Status: completed.

### Scope

- Update download links and format specs in `docs/index.html` and `docs/site/index.html`.
- Replace single warning line with structural warnings container block containing security, cache and SmartScreen notes.
- Refactor private beta instruction guides and installation guide.
- Add cache clearing warnings to the suite README.txt.
- Register Phase 23.5E in monorepo checklists and plans.

## Phase 19.5 - CI/CD and Official Installer Integration

Status: edited, awaiting user-triggered build validation.

### Scope

- Integrate GRAVITY into the official ASTRA Suite GitHub Actions workflow.
- Add GRAVITY as a third official product installer parallel to AUREQ and LUMINAR.
- Keep the legacy AUREQ-only Windows workflow unchanged.

### GitHub Actions

- Updated `.github/workflows/astra-suite-build.yml`.
- Added `GRAVITY_All` to macOS and Windows CMake build targets.
- Added GRAVITY VST3 and Standalone staging on macOS.
- Added GRAVITY VST3 and Standalone staging on Windows.
- Added macOS `pkgbuild` and `productbuild` steps for GRAVITY.
- Added Windows Inno Setup compilation for GRAVITY.
- Added macOS and Windows GRAVITY artifact uploads.

### macOS Installer

- Added `installer/macOS/gravity/distribution/Distribution.xml`.
- Added `installer/macOS/gravity/resources/welcome.html`.
- Added `installer/macOS/gravity/resources/license.html`.
- Package identifiers:
  - `br.com.sidyfurtado.gravity.vst3`
  - `br.com.sidyfurtado.gravity.standalone`

### Windows Installer

- Added `installer/Windows/gravity/scripts/GRAVITY-installer.iss`.
- Windows install destinations:
  - VST3: `C:\Program Files\Common Files\VST3\GRAVITY.vst3`
  - Standalone: `C:\Program Files\ASTRA Audio\GRAVITY`
- Unique GRAVITY AppId:
  - `{4F6CF39D-83DE-4A4D-8E4A-6A4A5F0C2B17}`

### Validation

Required after user approval:

```bash
cmake --build build --config Release
```

## Phase 19.4.1 - UTF-8 Preset Menu Hotfix

Status: edited, awaiting user-triggered build validation.

### Scope

- Fix mojibake in GRAVITY preset menu labels containing Brazilian Portuguese accents.
- Keep preset hierarchy, layout, DSP, APVTS IDs, and top-bar button coordinates unchanged.

### Implementation

- Added a local UTF-8 helper in `PluginEditor.cpp`:

```cpp
inline juce::String utf8 (const char* text)
{
    return juce::String (juce::CharPointer_UTF8 (text));
}
```

- Routed accented popup menu strings through the helper:
  - `Gêneros`
  - `Presets do Usuário`
  - `Forró`
  - Factory preset display names such as `Tamborzão`, `Mandelão`, and `Violão`.

### Validation

Required after user approval:

```bash
cmake --build build --config Release
```

## Phase 19.4 - Hierarchical Preset System

Status: edited, awaiting user-triggered build validation.

### Scope

- Add a discrete `Presets` button to the GRAVITY top bar.
- Build a hierarchical `juce::PopupMenu` with factory presets, source-based presets, and user presets.
- Keep factory presets hardcoded in C++ so GRAVITY works out of the box.
- Preserve the approved Phase 19.3 main control layout.
- Preserve DSP implementation and APVTS parameter IDs.

### Factory Menu

- Default Flat resets parameters to the current APVTS defaults.
- Salvar Preset Atual... opens a save dialog for local preset files.
- Gêneros submenu:
  - Forró: Vocal, Sanfona, Zabumba, Baixo, Mix Bus.
  - Trap: 808 Punch, Snare Snap, Vocal.
  - Hip-Hop.
  - Funk BR: Tamborzão, Mandelão.
  - Rock.
  - Sertanejo: Violão, Acordeon, Voz Principal.
  - Pop.
  - Gospel.
  - Electronic.
- Por Fonte submenu:
  - Bateria, Baixo, Voz, Violão, Master.

### User Presets

- User presets are read from:

```text
~/Music/ASTRA Audio/GRAVITY/Presets/
```

- Supported file extensions:
  - `.preset`
  - `.xml`

### APVTS Loading Strategy

- Factory presets call `GravityAudioProcessor::applyPresetValues()`.
- Values are written via `beginChangeGesture()`, `setValueNotifyingHost()`, and `endChangeGesture()`.
- User presets use the same APVTS XML format as plugin state persistence.

### Validation

Required after user approval:

```bash
cmake --build build --config Release
```
