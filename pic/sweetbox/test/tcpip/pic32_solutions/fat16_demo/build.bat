@echo off

REM  *********************************************************************
REM  *
REM  *            Windows batch file to build this module
REM  *
REM  *********************************************************************
REM  * Company:         Microchip Technology, Inc.
REM  *
REM  * Software License Agreement
REM  *
REM  * The software supplied herewith by Microchip Technology Incorporated
REM  * (the “Company”) for its PICmicro® Microcontroller is intended and
REM  * supplied to you, the Company’s customer, for use solely and
REM  * exclusively on Microchip PICmicro Microcontroller products. The
REM  * software is owned by the Company and/or its supplier, and is
REM  * protected under applicable copyright laws. All rights are reserved.
REM  * Any use in violation of the foregoing restrictions may subject the
REM  * user to criminal sanctions under applicable laws, as well as to
REM  * civil liability for the breach of the terms and conditions of this
REM  * license.
REM  *
REM  * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
REM  * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
REM  * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
REM  * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
REM  * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
REM  * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
REM  *
REM  * $Id: $
REM  * 
REM  * This batch file builds the current module.
REM  * 
REM  * Usage:
REM  *	1.	To build using default build options and processor (32MX360F512L):
REM  *			build.bat
REM  *
REM  *	2.	To build using a specific processor and default build options
REM  *			set BUILD_PROCESSOR=32MX360F256L.
REM  *			build.bat
REM  *
REM  *	3.	To build using custom build options (to use with pic32-gcc.exe)
REM  *			set BUILD_OPTIONS = -mips16
REM  *			build.bat
REM  *
REM  *	Note: BUILD_OPTIONS and BUILD_PROCESSOR variables will retain their
REM  *        values in the same command-prompt session.
REM  *	      If you want to revert to default options and are still executing
REM  *	      in same command-prompt window, you must explicitly clear
REM  *	      these variables.
REM  ********************************************************************

REM Module specific steps

REM Set the relative name of the output elf file name
set ELF_FILE_NAME=release/fat16_demo.elf

REM List of source files that make up this project
REM note the use of batch file line-continuation character
set SOURCE=source/main.c ^
	../Microchip/fat16/source/fat.c ../Microchip/fat16/source/fileio.c ^
	../Microchip/fat16/source/mediasd.c ../Microchip/Common/ex16lcd.c ^
	../Microchip/Common/mstimer.c


REM Define include search paths specific to your project.
REM If no special paths are required, keep this variable blank
set INCLUDE_PATHS=-Isource -I../Microchip/Include

REM Generic steps

REM Define the default processor to build for.
REM Caller may change the processor by specifying it from command-line
set DEFAULT_PROCESSOR=32MX360F512L


REM Type of processor we will be building for.
REM Use the value from command-line if there is any.
set PROCESSOR=%BUILD_PROCESSOR%


REM This build file uses following environment variables to
REM to allow caller to define its own settings
REM GCC_OPTIONS is a list of C and linker options to be passed via pic32-gcc
REM By default, this batch file does not define any GCC_OPTIONS


REM If there is a processor given via command-line, use it or else use the default one of 32MX360F512L
if NOT "%PROCESSOR%" == "" goto :Continue

REM Processor is not provided, use the default pic32-gcc build one
set PROCESSOR=%DEFAULT_PROCESSOR%

:Continue
REM Now build it
set GCC_OPTIONS_STRING=default
if NOT "%GCC_OPTIONS%" == "" set GCC_OPTIONS_STRING=%GCC_OPTIONS%

echo Building for %PROCESSOR% using %GCC_OPTIONS_STRING% options... >&2
echo Use BUILD_PROCESSOR and GCC_OPTIONS env variables to specify your choice.

REM Turn echo on so that user can see actual buld command we are executing
@echo on
pic32-gcc %SOURCE% -mprocessor=%PROCESSOR% -o %ELF_FILE_NAME% %INCLUDE_PATHS% %GCC_OPTIONS%
