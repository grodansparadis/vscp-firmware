@echo off

set PATCH_NAME=Olimex-PIC-WEB.patch


echo Please put this batch file, the patch.exe program and the patch
echo file "%PATCH_NAME%" inside the Microchip distribution 
echo directory 'Microchip Solutions' (the directory that contains the
echo subdirectories "Internet Bootloader App", "Internet Radio App", 
echo "Microchip", "SSL Demo App", "TCPIP Demo App", "TCPIP WebVend App").
echo.
echo WARNING: The only ported project is ".\TCPIP Demo App\TCPIP Demo App-C18.mcw"
echo.
echo. 
echo WARNING: This firmware is for PIC-WEB board revisions B and later.
echo.
echo. 
echo.
echo.
pause

rem set TEST=--dry-run
set QUOTE=--quoting-style=shell-always
patch.exe -u -l -E %TEST%  %QUOTE% --binary -p1 < %PATCH_NAME%

echo.
echo.
pause

