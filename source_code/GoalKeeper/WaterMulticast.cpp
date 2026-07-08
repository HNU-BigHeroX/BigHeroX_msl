// WaterMulticast.cpp: implementation of the CWaterMulticast class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoyVideo.h"
#include "WaterMulticast.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaterMulticast::CWaterMulticast()
{
    nJieShouJiShu = 0;
    m_nRecvCntLog = 0;
}

CWaterMulticast::~CWaterMulticast()
{
}

/****************************************
一、发送信息到组播：
   1)在CWaterMulticast类内部为m_FaSongShuJu成员变量赋值，然后直接调用PostData()函数
   2)在CGetImage类中，为m_zubo.m_FaSongShuJu成员变量赋值，调用m_zubo.PostData()函数
   二、从组播里接收信息:
   在WtData结构体中定义成员变量，可以在CGetImage类中，通过m_zubo.m_JieShouShuJu.xxx（xxx为成员变量名）进行读取
    在下面的Recieved函数中，inBuf指向接收到的数组，inLen是接收到的数据长度。
    注：若要判断组播数据是否已经中断，可以在下面的Recieved函数中定义一个变量，每次接收累加，读取时清零。
    再次读取时判断该变量是否为零，即可判断是否接收到数据。
****************************************/
void CWaterMulticast::Recieved(BYTE *inBuf, int inLen)
{
    PrintList("CWaterMulticast::Recieved!"); // 调试用，可注释

    if (inLen > sizeof(m_JieShouShuJu))
    {
        inLen = sizeof(m_JieShouShuJu);
    }
    memcpy(&m_JieShouShuJu, inBuf, inLen); // 将接收数据还原成结构体，供GetImage里通过m_zubo读取
    nJieShouJiShu++;                       // 接收计数，用于判断是否组播数据停止
    m_nRecvCntLog++;
}

void CWaterMulticast::PostData()
{
    // 直接将结构体组播了
    Post((BYTE *)&m_FaSongShuJu, sizeof(m_FaSongShuJu));
}
