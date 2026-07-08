// VoyCmd.cpp: implementation of the CVoyCmd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoyCmd.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 自动查询超声传感器线程
UINT QueryUSonicThread(LPVOID pParam)
{
    CVoyCmd *pcmd = (CVoyCmd *)pParam;
    const UINT Time = pcmd->QueryUSonicTime;
    while (Time == pcmd->QueryUSonicTime && FALSE == pcmd->bToEndThreads)
    {
        pcmd->QueryUltrasonicSensor();
        Sleep(Time);
    }
    return 0L;
}

// 自动查询红外传感器线程
UINT QueryInfraRedThread(LPVOID pParam)
{
    CVoyCmd *pcmd = (CVoyCmd *)pParam;
    const UINT Time = pcmd->QueryInfraRedTime;
    while (Time == pcmd->QueryInfraRedTime && FALSE == pcmd->bToEndThreads)
    {
        pcmd->QueryInfrared();
        Sleep(Time);
    }
    return 0L;
}

// 自动查询罗盘信息线程
UINT QueryCompassThread(LPVOID pParam)
{
    CVoyCmd *pcmd = (CVoyCmd *)pParam;
    const UINT Time = pcmd->QueryCompassTime;
    while (Time == pcmd->QueryCompassTime && FALSE == pcmd->bToEndThreads)
    {
        pcmd->QueryCompass();
        Sleep(Time);
    }
    return 0L;
}

// 罗盘标定线程
UINT DemarcateForCmps(LPVOID pParam)
{
    CVoyCmd *pcmd = (CVoyCmd *)pParam;
    Sleep(500);
    pcmd->SetBothMotorsSpeed(-700, 700);
    Sleep(20000);
    pcmd->Brake(1);
    return 0L;
}

CVoyCmd::CVoyCmd()
{
    m_charUltrasonic = new UCHAR[ULTRASONICAMOUNT];
    EnableUSonic = new BOOL[ULTRASONICAMOUNT];
    ValUSonic = new DOUBLE[ULTRASONICAMOUNT];

    m_charInfrared = new UCHAR[INFRAREDCHAR];
    EnableInfrared = new BOOL[INFRAREDMOUNT];
    ValInfrared = new BOOL[INFRAREDMOUNT];

    for (int i = 0; i < ULTRASONICAMOUNT; i++)
    {
        ValUSonic[i] = 0;
    }

    for (i = 0; i < ULTRASONICAMOUNT; i++)
    {
        EnableUSonic[i] = TRUE;
        EnableInfrared[i] = TRUE;
    }

    for (i = 0; i < INFRAREDMOUNT; i++)
    {
        ValInfrared[i] = FALSE;
    }

    for (i = 0; i < 8; i++)
    {
        ValServMotor[i] = 90;
    }

    event = CreateEvent(NULL, TRUE, FALSE, NULL);

    nState = 0;
    m_iLspeed = m_iRspeed = 0;
    // m_fSpReduc = 14.6;
    m_fLSpdCoe = 1;
    m_fRSpdCoe = 1;
    m_fSpdCoe = 1;

    m_pRcvBuf = new UCHAR[MAX_BUF];
    m_pSendBuf = new UCHAR[MAX_BUF];
    m_nRcvIndex = 0;

    QueryUSonicTime = 0;
    QueryInfraRedTime = 0;
    QueryCompassTime = 0;

    bToEndThreads = FALSE;

    CmdShow_Send = NULL;
    CmdShow_Rec = NULL;

    USDataShow = NULL;
    IRDataShow = NULL;
    CmpsDataShow = NULL;

    pMotorDis = NULL;
    pIODis = NULL;
    pADDis = NULL;

    for (i = 0; i < 4; i++)
    {
        m_markIO[i] = 0x01 << i;
    }

    for (i = 0; i < CMD_NUM; i++)
    {
        m_arSigCmd[i].Length = 0;
        m_arSigCmd[i].bToBeSend = false;
    }

    m_nDataCount = 0;
    bNoReply = false;
    m_bDaemonThreadisWorking = false;

    for (i = 0; i < 6; i++)
    {
        m_nMotorToSend[i] = 0;
    }
}

CVoyCmd::~CVoyCmd()
{
    QueryUSonicTime = 0;
    QueryInfraRedTime = 0;

    bToEndThreads = TRUE;

    CloseHandle(event);

    delete[] m_charUltrasonic;
    delete[] EnableUSonic;
    delete[] ValUSonic;
    delete[] m_charInfrared;
    delete[] ValInfrared;
    delete[] EnableInfrared;
    m_charInfrared = m_charUltrasonic = NULL;

    delete[] m_pRcvBuf;
    delete[] m_pSendBuf;

    // 延时，给查询线程足够的时间退出
    if (QueryUSonicTime > QueryInfraRedTime)
    {
        Sleep(QueryUSonicTime + 10);
    }
    else
    {
        Sleep(QueryInfraRedTime + 10);
    }
    CoUninitialize();
}

