#define MyAppName "Gemini Vision Practice Assistant"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "AuthorizedPractice"
#define MyAppExeName "GeminiVisionPracticeAssistant.exe"

[Setup]
AppId={{B6E1C6E4-10A6-4F2A-9B30-8C5E7F1A2D11}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={aut localappdata}\{#MyAppName}
DisableProgramGroupPage=yes
DisableDirPage=no
CloseApplications=yes
RestartApplications=no
Uninstallable=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=installer-output
OutputBaseFilename=GeminiVisionPracticeAssistant-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
UninstallDisplayIcon={app}\{#MyAppExeName}

[Files]
Source: "..\dist\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Icons]
Name: "{autprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autdesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
