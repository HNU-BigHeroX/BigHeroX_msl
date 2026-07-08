// 展示机器人的状态
#if !defined(AFX_DISROBOTSTATUS_H__31FB9B92_E0BD_4D5F_BD79_52366FD2A926__INCLUDED_)
#define AFX_DISROBOTSTATUS_H__31FB9B92_E0BD_4D5F_BD79_52366FD2A926__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisRobotStatus.h : header file
//

#include "GreenField.h"
#include "CoachUDP.h"
/////////////////////////////////////////////////////////////////////////////
// CDisRobotStatus window

class CDisRobotStatus : public CStatic
{
    // Construction
  public:
    CDisRobotStatus();

    // Attributes
  public:
    CGreenField *pField;  // 指向地图对象（获取agents数据）
    CCoachUDP *pCoachUDP; // 指向coach通讯对象（获取帧率）

    CDC dcMemory;
    CBitmap m_bk;
    CRect m_FrameRect;
    CRect m_rcRobot[5];
    CFont fontTitle;
    CFont fontText;

    CBitmap bmpACPlug; // 电脑交流电输入图标
    CDC dcACPlug;
    CBitmap bmpBatteryCharging; // 充电图标
    CDC dcBatteryCharging;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDisRobotStatus)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CDisRobotStatus();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CDisRobotStatus)
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISROBOTSTATUS_H__31FB9B92_E0BD_4D5F_BD79_52366FD2A926__INCLUDED_)
