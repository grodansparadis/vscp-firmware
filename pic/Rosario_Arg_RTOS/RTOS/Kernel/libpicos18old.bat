REM  ______________________________________________________________________
REM >                                                                      <
REM >                         PICos18 release 2.10                         <
REM >                                                                      <
REM >             PICos18 - Real-time kernel for PIC18 family              <
REM >                                                                      <
REM >                                                                      <
REM > www.picos18.com                                    www.pragmatec.net <
REM >______________________________________________________________________<

@echo off 
cls 

REM -- point to the source of the kernel 
set PATHTOKERNEL=c:\RTOS

REM -- what pic18 chip are you using 
set CHIP=18F258


set MCC18=c:\MCC18
set CFLAGS=-Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa- -On- -w 2


echo Trying to build the PICos18 kernel library 
echo ... 

%MCC18%\bin\mplib.exe /c %PATHTOKERNEL%\Kernel\picos18.lib 

echo 1...
pause

for %%i in (%PATHTOKERNEL%\Kernel\*.asm) do %MCC18%\mpasm\mpasmwin.exe /rDEC /l- /o /q /d__LARGE__  /p%CHIP% %%i 
pause
for %%i in (%PATHTOKERNEL%\Kernel\*.c) do %MCC18%\bin\mcc18.exe %CFLAGS%  /i"%MCC18%\h" /i"%PATHTOKERNEL%\Include" -p%CHIP% %%i 
pause
echo 2...
del PICos18.o 
del PICos18i.o 
del PICos18iz.o 

del PICos18_Boot.o 
del PICos18i_Boot.o 
del PICos18iz_Boot.o 

pause
for %%i in (%PATHTOKERNEL%\Kernel\*.o) do %MCC18%\bin\mplib.exe /r %PATHTOKERNEL%\Kernel\picos18.lib %%i 

echo 3...
pause
%MCC18%\bin\mcc18.exe %CFLAGS% /i"%MCC18%\h" /i"%PATHTOKERNEL%\Include" -p%CHIP% PICos18.c 
%MCC18%\bin\mcc18.exe %CFLAGS% /i"%MCC18%\h" /i"%PATHTOKERNEL%\Include" -p%CHIP% PICos18i.c 
%MCC18%\bin\mcc18.exe %CFLAGS% /i"%MCC18%\h" /i"%PATHTOKERNEL%\Include" -p%CHIP% PICos18iz.c 

%MCC18%\bin\mcc18.exe %CFLAGS% /i"%MCC18%\h" /i"%PATHTOKERNEL%\Include" -p%CHIP% PICos18_Boot.c 
%MCC18%\bin\mcc18.exe %CFLAGS% /i"%MCC18%\h" /i"%PATHTOKERNEL%\Include" -p%CHIP% PICos18i_Boot.c 
%MCC18%\bin\mcc18.exe %CFLAGS% /i"%MCC18%\h" /i"%PATHTOKERNEL%\Include" -p%CHIP% PICos18iz_Boot.c 
 

echo ================================================== 
echo           PICos18 Library has been rebuilt    
echo ================================================== 
