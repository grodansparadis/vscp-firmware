@echo off
cls

SET PACKAGENAME=the MCC18 compatible Delays library for fedC and wizC

echo.
echo Hello, I'm the installationscript for %PACKAGENAME%.
echo.

:CHECKFEDC
  set FED=C:\Program Files\FED\PIC_C
  echo.
  echo I'm checking your system for fedC
  if not exist "%FED%" goto NOFEDC
  echo YES, I found a fedC-installation!
  goto FOUNDFED
:NOFEDC
  echo I could not find a fedC-installation.


:CHECKWIZC
  set FED=C:\Program Files\FED\PIXIE
  echo.
  echo I'm checking your system for wizC
  if not exist "%FED%" goto NOWIZC
  echo YES, I found a wizC-installation!
  goto FOUNDFED
:noWIZC
  echo I could not find a wizC-installation.


:ERROR
  echo.
  echo.
  echo I could not find a FED C-compiler installation on your system.
  echo.
  echo Perhaps I got confused because you installed fedC or wizC in a non-default directory.
  echo If this is the case, please change the path at the top of this install-script.
  echo After that rerun the script and I will be happy to try again.
  echo.
  goto ENDIT


:FOUNDFED
  echo.
  echo.

  set FEDLIBS=%FED%\Libs
  set FEDLIBSUSER=%FEDLIBS%\LibsUser

  if exist "%FEDLIBS%" goto INSTALL
  echo The FED installationdirectory "%FED%"
  echo contains no Libs subdirectory. This is weird!
  echo.
  echo Installation is aborted, sorry...
  goto ENDIT


:INSTALL
  echo I am about to install %PACKAGENAME%
  echo into directory %FEDLIBSUSER%
  echo.
  echo   Press 'enter'  to let me do my thing
  echo   Press 'ctrl-c' to stop me
  pause >nul
  echo.
  echo Installing...


:RESET_READONLY
  echo.
  echo   Removing ReadOnly attributes
  attrib -R "%FEDLIBSUSER%\libDelays\Delays.c"         >nul
  attrib -R "%FEDLIBSUSER%\libDelays\Delays16.c"       >nul
  attrib -R "%FEDLIBSUSER%\libDelays\ReadMe.txt"       >nul
  attrib -R "%FEDLIBSUSER%\libDelays\Delays.htm"       >nul
  attrib -R "%FEDLIBSUSER%\libDelays\HMCservices.jpg"  >nul
  attrib -R "%FEDLIBSUSER%\Delays.h"                   >nul
  echo   Done


:CREATE_DIRECTORIES
  echo.
  echo   Creating directories (if necessary)...
  if not exist "%FEDLIBSUSER%"            mkdir "%FEDLIBSUSER%"
  if not exist "%FEDLIBSUSER%\libDelays"  mkdir "%FEDLIBSUSER%\libDelays"
  echo   Done


  echo.
  echo   Copying Files...
:COPY_LIBRARY
  echo     Library...
  copy /V /Y "libDelays\Delays.c"          "%FEDLIBSUSER%\libDelays\Delays.c"         >nul
  copy /V /Y "libDelays\Delays16.c"        "%FEDLIBSUSER%\libDelays\Delays16.c"       >nul

:COPY_HEADERS
  echo     Headers...
  copy /V /Y "libDelays\Delays.h"          "%FEDLIBSUSER%\Delays.h"                   >nul

:COPY_DOCS
  echo     Documents...
  copy /V /Y "ReadMe.txt"                  "%FEDLIBSUSER%\libDelays\ReadMe.txt"       >nul
  copy /V /Y "libDelays\Delays.htm"        "%FEDLIBSUSER%\libDelays\Delays.htm"       >nul
  copy /V /Y "libDelays\HMCservices.jpg"   "%FEDLIBSUSER%\libDelays\HMCservices.jpg"  >nul

  echo   Done


:SET_READONLY
  echo.
  echo   Setting files to ReadOnly
  attrib +R "%FEDLIBSUSER%\libDelays\Delays.c"         >nul
  attrib +R "%FEDLIBSUSER%\libDelays\Delays16.c"         >nul
  attrib +R "%FEDLIBSUSER%\libDelays\ReadMe.txt"       >nul
  attrib +R "%FEDLIBSUSER%\libDelays\Delays.htm"       >nul
  attrib +R "%FEDLIBSUSER%\libDelays\HMCservices.jpg"  >nul
  attrib +R "%FEDLIBSUSER%\Delays.h"                   >nul
  echo   Done


:FINISHED
  echo.
  echo I've copied some documentation into
  echo   %FEDLIBSUSER%\libDelays
  echo Thought you might like to know this.
  echo.
  echo.
  echo My work has finished. You can now use %PACKAGENAME%.
  echo I hope you have fun with it.
  echo Byebye!

  goto ENDIT


:ENDIT
  echo.
  echo.
  echo Press 'enter' to close this window
  pause >nul