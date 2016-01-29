setlocal
set JAVAPATH=c:/j2sdk1.4.2_07
set FILENAME=mxwt102

REM =========== Build project ===========
%JAVAPATH%/bin/javac.exe Webterm.java

REM =========== Pack into jar file without Manifest file =========
%JAVAPATH%/bin/jar cmf manifest.mf %FILENAME%.jar *.class  

copy /y %FILENAME%.jar ..\%FILENAME%.jar
REM copy /y *.class ..\*.class
pause

endlocal