// 将接受到的数据分割组合成完整指令
void CVoyCmd::m_ParseBuffer(const UCHAR buf)
{
    // search for frame start tag
    if (buf == (UCHAR)0xaa && m_cLast == (UCHAR)0x55 && !m_bFrameStart)
    {
        m_pRcvBuf[0] = 0x55;
        m_pRcvBuf[1] = 0xAA;
        m_nRcvIndex = 2;
        m_bFrameStart = true;
        return;
    }

    if (m_bFrameStart)
    {
        // put received data into buffer
        m_cLast = 0x00;
        m_pRcvBuf[m_nRcvIndex] = buf;
        if (m_nRcvIndex == 3)
        {
            m_nFrameLength = buf + 8;
        }
        m_nRcvIndex++;
        // receive one frame, invoke ParseFrame to parse
        if (m_nRcvIndex == m_nFrameLength)
        {
            m_ParseFrame(m_pRcvBuf, m_nFrameLength);
            m_ResetRcvBuf();
        }

        // receive buffer overflow
        if (m_nRcvIndex >= MAX_BUF)
        {
            m_ResetRcvBuf();
        }
    }
    else
        m_cLast = buf;
}

void CVoyCmd::m_ParseFrame(UCHAR *buf, int length)
{
    static CString str, strTemp;
    static int i;

    // 串口数据流量监控
    m_nDataCount++;
    if (bNoReply == true && length > 4 && buf[4] != 0x10) // 如果是MPU6050来的数据说明机器还没开机 继续等待
    {
        // 刚来新的返回数据，说明机器刚打开，重新初始化传感器查询周期
        InitSensor();
        bNoReply = false;
    }

    if (!m_ValidFrame(buf, length))
    {
        m_ResponseError();
        return;
    }

    bool ctrlcodevalid = true;

    switch (buf[4])
    {
    case 0x07: // AD和IO为一条（m_valAD[4]为电压值，10mV为单位）
        for (i = 0; i < 5; i++)
        {
            m_valAD[i] = m_WordFromChar(&(buf[7 + i * 2]));
        }
        // 显示在控件上
        if (pADDis != NULL)
        {
            str = "";
            for (i = 0; i < 5; i++)
            {
                strTemp.Format(" AD%d=%5d", i + 1, m_valAD[i]);
                str += strTemp;
            }
            pADDis->SetWindowText(str);
        }

        // IO (m_chIO&0x04为充电状态)
        m_chIO = buf[7 + 5 * 2];
        for (i = 0; i < 4; i++)
        {
            if ((m_chIO & m_markIO[i]) == 0)
            {
                m_valIO[i] = false;
            }
            else
            {
                m_valIO[i] = true;
            }
        }
        if (pIODis != NULL)
        {
            str = "";
            for (i = 0; i < 4; i++)
            {
                strTemp.Format(" IO%d=%d", i + 1, m_valIO[i]);
                str += strTemp;
            }
            pIODis->SetWindowText(str);
        }
        break;

    case 0x08: // 电机返回值
        for (i = 0; i < 3; i++)
        {
            nMotorPos[i] =
                (int)(double(m_IntFromChar(&(buf[7 + i * 6])))); //*(DIANJI);//m_IntFromChar(&(buf[7+i*6]));//
            wMotorCurrent[i] = m_WordFromChar(&(buf[7 + i * 6 + 4]));
        }
        // 显示在控件上
        if (pMotorDis != NULL)
        {
            str = "";
            for (i = 0; i < 3; i++)
            {
                strTemp.Format(" M%d(p=%.10d,c=%.5d)	", i + 1, nMotorPos[i], wMotorCurrent[i]);
                str += strTemp;
            }

            pMotorDis->SetWindowText(str);
        }
        break;
    case 0x10: // MPU6050data 数据
        if (m_nDataCount > 0)
            m_nDataCount--; // 6050数据 不属于开机判定函数头++所以要--
        for (i = 0; i < 40; i++)
        {
            Arduino_MPU6050Data[i] = buf[i + 5];
        }
        // 显示在控件上
        if (pIODis != NULL)
        {
            str = "";
            strTemp.Format(" MPU6050data read ");
            str += strTemp;
            pIODis->SetWindowText(str);
        }
        break;
    }

    SetEvent(event);

    m_Response(buf, length);
    // 	if (ctrlcodevalid)
    // 		m_Response(buf,length);
    // 	else
    // 		m_ResponseError();
}

void CVoyCmd::Parse(void *buf, int length)
{
    if (length == 0 || buf == NULL)
        return;

    for (int i = 0; i < length; i++)
        m_ParseBuffer(((UCHAR *)buf)[i]);
}

