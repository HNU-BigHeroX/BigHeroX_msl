// OmniMove.cpp : implementation file
//

#include "stdafx.h"
#include "OmniMove.h"
#include "resource.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COmniMove

COmniMove::COmniMove()
{
    m_nCenterx = 110;
    m_nCentery = 110;

    m_nVelocity = 1000;
    m_nAngle = 0;
    m_fK = 0.1;

    m_pntV.x = 110;
    m_pntV.y = 110;

    m_bPressed = false;

    motol = 0;
    motor = 0;
    motob = 0;
    zhuand = 0;
    chang = 0;
    jiao = 0;
    for (int i = 0; i < 5; i++)
    {
        speed[i] = 0;
    }
    pRobot = NULL;
}

COmniMove::~COmniMove()
{
}

BEGIN_MESSAGE_MAP(COmniMove, CStatic)
//{{AFX_MSG_MAP(COmniMove)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COmniMove message handlers

void COmniMove::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    dcMemory.CreateCompatibleDC(&dc);
    m_bk.LoadBitmap(IDB_OMNIMOVE_BK);
    dcMemory.SelectObject(&m_bk);

    // 显示航向角和速度
    // dcMemory.TextOut(100,0,m_strVal);
    // 显示三个电机速度
    m_strTmp.Format("[0]%4d", speed[0]); // left
    dcMemory.TextOut(0, 0, m_strTmp);
    m_strTmp.Format("[1]%4d", speed[1]); // right
    dcMemory.TextOut(160, 0, m_strTmp);
    m_strTmp.Format("[2]%4d", speed[2]); // back
    dcMemory.TextOut(90, 204, m_strTmp);

    CPen pos(PS_SOLID, 3, COLORREF(RGB(0, 255, 255)));
    dcMemory.SelectObject(&pos);
    dcMemory.MoveTo(110, 110);
    dcMemory.LineTo(m_pntV);

    dc.BitBlt(0, 0, 250, 250, &dcMemory, 0, 0, SRCCOPY);
    m_bk.DeleteObject();
    dcMemory.DeleteDC();

    // Do not call CStatic::OnPaint() for painting messages
}

void COmniMove::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    m_bPressed = true;
    PointToVelocity(point);

    CStatic::OnLButtonDown(nFlags, point);
}

void COmniMove::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    m_bPressed = false;

    CStatic::OnLButtonUp(nFlags, point);
}

void COmniMove::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    if (m_bPressed == true)
    {
        PointToVelocity(point);
    }

    CStatic::OnMouseMove(nFlags, point);
}

void COmniMove::PointToVelocity(CPoint inPnt)
{
    m_pntV = inPnt;
    VectorMove(inPnt);
    Invalidate(FALSE);
}

void COmniMove::m_Split4Bytes(unsigned char *inTarg, int inSrc)
{
    if (inTarg == NULL)
    {
        return;
    }

    unsigned char *pVal = (unsigned char *)&inSrc;

    inTarg[0] = *pVal;

    pVal++;
    inTarg[1] = *pVal;

    pVal++;
    inTarg[2] = *pVal;

    pVal++;
    inTarg[3] = *pVal;
}

void COmniMove::jizuobiao1(int x, int y, double *jiao, double *chang)
{
    x = x - 110;
    y = 110 - y;
    if (x != 0 || y != 0)
    {
        *chang = sqrt(x * x + y * y);
        if (x > 0)
            *jiao = (100 - asin(y / *chang) * 63.66) * 0.9;
        else
            *jiao = -(100 - asin(y / *chang) * 63.66) * 0.9;
    }
    else
    {
        *chang = 0;
        *jiao = 0;
    }
}

void COmniMove::Controlmoto(int jiao, int sudu, int zhuan, int *motol, int *motor, int *motob)
{
    double kSudu = 0.01;
    *motol = -int(sin((float(jiajiao(jiao, -60))) * 3.14 / 180) * sudu * sudu * kSudu) + zhuan;
    *motor = -int(sin((float(jiajiao(jiao, 60))) * 3.14 / 180) * sudu * sudu * kSudu) + zhuan;
    *motob = -int(sin((float(jiajiao(jiao, +180))) * 3.14 / 180) * sudu * sudu * kSudu) + zhuan;
}

int COmniMove::jiajiao(int jizhun, int jiao)
{
    int jiajiao;
    jiajiao = jiao - jizhun;
    if (jiajiao > 180)
        jiajiao = -360 + jiajiao;
    if (jiajiao < -180)
        jiajiao = 360 + jiajiao;
    return jiajiao;
}

void COmniMove::VectorMove(CPoint inPnt)
{
    jizuobiao1(inPnt.x, inPnt.y, &jiao, &chang);
    Controlmoto(jiao, chang * 2, zhuand, &motol, &motor, &motob);

    speed[0] = motol * 0.5;
    speed[1] = motor * 0.5;
    speed[2] = motob * 0.5;

    if (pRobot != NULL)
    {
        pRobot->SetFiveMotorsSpeed(speed);
    }
    // m_cmd.SetFiveMotorsSpeed(speed);
}
