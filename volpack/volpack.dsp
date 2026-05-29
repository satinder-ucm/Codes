# Microsoft Developer Studio Project File - Name="volpack" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=volpack - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "volpack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "volpack.mak" CFG="volpack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "volpack - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "volpack - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "volpack - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE F90 /compile_only /nologo /warn:nofileopt
# ADD F90 /compile_only /nologo /warn:nofileopt
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W1 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "COMP_AC11B" /D "COMP_AC31B" /D "COMP_AC32B" /D "COMP_AR11B" /D "COMP_AR31B" /D "COMP_AR32B" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "volpack - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE F90 /check:bounds /compile_only /dbglibs /debug:full /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD F90 /check:bounds /compile_only /dbglibs /debug:full /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W1 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "COMP_AC11B" /D "COMP_AC31B" /D "COMP_AC32B" /D "COMP_AR11B" /D "COMP_AR31B" /D "COMP_AR32B" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "volpack - Win32 Release"
# Name "volpack - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;f90;for;f;fpp"
# Begin Source File

SOURCE=.\vp_check.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC00G.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC11B.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC1NB.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC1NS.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC1PB.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC31B.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC32B.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC3NB.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC3NS.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAC3PB.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR00G.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR11B.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR1NB.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR1NS.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR1PB.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR31B.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR32B.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR3NB.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR3NS.c
# End Source File
# Begin Source File

SOURCE=.\vp_compAR3PB.c
# End Source File
# Begin Source File

SOURCE=.\vp_context.c
# End Source File
# Begin Source File

SOURCE=.\vp_extract.c
# End Source File
# Begin Source File

SOURCE=.\vp_file.c
# End Source File
# Begin Source File

SOURCE=.\vp_linalg.c
# End Source File
# Begin Source File

SOURCE=.\vp_octree.c
# End Source File
# Begin Source File

SOURCE=.\vp_renderA.c
# End Source File
# Begin Source File

SOURCE=.\vp_renderB.c
# End Source File
# Begin Source File

SOURCE=.\vp_renderC.c
# End Source File
# Begin Source File

SOURCE=.\vp_renderR.c
# End Source File
# Begin Source File

SOURCE=.\vp_resample.c
# End Source File
# Begin Source File

SOURCE=.\vp_rle.c
# End Source File
# Begin Source File

SOURCE=.\vp_shade.c
# End Source File
# Begin Source File

SOURCE=.\vp_transpose.c
# End Source File
# Begin Source File

SOURCE=.\vp_util.c
# End Source File
# Begin Source File

SOURCE=.\vp_view.c
# End Source File
# Begin Source File

SOURCE=.\vp_warp.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA101N.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA110N.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA111N.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA301N.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA330N.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA330R.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA331N.c
# End Source File
# Begin Source File

SOURCE=.\vp_warpA331R.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\volpack.h
# End Source File
# Begin Source File

SOURCE=.\vp_global.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\vp_CompA.m4

!IF  "$(CFG)" == "volpack - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\vp_CompA.m4

BuildCmds= \
	m4 -s -DSourceFile=vp_compAC1NB.c vp_CompA.m4 > vp_compAC1NB.c \
	m4 -s -DSourceFile=vp_compAC3NB.c vp_CompA.m4 > vp_compAC3NB.c \
	m4 -s -DSourceFile=vp_compAR1NB.c vp_CompA.m4 > vp_compAR1NB.c \
	m4 -s -DSourceFile=vp_compAR3NB.c vp_CompA.m4 > vp_compAR3NB.c \
	m4 -s -DSourceFile=vp_compAC1PB.c vp_CompA.m4 > vp_compAC1PB.c \
	m4 -s -DSourceFile=vp_compAC3PB.c vp_CompA.m4 > vp_compAC3PB.c \
	m4 -s -DSourceFile=vp_compAR1PB.c vp_CompA.m4 > vp_compAR1PB.c \
	m4 -s -DSourceFile=vp_compAR3PB.c vp_CompA.m4 > vp_compAR3PB.c \
	m4 -s -DSourceFile=vp_compAC00G.c vp_CompA.m4 > vp_compAC00G.c \
	m4 -s -DSourceFile=vp_compAR00G.c vp_CompA.m4 > vp_compAR00G.c \
	m4 -s -DSourceFile=vp_compAC1NS.c vp_CompA.m4 > vp_compAC1NS.c \
	m4 -s -DSourceFile=vp_compAC3NS.c vp_CompA.m4 > vp_compAC3NS.c \
	m4 -s -DSourceFile=vp_compAR1NS.c vp_CompA.m4 > vp_compAR1NS.c \
	m4 -s -DSourceFile=vp_compAR3NS.c vp_CompA.m4 > vp_compAR3NS.c \
	m4 -s -DSourceFile=vp_compAC11B.c vp_CompA.m4 > vp_compAC11B.c \
	m4 -s -DSourceFile=vp_compAC31B.c vp_CompA.m4 > vp_compAC31B.c \
	m4 -s -DSourceFile=vp_compAC32B.c vp_CompA.m4 > vp_compAC32B.c \
	m4 -s -DSourceFile=vp_compAR11B.c vp_CompA.m4 > vp_compAR11B.c \
	m4 -s -DSourceFile=vp_compAR31B.c vp_CompA.m4 > vp_compAR31B.c \
	m4 -s -DSourceFile=vp_compAR32B.c vp_CompA.m4 > vp_compAR32B.c \
	

