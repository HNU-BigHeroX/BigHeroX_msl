// ItemList.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "ItemList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemList

CItemList::CItemList()
{
    pset = NULL;

    pParent = NULL;
    plist = NULL;

    lastsel = 5;
}

CItemList::~CItemList()
{
}

BEGIN_MESSAGE_MAP(CItemList, CListCtrl)
//{{AFX_MSG_MAP(CItemList)
ON_WM_ERASEBKGND()
ON_WM_MOUSEWHEEL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemList message handlers

void CItemList::ShowParam(HSISet inHsi)
{
    CString str;
    // 将标定数据显示在列表框里，最先插入的显示在最上面，（注意SetItemText第一个参数）
    int nIndex = 0;

    // 黑色障碍物
    str.Format("%d ~ %d", inHsi.black.Hue_min, inHsi.black.Hue_max);
    SetItemText(nIndex, 1, str);
    str.Format("%d ~ %d", inHsi.black.Satu_min, inHsi.black.Satu_max);
    SetItemText(nIndex, 2, str);
    str.Format("%d ~ %d", inHsi.black.I_min, inHsi.black.I_max);
    SetItemText(nIndex, 3, str);
    nIndex++;

    // InsertItem(0,"绿色地毯");
    str.Format("%d ~ %d", inHsi.green.Hue_min, inHsi.green.Hue_max);
    SetItemText(nIndex, 1, str);
    str.Format("%d ~ %d", inHsi.green.Satu_min, inHsi.green.Satu_max);
    SetItemText(nIndex, 2, str);
    str.Format("%d ~ %d", inHsi.green.I_min, inHsi.green.I_max);
    SetItemText(nIndex, 3, str);
    nIndex++;

    // InsertItem(0,"白色场线");
    str.Format("%d ~ %d", inHsi.whitelines.Hue_min, inHsi.whitelines.Hue_max);
    SetItemText(nIndex, 1, str);
    str.Format("%d ~ %d", inHsi.whitelines.Satu_min, inHsi.whitelines.Satu_max);
    SetItemText(nIndex, 2, str);
    str.Format("%d ~ %d", inHsi.whitelines.I_min, inHsi.whitelines.I_max);
    SetItemText(nIndex, 3, str);
    nIndex++;

    // InsertItem(0,"球（全景）");
    str.Format("%d ~ %d", inHsi.overball.Hue_min, inHsi.overball.Hue_max);
    SetItemText(nIndex, 1, str);
    str.Format("%d ~ %d", inHsi.overball.Satu_min, inHsi.overball.Satu_max);
    SetItemText(nIndex, 2, str);
    str.Format("%d ~ %d", inHsi.overball.I_min, inHsi.overball.I_max);
    SetItemText(nIndex, 3, str);
    nIndex++;

    //	InsertItem(0,"队友色标");
    str.Format("%d ~ %d", inHsi.teammate.Hue_min, inHsi.teammate.Hue_max);
    SetItemText(nIndex, 1, str);
    str.Format("%d ~ %d", inHsi.teammate.Satu_min, inHsi.teammate.Satu_max);
    SetItemText(nIndex, 2, str);
    str.Format("%d ~ %d", inHsi.teammate.I_min, inHsi.teammate.I_max);
    SetItemText(nIndex, 3, str);
    nIndex++;

    // 对手队标
    str.Format("%d ~ %d", inHsi.opponent.Hue_min, inHsi.opponent.Hue_max);
    SetItemText(nIndex, 1, str);
    str.Format("%d ~ %d", inHsi.opponent.Satu_min, inHsi.opponent.Satu_max);
    SetItemText(nIndex, 2, str);
    str.Format("%d ~ %d", inHsi.opponent.I_min, inHsi.opponent.I_max);
    SetItemText(nIndex, 3, str);
    nIndex++;
}

void CItemList::ReSelect()
{
}

void CItemList::UpdateDisplay()
{
}

BOOL CItemList::OnEraseBkgnd(CDC *pDC)
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;

    return CListCtrl::OnEraseBkgnd(pDC);
}

BOOL CItemList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default
    if (pParent != NULL)
    {
        pParent->PostMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(pt.x, pt.y));
    }

    return TRUE;
    // return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}
