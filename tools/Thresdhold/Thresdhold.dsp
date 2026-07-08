# Microsoft Developer Studio Project File - Name="Thresdhold" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Thresdhold - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Thresdhold.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Thresdhold.mak" CFG="Thresdhold - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Thresdhold - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Thresdhold - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Thresdhold - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 tisgrabber.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Thresdhold - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tisgrabber.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Thresdhold - Win32 Release"
# Name "Thresdhold - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\VoyInclude\AgentNet.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\BmpLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraProcDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorList.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorTest.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Compasee.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\ConctDomain.cpp
# End Source File
# Begin Source File

SOURCE=.\DHCameraProc.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Dilation.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_FramePreview.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Erosion.cpp
# End Source File
# Begin Source File

SOURCE=.\GBRGMalvar.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\HSI_Vision.cpp
# End Source File
# Begin Source File

SOURCE=.\ICCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\IntPID.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemList.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\NetCmt.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\NetSend.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\PieSensor.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RGB2HSI.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RGBShow.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SerialCom.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SpreadVis.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StillView.cpp
# End Source File
# Begin Source File

SOURCE=.\Thresdhold.cpp
# End Source File
# Begin Source File

SOURCE=.\Thresdhold.rc
# End Source File
# Begin Source File

SOURCE=.\ThresdholdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UPGraphics.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Vision.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyHSI.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoySetting.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyWith2Cap.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\VoyInclude\AgentNet.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\BmpLoader.h
# End Source File
# Begin Source File

SOURCE=.\CameraProcDlg.h
# End Source File
# Begin Source File

SOURCE=.\Color.h
# End Source File
# Begin Source File

SOURCE=.\ColorList.h
# End Source File
# Begin Source File

SOURCE=.\ColorTest.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Compasee.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\ConctDomain.h
# End Source File
# Begin Source File

SOURCE=.\DHCameraProc.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Dilation.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_FramePreview.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Erosion.h
# End Source File
# Begin Source File

SOURCE=.\GBRGMalvar.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\HSI_Vision.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\HSIParam.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\IBehavior.h
# End Source File
# Begin Source File

SOURCE=.\ICCamera.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\IntPID.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\IPhy.h
# End Source File
# Begin Source File

SOURCE=.\ItemList.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\MSG_Control.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\MSG_Status.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\NetCmt.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\NetSend.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\PieSensor.h
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\Inc\Raw2Rgb.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RGB2HSI.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RGBShow.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SerialCom.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SpreadVis.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StillView.h
# End Source File
# Begin Source File

SOURCE=.\Thresdhold.h
# End Source File
# Begin Source File

SOURCE=.\ThresdholdDlg.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UPGraphics.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Vision.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyCmd.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyHSI.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoySetting.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyWith2Cap.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\YUVParam.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Color.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Pic00008.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Thresdhold.ico
# End Source File
# Begin Source File

SOURCE=.\res\Thresdhold.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\style.manifest
# End Source File
# End Target
# End Project
