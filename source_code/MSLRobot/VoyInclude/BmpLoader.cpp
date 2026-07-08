// BmpLoader.cpp: implementation of the CBmpLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BmpLoader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBmpLoader::CBmpLoader()
{
    m_pdata = m_pbuf = m_pImgBuf = NULL;
    pheader = NULL;
    bufsize = 0;
    m_imgbufsize = 0;
}

CBmpLoader::~CBmpLoader()
{
    if (m_pbuf != NULL)
    {
        delete[] m_pbuf;
    }
    if (m_pImgBuf != NULL)
    {
        delete[] m_pImgBuf;
    }
}

bool CBmpLoader::LoadFile(CString infilename)
{
    CFileFind finder;
    if (finder.FindFile(infilename) == FALSE)
    {
        AfxMessageBox("该文件不存在！");
        return false;
    }

    CFile mFile(infilename, CFile::modeRead);

    int filelen = mFile.GetLength();
    if (filelen == 0)
    {
        AfxMessageBox("文件为空！");
        return false;
    }

    if (filelen > bufsize)
    {
        if (m_pbuf != NULL)
        {
            delete[] m_pbuf;
            m_pbuf = NULL;
        }

        if (m_pImgBuf != NULL)
        {
            delete[] m_pImgBuf;
            m_pImgBuf = NULL;
        }
        bufsize = filelen;
        m_pbuf = new UCHAR[bufsize];
        m_pImgBuf = new UCHAR[bufsize];
    }

    mFile.Read(m_pbuf, filelen);
    mFile.Close();

    // file header
    BITMAPFILEHEADER *pfh = (BITMAPFILEHEADER *)m_pbuf;
    if (pfh->bfType != 'MB')
    {
        AfxMessageBox("此文件并非BMP格式！");
        return false;
    }

    // info header
    pheader = (BITMAPINFOHEADER *)(m_pbuf + sizeof(BITMAPFILEHEADER));
    if (pheader->biWidth < 0 || pheader->biHeight < 0)
    {
        AfxMessageBox("图片尺寸异常！");
        return false;
    }

    m_pdata = (m_pbuf + pfh->bfOffBits);

    m_imgbufsize = filelen - pfh->bfOffBits;
    memcpy(m_pImgBuf, m_pdata, m_imgbufsize);
    _ASSERTE(_CrtCheckMemory());

    return true;
}

BOOL CBmpLoader::DisplayPic(CStatic *inDisp)
{
    HWND hwnd = inDisp->GetSafeHwnd();
    RECT rc;
    ::GetWindowRect(hwnd, &rc);
    long lWidth = rc.right - rc.left;
    long lHeight = rc.bottom - rc.top;

    HDC hdc = ::GetDC(hwnd);
    PAINTSTRUCT ps;
    ::BeginPaint(hwnd, &ps);

    SetStretchBltMode(hdc, COLORONCOLOR);
    StretchDIBits(hdc, 0, 0, lWidth, lHeight, 0, 0, pheader->biWidth, pheader->biHeight, m_pImgBuf,
                  (BITMAPINFO *)pheader, DIB_RGB_COLORS, SRCCOPY);

    ::EndPaint(hwnd, &ps);
    ::ReleaseDC(hwnd, hdc);

    return TRUE;
}

bool CBmpLoader::ReloadBuf()
{
    if (m_pImgBuf != NULL)
    {
        memcpy(m_pImgBuf, m_pdata, m_imgbufsize);
        _ASSERTE(_CrtCheckMemory());
        return true;
    }
    else
    {
        return false;
    }
}
