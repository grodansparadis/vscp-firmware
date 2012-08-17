# Microsoft Developer Studio Generated NMAKE File, Based on canusbdrv.dsp
!IF "$(CFG)" == ""
CFG=canusbdrv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to canusbdrv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "canusbdrv - Win32 Release" && "$(CFG)" != "canusbdrv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "canusbdrv.mak" CFG="canusbdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "canusbdrv - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "canusbdrv - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "canusbdrv - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\canusbdrv.dll" "$(OUTDIR)\canusbdrv.pch" "$(OUTDIR)\canusbdrv.bsc"


CLEAN :
	-@erase "$(INTDIR)\canusb.obj"
	-@erase "$(INTDIR)\canusb.sbr"
	-@erase "$(INTDIR)\canusbdrv.obj"
	-@erase "$(INTDIR)\canusbdrv.pch"
	-@erase "$(INTDIR)\canusbdrv.sbr"
	-@erase "$(INTDIR)\dllist.obj"
	-@erase "$(INTDIR)\dllist.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\canusbdrv.bsc"
	-@erase "$(OUTDIR)\canusbdrv.dll"
	-@erase "$(OUTDIR)\canusbdrv.exp"
	-@erase "$(OUTDIR)\canusbdrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CANUSBDRV_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\canusbdrv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\canusb.sbr" \
	"$(INTDIR)\canusbdrv.sbr" \
	"$(INTDIR)\dllist.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\canusbdrv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FTD2XX.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\canusbdrv.pdb" /machine:I386 /def:"..\common\canusbdrv.def" /out:"$(OUTDIR)\canusbdrv.dll" /implib:"$(OUTDIR)\canusbdrv.lib" 
DEF_FILE= \
	"..\common\canusbdrv.def"
LINK32_OBJS= \
	"$(INTDIR)\canusb.obj" \
	"$(INTDIR)\canusbdrv.obj" \
	"$(INTDIR)\dllist.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\canusbdrv.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\canusbdrv.dll" "$(OUTDIR)\canusbdrv.pch" "$(OUTDIR)\canusbdrv.bsc"
   copy release\*.dll D:\WINNT\system32
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "canusbdrv - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\canusbdrv.dll" "$(OUTDIR)\canusbdrv.pch" "$(OUTDIR)\canusbdrv.bsc"


CLEAN :
	-@erase "$(INTDIR)\canusb.obj"
	-@erase "$(INTDIR)\canusb.sbr"
	-@erase "$(INTDIR)\canusbdrv.obj"
	-@erase "$(INTDIR)\canusbdrv.pch"
	-@erase "$(INTDIR)\canusbdrv.sbr"
	-@erase "$(INTDIR)\dllist.obj"
	-@erase "$(INTDIR)\dllist.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\canusbdrv.bsc"
	-@erase "$(OUTDIR)\canusbdrv.dll"
	-@erase "$(OUTDIR)\canusbdrv.exp"
	-@erase "$(OUTDIR)\canusbdrv.ilk"
	-@erase "$(OUTDIR)\canusbdrv.lib"
	-@erase "$(OUTDIR)\canusbdrv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CANUSBDRV_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\canusbdrv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\canusb.sbr" \
	"$(INTDIR)\canusbdrv.sbr" \
	"$(INTDIR)\dllist.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\canusbdrv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FTD2XX.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\canusbdrv.pdb" /debug /machine:I386 /def:"..\common\canusbdrv.def" /out:"$(OUTDIR)\canusbdrv.dll" /implib:"$(OUTDIR)\canusbdrv.lib" /pdbtype:sept 
DEF_FILE= \
	"..\common\canusbdrv.def"
LINK32_OBJS= \
	"$(INTDIR)\canusb.obj" \
	"$(INTDIR)\canusbdrv.obj" \
	"$(INTDIR)\dllist.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\canusbdrv.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\canusbdrv.dll" "$(OUTDIR)\canusbdrv.pch" "$(OUTDIR)\canusbdrv.bsc"
   copy debug\*.dll D:\WINNT\system32
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

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
!IF EXISTS("canusbdrv.dep")
!INCLUDE "canusbdrv.dep"
!ELSE 
!MESSAGE Warning: cannot find "canusbdrv.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "canusbdrv - Win32 Release" || "$(CFG)" == "canusbdrv - Win32 Debug"
SOURCE=.\canusb.c

"$(INTDIR)\canusb.obj"	"$(INTDIR)\canusb.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\canusbdrv.cpp

"$(INTDIR)\canusbdrv.obj"	"$(INTDIR)\canusbdrv.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\common\dllist.c

"$(INTDIR)\dllist.obj"	"$(INTDIR)\dllist.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "canusbdrv - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CANUSBDRV_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\canusbdrv.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\canusbdrv.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "canusbdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CANUSBDRV_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\canusbdrv.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\canusbdrv.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

