// ColorTest.cpp: implementation of the CColorTest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thresdhold.h"
#include "ColorTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorTest::CColorTest()
{
    m_vhsi.black.Hue_max = 360;
    m_vhsi.black.Hue_min = 0;
    m_vhsi.black.Satu_max = 10;
    m_vhsi.black.Satu_min = 0;
    m_vhsi.black.I_max = 100;
    m_vhsi.black.I_min = 0;

    m_vhsi.green.Hue_min = 80;
    m_vhsi.green.Hue_max = 160;
    m_vhsi.green.Satu_min = 100;
    m_vhsi.green.Satu_max = 255;
    m_vhsi.green.I_max = 255;
    m_vhsi.green.I_min = 0;

    m_vhsi.whitelines.Hue_max = 360;
    m_vhsi.whitelines.Hue_min = 0;
    m_vhsi.whitelines.Satu_max = 20;
    m_vhsi.whitelines.Satu_min = 0;
    m_vhsi.whitelines.I_max = 255;
    m_vhsi.whitelines.I_min = 200;

    m_vhsi.overball.Hue_min = 10;
    m_vhsi.overball.Hue_max = 50;
    m_vhsi.overball.Satu_min = 100;
    m_vhsi.overball.Satu_max = 255;
    m_vhsi.overball.I_max = 255;
    m_vhsi.overball.I_min = 0;

    m_vhsi.teammate.Hue_min = 285;
    m_vhsi.teammate.Hue_max = 315;
    m_vhsi.teammate.Satu_min = 100;
    m_vhsi.teammate.Satu_max = 255;
    m_vhsi.teammate.I_max = 255;
    m_vhsi.teammate.I_min = 0;

    m_vhsi.opponent.Hue_max = 360;
    m_vhsi.opponent.Hue_min = 0;
    m_vhsi.opponent.Satu_max = 10;
    m_vhsi.opponent.Satu_min = 0;
    m_vhsi.opponent.I_max = 100;
    m_vhsi.opponent.I_min = 0;

    //////////////////////////////////////////////////////////////////////////
    param[0] = &(m_vhsi.black);
    param[1] = &(m_vhsi.green);
    param[2] = &(m_vhsi.whitelines);
    param[3] = &(m_vhsi.overball);
    param[4] = &(m_vhsi.teammate);
    param[5] = &(m_vhsi.opponent);

    for (int i = 0; i < 5; i++)
    {
        en[i] = false;
    }

    bBlack = false;
    bGreen = false;
    bLines = false;
    bOverBall = false;
    bTeammate = false;
    bOpponent = false;

    bMidPoint = false;
    bFrontAng = false;

    // black
    clr[0].rgbtBlue = 100;
    clr[0].rgbtGreen = 00;
    clr[0].rgbtRed = 100;

    // green
    clr[1].rgbtBlue = 0;
    clr[1].rgbtGreen = 0xff;
    clr[1].rgbtRed = 0;

    // whitlines
    clr[2].rgbtBlue = 255;
    clr[2].rgbtGreen = 255;
    clr[2].rgbtRed = 0;

    // overball
    clr[3].rgbtBlue = 0;
    clr[3].rgbtGreen = 0;
    clr[3].rgbtRed = 0xff;

    // teammate
    clr[4].rgbtBlue = 255;
    clr[4].rgbtGreen = 0;
    clr[4].rgbtRed = 0;

    // opponent
    clr[5].rgbtBlue = 0;
    clr[5].rgbtGreen = 100;
    clr[5].rgbtRed = 0xff;

    focusclr.rgbtBlue = 0x9f;
    focusclr.rgbtGreen = 0x9f;
    focusclr.rgbtRed = 0xff;
    //////////////////////////////////////////////////////////////////////////
    nWidth = 640;
    nHeight = 480;

    // camera
    // 	m_vhsi.cameraproc.nBrightness = 125;
    // 	m_vhsi.cameraproc.nContrast = 20;
    // 	m_vhsi.cameraproc.nSaturation = 20;
    // 	m_vhsi.cameraproc.nSharpness = 179;
    // 	m_vhsi.cameraproc.nWhiteBalance = 5000;
    // 	m_vhsi.cameraproc.nBacklightCompensation = 1;
    // 	m_vhsi.cameraproc.nGain = 50;
    // 	m_vhsi.cameraproc.nExposure = -7;

    hwndDis = NULL;
    frmclr.rgbtBlue = 0;
    frmclr.rgbtGreen = 0;
    frmclr.rgbtRed = 255;

    // 连续采集帧
    nFrameCnt = FRAME_CNT_MAX;
    for (i = 0; i < FRAME_CNT_MAX; i++)
    {
        arFrame[i] = new unsigned char[640 * 480 * 3];
        memset(arFrame[i], 0, 640 * 480 * 3);
    }
    bufImg = new unsigned char[640 * 480 * 3];
    memset(bufImg, 0, 640 * 480 * 3);
}

