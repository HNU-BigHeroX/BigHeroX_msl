// Color.cpp : implementation file
//

#include "stdafx.h"
#include "Thresdhold.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColor

CColor::CColor()
{
    hsi.Hue_min = 10;
    hsi.Hue_max = 50;
    hsi.Satu_max = 200;
    hsi.Satu_min = 100;
    hsi.I_max = 255;
    hsi.I_min = 0;
    HSIToShow(hsi);
    m_status = 0;

    pParam = NULL;
    pList = NULL;

    leftmoving = rightmoving = bottommoving = topmoving = false;

    bColor = true;

    // distribution test
    for (int i = 0; i < 255; i++)
    {
        Hdistrib[i] = 0;
        Sdistrib[i] = 0;
        Idistrib[i] = 0;
    }
    Hcmax = 255;
    Scmax = 255;
    Icmax = 255;
    for (i = 255; i < 360; i++)
    {
        Hdistrib[i] = 0;
    }

    // zoom
    pZoombuf = NULL;
    nZoomHeight = nZoomWidth = 0;
    memset(&m_bih, 0, sizeof(m_bih));
    m_bih.biSize = sizeof(m_bih);
    m_bih.biWidth = 100;
    m_bih.biHeight = 100;
    m_bih.biPlanes = 1;
    m_bih.biBitCount = 24;

    // item
    VERIFY(m_itfont.CreateFont(36,                       // nHeight
                               0,                        // nWidth
                               0,                        // nEscapement
                               0,                        // nOrientation
                               FW_BLACK,                 // nWeight
                               FALSE,                    // bItalic
                               FALSE,                    // bUnderline
                               0,                        // cStrikeOut
                               ANSI_CHARSET,             // nCharSet
                               OUT_DEFAULT_PRECIS,       // nOutPrecision
                               CLIP_DEFAULT_PRECIS,      // nClipPrecision
                               DEFAULT_QUALITY,          // nQuality
                               DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
                               "Arial"));                // lpszFacename
    m_strItem = "球";
}

CColor::~CColor()
{
}

BEGIN_MESSAGE_MAP(CColor, CStatic)
//{{AFX_MSG_MAP(CColor)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColor message handlers

