// UPFullField.cpp : implementation file
//

#include "stdafx.h"
#include "UPFullField.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUPFullField

CUPFullField::CUPFullField()
{
    // m_BkBmp.LoadBitmap(IDB_BKFIELD);
    m_pa = NULL;

    m_px = m_py = NULL;
    m_pcnt = 0;
    m_curID = 0;

    m_pb = NULL;

    for (int i = 0; i < 100; i++)
    {
        m_obsts[i].bValid = false;
    }
}

CUPFullField::~CUPFullField()
{
    m_Clearpa();
    m_Clearpb();
}

BEGIN_MESSAGE_MAP(CUPFullField, CStatic)
//{{AFX_MSG_MAP(CUPFullField)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_RBUTTONDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUPFullField message handlers

void CUPFullField::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    CDC dcMemory;
    dcMemory.CreateCompatibleDC(&dc);

    m_BkBmp.DeleteObject();

    // IDB_BKFIELD 为场地图片
    m_BkBmp.LoadBitmap(IDB_BKFIELD);

    dcMemory.SelectObject(&m_BkBmp);
    dcMemory.SetBkMode(TRANSPARENT);
    CPen *oldpen;

    CPoint coodinate;
    CPoint direct;
    CString aNum;
    CBrush light(RGB(100, 100, 100));
    CBrush black(RGB(0, 0, 0));
    // 先绘制接收到的障碍物
    dcMemory.SelectObject(&black);
    dcMemory.SetTextColor(RGB(255, 255, 255));
    if (m_pNet != NULL)
    {
        for (int i = 0; i < AG_OBST_NUM; i++)
        {
            if (m_pNet->rec_obst[i].valid == true)
            {
                coodinate = m_CalPoint(m_pNet->rec_obst[i].x, m_pNet->rec_obst[i].y);
                dcMemory.Ellipse(coodinate.x - 8, coodinate.y - 8, coodinate.x + 8, coodinate.y + 8);
                aNum.Format("%d(%d)", m_pNet->rec_obst[i].ID, m_pNet->rec_obst[i].size);
                dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, aNum);
            }
        }
    }
    // 绘制障碍物
    dcMemory.SelectObject(&black);
    for (int i = 0; i < 100; i++)
    {
        if (m_obsts[i].bValid == true)
        {
            coodinate = m_CalPoint(m_obsts[i].x, m_obsts[i].y);
            dcMemory.Ellipse(coodinate.x - 8, coodinate.y - 8, coodinate.x + 8, coodinate.y + 8);
        }
    }
    // 绘制匹配点
    if (m_px != NULL && m_py != NULL)
    {
        CPen dot(PS_SOLID, 3, RGB(255, 155, 255));
        oldpen = dcMemory.SelectObject(&dot);
        for (int i = 0; i < m_pcnt; i++)
        {
            dcMemory.MoveTo(m_CalPoint(m_px[i], m_py[i]));
            dcMemory.LineTo(m_CalPoint(m_px[i], m_py[i]));
        }
        dcMemory.SelectObject(oldpen);
    }

    CPen dpen(PS_SOLID, 3, RGB(155, 0, 155));
    CPen redpen(PS_SOLID, 2, RGB(255, 0, 100));
    // 绘制接收到的队友信息
    if (m_pNet != NULL)
    {
        CBrush brTM(RGB(255, 0, 255));
        dcMemory.SelectObject(&brTM);
        for (int i = 0; i < AG_OBST_NUM; i++)
        {
            if (m_pNet->m_tm[i].valid == true)
            {
                coodinate = m_CalPoint(m_pNet->m_tm[i].x, m_pNet->m_tm[i].y);
                dcMemory.Ellipse(coodinate.x - 8, coodinate.y - 8, coodinate.x + 8, coodinate.y + 8);
                direct.x = long(m_pNet->m_tm[i].V * cos(m_pNet->m_tm[i].V_angle * 3.14 / 180));
                direct.y = long(-(m_pNet->m_tm[i].V) * sin(m_pNet->m_tm[i].V_angle * 3.14 / 180));
                direct += coodinate;
                oldpen = dcMemory.SelectObject(&redpen);
                dcMemory.MoveTo(coodinate);
                dcMemory.LineTo(direct);
                dcMemory.SelectObject(oldpen);
                aNum.Format("%d", m_pNet->m_tm[i].ID);
                dcMemory.SetTextColor(RGB(0, 255, 255));
                dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, aNum);
            }
        }
    }

    // 绘制Agnet
    CBrush blue(RGB(0, 0, 255));
    dcMemory.SelectObject(&blue);
    uagent *t = m_pa;
    while (t != NULL)
    {
        coodinate = m_CalPoint(t->x, t->y);
        dcMemory.Ellipse(coodinate.x - 8, coodinate.y - 8, coodinate.x + 8, coodinate.y + 8);
        direct.x = long(12 * cos(t->angle * 3.14 / 180));
        direct.y = long(-12 * sin(t->angle * 3.14 / 180));
        direct += coodinate;
        oldpen = dcMemory.SelectObject(&dpen);
        dcMemory.MoveTo(coodinate);
        dcMemory.LineTo(direct);
        dcMemory.SelectObject(oldpen);
        // 		if (t->ID == 1)
        // 		{
        // 			aNum.Format("T");
        // 		}
        // 		else
        // 		{
        // 			aNum.Format("b");
        // 		}

        //	aNum.Format("%d",m_curID);
        aNum.Format("%d", t->ID);
        //	aNum.Format("1");

        dcMemory.SetTextColor(RGB(0, 255, 255));
        dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, aNum);

        // 写出说明
        /*		if (t->ID == 1)
                {
                    aNum.Format("定位点1");
                }
                else
                {
                    aNum.Format("定位点2");
                }
                dcMemory.SetTextColor(RGB(255,55,0));
                dcMemory.TextOut(coodinate.x-20,coodinate.y-28,aNum);
        */
        t = t->next;
    }

    // 绘制Ball
    CBrush orange(RGB(255, 155, 0));
    t = m_pb;
    while (t != NULL)
    {
        coodinate = m_CalPoint(t->x, t->y);
        dcMemory.SelectObject(&orange);
        dcMemory.Ellipse(coodinate.x - 7, coodinate.y - 7, coodinate.x + 7, coodinate.y + 7);
        /**/ aNum.Format("%d", t->ID);
        /**/ dcMemory.SetTextColor(RGB(0, 0, 0));
        /**/ dcMemory.TextOut(coodinate.x - 4, coodinate.y - 8, aNum);

        t = t->next;
    }

    // 绘制接收到的定位点
    if (m_pNet != NULL)
    {
        oldpen = dcMemory.SelectObject(&dpen);
        coodinate = m_CalPoint(m_pNet->rec_init_x, m_pNet->rec_init_y);
        dcMemory.MoveTo(coodinate.x - 7, coodinate.y);
        dcMemory.LineTo(coodinate.x + 7, coodinate.y);
        dcMemory.MoveTo(coodinate.x, coodinate.y - 7);
        dcMemory.LineTo(coodinate.x, coodinate.y + 7);
        dcMemory.SetTextColor(RGB(255, 0, 0));
        dcMemory.TextOut(coodinate.x - 30, coodinate.y + 15, "匹配初始点");
        dcMemory.SelectObject(oldpen);
    }
    dc.BitBlt(0, 0, 493, 346, &dcMemory, 0, 0, SRCCOPY);
    dcMemory.DeleteDC();

    // Do not call CStatic::OnPaint() for painting messages
}