CColorTest::~CColorTest()
{
    for (int i = 0; i < FRAME_CNT_MAX; i++)
    {
        delete[] arFrame[i];
    }
}

void CColorTest::AfterUpdateOverlook(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState)
{
    // 送静态显示
    pStill->BufImg(pBuffer, lWidth, lHeight);

    // 开始处理
    nWidth = lWidth;
    nHeight = lHeight;
    m_rgb2hsi.InitBuf(pBuffer, lWidth, lHeight);
    m_rgb2hsi.Convert();

    //////////////////////////////////////////////////////////////////////////

    en[0] = bBlack;
    en[1] = bGreen;
    en[2] = bLines;
    en[3] = bOverBall;
    en[4] = bTeammate;
    en[5] = bOpponent;

    m_rgb2hsi.MappingM(param, en, clr, HSI_TARGET_NUM);

    if (bMidPoint)
    {
        m_rgb2hsi.DrawFocus(pBuffer, lWidth / 2, lHeight / 2, &focusclr);
        // printf("midpoint");
    }

    // pLive->m_fpsDis = dbTime;
    pLive->BufImg(pBuffer, lWidth, lHeight);
    memcpy(bufImg, pBuffer, 640 * 480 * 3);

    //////////////////////////////////////////////////////////////////////////
    // 连续帧采集
    if (nFrameCnt < FRAME_CNT_MAX)
    {
        memcpy(arFrame[nFrameCnt], pBuffer, 640 * 480 * 3);
        nFrameCnt++;
    }
    //////////////////////////////////////////////////////////////////////////
}

void CColorTest::ClearAllFlag()
{
    bBlack = false;
    bOverBall = false;
    bGreen = false;
    bTeammate = false;
    bLines = false;
    bOpponent = false;
}

void CColorTest::DisplayBits(BYTE *pbuf, int width, int height)
{
    if (hwndDis == NULL)
    {
        return;
    }
    // 计算显示位置
    RECT rc;
    ::GetWindowRect(hwndDis, &rc);
    long lStillWidth = rc.right - rc.left;
    long lStillHeight = rc.bottom - rc.top;

    // 创建位图格式
    BITMAPINFOHEADER bih;
    memset(&bih, 0, sizeof(bih));
    bih.biSize = sizeof(bih);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;

    HDC hdcDis = GetDC(hwndDis);
    PAINTSTRUCT ps;
    BeginPaint(hwndDis, &ps);

    SetStretchBltMode(hdcDis, COLORONCOLOR);
    StretchDIBits(
        // 目标
        hdcDis, 0, 0, lStillWidth, lStillHeight,
        // 源
        x1, y1, (x2 - x1), (y2 - y1), pbuf, (BITMAPINFO *)&bih, DIB_RGB_COLORS, SRCCOPY);

    EndPaint(hwndDis, &ps);
    ReleaseDC(hwndDis, hdcDis);
}

void CColorTest::printf(CString instr)
{
    pList->AddString(instr);
    pList->SetCurSel(pList->GetCount() - 1);
}
