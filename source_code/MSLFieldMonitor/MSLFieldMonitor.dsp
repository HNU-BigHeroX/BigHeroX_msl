# Microsoft Developer Studio Project File - Name="MSLFieldMonitor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MSLFieldMonitor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MSLFieldMonitor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MSLFieldMonitor.mak" CFG="MSLFieldMonitor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MSLFieldMonitor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MSLFieldMonitor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MSLFieldMonitor - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 gdiplus.lib cxcore.lib cv.lib cvaux.lib highgui.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MSLFieldMonitor - Win32 Debug"

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
# ADD LINK32 gdiplus.lib cxcore.lib cv.lib cvaux.lib highgui.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MSLFieldMonitor - Win32 Release"
# Name "MSLFieldMonitor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BallMoveEstimation.cpp
# End Source File
# Begin Source File

SOURCE=.\CatchPosition.cpp
# End Source File
# Begin Source File

SOURCE=.\CoachUDP.cpp
# End Source File
# Begin Source File

SOURCE=.\DataCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\DisRobotStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DisRoleStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DisSideClr.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAgentIPList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLocIPInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GreenField.cpp
# End Source File
# Begin Source File

SOURCE=.\DepthMap\IDepthMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\IRolePlay.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\ITactic.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\DepthMap\Map_FreeKick_Catch.cpp
# End Source File
# Begin Source File

SOURCE=.\DepthMap\Map_Playing_Catch.cpp
# End Source File
# Begin Source File

SOURCE=.\MSLFieldMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\MSLFieldMonitor.rc
# End Source File
# Begin Source File

SOURCE=.\MSLFieldMonitorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MSLFieldMonitorView.cpp
# End Source File
# Begin Source File

SOURCE=.\ObstFusion.cpp
# End Source File
# Begin Source File

SOURCE=.\ObstFusion_ZWJ.cpp
# End Source File
# Begin Source File

SOURCE=.\PassSwitch.cpp
# End Source File
# Begin Source File

SOURCE=.\RefClient.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Attack.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Catch.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_CatchFocus.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_CatchMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_DefBall.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Defence.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_DefGoal.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_FocusOnBall.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_GoalKeeper.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Manual.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_MoveTo.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Pass.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_PassMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Penalty.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_SearchBall.cpp
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Stop.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Counter_FreeKick.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Counter_KickOff.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Counter_Penalty.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_DroppedBall.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_FreeKick.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_KickOff.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Off.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_ParkIn.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Penalty.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Playing.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Playing_AreaDef.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Stop.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_TechChallenge.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_TechChallenge_Simple.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_4Pass.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_AllDef.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_CatchMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_MoveARound.cpp
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_Passing.cpp
# End Source File
# Begin Source File

SOURCE=.\TCPClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ToAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\UDPReceive.cpp
# End Source File
# Begin Source File

SOURCE=.\UDPSend.cpp
# End Source File
# Begin Source File

SOURCE=.\WndField.cpp
# End Source File
# Begin Source File

SOURCE=.\WndMap.cpp
# End Source File
# Begin Source File

SOURCE=.\WndMatchPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\WndReferee.cpp
# End Source File
# Begin Source File

SOURCE=.\WndStatus.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BallMoveEstimation.h
# End Source File
# Begin Source File

SOURCE=.\CatchPosition.h
# End Source File
# Begin Source File

SOURCE=.\CoachUDP.h
# End Source File
# Begin Source File

SOURCE=.\DataCenter.h
# End Source File
# Begin Source File

SOURCE=.\DisRobotStatus.h
# End Source File
# Begin Source File

SOURCE=.\DisRoleStatus.h
# End Source File
# Begin Source File

SOURCE=.\DisSideClr.h
# End Source File
# Begin Source File

SOURCE=.\DlgAgentIPList.h
# End Source File
# Begin Source File

SOURCE=.\DlgLocIPInfo.h
# End Source File
# Begin Source File

SOURCE=.\GreenField.h
# End Source File
# Begin Source File

SOURCE=.\DepthMap\IDepthMap.h
# End Source File
# Begin Source File

SOURCE=.\Role\IRolePlay.h
# End Source File
# Begin Source File

SOURCE=.\Tac\ITactic.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\DepthMap\Map_FreeKick_Catch.h
# End Source File
# Begin Source File

SOURCE=.\DepthMap\Map_Playing_Catch.h
# End Source File
# Begin Source File

SOURCE=.\MSL_Structs.h
# End Source File
# Begin Source File

SOURCE=.\MSLFieldMonitor.h
# End Source File
# Begin Source File

SOURCE=.\MSLFieldMonitorDoc.h
# End Source File
# Begin Source File

SOURCE=.\MSLFieldMonitorView.h
# End Source File
# Begin Source File

SOURCE=.\ObstFusion.h
# End Source File
# Begin Source File

SOURCE=.\ObstFusion_ZWJ.h
# End Source File
# Begin Source File

SOURCE=.\PassSwitch.h
# End Source File
# Begin Source File

SOURCE=.\RefClient.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Attack.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Catch.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_CatchFocus.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_CatchMove.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_DefBall.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Defence.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_DefGoal.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_FocusOnBall.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_GoalKeeper.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Manual.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_MoveTo.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Pass.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_PassMove.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Penalty.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_SearchBall.h
# End Source File
# Begin Source File

SOURCE=.\Role\Role_Stop.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Counter_FreeKick.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Counter_KickOff.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Counter_Penalty.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_DroppedBall.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_FreeKick.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_KickOff.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Off.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_ParkIn.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Penalty.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Playing.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Playing_AreaDef.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Stop.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_TechChallenge.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_TechChallenge_Simple.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_4Pass.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_AllDef.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_CatchMove.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_MoveARound.h
# End Source File
# Begin Source File

SOURCE=.\Tac\Tac_Test_Passing.h
# End Source File
# Begin Source File

SOURCE=.\TCPClient.h
# End Source File
# Begin Source File

SOURCE=.\ToAgent.h
# End Source File
# Begin Source File

SOURCE=.\UDPReceive.h
# End Source File
# Begin Source File

SOURCE=.\UDPSend.h
# End Source File
# Begin Source File

SOURCE=.\WndField.h
# End Source File
# Begin Source File

SOURCE=.\WndMap.h
# End Source File
# Begin Source File

SOURCE=.\WndMatchPanel.h
# End Source File
# Begin Source File

SOURCE=.\WndReferee.h
# End Source File
# Begin Source File

SOURCE=.\WndStatus.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AC_Plug.bmp
# End Source File
# Begin Source File

SOURCE=.\res\battery_charging.bmp
# End Source File
# Begin Source File

SOURCE=.\res\L2RFullField.bmp
# End Source File
# Begin Source File

SOURCE=.\res\L2RFullField2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MSLFieldMonitor.ico
# End Source File
# Begin Source File

SOURCE=.\res\MSLFieldMonitor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\MSLFieldMonitorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Field_LR.png
# End Source File
# Begin Source File

SOURCE=.\res\Field_RL.png
# End Source File
# Begin Source File

SOURCE=.\res\Flip.png
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\SetPlayerIn.png
# End Source File
# Begin Source File

SOURCE=.\res\ShowDefTarget.png
# End Source File
# Begin Source File

SOURCE=.\res\ShowEnemyTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\ShowObst.png
# End Source File
# Begin Source File

SOURCE=.\res\XPstyle.manifest
# End Source File
# End Target
# End Project
