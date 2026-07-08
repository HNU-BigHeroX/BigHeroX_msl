// MPUParser.h: interface for the CMPUParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MPUPARSER_H__9A38A7AD_5DA0_4142_B07A_A5503B2D5AA2__INCLUDED_)
#define AFX_MPUPARSER_H__9A38A7AD_5DA0_4142_B07A_A5503B2D5AA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoyInclude/SerialPort.h"
#include "VoyInclude/VoyCmd.h"

class CMPUParser : public CSerialPort
{
  public:
    void DisRecvData();
    int m_Piece2int(unsigned char *inTarg);
    void Parse(BYTE inData);
    CMPUParser();
    virtual ~CMPUParser();

    CVoyCmd *pCmd;

    CListBox *pRecvList;

    unsigned char m_ParseBuf[128];
    int m_nRecvIndex;         // 接收索引
    unsigned char m_lastRecv; // 上一个字符
    bool m_bFrameStart;       // 帧解析开始
    int m_nFrameLength;       // 帧长度

    long nQuatW;
    long nQuatX;
    long nQuatY;
    long nQuatZ;

    long nGyroX;
    long nGyroY;
    long nGyroZ;

    long nAccX;
    long nAccY;
    long nAccZ;

    int m_nRecvCntLog;

  protected:
    void m_ParseFrame(unsigned char *inBuf, int inLen);
};

#endif // !defined(AFX_MPUPARSER_H__9A38A7AD_5DA0_4142_B07A_A5503B2D5AA2__INCLUDED_)
