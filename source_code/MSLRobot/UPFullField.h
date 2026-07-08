/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // 图片显示类
  Others:         // 1.1为MSLCoach做了改动
*************************************************/

#if !defined(AFX_UPFULLFIELD_H__6D15FBB6_5DE4_408C_AE94_83C240DA6ED4__INCLUDED_)
#define AFX_UPFULLFIELD_H__6D15FBB6_5DE4_408C_AE94_83C240DA6ED4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UPFullField.h : header file
//
#include <math.h>
#include "VoyInclude\AgentUDP.h"
struct uagent
{
    UINT ID;
    int x;
    int y;
    double angle;
    uagent *next;
};

struct uObst
{
    UINT ID;
    int x;
    int y;
    bool bValid;
};

/////////////////////////////////////////////////////////////////////////////
// CUPFullField window

class CUPFullField : public CStatic
{
    // Construction
  public:
    CUPFullField();

    // Attributes
  public:
  protected:
    CBitmap m_BkBmp;
    uagent *m_pa;
    uagent *m_pb;
    int *m_px;
    int *m_py;
    int m_pcnt;
    uObst m_obsts[100];

    // Operations
  public:
    CAgentUDP *m_pNet;

  public:
    int m_curID;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CUPFullField)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void RemoveAllObsts();
    void SetObst(UINT inID, int inx, int iny);
    void SetBall(UINT inID, int inx, int iny);
    void RemovePoints();
    void SetPoints(int *inx, int *iny, int inNum);
    void SetAgent(UINT inID, int inx, int iny, double inangle);
    virtual ~CUPFullField();

    // Generated message map functions
  protected:
    CPoint m_CalPoint(int inx, int iny);
    void m_Clearpa();
    void m_Clearpb();
    //{{AFX_MSG(CUPFullField)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPFULLFIELD_H__6D15FBB6_5DE4_408C_AE94_83C240DA6ED4__INCLUDED_)
