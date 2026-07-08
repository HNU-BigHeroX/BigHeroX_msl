// MPUParser.cpp: implementation of the CMPUParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MPUParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMPUParser::CMPUParser()
{
    pRecvList = NULL;
    memset(m_ParseBuf, 0, 128);
    m_nRecvIndex = 0;      // 接收索引
    m_lastRecv = 0;        // 上一个字符
    m_bFrameStart = false; // 帧解析开始
    m_nFrameLength = 43;   // 帧长度

    nQuatW = 10;
    nQuatX = 10;
    nQuatY = 10;
    nQuatZ = 10;
    nGyroX = 10;
    nGyroY = 10;
    nGyroZ = 10;
    nAccX = 10;
    nAccY = 10;
    nAccZ = 10;

    pCmd = NULL;
    m_nRecvCntLog = 0;
}

CMPUParser::~CMPUParser()
{
}

void CMPUParser::Parse(BYTE inData)
{
    m_ParseBuf[m_nRecvIndex] = inData;

    if (m_lastRecv == 0x55 && inData == 0xAA && m_bFrameStart == 0)
    {
        m_bFrameStart = true;
        m_ParseBuf[0] = m_lastRecv;
        m_ParseBuf[1] = inData;
        m_nRecvIndex = 2;
        m_lastRecv = 0x00;
        return;
    }

    if (m_bFrameStart)
    {
        // put received data into buffer
        m_ParseBuf[m_nRecvIndex] = inData;
        m_nRecvIndex++;

        // receive one frame, invoke ParseFrame to parse
        if (m_nRecvIndex == m_nFrameLength)
        {
            // m_DispRecvCmd();
            m_ParseFrame(m_ParseBuf, m_nFrameLength);
            m_bFrameStart = false;
        }

        // receive buffer overflow
        if (m_nRecvIndex >= 128)
        {
            // m_ResetRcvBuf();
            m_bFrameStart = false;
        }
    }
    else
        m_lastRecv = inData;
}

int CMPUParser::m_Piece2int(unsigned char *inTarg)
{

    int ret;
    ret = inTarg[0];
    ret <<= 8;
    ret |= ((int)inTarg[1] & 0x00ff);
    ret <<= 8;
    ret |= ((int)inTarg[2] & 0x00ff);
    ret <<= 8;
    ret |= ((int)inTarg[3] & 0x00ff);
    return ret;
}

void CMPUParser::m_ParseFrame(unsigned char *inBuf, int inLen)
{
    m_nRecvCntLog++;
    DisRecvData();
    BYTE ckSum = 0;
    for (int i = 0; i < 42; i++)
    {
        ckSum += inBuf[i];
    }
    if (ckSum != inBuf[42])
    {
        if (pRecvList != NULL)
        {
            pRecvList->AddString("MPU6050校验和错误");
            pRecvList->SetCurSel(pRecvList->GetCount() - 1);
        }
        return;
    }
    BYTE *pBuf = inBuf + 2;

    if (pCmd != NULL)
    {
        memcpy(pCmd->Arduino_MPU6050Data, pBuf, 40);
    }
    else
    {
        pRecvList->AddString("MPU6050的pCmd为空！");
        pRecvList->SetCurSel(pRecvList->GetCount() - 1);
    }

    // 这里不处理，复制到voycmd里由陈松的图像线程去处理
    // 	nQuatW = m_Piece2int(&pBuf[0]);
    // 	nQuatX = m_Piece2int(&pBuf[4]);
    // 	nQuatY = m_Piece2int(&pBuf[8]);
    // 	nQuatZ = m_Piece2int(&pBuf[12]);
    //
    // 	nGyroX = m_Piece2int(&pBuf[16]);
    // 	nGyroY = m_Piece2int(&pBuf[20]);
    // 	nGyroZ = m_Piece2int(&pBuf[24]);
    //
    // 	nAccX = m_Piece2int(&pBuf[28]);
    // 	nAccY = m_Piece2int(&pBuf[32]);
    // 	nAccZ = m_Piece2int(&pBuf[36]);
}

void CMPUParser::DisRecvData()
{
    if (pRecvList != NULL)
    {
        CString strData, str;
        strData = "";
        int i;
        for (i = 0; i < 43; i++)
        {
            str.Format("%.2X ", m_ParseBuf[i]);
            strData += str;
        }
        pRecvList->AddString(strData);
        pRecvList->SetCurSel(pRecvList->GetCount() - 1);
    }
}
