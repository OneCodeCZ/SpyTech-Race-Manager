; SpyTech Race Manager
; Czech Version Install Script

[Setup]
AppName=SpyTech Race Manager
AppVerName=SpyTech Race Manager 1.6
AppPublisher=SpyTech
AppPublisherURL=http://www.spytech.cz
AppSupportURL=http://www.spytech.cz
AppUpdatesURL=http://www.spytech.cz
DefaultDirName={pf}\SpyTech\SpyTech Race Manager\1.6
DefaultGroupName=nogroup
AllowNoIcons=yes
UsePreviousAppDir=no
UsePreviousGroup=no
UsePreviousTasks=no
OutputBaseFilename=rm_1.6_cz_setup
OutputDir=.
DisableProgramGroupPage=yes
;DisableDirPage=yes
;DisableReadyPage=yes
PrivilegesRequired=admin

[Languages]
Name: "cz"; MessagesFile: "compiler:languages\czech.isl"

[Files]
Source: "..\Binary\SpyTech Race Manager.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Binary\dm.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Binary\mmnt.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\Binary\wsf.dll"; DestDir: "{app}"; Flags: ignoreversion

Source: "..\Binary\Data\*"; DestDir: "{app}\Data"; Flags: ignoreversion recursesubdirs
Source: "..\Help\cz\*"; DestDir: "{app}\Help"; Flags: ignoreversion recursesubdirs

Source: "db cz\db.dat"; DestDir: "{app}\Data"; Flags: ignoreversion
Source: "data\comm.dat"; DestDir: "{app}\Data"; Flags: ignoreversion

Source: "C:\WINNT\system32\msvcrt.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\WINNT\system32\mfc42.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\projects\binary\ShFolder.exe"; DestDir: "{app}\Data"; Flags: ignoreversion

[Icons]
Name: "{commonprograms}\SpyTech Race Manager 1.6"; Filename: "{app}\SpyTech Race Manager.exe"; WorkingDir: "{app}"

[Run]
Filename: "{app}\Data\ShFolder.exe"; StatusMsg: "Instaluji SHFolder.dll..."

[Registry]
; NOTE: On uninstall the commands are performed in the reversed order
Root: HKCU; SubKey: "Software\SpyTech"; Flags: uninsdeletekeyifempty
Root: HKCU; SubKey: "Software\SpyTech\SpyTech Race Manager"; Flags: uninsdeletekeyifempty
Root: HKCU; SubKey: "Software\SpyTech\SpyTech Race Manager\1.6"; Flags: uninsdeletekey

[UninstallDelete]
Type: filesandordirs; Name: "{userappdata}\SpyTech\SpyTech Race Manager\1.6"
Type: dirifempty; Name: "{userappdata}\SpyTech\SpyTech Race Manager"
Type: dirifempty; Name: "{userappdata}\SpyTech"

