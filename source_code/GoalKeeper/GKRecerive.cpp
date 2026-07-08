// GKRecerive.cpp: implementation of the CGKRecerive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoyVideo.h"
#include "GKRecerive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGKRecerive::CGKRecerive()
{
}

CGKRecerive::~CGKRecerive()
{
}

static CString strTmp;
static stBallFly tmpBF;
void CGKRecerive::Received(void *inbuf, int inlen, SOCKADDR *inpSock)
{
    // 1、验证包头
    memcpy(&tmpBF, inbuf, inlen);
    if (tmpBF.header != 0x55aa)
    {
        pDis->SetWindowText("接收格式非法！");
        m_BallFly.nSeen = 0;
        return;
    }
    // 验证尺寸
    if (inlen < sizeof(stBallFly))
    {
        pDis->SetWindowText("接收数据不完整！");
        m_BallFly.nSeen = 0;
        return;
    }
    // 合法数据，复制到正式结构
    memcpy(&m_BallFly, &tmpBF, inlen);

    // 显示
    if (m_BallFly.nSeen == 0)
    {
        //		pDis->SetWindowText("未看见球...");
    }
    else
    {
        //	m_BallFly.fBally=m_BallFly.fBally*cos(20/57.29578);
        //	m_BallFly.fBallz=m_BallFly.fBallz*sin(20/57.29578);
        //	m_BallFly.fBally=2;
        strTmp.Format("%d - (%.2f , %.2f , %.2f)", m_BallFly.nSeen, m_BallFly.fBallx, m_BallFly.fBally,
                      m_BallFly.fBallz);
        pDis->SetWindowText(strTmp);
    }
}