"vp_compAC1NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC3NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR1NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR3NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC1PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC3PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR1PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR3PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC00G.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR00G.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC1NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC3NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR1NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR3NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC11B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC31B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC32B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR11B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR31B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR32B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "volpack - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\vp_CompA.m4

BuildCmds= \
	m4 -s -DSourceFile=vp_compAC1NB.c vp_CompA.m4 > vp_compAC1NB.c \
	m4 -s -DSourceFile=vp_compAC3NB.c vp_CompA.m4 > vp_compAC3NB.c \
	m4 -s -DSourceFile=vp_compAR1NB.c vp_CompA.m4 > vp_compAR1NB.c \
	m4 -s -DSourceFile=vp_compAR3NB.c vp_CompA.m4 > vp_compAR3NB.c \
	m4 -s -DSourceFile=vp_compAC1PB.c vp_CompA.m4 > vp_compAC1PB.c \
	m4 -s -DSourceFile=vp_compAC3PB.c vp_CompA.m4 > vp_compAC3PB.c \
	m4 -s -DSourceFile=vp_compAR1PB.c vp_CompA.m4 > vp_compAR1PB.c \
	m4 -s -DSourceFile=vp_compAR3PB.c vp_CompA.m4 > vp_compAR3PB.c \
	m4 -s -DSourceFile=vp_compAC00G.c vp_CompA.m4 > vp_compAC00G.c \
	m4 -s -DSourceFile=vp_compAR00G.c vp_CompA.m4 > vp_compAR00G.c \
	m4 -s -DSourceFile=vp_compAC1NS.c vp_CompA.m4 > vp_compAC1NS.c \
	m4 -s -DSourceFile=vp_compAC3NS.c vp_CompA.m4 > vp_compAC3NS.c \
	m4 -s -DSourceFile=vp_compAR1NS.c vp_CompA.m4 > vp_compAR1NS.c \
	m4 -s -DSourceFile=vp_compAR3NS.c vp_CompA.m4 > vp_compAR3NS.c \
	m4 -s -DSourceFile=vp_compAC11B.c vp_CompA.m4 > vp_compAC11B.c \
	m4 -s -DSourceFile=vp_compAC31B.c vp_CompA.m4 > vp_compAC31B.c \
	m4 -s -DSourceFile=vp_compAC32B.c vp_CompA.m4 > vp_compAC32B.c \
	m4 -s -DSourceFile=vp_compAR11B.c vp_CompA.m4 > vp_compAR11B.c \
	m4 -s -DSourceFile=vp_compAR31B.c vp_CompA.m4 > vp_compAR31B.c \
	m4 -s -DSourceFile=vp_compAR32B.c vp_CompA.m4 > vp_compAR32B.c \
	

"vp_compAC1NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC3NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR1NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR3NB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC1PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC3PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR1PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR3PB.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC00G.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR00G.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC1NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC3NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR1NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR3NS.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC11B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC31B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAC32B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR11B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR31B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_compAR32B.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vp_WarpA.m4

!IF  "$(CFG)" == "volpack - Win32 Release"

# Begin Custom Build
InputPath=.\vp_WarpA.m4

BuildCmds= \
	m4 -s -DSourceFile=vp_warpA101N.c vp_WarpA.m4 > vp_warpA101N.c \
	m4 -s -DSourceFile=vp_warpA301N.c vp_WarpA.m4 > vp_warpA301N.c \
	m4 -s -DSourceFile=vp_warpA110N.c vp_WarpA.m4 > vp_warpA110N.c \
	m4 -s -DSourceFile=vp_warpA111N.c vp_WarpA.m4 > vp_warpA111N.c \
	m4 -s -DSourceFile=vp_warpA330N.c vp_WarpA.m4 > vp_warpA330N.c \
	m4 -s -DSourceFile=vp_warpA331N.c vp_WarpA.m4 > vp_warpA331N.c \
	m4 -s -DSourceFile=vp_warpA330R.c vp_WarpA.m4 > vp_warpA330R.c \
	m4 -s -DSourceFile=vp_warpA331R.c vp_WarpA.m4 > vp_warpA331R.c \
	

"vp_warpA101N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA301N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA110N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA111N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA330N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA331N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA330R.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA331R.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "volpack - Win32 Debug"

# Begin Custom Build
InputPath=.\vp_WarpA.m4

BuildCmds= \
	m4 -s -DSourceFile=vp_warpA101N.c vp_WarpA.m4 > vp_warpA101N.c \
	m4 -s -DSourceFile=vp_warpA301N.c vp_WarpA.m4 > vp_warpA301N.c \
	m4 -s -DSourceFile=vp_warpA110N.c vp_WarpA.m4 > vp_warpA110N.c \
	m4 -s -DSourceFile=vp_warpA111N.c vp_WarpA.m4 > vp_warpA111N.c \
	m4 -s -DSourceFile=vp_warpA330N.c vp_WarpA.m4 > vp_warpA330N.c \
	m4 -s -DSourceFile=vp_warpA331N.c vp_WarpA.m4 > vp_warpA331N.c \
	m4 -s -DSourceFile=vp_warpA330R.c vp_WarpA.m4 > vp_warpA330R.c \
	m4 -s -DSourceFile=vp_warpA331R.c vp_WarpA.m4 > vp_warpA331R.c \
	

"vp_warpA101N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA301N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA110N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA111N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA330N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA331N.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA330R.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"vp_warpA331R.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
