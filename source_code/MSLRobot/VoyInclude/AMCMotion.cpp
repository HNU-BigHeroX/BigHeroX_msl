// AMCMotion.cpp: implementation of the CAMCMotion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AMCMotion.h"
#include <math.h>
#include <IO.H>
#include <stdio.h>
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// #define AMC_MAX_BUF	64

// static unsigned char g_WriteBuf[AMC_MAX_BUF] = {0xA5,0x3f,0x02};
// //static unsigned char g_WriteBuf[AMC_MAX_BUF] = {0xA5,0x3F,0x02};
//
// static unsigned int g_accum, Gr1 = 0x0810;
//
// static bool mFrameStart = false;
// static int mIndex = 0;
// static int mDataLen = 0;
// static BYTE mLast = 0;
// static BYTE mRecvBuf[AMC_MAX_BUF] = {0};

// 文件记录
static CString strCurrentDataLog;
static CFile m_fileLog;
static CTime tm;
static CString strTmp;

static volatile bool bData = false;

UINT MultiAMC_Thread(LPVOID pParam)
{
    CAMCMotion *pAMC = (CAMCMotion *)pParam;

    // while (pAMC->IsRuning)
    while (1)
    {
        pAMC->MultiAMCAction();
        if (pAMC->m_bCurrentMeasured == true)
        {
            pAMC->DemandCurrentMeasured();
        }
        Sleep(5);
    }
    return 0L;
}

UINT ACCCtrl_Thread(LPVOID pParam)
{
    CAMCMotion *pAMC = (CAMCMotion *)pParam;

    // while (pAMC->IsRuning)
    while (1)
    {
        pAMC->VelocityAdj();
    }
    return 0L;
}

void CAMCMotion::CrunchCRC(char x)
{
    // Compute CRC using BitbyBit method
    int i, k;
    for (k = 0; k < 8; k++)
    {
        i = (x >> 7) & 1;
        if (g_accum & 0x8000)
        {
            g_accum = ((g_accum ^ Gr1) << 1) + (i ^ 1);
        }
        else
        {
            g_accum = (g_accum << 1) + i;
        }
        g_accum &= 0x0ffff;
        x <<= 1;
    }
}
unsigned short CAMCMotion::CalcFieldCRC(unsigned char *pDataArray, unsigned char numberOfchars)
{
    // Compute CRC using BitbyBit method
    int i, k;
    g_accum = 0;
    for (i = 0; i < numberOfchars; i++, pDataArray++)
    {
        CrunchCRC(*pDataArray);
    }
    CrunchCRC(0);
    CrunchCRC(0);

    return g_accum;
}

void CAMCMotion::AMCWrite(unsigned char Index, unsigned char Offset, unsigned char DataWords, unsigned char *pdata)
{
    unsigned short crc = 0;
    int i;
    g_WriteBuf[2] = 0x02;
    g_WriteBuf[3] = Index;
    g_WriteBuf[4] = Offset;
    g_WriteBuf[5] = DataWords;
    crc = CalcFieldCRC(g_WriteBuf, 6);
    g_WriteBuf[6] = crc >> 8;
    g_WriteBuf[7] = crc;
    for (i = 0, DataWords <<= 1; i < DataWords; i++)
    {
        g_WriteBuf[8 + i] = pdata[i];
    }
    crc = CalcFieldCRC(&g_WriteBuf[8], DataWords);
    g_WriteBuf[8 + i] = crc >> 8;
    g_WriteBuf[9 + i] = crc;
    // 	if(g_pDlg != NULL)
    // 		g_pDlg->m_SerialPort.SendData((char*)g_WriteBuf,10+i);
    Send(g_WriteBuf, 10 + i);

    m_bReplay = false;
}

void CAMCMotion::AMCSetVelocity(int speed)
{
    unsigned char data[4];
    //	speed = speed*2048/60*131072/20000;
    speed = speed * 4000 / 60 * 4096 / 625; // 先转化为cnt/s，再乘以系数4096/625
    data[0] = speed;
    speed >>= 8;
    data[1] = speed;
    speed >>= 8;
    data[2] = speed;
    speed >>= 8;
    data[3] = speed;
    AMCWrite(0x45, 0x00, 2, data);
}

void CAMCMotion::AMCGainWriteAccess()
{
    unsigned char data[2];
    data[0] = 0x0f;
    data[1] = 0;
    AMCWrite(0x07, 0x00, 1, data);
}

void CAMCMotion::AMCEnableBridge(void)
{
    unsigned char data[2];
    data[0] = 0;
    data[1] = 0;
    AMCWrite(0x01, 0x00, 1, data);
}

