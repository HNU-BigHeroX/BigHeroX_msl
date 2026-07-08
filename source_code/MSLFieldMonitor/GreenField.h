// 中间的绿色场地显示等
#if !defined(AFX_GREENFIELD_H__F8D8FEEA_9DB0_4A32_A523_09CD4635A1E1__INCLUDED_)
#define AFX_GREENFIELD_H__F8D8FEEA_9DB0_4A32_A523_09CD4635A1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GreenField.h : header file
//

#include "MSL_Structs.h"
#include "CoachUDP.h"
#include "DataCenter.h"
#include "ObstFusion_ZWJ.h"

// CGreenField
#define AGENT_TEST_NONE 0
#define AGENT_TEST_DEF_BALL 1
#define AGENT_TEST_DEF_GOAL 2
#define AGENT_TEST_PASS 3
#define AGENT_TEST_CATCH 4

#define OBST_NUM_NONE 0
#define OBST_NUM_CLOSETO_OWNGOAL 1
#define OBST_NUM_BACK_DEF 2

/////////////////////////////////////////////////////////////////////////////
// CGreenField window

class CGreenField : public CStatic
{
    // Construction
  public:
    CGreenField();

    // Attributes
  public:
    stDataSummary *pDataSummary; // 场地数据汇总
    CCoachUDP *pCoachUDP;        // 数据接收对象

    CBitmap m_BkBmp;
    CRect m_FrameRect;
    CDC dcMemory;
    // Graphics* m_graphScreen;

    Image *m_imgFieldLR; // 从左到右
    Image *m_imgFieldRL; // 从右到左
    RectF m_scrImg;

    int m_nSelAgentID;   // 选择机器人的ID
    bool m_bLeftToRight; // 地图从左到右
    Rect m_rcFlip;       // 翻转开关点击位置
    Image *m_imgFlip;    // 设置入场点图标

    // 设置入场位置
    CDataCenter *pDataCenter;
    bool m_bSetPlayerIn;
    Rect m_rcSetPlayerIn;    // 设置入场点开关点击位置
    Image *m_imgSetPlayerIn; // 设置入场点图标

    bool m_bShowOrgObst;     // 显示原始数据中的障碍物点
    Rect m_rcShowOrgObst;    // 设置显示原始障碍物点的目击位置
    Image *m_imgShowOrgObst; // 显示原始障碍物点图标

    bool m_bShowEnemyTrack;  // 显示障碍物追踪状态
    Rect m_rcShowEnemyTrack; // 设置显示障碍物追踪状态
    Image *m_imgEnemyTrack;  // 障碍物追踪状态图标

    int m_nObstNumMode; // 障碍物显示数字模式

    Rect m_rcObstNumDef;    // 设置显示防守顺序
    Image *m_imgObstNumDef; // 显示原始障碍物点图标

    int m_nAgentTestMode;    // 进入需要地图点击的辅助训练模式
    int m_nAgentTestDefDist; // 防守辅助训练时的距离值

    CObstFusion_ZWJ *pObstFusion;

    Point posKickInImage;
    Point posLastCatchInImage;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGreenField)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void MBToWC(CString inStr, WCHAR *inWC);
    bool PointInRect(CPoint inPnt, Rect inRect);
    CString GetCTRLString(int inCtrlCode);
    CPoint m_Point_Img2Field(int inX, int inY);
    Point m_Point_Field2Img(int inX, int inY);
    virtual ~CGreenField();

    // Generated message map functions
  protected:
    //{{AFX_MSG(CGreenField)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GREENFIELD_H__F8D8FEEA_9DB0_4A32_A523_09CD4635A1E1__INCLUDED_)