void CColor::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    // 创建缓存DC
    dcMemory.CreateCompatibleDC(&dc);
    if (bColor == true)
    {
        Bitmap.LoadBitmap(IDB_BITMAP1);
    }
    else
    {
        // 显示前视球对比图
        Bitmap.LoadBitmap(IDB_BITMAP2); // dc拷贝
        CBitmap *oldbmp = dcMemory.SelectObject(&Bitmap);

        dc.BitBlt(0, 0, 396, 344, &dcMemory, 0, 0, SRCCOPY);
        dcMemory.SelectObject(oldbmp);
        dcMemory.DeleteDC();

        Bitmap.DeleteObject();
        return;
    }
    // 显示背景
    CBitmap *oldbmp = dcMemory.SelectObject(&Bitmap);
    dcMemory.SetBkMode(TRANSPARENT);

    CPen black(PS_SOLID, 1, RGB(0, 0, 0));
    CPen dot(PS_DOT, 1, RGB(0, 0, 0));
    CPen *oldpen = dcMemory.SelectObject(&black);

    // 0到360界限的衔接
    if (left < right)
    {
        // 正常显示
        dcMemory.MoveTo(left, top);
        dcMemory.LineTo(left, bottom);
        dcMemory.LineTo(right, bottom);
        dcMemory.LineTo(right, top);
        dcMemory.LineTo(left, top);

        // 标记刻度范围
        HRect.top = 344;
        HRect.bottom = 394;
        HRect.left = left;
        HRect.right = right + 1;

        HRect_1 = HRect;

        SRect.left = 396;
        SRect.right = 446;
        SRect.top = top;
        SRect.bottom = bottom + 1;
    }
    else
    {
        // 跨边界显示
        dcMemory.MoveTo(0, bottom);
        dcMemory.LineTo(right, bottom);
        dcMemory.LineTo(right, top);
        dcMemory.LineTo(0, top);

        dcMemory.MoveTo(396, top);
        dcMemory.LineTo(left, top);
        dcMemory.LineTo(left, bottom);
        dcMemory.LineTo(396, bottom);

        // 标记刻度范围
        HRect.top = 344;
        HRect.bottom = 394;
        HRect.left = 0;
        HRect.right = right;

        HRect_1 = HRect;
        HRect_1.left = left;
        HRect_1.right = 396;

        SRect.left = 396;
        SRect.right = 446;
        SRect.top = top;
        SRect.bottom = bottom + 1;
    }
    // dcMemory.SelectObject(oldpen);

    /*右侧I显示*/
    CPen blue(PS_SOLID, 1, RGB(255, 0, 0));
    oldpen = dcMemory.SelectObject(&blue);
    dcMemory.MoveTo(491, itop);
    dcMemory.LineTo(513, itop);
    dcMemory.LineTo(513, ibottom);
    dcMemory.LineTo(491, ibottom);
    dcMemory.LineTo(491, itop);

    // 标记填充区域
    IRect.left = 514;
    IRect.right = 564;
    IRect.top = itop;
    IRect.bottom = ibottom;

    /*右侧I显示完成*/

    // 对标记区域进行填充
    CBrush *pBrush = new CBrush(RGB(200, 200, 255));
    dcMemory.FillRect(&HRect, pBrush);
    dcMemory.FillRect(&HRect_1, pBrush);
    dcMemory.FillRect(&SRect, pBrush);
    dcMemory.FillRect(&IRect, pBrush);

    delete pBrush;

    // 画虚线
    if (m_status == 9)
    {
        oldpen = dcMemory.SelectObject(&dot);
        dcMemory.MoveTo(right, top);
        dcMemory.LineTo(446, top);
        dcMemory.MoveTo(right, bottom);
        dcMemory.LineTo(446, bottom);
        dcMemory.MoveTo(left, bottom);
        dcMemory.LineTo(left, 394);
        dcMemory.MoveTo(right, bottom);
        dcMemory.LineTo(right, 394);
    }

    // 画分布图
    // CPen dblack(PS_SOLID,2,RGB(0,0,0));
    oldpen = dcMemory.SelectObject(&black);
    // 先画H的范围 x:0~396 y:344~394
    dcMemory.MoveTo(0, 394);
    int x, y;
    for (int i = 0; i < 360; i++)
    {
        y = Hdistrib[i] * 50 / Hcmax;
        y = 394 - y;
        x = i * 396 / 360;
        dcMemory.LineTo(x, y);
    }
    dcMemory.LineTo(395, 394);
    // 画S的范围 x:396~445 y:0~344
    dcMemory.MoveTo(445, 344);
    for (i = 0; i < 255; i++)
    {
        x = (445 - 396) * Sdistrib[i] / Scmax;
        x = 445 - x;
        y = i * 344 / 254;
        y = 344 - y;
        dcMemory.LineTo(x, y);
    }
    dcMemory.LineTo(446, 0);
    // 画I的范围 x:514~564 y:0~344
    dcMemory.MoveTo(564, 344);
    // dcMemory.LineTo(514,0);
    for (i = 0; i < 255; i++)
    {
        x = (564 - 514) * Idistrib[i] / Icmax;
        x = 564 - x;
        y = i * 344 / 254;
        y = 344 - y;
        dcMemory.LineTo(x, y);
    }
    dcMemory.LineTo(564, 0);

    if (m_status == 1)
    {
        CString str;
        str.Format("H:%d-%d,S:%d-%d", hsi.Hue_min, hsi.Hue_max, hsi.Satu_min, hsi.Satu_max);
        dcMemory.TextOut(tp.x, tp.y, str);
    }

    if (m_status == 2)
    {
        CString str;
        str.Format("I:%d-%d", hsi.I_min, hsi.I_max);
        dcMemory.TextOut(tp.x, tp.y, str);
    }

    // zoomimg的显示
    if (pZoombuf != NULL)
    {
        PAINTSTRUCT ps;
        BeginPaint(&ps);

        SetStretchBltMode(dcMemory.m_hDC, COLORONCOLOR);
        StretchDIBits(
            // 目标
            dcMemory.m_hDC, 150, 120, 100, 100,
            // 源
            0, 0, nZoomWidth, nZoomHeight, pZoombuf, (BITMAPINFO *)&m_bih, DIB_RGB_COLORS, SRCCOPY);

        EndPaint(&ps);
    }

    // item
    // 	pBrush = new CBrush(RGB(230,230,230));
    // 	CRect textbk(430,375,580,430);
    // 	dcMemory.FillRect(&textbk,pBrush);
    // 	delete pBrush;

    CFont *def_font = dcMemory.SelectObject(&m_itfont);
    dcMemory.SetTextColor(RGB(0, 200, 0));

    dcMemory.TextOut(430, 375, m_strItem);

    dcMemory.SelectObject(def_font);

    // dc拷贝
    dc.BitBlt(0, 0, 595, 419, &dcMemory, 0, 0, SRCCOPY);
    dcMemory.SelectObject(oldbmp);
    dcMemory.DeleteDC();

    Bitmap.DeleteObject();
    // Do not call CStatic::OnPaint() for painting messages
}

