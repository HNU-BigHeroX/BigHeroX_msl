// RGBShow.cpp : implementation file
//

#include "stdafx.h"
#include "RGBShow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRGBShow

CRGBShow::CRGBShow()
{
    updateing = false;
    _pImgbuf = NULL;
    _nImgWidth = _nImgHeight = 0;
}

CRGBShow::~CRGBShow()
{
    _DelBuf();
    _list.RemoveAll();
}

BEGIN_MESSAGE_MAP(CRGBShow, CStatic)
//{{AFX_MSG_MAP(CRGBShow)
ON_WM_PAINT()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRGBShow message handlers

void CRGBShow::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    // 创建缓存DC
    CDC dcMemory;
    dcMemory.CreateCompatibleDC(&dc);

    // 获取当前尺寸，将内存dc撑开
    GetClientRect(&_FrameRect);

    CBitmap Bitmap;
    Bitmap.CreateCompatibleBitmap(&dc, _FrameRect.right, _FrameRect.bottom);
    CBitmap *oldbmp = dcMemory.SelectObject(&Bitmap);

    if (_pImgbuf != NULL && updateing == false)
    {
        // 写目标位图格式信息
        BITMAPINFOHEADER bih;
        memset(&bih, 0, sizeof(bih));
        bih.biSize = sizeof(bih);
        bih.biWidth = _nImgWidth;
        bih.biHeight = _nImgHeight;
        bih.biPlanes = 1;
        bih.biBitCount = 24;

        ::StretchDIBits(dcMemory.m_hDC, 0, 0, _FrameRect.right - _FrameRect.left, _FrameRect.bottom - _FrameRect.top, 0,
                        0, _nImgWidth, _nImgHeight, _pImgbuf, (BITMAPINFO *)&bih, DIB_RGB_COLORS, SRCCOPY);

        CPen boxline(PS_SOLID, 1, COLORREF(RGB(255, 0, 0)));

        CPen *pOldPen = dcMemory.SelectObject(&boxline);

        // 逆时针画当前框
        dcMemory.MoveTo(_CurRect.left, _CurRect.top);
        dcMemory.LineTo(_CurRect.right, _CurRect.top);
        dcMemory.LineTo(_CurRect.right, _CurRect.bottom);
        dcMemory.LineTo(_CurRect.left, _CurRect.bottom);
        dcMemory.LineTo(_CurRect.left, _CurRect.top);

        // 绘制列表中存在的框
        CRect r;
        POSITION pos = _list.GetHeadPosition();
        while (pos != NULL)
        {
            r = _list.GetNext(pos);
            // 逆时针画框
            dcMemory.MoveTo(r.left, r.top);
            dcMemory.LineTo(r.right, r.top);
            dcMemory.LineTo(r.right, r.bottom);
            dcMemory.LineTo(r.left, r.bottom);
            dcMemory.LineTo(r.left, r.top);
        }
    }

    // dc.BitBlt(0, 0, _nImgWidth, _nImgHeight, &dcMemory, 0, 0, SRCCOPY);
    dc.BitBlt(0, 0, _FrameRect.right, _FrameRect.bottom, &dcMemory, 0, 0, SRCCOPY);

    dcMemory.DeleteDC();
    // Do not call CStatic::OnPaint() for painting messages
}

void CRGBShow::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    // 起点
    _CurRect.left = point.x;
    _CurRect.right = point.x;
    _CurRect.top = point.y;
    _CurRect.bottom = point.y;

    Invalidate();

    CStatic::OnLButtonDown(nFlags, point);
}

void CRGBShow::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (nFlags && MK_LBUTTON)
    {
        // 及时跟新当前选框
        _CurRect.right = point.x;
        _CurRect.bottom = point.y;

        int x = point.x * 320 / _FrameRect.right;
        int y = point.y * 240 / _FrameRect.bottom;
        CString str;
        str.Format("%d,%d", x, y);
        _pShow->SetWindowText(str);
        Invalidate();
    }

    CStatic::OnMouseMove(nFlags, point);
}

void CRGBShow::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    // 调整上下左右值，使其为一个正矩形
    if (_CurRect.top > _CurRect.bottom)
    {
        int temp = _CurRect.top;
        _CurRect.top = _CurRect.bottom;
        _CurRect.bottom = temp;
    }
    if (_CurRect.left > _CurRect.right)
    {
        int temp = _CurRect.left;
        _CurRect.left = _CurRect.right;
        _CurRect.right = temp;
    }

    _list.AddTail(_CurRect);

    CStatic::OnLButtonUp(nFlags, point);
}

void CRGBShow::CreateImg(BYTE *inBuf, int inWidth, int inHeight)
{
    updateing = true;

    // 先清除残存的选框
    RemoveRect();

    // 如果图像尺寸发生变化，则重新分配图像缓冲区
    if (inWidth != _nImgWidth || inHeight != _nImgHeight)
    {
        _DelBuf();
        _pImgbuf = new BYTE[inWidth * inHeight * 3];
        _nImgWidth = inWidth;
        _nImgHeight = inHeight;
    }

    // 复制图像
    memcpy(_pImgbuf, inBuf, inWidth * inHeight * 3);
    _ASSERTE(_CrtCheckMemory());
    updateing = false;
    // Invalidate();
}

void CRGBShow::_DelBuf()
{
    updateing = true;
    if (_pImgbuf != NULL)
    {
        delete[] _pImgbuf;
        _pImgbuf = NULL;
    }
    updateing = false;
}

// 清除图像上的方框
void CRGBShow::RemoveRect()
{
    updateing = true;

    _list.RemoveAll();
    _CurRect.top = _CurRect.bottom = _CurRect.left = _CurRect.right = 0;

    updateing = false;
}

int CRGBShow::GetCount()
{
    return _list.GetCount();
}

// 获取指定矩形位置，返回的矩形换算成DIB的坐标
CRect CRGBShow::GetRect(int index)
{
    if (index > _list.GetCount() && index < 0)
    {
        return NULL;
    }

    updateing = true;
    int i = index;
    POSITION pos = _list.GetHeadPosition();
    while (pos != NULL)
    {
        if (i == 0)
        {
            // 找到了，将其换算到DIB的坐标
            CRect bufrect = _list.GetAt(pos);

            int width = _FrameRect.right;
            int height = _FrameRect.bottom;

            bufrect.top = (height - bufrect.top) * _nImgHeight / height;
            bufrect.bottom = (height - bufrect.bottom) * _nImgHeight / height;
            bufrect.left = bufrect.left * _nImgWidth / width;
            bufrect.right = bufrect.right * _nImgWidth / width;

            updateing = false;
            return bufrect;
        }
        _list.GetNext(pos);
        i--;
    }
    updateing = false;
    return NULL;
}

BYTE *CRGBShow::GetIMGBuf()
{
    return _pImgbuf;
}