void CVoyCmd::m_ResetRcvBuf()
{
    memset(m_pRcvBuf, 0, MAX_BUF);
    m_nRcvIndex = 0;
    m_bFrameStart = false;
    m_cLast = 0x00;
    m_nFrameLength = 0;
}

void CVoyCmd::m_ResetSendBuf()
{
    memset(m_pSendBuf, 0, MAX_BUF);
    m_nSendlength = 0;
}

BOOL CVoyCmd::m_ValidFrame(UCHAR *buf, int length)
{
    // 	if ((buf[1] & 0x3f)!=length-4)
    // 		return false;

    int sum = 0x0000000;

    for (int i = 0; i < length - 1; i++)
        sum += buf[i];

    if (buf[length - 1] != (UCHAR)(sum & 0x000000ff))
        return false;

    return true;
}

void CVoyCmd::m_ResponseError()
{
    // 不合协议的指令
}

void CVoyCmd::m_Response(UCHAR *recbuf, int length)
{ // 解析合法指令回调
    // m_pPhy->bSending = FALSE;

    // 显示接收指令
    if (CmdShow_Rec != NULL)
    {
        static CString cmd, str;
        static CTime tm;
        static int i;
        tm = CTime::GetCurrentTime();
        cmd = tm.Format("%X -> ");
        for (i = 0; i < length; i++)
        {
            str.Format("%.2X ", m_pRcvBuf[i]);
            cmd += str;
        }

        if (CmdShow_Rec != NULL)
        {
            if (CmdShow_Rec->GetCount() > 1000)
            {
                CmdShow_Rec->ResetContent();
            }
        }
        if (CmdShow_Rec != NULL)
        {
            CmdShow_Rec->AddString(cmd);
        }
        if (CmdShow_Rec != NULL)
        {
            CmdShow_Rec->SetCurSel(CmdShow_Rec->GetCount() - 1);
        }
    }
}

UCHAR CVoyCmd::m_CalSum(int length)
{
    int temp = 0;
    for (int i = 0; i < length; i++)
        temp += m_pSendBuf[i];

    UCHAR ret;
    ret = (UCHAR)(temp & 0x000000ff);
    return ret;
}

DOUBLE CVoyCmd::m_CalDistance(UCHAR inUSChar)
{
    DOUBLE Distance;
    // Distance = (DOUBLE) inUSChar*0.02174;
    Distance = (DOUBLE)inUSChar * 0.0344;

    return Distance;
}

void CVoyCmd::m_GenerateSendBuffer(UCHAR addr, UCHAR status, UCHAR length, UCHAR ctrlcode, UCHAR *data)
{
    m_pSendBuf[0] = (UCHAR)0x55;
    m_pSendBuf[1] = (UCHAR)0xaa;
    m_pSendBuf[2] = addr;     // addr;
    m_pSendBuf[3] = length;   //((status << 6) & 0xc0) | (length & 0x3f);
    m_pSendBuf[4] = ctrlcode; // ctrlcode;
    m_pSendBuf[5] = 0x70;
    if (length > 0 && data != NULL)
    {
        memcpy(&m_pSendBuf[6], data, length);
    }
    m_pSendBuf[length + 6] = m_CalSum(length + 6);

    m_nSendlength = length + 7;

    // 55 AA 38 0A 08 70 1H 1L 2H 2L 3H 3L 4H 4L 5H 5L SUM	// 同时设置五电机速度
    // 55 AA 38 02 09 70 TH TL SUM				// 射门

    // 发送指令
    // if (m_pPhy != NULL )
    {
        // m_pPhy->Send(m_pSendBuf,m_nSendlength);
        Send(m_pSendBuf, m_nSendlength);

        // 显示发送指令
        if (CmdShow_Send != NULL)
        {
            CString cmd, str;
            CTime tm;
            tm = CTime::GetCurrentTime();
            cmd = tm.Format("%X: ");
            for (int i = 0; i < signed int(m_nSendlength); i++)
            {
                str.Format("%.2X ", m_pSendBuf[i]);
                cmd += str;
            }
            CmdShow_Send->AddString(cmd);
            CmdShow_Send->SetCurSel(CmdShow_Send->GetCount() - 1);
        }
    }
}

void CVoyCmd::Brake(UCHAR breakmode)
{
    // m_GenerateSendBuffer((UCHAR)0x01,0,1,(UCHAR)0x21,&breakmode);
    VectorMove(0, 0, 0);
    m_iLspeed = 0;
    m_iRspeed = 0;
}

