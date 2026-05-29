; AUREQ Windows VST3-only installer script
; Phase 17.0 prepares the installer definition only.
; Build the final setup EXE only on a real Windows environment with Inno Setup
; and a confirmed Windows AUREQ.vst3 payload in ..\staging\VST3\AUREQ.vst3.

#define MyAppName "AUREQ"
#define MyAppVersion "0.9.0"
#define MyAppPublisher "ASTRA Audio"
#define MyOutputBaseFilename "AUREQ-0.9.0-rc1-Windows-plugin-only-setup"

[Setup]
AppId={{9D15DCD6-711A-4F42-9C34-0D896229A9E0}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={commoncf}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
OutputDir=..\output
OutputBaseFilename={#MyOutputBaseFilename}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayName={#MyAppName} VST3 Plug-in
PrivilegesRequired=admin

#if FileExists("..\resources\AUREQ.ico")
SetupIconFile=..\resources\AUREQ.ico
#endif

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\staging\VST3\AUREQ.vst3"; DestDir: "{commoncf}\VST3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Messages]
WelcomeLabel2=This installer will install the AUREQ VST3 plug-in to the standard Windows VST3 folder:%n%nC:\Program Files\Common Files\VST3\AUREQ.vst3%n%nAfter installation, rescan VST3 plug-ins in your DAW.

[Code]
function InitializeSetup(): Boolean;
begin
  MsgBox(
    'AUREQ will be installed as a VST3 plug-in only.' + #13#10#13#10 +
    'Destination:' + #13#10 +
    'C:\Program Files\Common Files\VST3\AUREQ.vst3' + #13#10#13#10 +
    'This beta installer should only be built with a real Windows AUREQ.vst3 payload.',
    mbInformation,
    MB_OK
  );

  Result := True;
end;
