// RototPlayer.h: interface for the CRototPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROTOTPLAYER_H__1AA488FB_2FC0_4B51_9681_5E63BDEBC4A2__INCLUDED_)
#define AFX_ROTOTPLAYER_H__1AA488FB_2FC0_4B51_9681_5E63BDEBC4A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoyInclude/VoyCmd.h"

class CRototPlayer
{
  public:
    CRototPlayer();
    virtual ~CRototPlayer();

    CVoyCmd m_cmd;
    int m_nMotorToSend[6];
};

#endif // !defined(AFX_ROTOTPLAYER_H__1AA488FB_2FC0_4B51_9681_5E63BDEBC4A2__INCLUDED_)