void CVoyCmd::SetBothMotorsSpeed(int inleftspeed, int inrightspeed)
{
    if (inleftspeed == m_iLspeed && inrightspeed == m_iRspeed)
    {
        return;
    }

    m_iLspeed = inleftspeed;
    m_iRspeed = inrightspeed;

    WORD leftspeed = m_CalculateSpeed(int(m_iLspeed * m_fLSpdCoe));
    WORD rightspeed = m_CalculateSpeed(int(m_iRspeed * m_fRSpdCoe));

    UCHAR bothspeed[4];

    bothspeed[0] = (UCHAR)((leftspeed >> 8) & 0x00ff);
    bothspeed[1] = (UCHAR)(leftspeed & 0x00ff);

    bothspeed[2] = (UCHAR)((rightspeed >> 8) & 0x00ff);
    bothspeed[3] = (UCHAR)(rightspeed & 0x00ff);

    m_GenerateSendBuffer((UCHAR)0x01, 0, 4, (UCHAR)0x26, bothspeed);
}

void CVoyCmd::SetLMotorSpeed(int inleftspeed)
{
    if (inleftspeed == m_iLspeed)
    {
        return;
    }

    m_iLspeed = inleftspeed;

    WORD leftspeed = m_CalculateSpeed(int(inleftspeed * m_fLSpdCoe));

    UCHAR lspeed[2];

    lspeed[1] = (UCHAR)(leftspeed & 0x00ff);
    lspeed[0] = (UCHAR)((leftspeed >> 8) & 0x00ff);

    m_GenerateSendBuffer((UCHAR)0x01, 0, 2, (UCHAR)0x24, lspeed);
}

void CVoyCmd::SetRMotorSpeed(int inrightspeed)
{
    if (inrightspeed == m_iRspeed)
    {
        return;
    }
    m_iRspeed = inrightspeed;

    WORD rightspeed = m_CalculateSpeed(int(inrightspeed * m_fRSpdCoe));

    UCHAR rspeed[2];

    rspeed[1] = (UCHAR)(rightspeed & 0x00ff);
    rspeed[0] = (UCHAR)((rightspeed >> 8) & 0x00ff);

    m_GenerateSendBuffer((UCHAR)0x01, 0, 2, (UCHAR)0x25, rspeed);
}

void CVoyCmd::QueryInfrared()
{
    m_GenerateSendBuffer((UCHAR)0x02, 0, 0, (UCHAR)0x36, NULL);
}

void CVoyCmd::QueryUltrasonicSensor()
{
    UCHAR ultrasonicchar[3];

    for (int i = 0; i < 3; i++)
    {
        ultrasonicchar[i] = 0xff;
    }

    for (i = 0; i < 24; i++)
    {
        if (EnableUSonic[i] == FALSE)
        {
            ultrasonicchar[2 - i / 8] &= ~(0x01 << (i % 8));
        }
    }

    /*CString a;
    a.Format("%d,%d,%d",ultrasonicchar[0],ultrasonicchar[1],ultrasonicchar[2]);
    AfxMessageBox(a);*/

    m_GenerateSendBuffer((UCHAR)0x02, 0, 3, (UCHAR)0x30, ultrasonicchar);
}

WORD CVoyCmd::m_CalculateSpeed(int speed)
{
    // speed *= m_fSpdCoe;
    BOOL forward;
    if (speed >= 0)
    {
        forward = TRUE;
    }
    else
    {
        forward = FALSE;
        speed = -speed;
    }

    WORD ret = 0;
    speed &= 0x7fff;

    if (forward)
        speed &= 0x7fff;
    else
        speed |= 0x8000;

    ret = speed;
    return ret;
}

UINT CVoyCmd::GetState()
{
    return nState;
}

void CVoyCmd::AutoQueryUSonic(UINT TimeGap)
{
    if (TimeGap == QueryUSonicTime)
    {
        return;
    }
    QueryUSonicTime = TimeGap;

    if (QueryUSonicTime != 0)
    {
        UCHAR sw = 0x01;
        m_GenerateSendBuffer(2, 0, 1, 0x31, &sw);
        AfxBeginThread(QueryUSonicThread, (LPVOID)this, THREAD_PRIORITY_TIME_CRITICAL);
    }
    else
    { // 参数为0 ，停止超声硬件工作
        UCHAR sw = 0x00;
        m_GenerateSendBuffer(2, 0, 1, 0x31, &sw);
    }
}

