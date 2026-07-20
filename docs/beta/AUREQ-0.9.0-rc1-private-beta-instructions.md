# ASTRA Audio Suite 0.9.0-rc1 — Private Beta Instructions (macOS)

Thank you for testing the ASTRA Audio Suite.

This is a private macOS beta of the ASTRA Audio Suite (featuring AUREQ, LUMINAR, and GRAVITY) in Audio Unit (AU) format by ASTRA Audio.

## Download

Beta AU Suite Package:

```text
https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg
```

## Installation

1. Run `ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`.
2. Complete the installer steps.
3. The Audio Unit components will be installed at:

```text
/Library/Audio/Plug-Ins/Components/AUREQ.component
/Library/Audio/Plug-Ins/Components/LUMINAR.component
/Library/Audio/Plug-Ins/Components/GRAVITY.component
```

4. Open your DAW and rescan Audio Unit (AU) plug-ins.

> [!NOTE]
> **VST3 & Standalone:** In this phase, macOS VST3 is treated as a legacy/compatibility format, and the Standalone app is an internal testing tool. They are not included in the public beta installer.

## Clean Install (For Upgrading Testers)

If you have installed previous testing versions using the older manufacturer code (`Manu`):
1. Delete old user-level components:
   ```bash
   rm -rf ~/Library/Audio/Plug-Ins/Components/AUREQ.component
   rm -rf ~/Library/Audio/Plug-Ins/Components/LUMINAR.component
   rm -rf ~/Library/Audio/Plug-Ins/Components/GRAVITY.component
   ```
2. Clear the macOS AudioUnit cache to force a full DAW rescan of the new `AStr` components:
   ```bash
   rm -f ~/Library/Caches/AudioUnitCache/com.apple.audiounits.cache
   killall -9 AudioComponentRegistrar
   ```

## DAW Rescan & AU Validation

- **Logic Pro:** Automatically scans on start. You can force re-validation in `Plug-in Manager > Select ASTRA Audio plug-ins > Reset & Rescan Selection`.
- **FL Studio macOS:** `Plugin Manager > Find installed plugins`. Ensure `Verify plugins` is enabled.
- **Ableton Live:** `Preferences > Plug-Ins > Rescan` or toggle Use Audio Units off and on.
- **Reaper:** `Options > Preferences > Plug-ins > VST > Re-scan` (scans all formats, including AU).

## Security Notice

This beta is unsigned and not notarized. 

macOS Gatekeeper may show a warning when opening the installer. 
* **Workaround:** Right-click (or Control-click) the `.pkg` file and select **Open**, then click **Open Anyway**.

The public release will be fully notarized by Apple.

## What To Test

Please verify:
- Do AUREQ, LUMINAR, and GRAVITY appear in your DAW as Audio Units (AU)?
- Does the manufacturer correctly say `ASTRA Audio` (under manufacturer ID `AStr`)?
- Does audio pass through and process cleanly?
- Does the spectrum analyzer (AUREQ) run in real-time?
- Do factory presets load and operate correctly?
- Does DAW project saving/restoring preserve plug-in state?
- Any crashes, hangs, or installation issues?

Please send screenshots or short videos to the developer.
