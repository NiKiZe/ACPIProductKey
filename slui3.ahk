; AutoHotKey script slui3.ahk To install product keys where it fails with: cscript //Nologo c:\Windows\System32\slmgr.vbs -ipk $key
; Can for example be used to switch from Enterprise to Pro version
; Tip use Chocolatey to install portable: choco install -y autohotkey.portable
; Then run with & "C:\ProgramData\chocolatey\lib\autohotkey.portable\tools\AutoHotkey.exe" slui3.ahk $key
; Enter a product key
; ahk_class Shell_Dialog
; ahk_exe SystemSettingsAdminFlows.exe

; default environment
DetectHiddenWindows, off
SetControlDelay, 20

; modified environment
#NoEnv
#NoTrayIcon
DetectHiddenText, off
SetTitleMatchMode, 2  ;contains

; Run slui 3 or slui 0x03 to open the activation dialog
Run slui.exe, , 3

; define all Window title names
GroupAdd, GrpProdKeyWin, Enter a product key ahk_class Shell_Dialog
GroupAdd, GrpProdKeyWin, Ange en produktnyckel ahk_class Shell_Dialog

GroupAdd, GrpActivateWin, Activate Windows
GroupAdd, GrpActivateWin, Aktivera Windows

GroupAdd, GrpIsActivatedWin, Windows is activated
GroupAdd, GrpIsActivatedWin, Windows har aktiverats

; Wait for main window
WinWait, ahk_group GrpProdKeyWin, , 20
if ErrorLevel {  ; i.e. it's not blank or zero.
	ExitApp
	return
}
WinActivate ; Make sure the found window is active
SendInput %1% ; Send script argument as key
Send {Enter} ; Press enter to emulate Next
; Todo detect and valdate Next button
;ControlClick, Button1 ; Next

; Do you want to activate window, default yes
WinWait, ahk_group GrpActivateWin, , 20
if ErrorLevel {  ; i.e. it's not blank or zero.
	ExitApp
	return
}
WinActivate ; Use the window found by WinWait.
Send {Enter}

; You have now activated window, close
WinWait, ahk_group GrpIsActivatedWin, , 20
if ErrorLevel {  ; i.e. it's not blank or zero.
	ExitApp
	return
}
WinActivate ; Use the window found by WinWait.
Send {Enter}

ExitApp