void CColor::HSIToShow(HSIParam inHsi)
{
    hsi = inHsi;
    left = (hsi.Hue_min) * 396 / 360;
    right = (hsi.Hue_max) * 396 / 360;
    top = 344 - (hsi.Satu_max * 344 / 255);
    bottom = 344 - (hsi.Satu_min * 344 / 255);

    itop = 344 - (hsi.I_max * 344 / 255);
    ibottom = 344 - (hsi.I_min * 344 / 255);
    m_RegSelBox();
}

void CColor::ShowToHSI() // 将显示坐标换算成hsi
{
    /*if (left < right)
    {
        //非跨界情况
        //	hsi.Median = ((left + right)/2)*360/396;
        hsi.Hue_max = right;
        hsi.Hue_min = left;
    }
    else
    {
        //跨界情况
        if (right > (396-left))
        {
            //hsi.Median = (right - (right + 396 - left)/2)*360/396;
        }
        else
        {
            //hsi.Median = (left + (right + 396 - left)/2)*360/396;
        }
    }*/
    hsi.Hue_max = right * 360 / 396;
    hsi.Hue_min = left * 360 / 396;

    // 限定Median在0～360范围之内
    while (hsi.Hue_max < 0)
    {
        hsi.Hue_max += 360;
    }
    while (hsi.Hue_max > 360)
    {
        hsi.Hue_max -= 360;
    }

    while (hsi.Hue_min < 0)
    {
        hsi.Hue_min += 360;
    }
    while (hsi.Hue_min > 360)
    {
        hsi.Hue_min -= 360;
    }
    /*while (hsi.Median < 0)
    {
        hsi.Median += 360;
    }
    while (hsi.Median > 360)
    {
        hsi.Median -= 360;
    }

    hsi.Range = ((right - left)/2)*360/396;
    if (hsi.Range < 0)
    {
        hsi.Range += 180;
    }*/

    hsi.Satu_min = (344 - bottom) * 255 / 344;
    hsi.Satu_max = (344 - top) * 255 / 344;

    hsi.I_min = (344 - ibottom) * 255 / 344;
    hsi.I_max = (344 - itop) * 255 / 344;
}

void CColor::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (bColor == false)
    {
        CStatic::OnLButtonDown(nFlags, point);
        return;
    }
    pZoombuf = NULL;
    // 捕捉控件外的鼠标位置
    SetCapture();

    int outskirts = 5;     // 鼠标选择判定的幅度
    bool Selected = false; // 鼠标点在HS选区的判定标记

    if (point.x > left - outskirts && point.x < right + outskirts && point.y > top - outskirts &&
        point.y < bottom + outskirts)
    {
        Selected = true;
    }
    if (left > right && point.x < right + outskirts && point.x > 0 && point.y > top - outskirts &&
        point.y < bottom + outskirts)
    {
        // 点在左侧半框内
        Selected = true;
    }
    if (left > right && point.x > left - outskirts && point.x < 396 && point.y > top - outskirts &&
        point.y < bottom + outskirts)
    {
        Selected = true;
    }

    if (Selected == true)
    {
        // 在这个区域中
        if (point.x < left + outskirts && point.x > left - outskirts)
        {
            // 点在边框左边界附近
            leftmoving = true;
        }

        if (point.x < right + outskirts && point.x > right - outskirts)
        {
            // 点在边框右边界附近
            rightmoving = true;
        }

        // 假如左右贴得太近，会左右都选上，改变一下
        if (leftmoving == true && rightmoving == true)
        {
            if ((right - point.x) > (point.x - left))
            {
                // 偏左侧
                rightmoving = false;
            }
            else
            {
                leftmoving = false;
            }
        }

        if (point.y > bottom - outskirts)
        {
            bottommoving = true;
        }

        if (point.y < top + outskirts)
        {
            topmoving = true;
        }

        // 如果上下贴太近了，也需要处理一下
        if (topmoving == true && bottommoving == true)
        {
            if ((point.y - top) > (bottom - point.y))
            {
                // 偏下
                topmoving = false;
            }
            else
            {
                bottommoving = false;
            }
        }

        Dx = point.x - left;
        Dy = point.y - top;
        m_status = 1;
    }

    // I视图的选择判定
    bool iselected = false;
    if (point.y > itop - outskirts && point.y < ibottom + outskirts && point.x > 491 && point.x < 513)
    {
        iselected = true;
    }
    if (iselected == true)
    {
        if (point.y < itop + outskirts)
        {
            itopmoving = true;
        }
        if (point.y > ibottom - outskirts)
        {
            ibottommoving = true;
        }

        // 如果上下贴太近了，也需要处理一下
        if (itopmoving == true && ibottommoving == true)
        {
            if ((point.y - itop) > (ibottom - point.y))
            {
                // 偏下
                itopmoving = false;
            }
            else
            {
                ibottommoving = false;
            }
        }
        iDy = point.y - itop;
        m_status = 2;
    }

    CStatic::OnLButtonDown(nFlags, point);
}

