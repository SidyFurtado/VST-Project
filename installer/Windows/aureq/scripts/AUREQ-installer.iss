; AUREQ Windows VST3 + Standalone Installer Script

#define MyAppName "AUREQ"
#ifndef MyAppVersion
  #define MyAppVersion "0.9.0"
#endif
#define MyAppPublisher "ASTRA Audio"
#ifndef MyOutputBaseFilename
  #define MyOutputBaseFilename "AUREQ-0.9.0-Windows-Setup"
#endif

[Setup]
AppId={{9D15DCD6-711A-4F42-9C34-0D896229A9E0}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={commonpf}\ASTRA Audio\AUREQ
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
Name: "vst3"; Description: "AUREQ VST3 Plug-in"; Types: full vst3only custom; Flags: fixed
Name: "standalone"; Description: "AUREQ Standalone App"; Types: full custom

[Files]
Source: "..\staging\VST3\AUREQ.vst3\*"; DestDir: "{commoncf}\VST3\AUREQ.vst3"; Components: vst3; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\staging\Standalone\AUREQ.exe"; DestDir: "{app}"; Components: standalone; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\AUREQ.exe"; Components: standalone
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\AUREQ.exe"; Components: standalone; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional tasks:"; Components: standalone

[Messages]
WelcomeLabel2=This installer will install the AUREQ Suite to your computer.%n%n- AUREQ VST3 will be installed in C:\Program Files\Common Files\VST3%n- AUREQ Standalone will be installed in C:\Program Files\ASTRA Audio\AUREQ%n%nAfter installation, rescan VST3 plug-ins in your DAW.
