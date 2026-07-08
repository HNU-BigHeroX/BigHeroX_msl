// VoyVideoDlg.h : header file
//

#if !defined(AFX_VOYVIDEODLG_H__05D42E41_34A0_4214_AC44_BE431420B50C__INCLUDED_)
#define AFX_VOYVIDEODLG_H__05D42E41_34A0_4214_AC44_BE431420B50C__INCLUDED_

// #include "VoyInclude/CaptureVideo.h"
// #include "VidDemo.h"
#include "VoyInclude/VoyCmd.h"
#include "VoyInclude/GetImage.h"
#include "VoyInclude/Vision.h"
#include "VoyInclude/path.h"
// #include "VoyInclude/SerialCom.h"
// #include "VoyInclude/Demo.h"
#include "VoyInclude/VoySetting.h"
#include "CompassEX.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "UPFullField.h"
#include "TemMapFile.h"
#include "DlgView.h"
#include "ICCamera.h"
#include "DlgMotorInfo.h"
#include "VoyInclude/SerialPort.h"
#include "TxtArray.h"
#include "DlgDebugInfo.h"
#include "math.h"
#include "DlgDebugView.h"
#include "OmniMove.h"

#include "VoyInclude/RobotPlayerAMC.h"
#include "CPU_Battery.h"
#include "MPUParser.h"

/////////////////////////////////////////////////////////////////////////////
// CVoyVideoDlg dialog

class CVoyVideoDlg : public CDialog
{
    // Construction
  public:
    void EmuCamera();

#ifdef RUN_WITH_EMU
    static UINT EmuOverlook(LPVOID pParam);
#endif
    static UINT JoyReadThread(LPVOID pParam);

    void CreateView();
    void LoadCfg();
    void SaveCfg();
    void m_EnableAll();
    void m_DisableAll();
    void m_printf(CString info);
    void jizuobiao1(int x, int y, double *jiao, double *chang);
    int jiajiao(int jizhun, int jiao);
    void Controlmoto(int jiao, int sudu, int zhuan, int *motol, int *motor, int *motob);
    CVoyVideoDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CVoyVideoDlg)
    enum
    {
        IDD = IDD_VOYVIDEO_DIALOG
    };
    COmniMove m_cOmniMove;
    CComboBox m_cOverlookCombo;
    CListBox m_cStatusList;
    CUPFullField m_cField;
    CEdit m_cPosShow;
    int m_nPort;
    CString m_strCoachIP;
    UINT m_nAgentID;
    double m_nHoldBall;
    CString m_setword;
    int m_nSubSpeed;
    int m_nSubSpeedR;
    int m_nKickStrength;
    int m_nCamOffsetX;
    int m_nCamOffsetY;
    int m_nLiftPos;
    int m_nMpuPort;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CVoyVideoDlg)
  public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);

  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                     //}}AFX_VIRTUAL

    // Implementation
  protected:
    //	CUPFullField* changdi;
    CVoySetting s;
    Voy3Set m_cfg;
    CTemMapFile m_mapfile;
    int button;
    int button1;
    int motol;
    int motor;
    int motob;
    int zhuand;
    double jiao;
    double chang;

    CPoint mos;
    //	Cpath m_path;
    HICON m_hIcon;
    //	CSerialCom m_Com;
    //	CSerialPort m_Com;
    //	CSerialPort m_Com_2;
    //	CVoyCmd m_Cmd;

    //	CCompassEX m_Compass;
    //	IBehavior m_idle;		//空闲行为对象
    //	CVoyCmd m_cmd;			//指令协议类对象
    // CVidDemo m_vdemo;		//视觉行为类对象
    CGetImage m_find;
    //	HICON m_hIcon;
    // CCamDHHV m_Camera;
    CICCamera m_Camera;

    CDlgView *m_dlgView;

    short speed[5];

    CDlgMotorInfo *m_dlgMInfo;

    CDlgDebugInfo *m_pdlgDebug;

    CDlgDebugView *m_pdlgDView;

    // CRobotPlayerAMC m_robot;
    CVoyCmd m_robot;
    CCPU_Battery m_cpu_battery;
    CMPUParser m_mpu;

    // Generated message map functions
    //{{AFX_MSG(CVoyVideoDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnCapstart();
    afx_msg void OnStopcap();
    afx_msg void OnRemove();
    afx_msg void OnHsi();
    afx_msg void OnYuzhi1();
    afx_msg void OnButton1();
    afx_msg void OnButton2();
    afx_msg void OnButton4();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnButton6();
    afx_msg void OnCntcoach();
    afx_msg void OnChangeid();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnAutostart();
    afx_msg void OnParam();
    afx_msg void OnUpdateparam();
    afx_msg void OnClose();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnViewClosing(WPARAM wParam, LPARAM lParam);
    afx_msg void OnViewMoving(WPARAM wParam, LPARAM lParam);
    afx_msg void OnViewKeyDown(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSucball();
    afx_msg void OnInfodis();
    afx_msg void OnMInfoClosing(WPARAM wParam, LPARAM lParam);
    afx_msg void OnNowatchdog();
    afx_msg void OnShowvideo();
    afx_msg void OnDebugInfo();
    afx_msg void OnShowdbgview();
    afx_msg void OnNetsend();
    afx_msg void OnAmcEn();
    afx_msg void OnAmcBrake();
    afx_msg void OnAmcDisen();
    afx_msg void OnCamSetOffset();
    afx_msg void OnSetLiftpos();
    afx_msg void OnOpenMpuPort();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOYVIDEODLG_H__05D42E41_34A0_4214_AC44_BE431420B50C__INCLUDED_)
