// Compasee.cpp : implementation file
//

#include "stdafx.h"
#include "Compasee.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompasee

CCompasee::CCompasee()
{
    m_nBkbmp = 0;
    m_angle = 0;
    m_nBkwidth = 0;
    m_nBkheight = 0;
    m_showagl = 0;
    Init();
}

CCompasee::~CCompasee()
{
}

BEGIN_MESSAGE_MAP(CCompasee, CStatic)
//{{AFX_MSG_MAP(CCompasee)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompasee message handlers

void CCompasee::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    CDC dcMemory;
    dcMemory.CreateCompatibleDC(&dc);

    CBitmap compassbmp;

    // Ìù±³¾°Í¼
    if (m_nBkbmp != 0)
    {
        compassbmp.LoadBitmap(m_nBkbmp);
        dcMemory.SelectObject(compassbmp);
    }

    CPen pen(PS_SOLID, 3, COLORREF(RGB(255, 0, 0)));
    CPen *pOldPen = dcMemory.SelectObject(&pen);

    CPoint center(m_CenterX, m_CenterY);
    dcMemory.MoveTo(center);
    dcMemory.LineTo(CalPoint());

    m_strShow.Format("%.0f¡ã", m_showagl);
    dcMemory.SetBkMode(TRANSPARENT);
    dcMemory.TextOut(0, 0, m_strShow);

    dcMemory.SelectObject(pOldPen);

    dc.BitBlt(0, 0, m_nBkwidth, m_nBkheight, &dcMemory, 0, 0, SRCCOPY);
    dcMemory.DeleteDC();
    // Do not call CStatic::OnPaint() for painting messages
}

void CCompasee::Init()
{
    // m_nBkbmp = IDB_COMPASS;
    m_nBkwidth = 110;
    m_nBkheight = 110;

    m_CenterX = m_nBkwidth / 2 - 1;
    m_CenterY = m_nBkheight / 2 - 1;

    m_length = 40;
}

CPoint CCompasee::CalPoint()
{
    // ¼ÆËãÄ©¶Ëµã
    int x = (int)(m_length * cos(m_angle - 1.57));
    int y = (int)(m_length * sin(m_angle - 1.57));

    CPoint pin(m_CenterX + x, m_CenterY + y);
    return pin;
}

void CCompasee::Update(FLOAT inangle, int length)
{
    // ¼ÆËã½Ç¶È
    m_angle = float((inangle / 180) * 3.1415926);
    m_showagl = inangle;
}
