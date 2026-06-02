; GRAVITY Windows VST3 + Standalone Installer Script

#define MyAppName "GRAVITY"
#ifndef MyAppVersion
  #define MyAppVersion "1.0.0"
#endif
#define MyAppPublisher "ASTRA Audio"
#ifndef MyOutputBaseFilename
  #define MyOutputBaseFilename "GRAVITY-1.0.0-Windows-Setup"
#endif

[Setup]
AppId={{4F6CF39D-83DE-4A4D-8E4A-6A4A5F0C2B17}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={commonpf}\ASTRA Audio\GRAVITY
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
Name: "vst3"; Description: "GRAVITY VST3 Plug-in"; Types: full vst3only custom; Flags: fixed
Name: "standalone"; Description: "GRAVITY Standalone App"; Types: full custom

[Files]
Source: "..\staging\VST3\GRAVITY.vst3\*"; DestDir: "{commoncf}\VST3\GRAVITY.vst3"; Components: vst3; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\staging\Standalone\GRAVITY.exe"; DestDir: "{app}"; Components: standalone; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\GRAVITY.exe"; Components: standalone
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\GRAVITY.exe"; Components: standalone; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional tasks:"; Components: standalone

[Messages]
WelcomeLabel2=This installer will install the GRAVITY Suite to your computer.%n%n- GRAVITY VST3 will be installed in C:\Program Files\Common Files\VST3%n- GRAVITY Standalone will be installed in C:\Program Files\ASTRA Audio\GRAVITY%n%nAfter installation, rescan VST3 plug-ins in your DAW.
