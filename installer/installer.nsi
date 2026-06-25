!include "MUI2.nsh"

; General
Name "The Battles"
OutFile "TheBattles-Setup.exe"
InstallDir "$LOCALAPPDATA\TheBattles"
InstallDirRegKey HKCU "Software\TheBattles" "InstallDir"
RequestExecutionLevel user

; UI Settings
!define MUI_ICON "icon.ico"
!define MUI_UNICON "icon.ico"
!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE "Welcome to The Battles Setup"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of The Battles v1.0.$\r$\n$\r$\nThe ultimate 2D Battle Royale experience.$\r$\n$\r$\nClick Next to continue."
!define MUI_FINISHPAGE_RUN "$INSTDIR\TheBattles.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch The Battles"
!define MUI_FINISHPAGE_TITLE "Installation Complete"
!define MUI_FINISHPAGE_TEXT "The Battles has been installed on your computer.$\r$\n$\r$\nClick Finish to close this wizard."

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language
!insertmacro MUI_LANGUAGE "English"

; Installer Section
Section "Install"
    SetOutPath "$INSTDIR"
    
    ; Copy game files
    File "TheBattles.exe"
    File "sfml-graphics-2.dll"
    File "sfml-window-2.dll"
    File "sfml-system-2.dll"
    File "openal32.dll"
    
    ; Create Start Menu shortcuts
    CreateDirectory "$SMPROGRAMS\The Battles"
    CreateShortcut "$SMPROGRAMS\The Battles\The Battles.lnk" "$INSTDIR\TheBattles.exe" "" "$INSTDIR\TheBattles.exe"
    CreateShortcut "$SMPROGRAMS\The Battles\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    
    ; Create Desktop shortcut
    CreateShortcut "$DESKTOP\The Battles.lnk" "$INSTDIR\TheBattles.exe" "" "$INSTDIR\TheBattles.exe"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Registry
    WriteRegStr HKCU "Software\TheBattles" "InstallDir" "$INSTDIR"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheBattles" "DisplayName" "The Battles"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheBattles" "UninstallString" '"$INSTDIR\Uninstall.exe"'
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheBattles" "DisplayIcon" "$INSTDIR\TheBattles.exe"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheBattles" "Publisher" "The Battles Team"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheBattles" "DisplayVersion" "1.0"
SectionEnd

; Uninstaller Section
Section "Uninstall"
    ; Remove files
    Delete "$INSTDIR\TheBattles.exe"
    Delete "$INSTDIR\sfml-graphics-2.dll"
    Delete "$INSTDIR\sfml-window-2.dll"
    Delete "$INSTDIR\sfml-system-2.dll"
    Delete "$INSTDIR\openal32.dll"
    Delete "$INSTDIR\Uninstall.exe"
    RMDir "$INSTDIR"
    
    ; Remove shortcuts
    Delete "$SMPROGRAMS\The Battles\The Battles.lnk"
    Delete "$SMPROGRAMS\The Battles\Uninstall.lnk"
    RMDir "$SMPROGRAMS\The Battles"
    Delete "$DESKTOP\The Battles.lnk"
    
    ; Remove registry
    DeleteRegKey HKCU "Software\TheBattles"
    DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheBattles"
SectionEnd
