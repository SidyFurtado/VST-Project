# AUREQ 0.9.0-rc1 — Windows Beta Instructions

Thank you for testing AUREQ on Windows.

This is a private Windows beta of the AUREQ VST3 plug-in by ASTRA Audio, now available through our automated build pipeline.

## Download

Beta Installer:

```text
https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/AUREQ-1.0.0-Windows-Setup.exe
```

## Installation

1. Run the `AUREQ-1.0.0-Windows-Setup.exe` installer.
2. Complete the installer steps.
3. The VST3 plug-in will be installed at the standard system path:

```text
C:\Program Files\Common Files\VST3\AUREQ.vst3
```

4. Open your DAW and rescan VST3 plug-ins.

## DAW Rescan

After installation, rescan VST3 plug-ins:
- **FL Studio:** `Options > Manage plugins > Find installed plugins`
- **Ableton Live:** `Preferences > Plug-Ins > Rescan`
- **Reaper:** `Options > Preferences > Plug-ins > VST > Re-scan`
- **Studio One / Cubase / Bitwig:** use the DAW's plug-in manager/rescan flow.

Expected identity:
- **Name:** `AUREQ`
- **Vendor:** `ASTRA Audio`
- **Format:** `VST3`

## Security Notice

This installer is unsigned. 

Windows SmartScreen may trigger a warning when you execute the installer.
* **Workaround:** Click on **"More info"** (Mais Informações) and then choose **"Run anyway"** (Executar assim mesmo).

## Tester Checklist

- [ ] Installer opens cleanly.
- [ ] VST3 destination path is correctly identified.
- [ ] AUREQ appears as VST3 in the DAW under ASTRA Audio.
- [ ] UI opens and renders correctly.
- [ ] Audio passes through and processes cleanly.
- [ ] Spectrum analyzer operates in real-time.
- [ ] Presets load successfully.
- [ ] User presets save/load correctly.
- [ ] DAW project saves and restores state.
- [ ] Uninstaller removes the VST3 plug-in from the system directory.