void CUPFullField::m_Clearpa()
{
    uagent *t = NULL;
    while (m_pa != NULL)
    {
        t = m_pa;
        m_pa = t->next;
        delete t;
    }
}

void CUPFullField::SetAgent(UINT inID, int inx, int iny, double inangle)
{
    uagent *t = m_pa;
    while (t != NULL)
    {
        if (t->ID == inID)
        {
            t->x = inx;
            t->y = iny;
            t->angle = inangle;
            // Invalidate();
            return;
        }
        t = t->next;
    }
    t = new uagent;
    t->ID = inID;
    t->x = inx;
    t->y = iny;
    t->angle = inangle;
    t->next = m_pa;
    m_pa = t;

    // Invalidate();
}

CPoint CUPFullField::m_CalPoint(int inx, int iny)
{
    CPoint p;
    p.x = inx * 400 / 1800 + 48;
    p.y = 305 - iny * 266 / 1200;
    // 限一下值
    if (p.x > 490)
    {
        p.x = 490;
    }
    if (p.x < 0)
    {
        p.x = 0;
    }
    if (p.y < 0)
    {
        p.y = 0;
    }
    if (p.y > 340)
    {
        p.y = 340;
    }
    return p;
}

void CUPFullField::SetPoints(int *inx, int *iny, int inNum)
{
    if (inx == NULL || iny == NULL)
    {
        return;
    }
    m_px = inx;
    m_py = iny;
    m_pcnt = inNum;

    Invalidate();
}

void CUPFullField::RemovePoints()
{
    m_px = NULL;
    m_py = NULL;
    m_pcnt = 0;

    // Invalidate();
}

void CUPFullField::SetBall(UINT inID, int inx, int iny)
{
    uagent *t = m_pb;
    while (t != NULL)
    {
        if (t->ID == inID)
        {
            t->x = inx;
            t->y = iny;
            //	t->angle = 2;
            //	Invalidate();
            return;
        }
        t = t->next;
    }
    t = new uagent;
    t->ID = inID;
    t->x = inx;
    t->y = iny;
    t->next = m_pb;
    m_pb = t;

    // Invalidate();
}

void CUPFullField::m_Clearpb()
{
    uagent *t = NULL;
    while (m_pb != NULL)
    {
        t = m_pb;
        m_pb = t->next;
        delete t;
    }
}

void CUPFullField::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CStatic::OnLButtonDown(nFlags, point);
}

void CUPFullField::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CStatic::OnRButtonDown(nFlags, point);
}

void CUPFullField::SetObst(UINT inID, int inx, int iny)
{
    for (int i = 0; i < 100; i++)
    {
        if (m_obsts[i].bValid == false)
        {
            m_obsts[i].ID = inID;
            m_obsts[i].x = inx;
            m_obsts[i].y = iny;
            m_obsts[i].bValid = true;
            break;
        }
    }
}

void CUPFullField::RemoveAllObsts()
{
    for (int i = 0; i < 100; i++)
    {
        m_obsts[i].bValid = false;
    }
}