void CAMCMotion::AMCDisableBridge(void)
{
    unsigned char data[2];
    data[0] = 0x01;
    data[1] = 0;
    AMCWrite(0x01, 0x00, 1, data);
}

void CAMCMotion::SetSpeed(unsigned char inAddr, int inSpeed)
{
    SetAddress(inAddr);
    AMCSetVelocity(inSpeed);
}

CAMCMotion::CAMCMotion()
{
    // static unsigned char g_WriteBuf[AMC_MAX_BUF] = {0xA5,0x3f,0x02};
    g_WriteBuf[0] = 0xA5;
    g_WriteBuf[1] = 0x3f;
    g_WriteBuf[2] = 0x02;

    g_accum = 0x0810;
    Gr1 = 0x0810;

    mFrameStart = false;
    int mIndex = 0;
    int mDataLen = 0;
    BYTE mLast = 0;
    for (int i = 0; i < AMC_MAX_BUF; i++)
    {
        mRecvBuf[i] = 0;
    }

    pRecvList = NULL;
    pInfoList = NULL;

    for (i = 0; i < MOTOR_NUM; i++)
    {
        AMCAddr[i] = i + 1;
        AMCVelocity[i] = 0;
        m_bNewVelocity[i] = false;
        m_bGainWriteAccess[i] = false;
        m_bBridgeOn[i] = false;
        m_bBridgeOff[i] = false;
        TargetVelocity[i] = 0;
        m_bLogVelocity[i] = false;
    }
    VelocityStep = 400;
    VA_Snap = 100;
    m_curAddrIndex = 0;

    m_bCurrentMeasured = false;

    m_bReplay = true;

    m_hReplyEvent = CreateEvent(NULL, false, false, NULL);
    SetEvent(m_hReplyEvent);
}

CAMCMotion::~CAMCMotion()
{
    CloseHandle(m_hReplyEvent);
}

void CAMCMotion::SetAddress(BYTE inAddr)
{
    g_WriteBuf[1] = inAddr;
}

void CAMCMotion::Parse(BYTE inData)
{
    if (mFrameStart == false)
    {
        if (inData == 0xA5)
        {
            mFrameStart = true;
            mRecvBuf[0] = inData;
            mIndex = 1;
            mDataLen = 8;
        }
    }
    else
    {
        mRecvBuf[mIndex] = inData;
        if (mIndex == 7) // header接收完毕，解析数据长度
        {
            // 此处未做crc校验
            mDataLen += (mRecvBuf[5] * 2 + 2);

            if (mDataLen > AMC_MAX_BUF)
            {
                mDataLen = AMC_MAX_BUF;
            }

            if (bData == true)
            {
                bData = false;
            }
            else
            {
                SetEvent(m_hReplyEvent);
            }
        }

        mIndex++;
        // 		strTmp.Format("%d",mIndex);
        // 		mPrintf(strTmp,pRecvList);
        if (mIndex >= mDataLen)
        {
            mParseFrame();
            mFrameStart = false;
            mIndex = 0;
            m_bReplay = true;
            SetEvent(m_hReplyEvent);
        }

        if (mIndex >= AMC_MAX_BUF)
        {
            mFrameStart = false;
            mIndex = 0;
            return;
        }
    }
}

