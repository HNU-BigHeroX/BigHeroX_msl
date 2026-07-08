// WaterMulticast.h: interface for the CWaterMulticast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATERMULTICAST_H__FABEE713_D490_4237_A6E8_2F44502DAB21__INCLUDED_)
#define AFX_WATERMULTICAST_H__FABEE713_D490_4237_A6E8_2F44502DAB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MulticastSocket.h"

// 组播的数据可以是个结构体
typedef struct WtData
{
    int nJieQiu_x;
    int nJieQiu_y;
} WtData;

class CWaterMulticast : public CMulticastSocket
{
  public:
    void PostData();
    void Recieved(BYTE *inBuf, int inLen);
    CWaterMulticast();
    virtual ~CWaterMulticast();

    WtData m_FaSongShuJu;  // 待发送的数据
    WtData m_JieShouShuJu; // 用来接收的数据
    int nJieShouJiShu;     // 接收计数，用来判断是否组播停止

    int m_nRecvCntLog; // 接收计数，用来发送给教练机的
};

#endif // !defined(AFX_WATERMULTICAST_H__FABEE713_D490_4237_A6E8_2F44502DAB21__INCLUDED_)
