# AUREQ 0.9.0-rc1 — Private Beta Instructions

Thank you for testing AUREQ.

This is a private macOS beta of the AUREQ VST3 plug-in by ASTRA Audio.

## Download

Beta DMG:

```text
installer/macOS/dmg/output/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg
```

## Installation

1. Open `AUREQ-0.9.0-rc1-macOS-plugin-only.dmg`.
2. Double-click `AUREQ Installer.pkg`.
3. Complete the installer steps.
4. The VST3 plug-in will be installed at:

```text
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

5. Open your DAW and rescan VST3 plug-ins.

## DAW Rescan

- FL Studio: `Plugin Manager > Find installed plugins`
- Ableton Live: `Preferences > Plug-Ins > Rescan`
- Reaper: `Options > Preferences > Plug-ins > VST > Re-scan`
- Studio One / Cubase / Bitwig: use the DAW's plug-in manager/rescan flow if available.

Logic Pro is not a direct VST3 target. Logic validation will require AU support or a wrapper in a future phase.

## Security Notice

This beta is unsigned and not notarized.

macOS may show a security warning when opening the installer or loading the plug-in. This is expected for this private beta.

The public release will require Apple Developer ID signing and Apple notarization.

## What To Test

Please test:

- Does AUREQ appear as a VST3?
- Does the UI open?
- Does audio pass through?
- Does the spectrum analyzer appear while audio is playing?
- Do factory presets load?
- Do user presets save/load?
- Does A/B work?
- Does Undo/Redo work?
- Does Dynamic EQ work?
- Do Channel Modes work?
- Does saving/reopening a DAW project restore the state?
- Any crashes, hangs, visual glitches, or installation confusion?

Please send screenshots or short videos if possible.
