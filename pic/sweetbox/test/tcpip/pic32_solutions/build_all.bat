@echo off

REM $Id: $

REM Keep a local errorlevel to latch any error durnig build process.
SET TEMP_ERRORLEVEL=0

for /R %%f in (build.bat) do @cd %%~pf &&  if exist build.bat call build.bat && if not ERRORLEVEL 0 set TEMP_ERRORLEVEL=1

REM Set the system errorlevel to our local errorlevel to reflect latched value.
set ERRORLEVEL=%TEMP_ERRORLEVEL%



