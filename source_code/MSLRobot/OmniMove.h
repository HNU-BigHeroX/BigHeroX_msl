#if !defined(AFX_OMNIMOVE_H__4389925E_7954_42FA_8C88_24505F1CA487__INCLUDED_)
#define AFX_OMNIMOVE_H__4389925E_7954_42FA_8C88_24505F1CA487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OmniMove.h : header file
//
#include "VoyInclude/RobotPlayerAMC.h"

/////////////////////////////////////////////////////////////////////////////
// COmniMove window

class COmniMove : public CStatic
{
    // Construction
  public:
    COmniMove();

    // Attributes
  public:
    CDC dcMemory;
    CBitmap m_bk;
    CRect m_FrameRect;

    int m_nCenterx;
    int m_nCentery;
    double m_fK;
    bool m_bPressed;

    CPoint m_pntV;

    // val
    int m_nAngle;
    int m_nVelocity;
    CString m_strVal;
    CString m_strTmp;

    // xxx
    int motol;
    int motor;
    int motob;
    int zhuand;
    double chang;
    double jiao;
    short speed[5];

    // CRobotPlayerAMC* pRobot;
    CVoyCmd *pRobot;

    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COmniMove)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void PointToVelocity(CPoint inPnt);
    void VectorMove(CPoint inPnt);
    int jiajiao(int jizhun, int jiao);
    void Controlmoto(int jiao, int sudu, int zhuan, int *motol, int *motor, int *motob);
    void jizuobiao1(int x, int y, double *jiao, double *chang);
    void m_Split4Bytes(unsigned char *inTarg, int inSrc);
    virtual ~COmniMove();

    // Generated message map functions
  protected:
    //{{AFX_MSG(COmniMove)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMNIMOVE_H__4389925E_7954_42FA_8C88_24505F1CA487__INCLUDED_)