void CColor::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (bColor == false)
    {
        CStatic::OnLButtonUp(nFlags, point);
        return;
    }

    ReleaseCapture();
    m_status = 0;

    leftmoving = rightmoving = bottommoving = topmoving = itopmoving = ibottommoving = false;

    // 将left和right都限定在合理范围之内
    m_RegSelBox();

    Invalidate();

    CStatic::OnLButtonUp(nFlags, point);
}

void CColor::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (m_status == 1)
    {
        // 拖动边框状态
        if (leftmoving == true)
        {
            left = point.x;
            while (left < 0)
            {
                left += 396;
            }
            while (left > 396)
            {
                left -= 396;
            }
        }
        if (rightmoving == true)
        {
            right = point.x;
            while (right < 0)
            {
                right += 396;
            }
            while (right > 396)
            {
                right -= 396;
            }
        }
        if (bottommoving == true)
        {
            bottom = point.y;
            if (bottom <= top)
            {
                bottom = top + 1;
            }
            if (bottom > 344)
            {
                bottom = 344;
            }
        }
        if (topmoving == true)
        {
            top = point.y;
            if (top < 0)
            {
                top = 0;
            }
            if (top >= bottom)
            {
                top = bottom - 1;
            }
        }

        // 移动框选区域
        if (leftmoving == false && rightmoving == false && bottommoving == false && topmoving == false)
        {
            int temp = right - left;
            left = point.x - Dx;
            right = left + temp;

            temp = bottom - top;
            // 上边界超出，纠正
            if (point.y - Dy < 0)
            {
                top = 0;
            }
            else
            {
                top = point.y - Dy;
            }

            bottom = top + temp;
            // 检测下边界
            if (bottom > 343)
            {
                bottom = 343;
                top = bottom - temp;
            }

            m_RegSelBox();
        }
    }

    if (m_status == 2)
    {
        // 上标移动
        if (ibottommoving == true)
        {
            ibottom = point.y;
            if (ibottom <= itop)
            {
                ibottom = itop + 1;
            }
            if (ibottom > 344)
            {
                ibottom = 344;
            }
        }
        // 下标移动
        if (itopmoving == true)
        {
            itop = point.y;
            if (itop < 0)
            {
                itop = 0;
            }
            if (itop >= ibottom)
            {
                itop = ibottom - 1;
            }
        }

        // 移动整个区间
        if (itopmoving == false && ibottommoving == false)
        {
            int temp = ibottom - itop;
            itop = point.y - iDy;
            if (itop < 0)
            {
                itop = 0;
            }
            ibottom = itop + temp;
            if (ibottom > 343)
            {
                ibottom = 343;
                itop = ibottom - temp;
            }
        }
    }

    if (m_status != 0)
    {
        // 将显示坐标换算成hsi
        ShowToHSI();
        tp = point;
        tp.x += 12;
        switch (m_status)
        {
        case 1:
            if (tp.x > 273)
            {
                tp.x = 273;
            }
            if (tp.x < 0)
            {
                tp.x = 0;
            }
            if (tp.y > 330)
            {
                tp.y = 330;
            }
            if (tp.y < 0)
            {
                tp.y = 0;
            }
            break;

        case 2:
            tp.x = 517;
            break;
        }
        Invalidate();
        if (pList != NULL)
        {
            pList->ShowParam(*(pList->pset));
        }
    }

    if (pParam != NULL)
    {
        *pParam = hsi;
    }

    CStatic::OnMouseMove(nFlags, point);
}

void CColor::m_RegSelBox()
{
    // 将left和right都限定在合理范围之内
    while (left > 396)
    {
        left -= 396;
    }

    while (left < 0)
    {
        left += 396;
    }

    while (right > 396)
    {
        right -= 396;
    }

    while (right < 0)
    {
        right += 396;
    }
}

void CColor::CleanStat()
{
    for (int i = 0; i < 255; i++)
    {
        Hdistrib[i] = 0;
        Sdistrib[i] = 0;
        Idistrib[i] = 0;
    }
    Hcmax = 0;
    Scmax = 0;
    Icmax = 0;
    for (i = 255; i < 360; i++)
    {
        Hdistrib[i] = 0;
    }
}
