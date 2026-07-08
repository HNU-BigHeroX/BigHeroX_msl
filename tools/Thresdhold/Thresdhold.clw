; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDlg_FramePreview
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Thresdhold.h"

ClassCount=10
Class1=CThresdholdApp
Class2=CThresdholdDlg
Class3=CAboutDlg

ResourceCount=6
Resource1=IDD_THRESDHOLD_DIALOG
Resource2=IDR_MAINFRAME
Class4=CColor
Class5=CColorList
Class6=CStillView
Class7=CItemList
Resource3=IDD_CAMERAPROC_DIALOG
Class8=CCameraProcDlg
Resource4=IDD_DHCAMERA_DIALOG
Class9=CDHCameraProc
Resource5=IDD_ABOUTBOX
Class10=CDlg_FramePreview
Resource6=IDD_FRAME_PREVIEW_DIALOG

[CLS:CThresdholdApp]
Type=0
HeaderFile=Thresdhold.h
ImplementationFile=Thresdhold.cpp
Filter=N

[CLS:CThresdholdDlg]
Type=0
HeaderFile=ThresdholdDlg.h
ImplementationFile=ThresdholdDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CThresdholdDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=ThresdholdDlg.h
ImplementationFile=ThresdholdDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_INFO,static,1342308352

[DLG:IDD_THRESDHOLD_DIALOG]
Type=1
Class=CThresdholdDlg
ControlCount=24
Control1=IDC_COLOR,static,1342308608
Control2=IDC_VIEW,static,1342177540
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,button,1342177287
Control5=IDC_CREATCAP,button,1342242816
Control6=IDC_CLOSECAP,button,1476460544
Control7=IDC_COMBO1,combobox,1478557954
Control8=IDC_SAVEHSI,button,1342242816
Control9=IDC_LOADHSI,button,1342242816
Control10=IDC_PREVIEW,button,1342242816
Control11=IDC_DSPPROP,button,1342242816
Control12=IDC_OVERCENTER,button,1342242819
Control13=IDC_FRONTANG,button,1073807363
Control14=IDC_ITEMLIST,SysListView32,1350631425
Control15=IDC_VIDEO,static,1342177284
Control16=IDC_STATIC,button,1342177287
Control17=IDC_INFOLIST,listbox,1352728832
Control18=IDC_CAPTURESTILL,button,1342242816
Control19=IDC_LOADIMG,button,1342242816
Control20=IDC_VIDEOEMU,button,1476460544
Control21=IDC_FRAME_PREVIEW,button,1342242816
Control22=IDC_BUTTON1,button,1342242816
Control23=IDC_BUTTON2,button,1342242816
Control24=IDC_BUTTON3,button,1342242816

[CLS:CColor]
Type=0
HeaderFile=Color.h
ImplementationFile=Color.cpp
BaseClass=CStatic
Filter=W
VirtualFilter=WC

[CLS:CColorList]
Type=0
HeaderFile=ColorList.h
ImplementationFile=ColorList.cpp
BaseClass=CListBox
Filter=W
VirtualFilter=bWC

[CLS:CStillView]
Type=0
HeaderFile=StillView.h
ImplementationFile=StillView.cpp
BaseClass=CStatic
Filter=W
VirtualFilter=WC
LastObject=CStillView

[CLS:CItemList]
Type=0
HeaderFile=ItemList.h
ImplementationFile=ItemList.cpp
BaseClass=CListCtrl
Filter=W
LastObject=CItemList
VirtualFilter=FWC

