# ACPIProductKey
Windows ProductKey Util

For quite some time Windows Product keys have been encoded in BIOS/Firmware on machines from most large manufacturers.
If the windows install is done with the exact correct version this is piced up automatically.
But if something differs (for example installing Windows 10 on a machine that shipped with Win 8) it does not.

Using these scripts the correct key can be installed, and even automated, from background scripts so PC Techs and SysAdmins don't have to care much about it.

First version was to be used to grab key in WinPE environment before installation of Windows 8

The `.ps1` version is to be used during Windows 10 installation after WinPE, or from a AD GPO script.
