// AMCMotion.h: interface for the CAMCMotion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMCMOTION_H__2EE5FE0F_D8AF_403F_814A_417877465552__INCLUDED_)
#define AFX_AMCMOTION_H__2EE5FE0F_D8AF_403F_814A_417877465552__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialPort.h"

#define MOTOR_NUM 5
#define AMC_MAX_BUF 64

class CAMCMotion : public CSerialPort
{
  public:
    void AMCWrite(unsigned char Index, unsigned char Offset, unsigned char DataWords, unsigned char *pdata);
    void AMCSetVelocity(int inSpeed);
    void AMCGainWriteAccess();
    void AMCEnableBridge();
    void AMCDisableBridge();
    void SetSpeed(unsigned char inAddr, int speed);

    CListBox *pRecvList;
    CListBox *pInfoList;

    volatile int AMCVelocity[MOTOR_NUM];
    volatile bool m_bNewVelocity[MOTOR_NUM];
    bool m_bGainWriteAccess[MOTOR_NUM];
    bool m_bBridgeOn[MOTOR_NUM];
    bool m_bBridgeOff[MOTOR_NUM];
    BYTE AMCAddr[MOTOR_NUM];

    int TargetVelocity[MOTOR_NUM];
    int VelocityStep;
    int VA_Snap;

    unsigned char g_WriteBuf[AMC_MAX_BUF];
    // static unsigned char g_WriteBuf[AMC_MAX_BUF] = {0xA5,0x3F,0x02};

    unsigned int g_accum;
    unsigned int Gr1;

    bool mFrameStart;
    int mIndex;
    int mDataLen;
    BYTE mLast;
    BYTE mRecvBuf[AMC_MAX_BUF];

    volatile bool m_bCurrentMeasured;
    volatile int m_curAddrIndex;
    volatile bool m_bLogVelocity[MOTOR_NUM];

    volatile bool m_bReplay;
    HANDLE m_hReplyEvent;

  public:
    void Slippage_AddVData(BYTE inAddr, int inData);
    void Log_AddVData(BYTE inAddr, int inData);
    void Log_AddCurrentData(BYTE inAddr, double inData);
    void Log_CreatFile();
    void SaveCurrentLog();
    void AMCRead(unsigned char Index, unsigned char Offset, unsigned char DataWords);
    void DemandCurrentMeasured();
    void SetMultiTargetVelocity(int *inVel, int inNum);
    void VelocityAdj();
    void ACCCtrlMode();
    void SetBridge(int inIndex, bool inEN);
    void SetGWA(int inIndex);
    void SingleBridge(unsigned char inAddr, bool inEnable);
    void SingleGWA(unsigned char inAddr);
    void MultiAMCAction();
    void MultiMode();
    void SetMultiVelocity(int *inVel, int inNum);
    void SetMultiAddrs(BYTE *inarAddr, int inNum);
    void AMCStore();
    bool RS485_ChangeToAddr(BYTE inNewAddr);
    void Parse(BYTE inData);
    void SetAddress(BYTE inAddr);
    CAMCMotion();
    virtual ~CAMCMotion();

  protected:
    void mPrintf(CString inStr, CListBox *inList);
    void mParseFrame();
    void CrunchCRC(char x);
    unsigned short CalcFieldCRC(unsigned char *pDataArray, unsigned char numberOfchars);
};

#endif // !defined(AFX_AMCMOTION_H__2EE5FE0F_D8AF_403F_814A_417877465552__INCLUDED_)
