// VoyWith2Cap.h: interface for the CVoyWith2Cap class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 张万杰      Version: 1.1      Date: 2007/10/19
  Email:  zwj_uptech@126.com
  Description:    // 带有两个视频接口的控制组合
  Others:         // 1.1 添加图片仿真功能
*************************************************/

#if !defined(AFX_VOYWITH2CAP_H__5470B2D3_8769_4826_9F3C_15E4A3E66C83__INCLUDED_)
#define AFX_VOYWITH2CAP_H__5470B2D3_8769_4826_9F3C_15E4A3E66C83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoyCmd.h"
#include "BmpLoader.h"
#include "..\ICCamera.h"

class CVoyWith2Cap
{
  public:
    void CloseEmuOverlook();
    void ReloadOverlook();
    void EmuOverlookVideo(CString invideofile, CStatic *inPrev);
    void EmuFront(CString inbmpfile, CStatic *inPrev);
    void EmuOverlook(CString inbmpfile, CStatic *inPrev);
    void VectorMove(double inAngle, int inLV, int inPSpeed);
    void CloseOverCap();
    void CloseFrontCap();
    void StopAll();
    void SendSpeed(int inLeft, int inRight);
    void SetBehavior(IBehavior *inbeh);
    BOOL CreatOverCap(int iDeviceID, CStatic *inpPrev);
    BOOL CreatFrontCap(int iDeviceID, CStatic *inpPrev);

    CVoyCmd m_cmd; // voy协议解析对象

    IBehavior m_idle; // 闲置行为
    CICCamera m_cOverCap;

    CBmpLoader m_bmploader; // 仿真图片加载
    CStatic *m_emudis;

    CVoyWith2Cap();
    virtual ~CVoyWith2Cap();
};

#endif // !defined(AFX_VOYWITH2CAP_H__5470B2D3_8769_4826_9F3C_15E4A3E66C83__INCLUDED_)
