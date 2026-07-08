; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMSLFieldLineDlg
LastTemplate=CStatic
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MSLFieldLine.h"

ClassCount=4
Class1=CMSLFieldLineApp
Class2=CMSLFieldLineDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CFieldMap
Resource3=IDD_MSLFIELDLINE_DIALOG

[CLS:CMSLFieldLineApp]
Type=0
HeaderFile=MSLFieldLine.h
ImplementationFile=MSLFieldLine.cpp
Filter=N

[CLS:CMSLFieldLineDlg]
Type=0
HeaderFile=MSLFieldLineDlg.h
ImplementationFile=MSLFieldLineDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_COR_R

[CLS:CAboutDlg]
Type=0
HeaderFile=MSLFieldLineDlg.h
ImplementationFile=MSLFieldLineDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MSLFIELDLINE_DIALOG]
Type=1
Class=CMSLFieldLineDlg
ControlCount=26
Control1=IDC_FIELDMAP,static,1342177284
Control2=IDC_CREATE,button,1342242816
Control3=IDC_CREATEBMP,button,1342242816
Control4=IDC_MAPVIEW,static,1342177284
Control5=IDC_DISPLAY,button,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_INFOLIST,listbox,1352728833
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_FIELD_LEN,edit,1350631552
Control12=IDC_FIELD_WID,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_MID_DIAMETER,edit,1350631552
Control18=IDC_COR_R,edit,1350631552
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_GOAL_LEN,edit,1350631552
Control22=IDC_GOAL_WID,edit,1350631552
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_PEN_LEN,edit,1350631552
Control26=IDC_PEN_WID,edit,1350631552

[CLS:CFieldMap]
Type=0
HeaderFile=FieldMap.h
ImplementationFile=FieldMap.cpp
BaseClass=CStatic
Filter=W
VirtualFilter=WC

