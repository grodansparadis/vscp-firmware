@echo off
cls

SET PACKAGENAME=the OneWire-library for fedC and wizC

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
  attrib -R "%FEDLIBSUSER%\libOWnet\MultiBus\OWcrc.c"   >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\MultiBus\OWlnk.c"   >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\MultiBus\OWnet.c"   >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\MultiBus\OWses.c"   >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\MultiBus\OWtran.c"  >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\SingleBus\OWcrc.c"  >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\SingleBus\OWlnk.c"  >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\SingleBus\OWnet.c"  >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\SingleBus\OWses.c"  >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\SingleBus\OWtran.c" >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\ReadMe.txt"         >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\OWnet.htm"          >nul
  attrib -R "%FEDLIBSUSER%\libOWnet\HMCservices.jpg"    >nul
  attrib -R "%FEDLIBSUSER%\OWnet.h"                     >nul
  attrib -R "%FEDLIBSUSER%\OWnetConfig.h"               >nul
  echo   Done


:CREATE_DIRECTORIES
  echo.
  echo   Creating directories (if necessary)...
  if not exist "%FEDLIBSUSER%"                     mkdir "%FEDLIBSUSER%"
  if not exist "%FEDLIBSUSER%\libOWnet"            mkdir "%FEDLIBSUSER%\libOWnet"
  if not exist "%FEDLIBSUSER%\libOWnet\MultiBus"   mkdir "%FEDLIBSUSER%\libOWnet\MultiBus"
  if not exist "%FEDLIBSUSER%\libOWnet\SingleBus"  mkdir "%FEDLIBSUSER%\libOWnet\SingleBus"
  echo   Done


  echo.
  echo   Copying Files...
:COPY_LIBRARY
  echo     Library...
  copy /V /Y "libOWnet\MultiBus\OWcrc.c"   "%FEDLIBSUSER%\libOWnet\MultiBus\OWcrc.c"   >nul
  copy /V /Y "libOWnet\MultiBus\OWlnk.c"   "%FEDLIBSUSER%\libOWnet\MultiBus\OWlnk.c"   >nul
  copy /V /Y "libOWnet\MultiBus\OWnet.c"   "%FEDLIBSUSER%\libOWnet\MultiBus\OWnet.c"   >nul
  copy /V /Y "libOWnet\MultiBus\OWses.c"   "%FEDLIBSUSER%\libOWnet\MultiBus\OWses.c"   >nul
  copy /V /Y "libOWnet\MultiBus\OWtran.c"  "%FEDLIBSUSER%\libOWnet\MultiBus\OWtran.c"  >nul
  copy /V /Y "libOWnet\SingleBus\OWcrc.c"  "%FEDLIBSUSER%\libOWnet\SingleBus\OWcrc.c"  >nul
  copy /V /Y "libOWnet\SingleBus\OWlnk.c"  "%FEDLIBSUSER%\libOWnet\SingleBus\OWlnk.c"  >nul
  copy /V /Y "libOWnet\SingleBus\OWnet.c"  "%FEDLIBSUSER%\libOWnet\SingleBus\OWnet.c"  >nul
  copy /V /Y "libOWnet\SingleBus\OWses.c"  "%FEDLIBSUSER%\libOWnet\SingleBus\OWses.c"  >nul
  copy /V /Y "libOWnet\SingleBus\OWtran.c" "%FEDLIBSUSER%\libOWnet\SingleBus\OWtran.c" >nul

:COPY_HEADERS
  echo     Headers...
  copy /V /Y "libOWnet\OWnet.h"            "%FEDLIBSUSER%\OWnet.h"                     >nul
  copy /V /Y "libOWnet\OWnetConfig.h"      "%FEDLIBSUSER%\OWnetConfig.h"               >nul

:COPY_DOCS
  echo     Documents...
  copy /V /Y "ReadMe.txt"                  "%FEDLIBSUSER%\libOWnet\ReadMe.txt"         >nul
  copy /V /Y "libOWnet\OWnet.htm"          "%FEDLIBSUSER%\libOWnet\OWnet.htm"          >nul
  copy /V /Y "libOWnet\HMCservices.jpg"    "%FEDLIBSUSER%\libOWnet\HMCservices.jpg"    >nul

  echo   Done


:SET_READONLY
  echo.
  echo   Setting files to ReadOnly
  attrib +R "%FEDLIBSUSER%\libOWnet\MultiBus\OWcrc.c"   >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\MultiBus\OWlnk.c"   >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\MultiBus\OWnet.c"   >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\MultiBus\OWses.c"   >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\MultiBus\OWtran.c"  >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\SingleBus\OWcrc.c"  >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\SingleBus\OWlnk.c"  >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\SingleBus\OWnet.c"  >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\SingleBus\OWses.c"  >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\SingleBus\OWtran.c" >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\ReadMe.txt"         >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\OWnet.htm"          >nul
  attrib +R "%FEDLIBSUSER%\libOWnet\HMCservices.jpg"    >nul
  attrib +R "%FEDLIBSUSER%\OWnet.h"                     >nul
  attrib +R "%FEDLIBSUSER%\OWnetConfig.h"               >nul
  echo   Done


:FINISHED
  echo.
  echo I've copied some documentation into
  echo   %FEDLIBSUSER%\libOWnet
  echo You can find some demo applications in the
  echo Demo directory. Thought you might like to know this.
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