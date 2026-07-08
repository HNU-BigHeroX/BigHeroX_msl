/*************************************************
  http://robot.up-tech.com
  Author: ÕÅÍò½Ü      Version: 1.0       Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // ÂÞÅÌÏÔÊ¾
  Others:         //
*************************************************/

#if !defined(AFX_COMPASEE_H__1798B4E6_9A6C_406D_9C42_CAD4ADD6FBD3__INCLUDED_)
#define AFX_COMPASEE_H__1798B4E6_9A6C_406D_9C42_CAD4ADD6FBD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Compasee.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompasee window

class CCompasee : public CStatic
{
    // Construction
  public:
    CCompasee();

    // Attributes
  public:
    // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCompasee)
    //}}AFX_VIRTUAL

    // Implementation
  public:
    void Update(FLOAT inangle, int length);
    void Init();
    virtual ~CCompasee();

    // Generated message map functions
  protected:
    FLOAT m_angle;
    FLOAT m_showagl;
    //{{AFX_MSG(CCompasee)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
  private:
    CPoint CalPoint();
    UINT m_nBkbmp;
    int m_nBkwidth;
    int m_nBkheight;
    int m_CenterX;
    int m_CenterY;

    UINT m_length;
    CString m_strShow;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPASEE_H__1798B4E6_9A6C_406D_9C42_CAD4ADD6FBD3__INCLUDED_)
