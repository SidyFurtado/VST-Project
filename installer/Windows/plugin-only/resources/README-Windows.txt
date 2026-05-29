AUREQ 0.9.0-rc1 - Windows VST3 Private Beta
ASTRA Audio

Status
------
This Windows package is planned as a VST3-only installer.
The final setup EXE is blocked until a real Windows build of AUREQ.vst3 exists
and Inno Setup is run on an approved Windows environment.

Do not use the macOS AUREQ.vst3 as the Windows payload.

Expected Install Location
-------------------------
C:\Program Files\Common Files\VST3\AUREQ.vst3

DAW Rescan
----------
After installation, rescan VST3 plug-ins in your DAW:

- FL Studio: Options > Manage plugins > Find installed plugins
- Ableton Live: Preferences > Plug-Ins > Rescan
- Reaper: Options > Preferences > Plug-ins > VST > Re-scan
- Studio One / Cubase / Bitwig: use the DAW's plug-in manager or rescan command.

Expected Plug-In Identity
-------------------------
Name: AUREQ
Vendor: ASTRA Audio
Format: VST3

Security Notice
---------------
This beta is expected to be unsigned until Windows code signing is available.
Windows SmartScreen may show a warning for unsigned installers.
Public Windows release should use Authenticode code signing to reduce warnings.

Uninstall
---------
The Inno Setup installer will provide the standard Windows uninstaller entry.
The uninstaller is expected to remove:

C:\Program Files\Common Files\VST3\AUREQ.vst3

User presets should not be removed automatically in this phase.
