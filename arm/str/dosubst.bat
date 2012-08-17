rem use setenv_vscp.bat in VSCP root to set j to the correct folder 
rem 

rem gcc
rem ===

subst k: "C:/Program Files/yagarto/arm-elf" 
set INCLUDE = "C:\Program Files\yagarto\arm-elf\include;j:\common"

rem openocd
rem =======
rem if openocd-ftd2xx.exe is not in the path add it or edit the str makefile to point at it

rem openocd does not handle long paths so we have to do this to make them 
rem shorter.
subst l: d:\development\m2m\firmware\arm\str\common\openocd\windows
