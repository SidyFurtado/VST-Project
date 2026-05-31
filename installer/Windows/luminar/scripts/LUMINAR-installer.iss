; LUMINAR Windows VST3 + Standalone Installer Script

#define MyAppName "LUMINAR"
#ifndef MyAppVersion
  #define MyAppVersion "1.0.0"
#endif
#define MyAppPublisher "ASTRA Audio"
#ifndef MyOutputBaseFilename
  #define MyOutputBaseFilename "LUMINAR-1.0.0-Windows-Setup"
#endif

[Setup]
AppId={{E6A0F1D8-927A-4FB7-B67B-6447DE294EA3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={commonpf}\ASTRA Audio\LUMINAR
DefaultGroupName=ASTRA Audio
DisableProgramGroupPage=yes
OutputDir=..\output
OutputBaseFilename={#MyOutputBaseFilename}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayName={#MyAppName} Suite
PrivilegesRequired=admin

#if FileExists("..\..\plugin-only\resources\AUREQ.ico")
SetupIconFile=..\..\plugin-only\resources\AUREQ.ico
#endif

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "Full installation (VST3 + Standalone App)"
Name: "vst3only"; Description: "VST3 Plug-in only"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "vst3"; Description: "LUMINAR VST3 Plug-in"; Types: full vst3only custom; Flags: fixed
Name: "standalone"; Description: "LUMINAR Standalone App"; Types: full custom

[Files]
Source: "..\staging\VST3\LUMINAR.vst3\*"; DestDir: "{commoncf}\VST3\LUMINAR.vst3"; Components: vst3; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\staging\Standalone\LUMINAR.exe"; DestDir: "{app}"; Components: standalone; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\LUMINAR.exe"; Components: standalone
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\LUMINAR.exe"; Components: standalone; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional tasks:"; Components: standalone

[Messages]
WelcomeLabel2=This installer will install the LUMINAR Suite to your computer.%n%n- LUMINAR VST3 will be installed in C:\Program Files\Common Files\VST3%n- LUMINAR Standalone will be installed in C:\Program Files\ASTRA Audio\LUMINAR%n%nAfter installation, rescan VST3 plug-ins in your DAW.
