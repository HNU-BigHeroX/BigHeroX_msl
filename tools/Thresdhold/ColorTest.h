// ColorTest.h: interface for the CColorTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORTEST_H__13833741_BDDD_4667_9391_F58C8215663C__INCLUDED_)
#define AFX_COLORTEST_H__13833741_BDDD_4667_9391_F58C8215663C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FRAME_CNT_MAX 10

#include "VOYINCLUDE\IBehavior.h"
#include "VoyInclude\VoyHSI.h"
#include "VoyInclude\RGB2HSI.h"
#include "VoyInclude\UPGraphics.h"
#include "StillView.h"

class CColorTest : public IBehavior
{
  public:
    void printf(CString instr);
    void ClearAllFlag();
    // 颜色标记
    bool bBlack;
    bool bGreen;
    bool bLines;
    bool bOverBall;
    bool bTeammate;
    bool bOpponent;

    // 中心对准标记
    BOOL bMidPoint;
    RGBTRIPLE focusclr;

    // 前置摄像头角度
    bool bFrontAng;

    CUPGraphics blackmap;
    CUPGraphics ballmap;
    CUPGraphics greenmap;
    CUPGraphics teammatemap;
    CUPGraphics whitelinemap;
    CUPGraphics oppomap;
    // 颜色参数体
    HSISet m_vhsi;
    // 颜色空间转换对象
    CRGB2HSI m_rgb2hsi;

    // 局部显示功能
    int x1;
    int y1;
    int x2;
    int y2;
    int nWidth;
    int nHeight;
    void DisplayBits(BYTE *pbuf, int width, int height);
    HWND hwndDis;
    RGBTRIPLE frmclr;
    CStillView *pStill;
    CStillView *pLive;

    // 信息显示
    CListBox *pList;

    void AfterUpdateOverlook(BYTE *pBuffer, long lWidth, long lHeight, double dbTime, UINT nState);
    CColorTest();
    virtual ~CColorTest();

    HSIParam *param[HSI_TARGET_NUM];
    bool en[HSI_TARGET_NUM];
    RGBTRIPLE clr[HSI_TARGET_NUM];

    // 连续采十帧
    int nFrameCnt;
    unsigned char *arFrame[FRAME_CNT_MAX];
    unsigned char *bufImg;
};

#endif // !defined(AFX_COLORTEST_H__13833741_BDDD_4667_9391_F58C8215663C__INCLUDED_)