void CAMCMotion::mParseFrame()
{
    static CString str;
    static CString strData;
    static int i;
    static BYTE byTemp;
    ////////////////////////////////
    // 	if (pRecvList != NULL)
    // 	{
    // 		for (i=0;i<mDataLen;i++)
    // 		{
    // 			str.Format("%.2X ",mRecvBuf[i]);
    // 			strData += str;
    // 		}
    // 		mPrintf(strData,pRecvList);
    // 		strData = "";
    // 	}
    // 	return;
    ////////////////////////////////
    static WORD wtmp;
    static double current;
    // 	if (mRecvBuf[2] == 0x02 && mRecvBuf[5] == 0x01)	//返回数据包含一个字的数据
    // 	{
    // 		wtmp = 0;
    // 		wtmp |= mRecvBuf[9];
    // 		wtmp = wtmp << 8;
    // 		wtmp |= mRecvBuf[8];
    // 	}
    // 	current = wtmp;
    // 	current = current/(pow(2,13)/15);

    if (mRecvBuf[2] == 0x02 && mRecvBuf[5] == 0x01) // 返回数据包含一个字的数据
    {

        //////////////////////////////////////////////////////////////////////////
        // if (pRecvList != NULL)
        {
            for (i = 0; i < mDataLen; i++)
            {
                str.Format("%.2X ", mRecvBuf[i]);
                strData += str;
            }
            // mPrintf(strData,pRecvList);
            // strData = "";
        }
        //////////////////////////////////////////////////////////////////////////
        wtmp = 0;
        wtmp |= mRecvBuf[9];
        wtmp = wtmp << 8;
        wtmp |= mRecvBuf[8];
        current = wtmp;
        current = current / (pow(2, 13) / 15);

        //////////////////////////////////////////////////////////////////////////

        if (pRecvList != NULL)
        {
            str.Format("地址%.2x 换算电流%f 原始数据 %s ", AMCAddr[m_curAddrIndex], current, strData);
            mPrintf(str, pRecvList);
        }

        tm = CTime::GetCurrentTime();
        strTmp.Format("[Current]_%.2d:%.2d:%.2d Addr=0x%.2x  RawData= %s\r\n", tm.GetHour(), tm.GetMinute(),
                      tm.GetSecond(), m_curAddrIndex, strData);
        strCurrentDataLog += strTmp;

        strData = "";
        // Log_AddCurrentData(AMCAddr[m_curAddrIndex],current);
    }
}

void CAMCMotion::mPrintf(CString inStr, CListBox *inList)
{
    if (inList != NULL)
    {
        inList->AddString(inStr);
        inList->SetCurSel(inList->GetCount() - 1);
    }
}

bool CAMCMotion::RS485_ChangeToAddr(BYTE inNewAddr)
{
    if (inNewAddr < 1 || inNewAddr > 0x3f)
    {
        return false;
    }
    unsigned char data[2];
    data[0] = inNewAddr;
    data[1] = 0;
    AMCWrite(0x05, 0x02, 1, data);

    return true;
}

void CAMCMotion::AMCStore()
{
    unsigned char data[2];
    data[0] = 0x16;
    data[1] = 0x5b;
    AMCWrite(0x0a, 0x00, 1, data);
}

void CAMCMotion::SetMultiAddrs(BYTE *inarAddr, int inNum)
{
    if (inNum > MOTOR_NUM)
    {
        inNum = MOTOR_NUM;
    }

    memcpy(AMCAddr, inarAddr, inNum);
}

void CAMCMotion::SetMultiVelocity(int *inVel, int inNum)
{
    SetMultiTargetVelocity(inVel, inNum);
    // 	if (inNum >MOTOR_NUM)
    // 	{
    // 		inNum = MOTOR_NUM;
    // 	}
    //
    // 	for (int i=0;i<inNum;i++)
    // 	{
    // 		if (inVel[i] != AMCVelocity[i])
    // 		{
    // 			AMCVelocity[i] = inVel[i];
    // 			m_bNewVelocity[i] = true;
    // 		}
    // 	}
}

