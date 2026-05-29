# AUREQ 0.9.0-rc1 — Windows Beta Instructions

Status: planned / blocked until a real Windows build exists.

This document describes the intended Windows VST3-only beta flow for AUREQ by ASTRA Audio.

## Current Status

- Windows VST3 payload: blocked until Windows build.
- Windows installer EXE: blocked until Windows build and Inno Setup environment.
- Code signing: blocked until Windows code signing certificate.
- macOS `.vst3` must not be used as a Windows payload.

## Intended Installer

Planned installer path:

```text
installer/Windows/plugin-only/output/AUREQ-0.9.0-rc1-Windows-plugin-only-setup.exe
```

The installer must be generated only on an approved Windows environment with Inno Setup and a confirmed Windows `AUREQ.vst3` payload.

## Install Location

The VST3 plug-in must install to:

```text
C:\Program Files\Common Files\VST3\AUREQ.vst3
```

In Inno Setup this is represented as:

```text
{commoncf}\VST3\AUREQ.vst3
```

## DAW Rescan

After installation, rescan VST3 plug-ins:

- FL Studio: `Options > Manage plugins > Find installed plugins`
- Ableton Live: `Preferences > Plug-Ins > Rescan`
- Reaper: `Options > Preferences > Plug-ins > VST > Re-scan`
- Studio One / Cubase / Bitwig: use the DAW's plug-in manager/rescan flow.

Expected identity:

- Name: `AUREQ`
- Vendor: `ASTRA Audio`
- Format: `VST3`

## Security Notice

This beta is expected to be unsigned until Windows code signing is available.

Windows SmartScreen may show a warning for unsigned installers or binaries. Public Windows release should use Authenticode code signing to reduce warnings.

## Tester Checklist

- [ ] Installer opens.
- [ ] Installer clearly says the VST3 destination.
- [ ] AUREQ appears as VST3 in the DAW.
- [ ] Vendor appears as ASTRA Audio.
- [ ] UI opens.
- [ ] Audio passes through.
- [ ] Spectrum analyzer appears while audio is playing.
- [ ] Presets load.
- [ ] User presets save/load.
- [ ] A/B works.
- [ ] Undo/Redo works.
- [ ] Dynamic EQ works.
- [ ] Channel Mode works.
- [ ] DAW project saves/reopens with AUREQ state.
- [ ] Uninstaller removes `C:\Program Files\Common Files\VST3\AUREQ.vst3`.
- [ ] User presets are not removed automatically.

## Known Blocks

- Windows build environment required.
- Real Windows `AUREQ.vst3` required.
- Inno Setup required.
- Windows code signing certificate required for public release.
