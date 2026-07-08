# Microsoft Developer Studio Project File - Name="VoyVideo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VoyVideo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VoyVideo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VoyVideo.mak" CFG="VoyVideo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VoyVideo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VoyVideo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VoyVideo - Win32 Release"

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
# ADD CPP /nologo /MD /w /W0 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 tisgrabber.lib cxcore.lib cv.lib cvaux.lib highgui.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "VoyVideo - Win32 Debug"

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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tisgrabber.lib cxcore.lib cv.lib cvaux.lib highgui.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "VoyVideo - Win32 Release"
# Name "VoyVideo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\VoyInclude\AgentUDP.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\AMCMotion.cpp
# End Source File
# Begin Source File

SOURCE=.\class\ballshot.cpp
# End Source File
# Begin Source File

SOURCE=.\class\ballshot.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\BmpLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Compasee.cpp
# End Source File
# Begin Source File

SOURCE=.\CompassEX.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\ConctDomain.cpp
# End Source File
# Begin Source File

SOURCE=.\CPU_Battery.cpp
# End Source File
# Begin Source File

SOURCE=.\class\Debug_Display.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugView.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Dilation.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDebugInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDebugView.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMotorInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgView.cpp
# End Source File
# Begin Source File

SOURCE=.\class\Dribble.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Erosion.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\GetImage.cpp
# End Source File
# Begin Source File

SOURCE=.\GKRecerive.cpp
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

SOURCE=.\class\LOCALEMU.cpp
# End Source File
# Begin Source File

SOURCE=.\class\Math2.cpp
# End Source File
# Begin Source File

SOURCE=.\class\Movement2Control.cpp
# End Source File
# Begin Source File

SOURCE=.\MPUParser.cpp
# End Source File
# Begin Source File

SOURCE=.\MulticastSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\OmniMove.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\path.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\PieSensor.cpp
# End Source File
# Begin Source File

SOURCE=.\class\Pp.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RadarScan.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RGB2HSI.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RGBShow.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RobotPlayerAMC.cpp
# End Source File
# Begin Source File

SOURCE=.\RototPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SerialPort.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SpreadVis.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\StillView.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\TCPClient.cpp
# End Source File
# Begin Source File

SOURCE=.\TemMapFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TxtArray.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UDPServer.cpp
# End Source File
# Begin Source File

SOURCE=.\UPFullField.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UPGraphics.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UPRColor.cpp
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

SOURCE=.\VoyVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyVideo.rc
# End Source File
# Begin Source File

SOURCE=.\VoyVideoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyWith2Cap.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterMulticast.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\VoyInclude\09Protocol.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\AgentUDP.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\AMCMotion.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\BmpLoader.h
# End Source File
# Begin Source File

SOURCE=.\ConstantFile\CamConstantSet.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Compasee.h
# End Source File
# Begin Source File

SOURCE=.\CompassEX.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\ConctDomain.h
# End Source File
# Begin Source File

SOURCE=.\CPU_Battery.h
# End Source File
# Begin Source File

SOURCE=.\class\Debug_Display.h
# End Source File
# Begin Source File

SOURCE=.\DebugView.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Dilation.h
# End Source File
# Begin Source File

SOURCE=.\DlgDebugInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgDebugView.h
# End Source File
# Begin Source File

SOURCE=.\DlgMotorInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgView.h
# End Source File
# Begin Source File

SOURCE=.\class\Dribble.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Erosion.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\GetImage.h
# End Source File
# Begin Source File

SOURCE=.\GKRecerive.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\HSI_Vision.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\HSIParam.h
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\Inc\HVDAILT.h
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\Inc\HVDef.h
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\Inc\HVUtil.h
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

SOURCE=.\class\LOCALEMU.h
# End Source File
# Begin Source File

SOURCE=.\class\Math2.h
# End Source File
# Begin Source File

SOURCE=.\class\MomentSetFile.h
# End Source File
# Begin Source File

SOURCE=.\class\Movement2Control.h
# End Source File
# Begin Source File

SOURCE=.\MPUParser.h
# End Source File
# Begin Source File

SOURCE=.\MulticastSocket.h
# End Source File
# Begin Source File

SOURCE=.\OmniMove.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\path.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\PieSensor.h
# End Source File
# Begin Source File

SOURCE=.\class\Pp.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\RadarScan.h
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

SOURCE=.\VoyInclude\RobotPlayerAMC.h
# End Source File
# Begin Source File

SOURCE=.\RototPlayer.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SerialPort.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\SpreadVis.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\StillView.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\TCPClient.h
# End Source File
# Begin Source File

SOURCE=.\TemMapFile.h
# End Source File
# Begin Source File

SOURCE=.\TxtArray.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UDPServer.h
# End Source File
# Begin Source File

SOURCE=.\UPFullField.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UPGraphics.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\UPRColor.h
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

SOURCE=.\VoyVideo.h
# End Source File
# Begin Source File

SOURCE=.\VoyVideoDlg.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\VoyWith2Cap.h
# End Source File
# Begin Source File

SOURCE=.\WaterMulticast.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\YUVParam.h
# End Source File
# Begin Source File

SOURCE=.\VoyInclude\Yuzhi.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\22.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bk.bmp
# End Source File
# Begin Source File

SOURCE=.\res\FullField.bmp
# End Source File
# Begin Source File

SOURCE=.\res\OmniMove.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VoyVideo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VoyVideo.ico
# End Source File
# Begin Source File

SOURCE=.\res\VoyVideo.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\XPstyle.manifest
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\Lib\HVDAILT.lib
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\Lib\HVUtil.lib
# End Source File
# Begin Source File

SOURCE=.\CamDHHV\Lib\Raw2Rgb.lib
# End Source File
# End Target
# End Project