void CAMCMotion::MultiMode()
{
    // 开启多AMC驱动器更新线程
    AfxBeginThread(MultiAMC_Thread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
}

static int MultiAct_Timeout = 0;
void CAMCMotion::MultiAMCAction()
{
    // mPrintf("CAMCMotion::MultiAMCAction()",pInfoList);
    static int mi;
    DWORD dwRes;
    for (mi = 0; mi < MOTOR_NUM; mi++)
    {
        /*检查是否有获取写权限指令需要发送*/
        if (m_bGainWriteAccess[mi] == true)
        {
            while (m_bReplay == false) // 【485主从机制】上一条指令的回复未收到，则进行超时等待
            {
                MultiAct_Timeout++;
                Sleep(5);
                if (MultiAct_Timeout > 50) // 等待超时
                {
                    MultiAct_Timeout = 0;
                    break;
                }
            }
            // 			dwRes = WaitForSingleObject(m_hReplyEvent,INFINITE);
            //
            // 			if(dwRes == WAIT_OBJECT_0 /*|| dwRes == WAIT_TIMEOUT*/)
            // 			{
            SingleGWA(AMCAddr[mi]);
            m_bGainWriteAccess[mi] = false;
            //			}
            // 			else
            // 			{
            // 				return;
            // 			}

            //////////////////////////////////////////////////////////////////////////
            // 			CString str;
            // 			str.Format("MultiAMCAction GWA【%d】",AMCAddr[mi]);
            // 			mPrintf(str,pInfoList);
            //////////////////////////////////////////////////////////////////////////
        }

        /*检查是否有Bridge ON指令需要发送*/
        if (m_bBridgeOn[mi] == true)
        {
            while (m_bReplay == false) // 【485主从机制】上一条指令的回复未收到，则进行超时等待
            {
                MultiAct_Timeout++;
                Sleep(5);
                if (MultiAct_Timeout > 20) // 等待超时
                {
                    MultiAct_Timeout = 0;
                    break;
                }
            }

            // 			dwRes = WaitForSingleObject(m_hReplyEvent,INFINITE);
            //
            // 			if(dwRes == WAIT_OBJECT_0 /*|| dwRes == WAIT_TIMEOUT*/)
            // 			{

            SingleBridge(AMCAddr[mi], true);
            m_bBridgeOn[mi] = false;
            //			}
            // 			else
            // 			{
            // 				return;
            // 			}

            //////////////////////////////////////////////////////////////////////////
            // 			CString str;
            // 			str.Format("MultiAMCAction BridgeON【%d】",AMCAddr[mi]);
            // 			mPrintf(str,pInfoList);
            //////////////////////////////////////////////////////////////////////////
        }

        /*检查是否有Bridge OFF指令需要发送*/
        if (m_bBridgeOff[mi] == true)
        {
            while /*(false);*/ (m_bReplay == false) // 【485主从机制】上一条指令的回复未收到，则进行超时等待
            {
                MultiAct_Timeout++;
                Sleep(5);
                if (MultiAct_Timeout > 50) // 等待超时
                {
                    MultiAct_Timeout = 0;
                    break;
                }
            }

            // 			dwRes = WaitForSingleObject(m_hReplyEvent,INFINITE);
            //
            // 			if(dwRes == WAIT_OBJECT_0 /*|| dwRes == WAIT_TIMEOUT*/)
            // 			{

            SingleBridge(AMCAddr[mi], false);
            m_bBridgeOff[mi] = false;
            //			}

            //////////////////////////////////////////////////////////////////////////
            // 			CString str;
            // 			str.Format("MultiAMCAction BridgeOFF【%d】",AMCAddr[mi]);
            // 			mPrintf(str,pInfoList);
            //////////////////////////////////////////////////////////////////////////
        }

        /*检查是否有速度指令需要发送*/
        if (m_bNewVelocity[mi] == true)
        {
            while (m_bReplay == false) // 【485主从机制】上一条指令的回复未收到，则进行超时等待
            {
                MultiAct_Timeout++;
                Sleep(5);
                if (MultiAct_Timeout > 10) // 等待超时
                {
                    MultiAct_Timeout = 0;
                    break;
                }
            }

            // 			dwRes = WaitForSingleObject(m_hReplyEvent,INFINITE);
            //
            // 			if(dwRes == WAIT_OBJECT_0 /*|| dwRes == WAIT_TIMEOUT*/)
            // 			{

            SetSpeed(AMCAddr[mi], AMCVelocity[mi]);
            m_bNewVelocity[mi] = false;
            if (m_bLogVelocity[mi] == true)
            {
                Log_AddVData(AMCAddr[mi], AMCVelocity[mi]);
                m_bLogVelocity[mi] = false;
            }
            //			}
            //////////////////////////////////////////////////////////////////////////
            // 			CString str;
            // 			str.Format("MultiAMCAction 【%d】- %d",AMCAddr[mi],AMCVelocity[mi]);
            // 			mPrintf(str,pInfoList);
            //////////////////////////////////////////////////////////////////////////
        }
    }
}

void CAMCMotion::SingleGWA(unsigned char inAddr)
{
    SetAddress(inAddr);
    AMCGainWriteAccess();
}

void CAMCMotion::SingleBridge(unsigned char inAddr, bool inEnable)
{
    SetAddress(inAddr);
    if (inEnable == true)
    {
        AMCEnableBridge();
    }
    else
    {
        AMCDisableBridge();
    }
}

void CAMCMotion::SetGWA(int inIndex)
{
    if (inIndex < 0 || inIndex > MOTOR_NUM)
    {
        return;
    }
    m_bGainWriteAccess[inIndex] = true;
}

void CAMCMotion::SetBridge(int inIndex, bool inEN)
{
    if (inIndex < 0 || inIndex > MOTOR_NUM)
    {
        return;
    }

    if (inEN == true)
    {
        m_bBridgeOn[inIndex] = true;
    }
    else
    {
        m_bBridgeOff[inIndex] = true;
    }
}

void CAMCMotion::ACCCtrlMode()
{
    // 开启加速度控制线程
    AfxBeginThread(ACCCtrl_Thread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
}

void CAMCMotion::VelocityAdj()
{
    static int dV = 0;
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        if (TargetVelocity[i] != AMCVelocity[i])
        {
            // 将速度调节量控制在给定范围
            dV = abs(TargetVelocity[i] - AMCVelocity[i]);
            if (dV > VelocityStep)
            {
                dV = VelocityStep;
            }

            if (TargetVelocity[i] > AMCVelocity[i])
            {
                AMCVelocity[i] += dV;
            }
            else
            {
                AMCVelocity[i] -= dV;
            }
            m_bNewVelocity[i] = true;
        }
    }
    Sleep(VA_Snap);
}

void CAMCMotion::SetMultiTargetVelocity(int *inVel, int inNum)
{
    if (inNum > MOTOR_NUM)
    {
        inNum = MOTOR_NUM;
    }

    for (int i = 0; i < inNum; i++)
    {
        TargetVelocity[i] = inVel[i];
    }
}

void CAMCMotion::DemandCurrentMeasured()
{
    int loop = 2;
    DWORD dwRes;
    for (m_curAddrIndex = 0; m_curAddrIndex < MOTOR_NUM; m_curAddrIndex++)
    {
        // 		while(m_bReplay == false)
        // 		{
        // // 			loop --;
        // // 			if (loop < 0)
        // // 			{
        // // 			//	break;
        // // 			}
        //  			Sleep(5);
        // 		}
        dwRes = WaitForSingleObject(m_hReplyEvent, INFINITE);

        if (dwRes == WAIT_OBJECT_0 /*|| dwRes == WAIT_TIMEOUT*/)
        {
            SetAddress(AMCAddr[m_curAddrIndex]);
            AMCRead(0x10, 0x03, 0x01);
            bData = true;
        }
        else
        {
            return;
        }
    }
}

void CAMCMotion::AMCRead(unsigned char Index, unsigned char Offset, unsigned char DataWords)
{
    unsigned short crc = 0;
    int i;
    g_WriteBuf[2] = 0x01;
    g_WriteBuf[3] = Index;
    g_WriteBuf[4] = Offset;
    g_WriteBuf[5] = DataWords;
    crc = CalcFieldCRC(g_WriteBuf, 6);
    g_WriteBuf[6] = crc >> 8;
    g_WriteBuf[7] = crc;
    Send(g_WriteBuf, 8);

    m_bReplay = false;
}

void CAMCMotion::SaveCurrentLog()
{
    m_fileLog.Write(strCurrentDataLog.GetBuffer(strCurrentDataLog.GetLength()), strCurrentDataLog.GetLength());
    strCurrentDataLog.ReleaseBuffer();
    m_bCurrentMeasured = false;
    strCurrentDataLog = "";
    m_fileLog.Close();
}

void CAMCMotion::Log_CreatFile()
{
    // 【0】探测目录是否存在
    CString sLogPath = ".\\Log\\ ";
    int retCode = _access(sLogPath, 0);
    if (retCode != 0)
    {
        retCode = _mkdir(sLogPath);
    }
    if (retCode < 0)
    {
        AfxMessageBox("创建记录文件失败!");
        return;
    }

    CString m_strFile;
    tm = CTime::GetCurrentTime();
    m_strFile.Format("LOG_%.4d-%.2d-%.2d_(%.2d时%.2d分%.2d秒).txt", tm.GetYear(), tm.GetMonth(), tm.GetDay(),
                     tm.GetHour(), tm.GetMinute(), tm.GetSecond());

    m_strFile = ".\\Log\\" + m_strFile;
    m_fileLog.Open(m_strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);

    strCurrentDataLog = "";
    m_bCurrentMeasured = true;
}

void CAMCMotion::Log_AddCurrentData(BYTE inAddr, double inData)
{
    tm = CTime::GetCurrentTime();
    strTmp.Format("[Current]_%.2d:%.2d:%.2d Addr=0x%.2x  Current= %f amps\r\n", tm.GetHour(), tm.GetMinute(),
                  tm.GetSecond(), inAddr, inData);
    strCurrentDataLog += strTmp;
}

void CAMCMotion::Log_AddVData(BYTE inAddr, int inData)
{
    tm = CTime::GetCurrentTime();
    strTmp.Format("[Velocity]_%.2d:%.2d:%.2d Addr=0x%.2x  Velocity= %d\r\n", tm.GetHour(), tm.GetMinute(),
                  tm.GetSecond(), inAddr, inData);
    strCurrentDataLog += strTmp;
}

void CAMCMotion::Slippage_AddVData(BYTE inAddr, int inData)
{
}
