// GKRecerive.h: interface for the CGKRecerive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GKRECERIVE_H__530DF075_AFFB_4D69_95F8_86899D9F1674__INCLUDED_)
#define AFX_GKRECERIVE_H__530DF075_AFFB_4D69_95F8_86899D9F1674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VOYINCLUDE\UDPServer.h"

typedef struct stBallFly
{
    WORD header;
    WORD nSeen;
    float fBallx;
    float fBally;
    float fBallz;
} stBallFly;

class CGKRecerive : public CUDPServer
{
  public:
    void Received(void *inbuf, int inlen, SOCKADDR *inpSock);
    CGKRecerive();
    virtual ~CGKRecerive();

    stBallFly m_BallFly;
    CEdit *pDis;
};

#endif // !defined(AFX_GKRECERIVE_H__530DF075_AFFB_4D69_95F8_86899D9F1674__INCLUDED_)
