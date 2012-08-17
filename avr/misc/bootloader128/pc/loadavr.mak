# Microsoft Developer Studio Generated NMAKE File, Based on loadavr.dsp
!IF "$(CFG)" == ""
CFG=loadavr - Win32 Debug
!MESSAGE No configuration specified. Defaulting to loadavr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "loadavr - Win32 Release" && "$(CFG)" != "loadavr - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "loadavr.mak" CFG="loadavr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "loadavr - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "loadavr - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "loadavr - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\loadmega128.exe"


CLEAN :
	-@erase "$(INTDIR)\CommEngine.obj"
	-@erase "$(INTDIR)\HexEngine.obj"
	-@erase "$(INTDIR)\linkEngine.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\loadmega128.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\loadavr.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\loadavr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\loadmega128.pdb" /machine:I386 /out:"$(OUTDIR)\loadmega128.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CommEngine.obj" \
	"$(INTDIR)\HexEngine.obj" \
	"$(INTDIR)\linkEngine.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\loadmega128.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "loadavr - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\loadmega128.exe"


CLEAN :
	-@erase "$(INTDIR)\CommEngine.obj"
	-@erase "$(INTDIR)\HexEngine.obj"
	-@erase "$(INTDIR)\linkEngine.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\loadmega128.exe"
	-@erase "$(OUTDIR)\loadmega128.ilk"
	-@erase "$(OUTDIR)\loadmega128.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\loadavr.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\loadavr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\loadmega128.pdb" /debug /machine:I386 /out:"$(OUTDIR)\loadmega128.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CommEngine.obj" \
	"$(INTDIR)\HexEngine.obj" \
	"$(INTDIR)\linkEngine.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\loadmega128.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("loadavr.dep")
!INCLUDE "loadavr.dep"
!ELSE 
!MESSAGE Warning: cannot find "loadavr.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "loadavr - Win32 Release" || "$(CFG)" == "loadavr - Win32 Debug"
SOURCE=.\CommEngine.cpp

"$(INTDIR)\CommEngine.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HexEngine.cpp

"$(INTDIR)\HexEngine.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\linkEngine.cpp

"$(INTDIR)\linkEngine.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