void CVoyCmd::AutoQueryInfraRed(UINT TimeGap)
{
    if (TimeGap == QueryInfraRedTime)
    {
        return;
    }
    QueryInfraRedTime = TimeGap;
    if (QueryInfraRedTime != 0)
    {
        AfxBeginThread(QueryInfraRedThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
    }
}

void CVoyCmd::AutoQueryCompass(UINT TimeGap)
{
    if (TimeGap == QueryCompassTime)
    {
        return;
    }
    QueryCompassTime = TimeGap;
    if (QueryCompassTime != 0)
    {
        AfxBeginThread(QueryCompassThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
    }
}

void CVoyCmd::ShowSensor()
{
    CString strUS, strIR;
    CString temp;
    int i;

    if (USDataShow != NULL)
    {
        for (i = 0; i < ULTRASONICAMOUNT; i++)
        {
            if (EnableUSonic[i] == FALSE)
            {
                continue;
            }

            temp.Format("%2d号:%2.2f   ", i + 1, ValUSonic[i]);
            strUS += temp;
            /*if ((i+1)%3 == 0)
            {
                *inUSstr += "\r";
            }*/
        }
        USDataShow->SetWindowText(strUS);
    }

    if (IRDataShow != NULL)
    {
        for (i = 0; i < INFRAREDMOUNT; i++)
        {
            if (EnableInfrared[i] == FALSE)
            {
                continue;
            }

            temp.Format("%2d号:", i + 1);
            if (ValInfrared[i] == TRUE)
            {
                temp += "有  ";
            }
            else
            {
                temp += "无  ";
            }
            strIR += temp;
            /*if (i%2 == 1)
            {
                *inIRstr += "\r";
            }*/
        }
        IRDataShow->SetWindowText(strIR);
    }
}

void CVoyCmd::SpeedByGyro(int inSpeed)
{
    float fspdcoe;
    if (m_fLSpdCoe > m_fRSpdCoe)
    {
        fspdcoe = m_fRSpdCoe;
    }
    else
    {
        fspdcoe = m_fLSpdCoe;
    }

    WORD wspeed = m_CalculateSpeed(int(inSpeed * fspdcoe));
    UCHAR speed[4];
    speed[0] = 0x00;
    speed[1] = 0x00;
    speed[3] = (UCHAR)(wspeed & 0x00ff);
    speed[2] = (UCHAR)((wspeed >> 8) & 0x00ff);

    m_GenerateSendBuffer((UCHAR)0x01, 0, 4, 0x2C, speed);
}

void CVoyCmd::CircleByGyro(int inAngle, int inSpeed)
{
    if (inAngle < -360 || inAngle > 360)
    {
        inAngle = inAngle % 360;
    }

    float fspdcoe;
    if (m_fLSpdCoe > m_fRSpdCoe)
    {
        fspdcoe = m_fRSpdCoe;
    }
    else
    {
        fspdcoe = m_fLSpdCoe;
    }

    WORD wAngle, wSpeed;
    UCHAR toSend[4];

    wAngle = m_CalculateSpeed(inAngle * 39000 / 3445);
    toSend[1] = (UCHAR)wAngle & 0x00ff;
    toSend[0] = (UCHAR)(wAngle >> 8) & 0x00ff;

    wSpeed = m_CalculateSpeed(int(inSpeed * fspdcoe));
    toSend[3] = (UCHAR)(wSpeed & 0x00ff);
    toSend[2] = (UCHAR)((wSpeed >> 8) & 0x00ff);

    m_GenerateSendBuffer((UCHAR)0x01, 0, 4, 0x2D, toSend);
}

void CVoyCmd::QueryCompass()
{
    m_GenerateSendBuffer(0x03, 0, 0, 0x34, NULL);
}

void CVoyCmd::Kick(int st)
{
#ifdef RUN_WITH_EMU
    sendToServerData.kickTime = st; // 向服务器传递击球器开启时间
#endif
    // m_GenerateSendBuffer(0x04,0,0,0x2f,NULL);
    static UCHAR val[2];
    m_Split2Bytes(val, (short)st);

    // 55 AA 38 02 09 70 TH TL SUM				// 射门
    // m_GenerateSendBuffer(0x38,0x70,2,0x09,(UCHAR*)val);
    m_GenerateSigCmd(SIG_KICK_INDEX, 0x38, 0x02, 0x09, 0x70, (UCHAR *)val);
}

void CVoyCmd::Kick(int inPwm, int inTime)
{
    static UCHAR val[2];
    m_Split2Bytes(val, (short)inTime);
    //	55 AA 38 02 09 70 TH TL SUM		射门
    m_GenerateSendBuffer(0x38, 0x70, 2, 0x09, (UCHAR *)val);
}

void CVoyCmd::Demarcate()
{
    m_GenerateSendBuffer(0x01, 0, 0, 0x2f, NULL);
}

void CVoyCmd::DemarcateForCompass()
{
    m_GenerateSendBuffer(0x03, 0, 0, 0x33, NULL);
    AfxBeginThread(DemarcateForCmps, (LPVOID)this, THREAD_PRIORITY_NORMAL);
}

void CVoyCmd::SetLMotorPos(int inPos, int inSpeed)
{
    UCHAR buf[6];
    WORD speed;
    UINT Pos;
    if (inSpeed < 0)
    {
        speed = (WORD)(-inSpeed * m_fLSpdCoe);
    }
    else
    {
        speed = (WORD)(inSpeed * m_fLSpdCoe);
    }
    buf[0] = (UCHAR)(speed << 8);
    buf[1] = (UCHAR)speed;

    if (inPos < 0)
    { // 反向位置
        Pos = (UINT)((-inPos / 360) * m_fLSpdCoe * 4096);
        buf[2] = Pos << 24;
        buf[3] = Pos << 16;
        buf[4] = Pos << 8;
        buf[5] = Pos;
        buf[2] |= 0x80;
    }
    else
    { // 正向位置
        Pos = (UINT)((inPos / 360) * m_fLSpdCoe * 4096);
        buf[2] = Pos << 24;
        buf[3] = Pos << 16;
        buf[4] = Pos << 8;
        buf[5] = Pos;
        buf[2] &= 0x7f;
    }

    m_GenerateSendBuffer((UCHAR)0x01, 0, 6, (UCHAR)0x22, buf);
}

void CVoyCmd::SetRMotorPos(int inPos, int inSpeed)
{
    UCHAR buf[6];
    WORD speed;
    UINT Pos;
    if (inSpeed < 0)
    {
        speed = (WORD)(-inSpeed * m_fRSpdCoe);
    }
    else
    {
        speed = (WORD)(inSpeed * m_fRSpdCoe);
    }
    buf[0] = (UCHAR)(speed << 8);
    buf[1] = (UCHAR)speed;

    if (inPos < 0)
    { // 反向位置
        Pos = (UINT)((-inPos / 360) * m_fRSpdCoe * 4096);
        buf[2] = Pos << 24;
        buf[3] = Pos << 16;
        buf[4] = Pos << 8;
        buf[5] = Pos;
        buf[2] |= 0x80;
    }
    else
    { // 正向位置
        Pos = (UINT)((inPos / 360) * m_fRSpdCoe * 4096);
        buf[2] = Pos << 24;
        buf[3] = Pos << 16;
        buf[4] = Pos << 8;
        buf[5] = Pos;
        buf[2] &= 0x7f;
    }

    m_GenerateSendBuffer((UCHAR)0x01, 0, 6, (UCHAR)0x23, buf);
}

void CVoyCmd::SetServMotor(int inSpeed)
{
    UCHAR buf[9];
    if (inSpeed > 0xff)
    {
        buf[8] = 0xff;
    }
    else if (inSpeed < 0)
    {
        buf[8] = 0;
    }
    else
    {
        buf[8] = (BYTE)inSpeed;
    }

    for (int i = 0; i < 8; i++)
    {
        buf[i] = ValServMotor[i];
    }

    m_GenerateSendBuffer(0x06, 0, 10, 0x01, buf);
}

void CVoyCmd::SetFourMotorsSpeed(int No1speed, int No2speed, int No3speed, int No4speed)
{
    if (No1speed < 0)
        No1speed = -65534 - No1speed;
    if (No2speed < 0)
        No2speed = -65534 - No2speed;
    if (No3speed < 0)
        No3speed = -65534 - No3speed;
    if (No4speed < 0)
        No4speed = -65534 - No4speed;
    WORD m1speed = m_CalculateSpeed((int)(No1speed * m_fSpdCoe));
    WORD m2speed = m_CalculateSpeed((int)(No2speed * m_fSpdCoe));
    WORD m3speed = m_CalculateSpeed((int)(No3speed * m_fSpdCoe));
    WORD m4speed = m_CalculateSpeed((int)(No4speed * m_fSpdCoe));
    UCHAR bothspeed[8];
    bothspeed[0] = (UCHAR)((m1speed >> 8) & 0x00ff);
    bothspeed[1] = (UCHAR)(m1speed & 0x00ff);

    bothspeed[2] = (UCHAR)((m2speed >> 8) & 0x00ff);
    bothspeed[3] = (UCHAR)(m2speed & 0x00ff);

    bothspeed[4] = (UCHAR)((m3speed >> 8) & 0x00ff);
    bothspeed[5] = (UCHAR)(m3speed & 0x00ff);

    bothspeed[6] = (UCHAR)((m4speed >> 8) & 0x00ff);
    bothspeed[7] = (UCHAR)(m4speed & 0x00ff);

    m_GenerateSendBuffer((UCHAR)0x01, 0, 8, (UCHAR)0x21, bothspeed);
}

int CVoyCmd::CalAngle(int angle1, int angle2)
{
    int ret;
    ret = angle2 - angle1;
    if (ret > 180)
        ret = -360 + ret;
    if (ret < -180)
        ret = 360 + ret;
    return ret;
}

void CVoyCmd::VectorMove(double inAngle, int inLV, int inPSpeed)
{
    // int motor[3];	//后 左前 右前
    m_nMotorToSend[0] = -((int)(sin(((double)(CalAngle(int(-inAngle), +180))) * 3.14 / 180) * inLV) - inPSpeed);
    m_nMotorToSend[1] = -((int)(sin((double(CalAngle(int(-inAngle), 60))) * 3.14 / 180) * inLV) - inPSpeed);
    m_nMotorToSend[2] = -((int)(sin((double(CalAngle(int(-inAngle), -60))) * 3.14 / 180) * inLV) - inPSpeed);
    SendMotorValAgain();
}

void CVoyCmd::SetFiveMotorsSpeed(short *inSpeed)
{
    for (int i = 0; i < 5; i++)
    {
        m_nMotorToSend[i] = inSpeed[i];
    }
    SetSixMotorsSpeed(m_nMotorToSend);
    /* 2013年前的协议
    //55 AA 38 0A 08 70 1H 1L 2H 2L 3H 3L 4H 4L 5H 5L SUM	// 同时设置五电机速度
    unsigned char speedbuf[10];

    m_Split2Bytes(&speedbuf[0],inSpeed[0]);

    m_Split2Bytes(&speedbuf[2],inSpeed[1]);

    m_Split2Bytes(&speedbuf[4],inSpeed[2]);

    m_Split2Bytes(&speedbuf[6],inSpeed[3]);

    m_Split2Bytes(&speedbuf[8],inSpeed[4]);

    //m_GenerateSendBuffer(0x38,0x0a,10,0x08,speedbuf);
    m_GenerateSigCmd(SIG_MOTOR_INDEX,0x38,0x0a,0x08,0x70,speedbuf);
    */
}

void CVoyCmd::m_Split2Bytes(unsigned char *inTarg, short inSrc)
{
    if (inTarg == NULL)
    {
        return;
    }

    static unsigned short temp;
    memcpy(&temp, &inSrc, sizeof(short));
    inTarg[1] = (unsigned char)temp & 0x00ff;

    temp >>= 8;

    inTarg[0] = (unsigned char)temp & 0x00ff;
}

void CVoyCmd::m_Split2Bytes(unsigned char *inTarg, UINT inSrc)
{
    if (inTarg == NULL)
    {
        return;
    }

    inTarg[1] = (unsigned char)inSrc & 0x00ff;

    inSrc >>= 8;

    inTarg[0] = (unsigned char)inSrc & 0x00ff;
}

int CVoyCmd::m_IntFromChar(BYTE *inBuf)
{
    int mtemp;
    mtemp = 0;
    mtemp = *(inBuf);

    mtemp <<= 8;
    mtemp |= *(inBuf + 1);

    mtemp <<= 8;
    mtemp |= *(inBuf + 2);

    mtemp <<= 8;
    mtemp |= *(inBuf + 3);

    return mtemp;
}

short CVoyCmd::m_WordFromChar(BYTE *inBuf)
{
    static short wtemp;
    wtemp = 0;
    wtemp = *(inBuf);

    wtemp <<= 8;
    wtemp |= *(inBuf + 1);

    return wtemp;
}

void CVoyCmd::SetWatchDog(UINT inTime)
{
    static UCHAR wdTime[2];
    m_Split2Bytes(wdTime, inTime);

    m_GenerateSendBuffer(0x38, 0x00, 2, 0x04, wdTime);
}

void CVoyCmd::SynGetMotorPos(UINT TimeGap)
{
    UCHAR time[2];

    m_Split2Bytes(time, TimeGap);

    m_GenerateCmd(0x38, 2, 0x08, 0x60, time);
}

void CVoyCmd::SynGetAD(UINT TimeGap)
{
    UCHAR time[2];

    m_Split2Bytes(time, TimeGap);

    m_GenerateCmd(0x38, 2, 0x07, 0x60, time);
}

void CVoyCmd::SynGetIO(UINT TimeGap)
{
    UCHAR time[2];

    m_Split2Bytes(time, TimeGap);

    m_GenerateCmd(0x38, 2, 0x06, 0x60, time);
}

void CVoyCmd::m_GenerateCmd(UCHAR inID, UCHAR inLen, UCHAR inMode, UCHAR inMethod, UCHAR *data)
{
    m_pSendBuf[0] = (UCHAR)0x55;
    m_pSendBuf[1] = (UCHAR)0xaa;
    m_pSendBuf[2] = inID;
    m_pSendBuf[3] = inLen;
    m_pSendBuf[4] = inMode;
    m_pSendBuf[5] = inMethod;
    if (inLen > 0 && data != NULL)
    {
        memcpy(&m_pSendBuf[6], data, inLen);
    }
    m_pSendBuf[inLen + 6] = m_CalSum(inLen + 6);

    m_nSendlength = inLen + 7;

    // 55 AA 38 0A 08 70 1H 1L 2H 2L 3H 3L 4H 4L 5H 5L SUM	// 同时设置五电机速度
    // 55 AA 38 02 09 70 TH TL SUM				// 射门

    // 发送指令
    // if (m_pPhy != NULL )
    {
        // m_pPhy->Send(m_pSendBuf,m_nSendlength);
        Send(m_pSendBuf, m_nSendlength);

        // 显示发送指令
        if (CmdShow_Send != NULL)
        {
            CString cmd, str;
            CTime tm;
            tm = CTime::GetCurrentTime();
            cmd = tm.Format("%X: ");
            for (int i = 0; i < signed int(m_nSendlength); i++)
            {
                str.Format("%.2X ", m_pSendBuf[i]);
                cmd += str;
            }
            CmdShow_Send->AddString(cmd);
            CmdShow_Send->SetCurSel(CmdShow_Send->GetCount() - 1);
        }
    }

    // 	if (m_pBeh!= NULL)
    // 	{
    // 		m_pBeh->AfterSendCommand(m_pSendBuf,m_nSendlength,nState);
    // 	}
}

void CVoyCmd::m_GenerateSigCmd(UINT inIndex, UCHAR inID, UCHAR inLen, UCHAR inMode, UCHAR inMethod, UCHAR *data)
{
    m_pSendBuf[0] = (UCHAR)0x55;
    m_pSendBuf[1] = (UCHAR)0xaa;
    m_pSendBuf[2] = inID;
    m_pSendBuf[3] = inLen;
    m_pSendBuf[4] = inMode;
    m_pSendBuf[5] = inMethod;
    if (inLen > 0 && data != NULL)
    {
        memcpy(&m_pSendBuf[6], data, inLen);
    }
    m_pSendBuf[inLen + 6] = m_CalSum(inLen + 6);

    Send(m_pSendBuf, inLen + 7);
}

void CVoyCmd::InitSensor()
{
    SynGetIO(500);      // io查询周期
    SynGetAD(30);       // ad查询周期
    SynGetMotorPos(20); //(100);		//motor查询周期
    SetWatchDog(500);   // 停机保护周期

    if (m_bDaemonThreadisWorking == false)
    {
        AfxBeginThread(DaemonThread, this, THREAD_PRIORITY_LOWEST);
    }
}

UINT CVoyCmd::DaemonThread(LPVOID pParam)
{
    CVoyCmd *pcmd = (CVoyCmd *)pParam;
    pcmd->bNoReply = true;
    pcmd->m_bDaemonThreadisWorking = true;
    while (1)
    {
        if (pcmd->m_nDataCount > 0)
        {
            // 有数据返回，硬件工作正常
            //	if (pcmd->bNoReply == true)
            //	{
            pcmd->bNoReply = false;
            //	}
            pcmd->m_nDataCount = 0;
        }
        else
        {
            // 无数据返回
            pcmd->bNoReply = true;
        }
        Sleep(1500);
    }
    pcmd->m_bDaemonThreadisWorking = false;
    return 0L;
}

void CVoyCmd::Parse(BYTE inData)
{
    m_ParseBuffer(inData);
}

void CVoyCmd::SetSixMotorsSpeed(int *inSpeed)
{
    // 55 AA 38 0A 08 70 1H 1L 2H 2L 3H 3L 4H 4L 5H 5L SUM	// 同时设置六电机速度
    unsigned char speedbuf[12];
    short tmpShort = 0;

    tmpShort = inSpeed[0];
    m_Split2Bytes(&speedbuf[0], tmpShort);

    tmpShort = inSpeed[1];
    m_Split2Bytes(&speedbuf[2], tmpShort);

    tmpShort = inSpeed[2];
    m_Split2Bytes(&speedbuf[4], tmpShort);

    tmpShort = inSpeed[3];
    m_Split2Bytes(&speedbuf[6], tmpShort);

    tmpShort = inSpeed[4];
    m_Split2Bytes(&speedbuf[8], tmpShort);

    // 射门杆
    if (inSpeed[5] < 0)
    {
        inSpeed[5] = 0;
    }
    if (inSpeed[5] > 21000)
    {
        inSpeed[5] = 21000;
    }
    WORD tmpWord = inSpeed[5];
    speedbuf[10] = (tmpWord >> 8) & 0x00ff;
    speedbuf[11] = tmpWord & 0x00ff;

    m_GenerateSigCmd(SIG_MOTOR_INDEX, 0x38, 12, 0x08, 0x70, speedbuf);
}

void CVoyCmd::SetLiftPos(int inLiftPos)
{
    if (inLiftPos < 0)
    {
        inLiftPos = 0;
    }
    if (inLiftPos > 21000)
    {
        inLiftPos = 21000;
    }

    m_nMotorToSend[5] = inLiftPos;
}

void CVoyCmd::SendMotorValAgain()
{
    SetSixMotorsSpeed(m_nMotorToSend);
}