[DLG:IDD_CAMERAPROC_DIALOG]
Type=1
Class=CCameraProcDlg
ControlCount=30
Control1=IDC_STATIC,static,1342308352
Control2=IDC_WB_SLIDER,msctls_trackbar32,1342242840
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EXPO_SLIDER,msctls_trackbar32,1342242840
Control5=IDC_STATIC,static,1342308352
Control6=IDC_GAIN_SLIDER,msctls_trackbar32,1342242840
Control7=IDC_STATIC,static,1342308352
Control8=IDC_BRI_SLIDER,msctls_trackbar32,1342242840
Control9=IDC_STATIC,static,1342308352
Control10=IDC_CONT_SLIDER,msctls_trackbar32,1342242840
Control11=IDC_STATIC,static,1342308352
Control12=IDC_SATU_SLIDER,msctls_trackbar32,1342242840
Control13=IDC_STATIC,static,1342308352
Control14=IDC_SHAR_SLIDER,msctls_trackbar32,1342242840
Control15=IDC_STATIC,static,1342308352
Control16=IDC_BACKL_SLIDER,msctls_trackbar32,1342242840
Control17=IDOK,button,1342242817
Control18=IDCANCEL,button,1342242816
Control19=IDC_STATIC,button,1342177287
Control20=IDC_STATIC,button,1342177287
Control21=IDC_WB_VAL,edit,1350631552
Control22=IDC_EXPO_VAL,edit,1350631552
Control23=IDC_GAIN_VAL,edit,1350631552
Control24=IDC_BRI_VAL,edit,1350631552
Control25=IDC_CONT_VAL,edit,1350631552
Control26=IDC_SATU_VAL,edit,1350631552
Control27=IDC_SHAR_VAL,edit,1350631552
Control28=IDC_BACKL_VAL,edit,1350631552
Control29=IDC_DEFAULTVAL,button,1342242816
Control30=IDC_CAMERACOMBO,combobox,1075904770

[CLS:CCameraProcDlg]
Type=0
HeaderFile=CameraProcDlg.h
ImplementationFile=CameraProcDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_GAIN_VAL
VirtualFilter=dWC

[DLG:IDD_DHCAMERA_DIALOG]
Type=1
Class=CDHCameraProc
ControlCount=30
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,static,1342308352
Control6=IDC_GAIN2_SLIDER,msctls_trackbar32,1342242840
Control7=IDC_GAIN2_VAL,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EU_SLIDER,msctls_trackbar32,1342242840
Control10=IDC_EU_VAL,edit,1350631552
Control11=IDC_STATIC,static,1476526080
Control12=IDC_LUTALL_SLIDER,msctls_trackbar32,1476460568
Control13=IDC_LUTALL_VAL,edit,1484849280
Control14=IDC_STATIC,static,1476526080
Control15=IDC_LUTRED_SLIDER,msctls_trackbar32,1476460568
Control16=IDC_LUTRED_VAL,edit,1484849280
Control17=IDC_STATIC,static,1476526080
Control18=IDC_LUTGREEN_SLIDER,msctls_trackbar32,1476460568
Control19=IDC_LUTGREEN_VAL,edit,1484849280
Control20=IDC_STATIC,static,1476526080
Control21=IDC_LUTBLUE_SLIDER,msctls_trackbar32,1476460568
Control22=IDC_LUTBLUE_VAL,edit,1484849280
Control23=IDC_STATIC,static,1342308352
Control24=IDC_EL_SLIDER,msctls_trackbar32,1342242840
Control25=IDC_EL_VAL,edit,1350631552
Control26=IDC_STATIC,button,1342177287
Control27=IDC_STATIC,static,1342308352
Control28=IDC_CM_ADC,combobox,1344339971
Control29=IDC_STATIC,static,1342308352
Control30=IDC_CM_GAIN,combobox,1344339970

[CLS:CDHCameraProc]
Type=0
HeaderFile=DHCameraProc.h
ImplementationFile=DHCameraProc.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CM_ADC
VirtualFilter=dWC

[CLS:CDlg_FramePreview]
Type=0
HeaderFile=Dlg_FramePreview.h
ImplementationFile=Dlg_FramePreview.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_FRAME_1
VirtualFilter=dWC

[DLG:IDD_FRAME_PREVIEW_DIALOG]
Type=1
Class=CDlg_FramePreview
ControlCount=7
Control1=IDC_FRAME_PRE,static,1342177284
Control2=IDC_DIS_FRAMEPRE,static,1342308352
Control3=IDC_FRAME_NEXT,static,1342177284
Control4=IDC_DIS_FRAMENEXT,static,1342308352
Control5=IDC_STATIC,button,1342177287
Control6=IDC_FRAME_GOPRE,button,1342242816
Control7=IDC_FRAME_GONEXT,button,1342242816

