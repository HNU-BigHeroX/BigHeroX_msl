// ColorList.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "ColorList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorList

CColorList::CColorList()
{
    lastSel = 2;
    pTest = NULL;
    pclr = NULL;
}

CColorList::~CColorList()
{
}

BEGIN_MESSAGE_MAP(CColorList, CListBox)
//{{AFX_MSG_MAP(CColorList)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorList message handlers

void CColorList::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CListBox::OnLButtonDown(nFlags, point);
}

void CColorList::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    lastSel = GetCurSel();
    switch (lastSel)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        pTest->ClearAllFlag();
        pTest->bOverBall = true;
        pclr->HSIToShow(pTest->m_vhsi.overball);
        pclr->Invalidate();
        pclr->pParam = &(pTest->m_vhsi.overball);
        break;
    case 3:
        pTest->ClearAllFlag();
        // pTest->bOppo = true;
        pclr->HSIToShow(pTest->m_vhsi.teammate);
        pclr->Invalidate();
        pclr->pParam = &(pTest->m_vhsi.teammate);
        break;
    case 4:
        pTest->ClearAllFlag();
        pTest->bGreen = true;
        pclr->HSIToShow(pTest->m_vhsi.green);
        pclr->Invalidate();
        pclr->pParam = &(pTest->m_vhsi.green);
        break;
    case 5:
        break;
    }
    ShowParam(pTest->m_vhsi);

    CListBox::OnLButtonUp(nFlags, point);
}

void CColorList::ReSelect()
{
    SetCurSel(lastSel);
    if (pTest == NULL || pclr == NULL)
    {
        return;
    }
    switch (lastSel)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        pclr->HSIToShow(pTest->m_vhsi.overball);
        break;
    case 3:
        pclr->HSIToShow(pTest->m_vhsi.teammate);
        break;
    case 4:
        pclr->HSIToShow(pTest->m_vhsi.green);
        break;
    case 5:
        break;
    }
}

void CColorList::ShowParam(HSISet inHsi)
{
    ResetContent();
    CString str;
    str.Format("球(全景)： Hmax:%d Hmin:%d S:%d", inHsi.overball.Hue_min, inHsi.overball.Hue_max,
               inHsi.overball.Satu_min);
    AddString(str);
    str.Format("障碍颜色： Hmax:%d Hmin:%d S:%d", inHsi.teammate.Hue_min, inHsi.teammate.Hue_max,
               inHsi.teammate.Satu_min);
    AddString(str);
    str.Format("绿色地毯： Hmax:%d Hmin:%d S:%d", inHsi.green.Hue_min, inHsi.green.Hue_max, inHsi.green.Satu_min);
    AddString(str);
    str.Format("白色场线： Hmax:%d Hmin:%d S:%d", inHsi.whitelines.Hue_min, inHsi.whitelines.Hue_max,
               inHsi.whitelines.Satu_min);
    AddString(str);
    str.Format("队友颜色： Hmax:%d Hmin:%d S:%d", inHsi.teammate.Hue_min, inHsi.teammate.Hue_max,
               inHsi.teammate.Satu_min);
    AddString(str);
    ReSelect();
}

void CColorList::UpdateDisplay()
{
    ShowParam(pTest->m_vhsi);
}
