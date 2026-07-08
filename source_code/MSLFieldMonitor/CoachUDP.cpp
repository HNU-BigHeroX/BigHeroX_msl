// CoachUDP.cpp: implementation of the CCoachUDP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CoachUDP.h"
#include "MSL_Structs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoachUDP::CCoachUDP()
{
    m_sendbuf = new unsigned char[UPUDPBUFLEN];
    m_ParseBuf = new unsigned char[UPUDPBUFLEN];
    m_nRecvIndex = 0;
    m_lastRecv = 0;
    m_bFrameStart = 0;
    m_nFrameLength = 15;

    // cnt
    for (int i = 0; i < 20; i++)
    {
        m_unknownCnt[i].ID = -1;
        m_unknownCnt[i].reccnt = 0;
    }
    m_nCntNum = 0;
    m_pCntList = NULL;
    m_curID = 0;

    for (i = 0; i < 4; i++)
    {
        m_arCurIP[i] = 0;
    }

    m_bFoundBall = false;

    m_sendbuf[0] = 0x55;
    m_sendbuf[1] = 0xaa;

    m_bEnIPFilter = false;
    unsigned char defIP[4] = {127, 0, 0, 1};
    for (i = 0; i < AG_NUM; i++)
    {
        memcpy(m_creditCnt[i].ip, defIP, 4);
        m_recvCnt[i] = 0;
        m_recvLast[i] = 0;
    }

    // 清空接收数据缓冲区
    for (i = 0; i < AG_NUM; i++)
    {
        memset(&m_recvAgent[i], 0, sizeof(uAgent));
        m_recvAgent[i].ID = i + 1;
    }

    m_bPassKick = false;
    nPassKickTime = 0;
    posTouchBall.x = 900;
    posTouchBall.y = 600;

    bHoldBall = false;
    m_nLastDribblingID = 0;
    pInfoList = NULL;
}

CCoachUDP::~CCoachUDP()
{
    delete[] m_sendbuf;
    delete[] m_ParseBuf;
}

void CCoachUDP::CallbackReceived(char *inBuf, int inLen, SOCKADDR *inpSock)
{
    // 获取来源ip,并与当前IP进行比对
    unsigned char *pch = (unsigned char *)inpSock->sa_data;
    if (pch[2] != m_arCurIP[0] || pch[3] != m_arCurIP[1] || pch[4] != m_arCurIP[2] || pch[5] != m_arCurIP[3])
    {
        WORD iPort = ntohs(*(WORD *)pch);
        strCurIP.Format("%d.%d.%d.%d", pch[2], pch[3], pch[4], pch[5]);
        m_arCurIP[0] = pch[2];
        m_arCurIP[1] = pch[3];
        m_arCurIP[2] = pch[4];
        m_arCurIP[3] = pch[5];
    }
    //////////////////////////////////////////////////////////////////////////
    /*CString info;
    info.Format("rec %d bytes from ", inLen);
    info += strCurIP;
    PrintToList(info);

    info.Format("[R]");
    CString strbyte;
    for (int j=0;j<inLen;j++)
    {
    strbyte.Format("%.2X ", (unsigned char)inBuf[j]);
    info+=strbyte;
    }*/
    //////////////////////////////////////////////////////////////////////////

    // 保留一下数据来源ip
    /*pch = (unsigned char*)inpSock->sa_data;
    memcpy(m_curIP,pch+2,4);

    //当启用IP过滤时，检查来源IP是否可信
    if (m_bEnIPFilter == true)
    {
        if (m_CheckCredit(m_curIP) == false)
        {
            return;
        }
    }*/

    // 逐个解析数据
    pbuf = (unsigned char *)inBuf;
    for (int i = 0; i < inLen; i++)
    {
        m_ParseByte(pbuf[i]);
    }
}

void CCoachUDP::m_ParseByte(unsigned char inChar)
{
    m_ParseBuf[m_nRecvIndex] = inChar;

    if (m_lastRecv == 0x55 && inChar == 0xAA && m_bFrameStart == 0)
    {
        m_bFrameStart = 1;
        m_ParseBuf[0] = m_lastRecv;
        m_ParseBuf[1] = inChar;
        m_nRecvIndex = 2;
        m_lastRecv = 0x00;
        return;
    }

    if (m_bFrameStart)
    {
        // 指令长度
        if (m_nRecvIndex == 2)
        {
            m_nFrameLength = inChar;
        }

        // put received data into buffer
        m_ParseBuf[m_nRecvIndex] = inChar;
        m_nRecvIndex++;

        // receive one frame, invoke ParseFrame to parse
        if (m_nRecvIndex == m_nFrameLength)
        {
            // m_DispRecvCmd();
            m_ParseFrame(m_ParseBuf, m_nFrameLength);
            m_bFrameStart = 0;
        }

        // receive buffer overflow
        if (m_nRecvIndex >= UPUDPBUFLEN)
        {
            // m_ResetRcvBuf();
            m_bFrameStart = 0;
        }
    }
    else
        m_lastRecv = inChar;
}

void CCoachUDP::m_ParseFrame(unsigned char *inBuf, int inLen)
{
    //////////////////////////////////////////////////////////////////////////
    // 校验和检查
    unsigned char chk_sum = m_calsum(inBuf, inLen - 1);
    unsigned char rec_sum = inBuf[inLen - 1];
    if (chk_sum != rec_sum)
    {
        printf("校验和错误！");
        return;
    }
    //////////////////////////////////////////////////////////////////////////
    bool bNewCnt = true;
    m_curID = inBuf[3];

    // 参数检测
    if (m_curID > 5 || m_curID < 1)
    {
        return;
    }

    // 数据来源检查
    // for (int i=0;i<m_nCntNum;i++)
    //{
    //	//检查是否是已经接收过的ip
    //	if (memcmp(m_unknownCnt[i].ip,m_curIP,4) == 0)
    //	{
    //		//IP一样但id不一样的，也作为新连接处理显示出来
    //		if (m_curID != m_unknownCnt[i].ID)
    //		{
    //			continue;
    //		}
    //		m_unknownCnt[i].reccnt++;
    //		bNewCnt = false;
    //		//////////////////////////////////////////////////////////////////////////
    //		//显示接收数据包数量
    //		CString strT;
    //		strT.Format("%d",m_unknownCnt[i].reccnt);
    //		m_pCntList->SetItemText(i,2,strT);
    //		//////////////////////////////////////////////////////////////////////////
    //		break;
    //	}
    // }

    // 新连接，增加统计项
    //	if (bNewCnt == true)
    //	{
    //		if (m_nCntNum >= CNTMAX)
    //		{
    //			return;
    //		}
    //		m_unknownCnt[m_nCntNum].ID = inBuf[3];
    //		memcpy(m_unknownCnt[m_nCntNum].ip,m_curIP,4);
    //		m_unknownCnt[m_nCntNum].reccnt = 1;
    //
    //		//////////////////////////////////////////////////////////////////////////
    //		//在列表控件里显示
    //		CString strT;
    //		strT.Format("%d.%d.%d.%d",m_unknownCnt[m_nCntNum].ip[0],m_unknownCnt[m_nCntNum].ip[1],m_unknownCnt[m_nCntNum].ip[2],m_unknownCnt[m_nCntNum].ip[3]);
    //		if (m_pCntList != NULL)
    //		{
    //			m_pCntList->InsertItem(m_nCntNum,strT);
    //			m_pCntList->SetItemText(m_nCntNum,0,strT);
    //			strT.Format("%d",m_unknownCnt[m_nCntNum].ID);
    //			m_pCntList->SetItemText(m_nCntNum,1,strT);
    //// 			strT.Format("%d",m_unknownCnt[m_nCntNum].reccnt);
    //// 			m_pCntList->SetItemText(m_nCntNum,2,strT);
    //		}
    //		//////////////////////////////////////////////////////////////////////////
    //		m_nCntNum++;	//连接的数量+1
    //	}

    //////////////////////////////////////////////////////////////////////////
    // 若对应id的Agent还未被注册，则注册该id
    // if (m_ToAgent[m_curID-1].bConnected == false)
    //{
    //	m_ToAgent[m_curID - 1].SetID(m_curID);
    //	//设置新队员IP和端口
    //	m_ToAgent[m_curID - 1].SetRemoteStr(strCurIP, (20090 + m_curID));
    //
    // }
    //////////////////////////////////////////////////////////////////////////
    if (m_recvAgent[m_curID - 1].online == false)
    {
        m_ToAgent[m_curID - 1].SetID(m_curID);
        // 设置新队员IP和端口
        m_ToAgent[m_curID - 1].SetRemoteStr(strCurIP, (20090 + m_curID));

        m_ToAgent[m_curID - 1].strRemoteIP = strCurIP;

        // m_nAgentNum++;
        // AChangeStatus(inID, CTRL_STOP, 0);
        m_recvAgent[m_curID - 1].online = true;
        // 给上线的机器发一条停止指令
        m_ToAgent[m_curID - 1].CtrlCmd(CTRL_STOP, 0, 0, 0, 0, 0);
    }
    //////////////////////////////////////////////////////////////////////////

    // 辨别接收到的信息类型
    switch (inBuf[4])
    {
    case MSL_INFO: // 队员信息
        // 检测该队员是否找到球
        if ((0x80 & inBuf[11]) > 0)
        {
            m_bFoundBall = true;
        }
        else
        {
            m_bFoundBall = false;
        }
        // 更新队员信息
        m_recvAgent[m_curID - 1].ID = m_curID;
        m_recvAgent[m_curID - 1].x = m_Piece2int(&inBuf[5]);
        m_recvAgent[m_curID - 1].y = m_Piece2int(&inBuf[7]);
        m_recvAgent[m_curID - 1].angle = m_Piece2int(&inBuf[9]);
        m_recvAgent[m_curID - 1].V = m_Piece2int(&inBuf[16]);
        m_recvAgent[m_curID - 1].V_Angle = m_Piece2int(&inBuf[18]);
        m_recvAgent[m_curID - 1].status = (0x7f & inBuf[11]);
        m_recvAgent[m_curID - 1].foundball = m_bFoundBall;
        m_recvCnt[m_curID - 1]++;

        if (m_recvCnt[m_curID - 1] < 0)
        {
            PrintInfo("异常：m_recvCnt[m_curID - 1] < 0");
        }

        // 更新球信息
        m_recvAgent[m_curID - 1].ball.x = m_Piece2int(&inBuf[12]);
        m_recvAgent[m_curID - 1].ball.y = m_Piece2int(&inBuf[14]);

        // 判断队员是否起脚
        m_recvAgent[m_curID - 1].bKick = inBuf[20];

        // 特殊处理：检测起脚队员是否传球队员
        if (m_recvAgent[m_curID - 1].bKick != 0 &&
            (m_recvAgent[m_curID - 1].status == CTRL_PASS || m_recvAgent[m_curID - 1].status == CTRL_PASS_MOVE))
        {
            // 记录传球队友起脚状态和时间
            m_bPassKick = true;
            nPassKickTime = GetTickCount() / 1000;

            posPasserKickTo_x = m_Piece2int(&inBuf[21]);
            posPasserKickTo_y = m_Piece2int(&inBuf[23]);
            PrintInfo("CoachUDP ：Pass Kick");
        }

        // 目标坐标(2014新增)
        m_recvAgent[m_curID - 1].dest_x = m_Piece2int(&inBuf[21]);
        m_recvAgent[m_curID - 1].dest_y = m_Piece2int(&inBuf[23]);

        // 障碍物信息
        for (int i = 0; i < 10; i++)
        {
            m_recvAgent[m_curID - 1].obstacle[i].valid = false; // 先清空标记

            int baseIndex = 25 + 5 * i;
            if (inBuf[baseIndex] > 0) //	(m_Piece2int(&inBuf[baseIndex+1]) > 0)
            {
                // 有效障碍物，添加信息
                m_recvAgent[m_curID - 1].obstacle[i].ID = m_curID;
                m_recvAgent[m_curID - 1].obstacle[i].x = m_Piece2int(&inBuf[baseIndex + 1]);
                m_recvAgent[m_curID - 1].obstacle[i].y = m_Piece2int(&inBuf[baseIndex + 3]);
                m_recvAgent[m_curID - 1].obstacle[i].size = inBuf[baseIndex];
                m_recvAgent[m_curID - 1].obstacle[i].valid = true;
            }
        }

        // 75~88，是机载电脑信息
        m_recvAgent[m_curID - 1].nComputer_ACPlug = inBuf[75];
        m_recvAgent[m_curID - 1].nComputer_BatteryFlag = inBuf[76];
        m_recvAgent[m_curID - 1].nComputer_BatteryPercent = inBuf[77];
        m_recvAgent[m_curID - 1].nComputer_BatteryLife = m_Piece2int(&inBuf[78]);
        m_recvAgent[m_curID - 1].nComputer_CPULoadPercent = inBuf[80];
        m_recvAgent[m_curID - 1].fComputer_CPUFrequency = m_Piece2float(&inBuf[81]);
        m_recvAgent[m_curID - 1].fComputer_SoftwareVer = m_Piece2float(&inBuf[85]);

        // 89~91，机器信息
        m_recvAgent[m_curID - 1].nRobot_Power = inBuf[89];
        m_recvAgent[m_curID - 1].nRobot_Charging = inBuf[90];

        // 串口帧率
        m_recvAgent[m_curID - 1].nSerialPort_FPS = inBuf[91];
        // 视觉帧率
        m_recvAgent[m_curID - 1].nVision_FPS = inBuf[92];
        // 组播帧率
        m_recvAgent[m_curID - 1].nMulticast_FPS = inBuf[93];
        // 姿态模块帧率
        m_recvAgent[m_curID - 1].nAttitude_FPS = inBuf[94];

        // 查看之前是否有人持球
        if (bHoldBall == false)
        {
            // 之前无人触球，查看此队员是否触球，如果是，则触球点坐标
            if (m_recvAgent[m_curID - 1].x == m_recvAgent[m_curID - 1].ball.x &&
                m_recvAgent[m_curID - 1].y == m_recvAgent[m_curID - 1].ball.y)
            {
                posTouchBall.x = m_recvAgent[m_curID - 1].x;
                posTouchBall.y = m_recvAgent[m_curID - 1].y;
                m_nLastDribblingID = m_curID;
                bHoldBall = true;
            }
        }
        else
        {
            // 之前有人触球，检测是否就是该队员（为了检测是否已经离开球，以复位bHoldBall）
            if (m_curID == m_nLastDribblingID)
            {
                // 之前记录的触球队员就是此队员，检测其是否已经离开球
                if (m_recvAgent[m_curID - 1].x != m_recvAgent[m_curID - 1].ball.x ||
                    m_recvAgent[m_curID - 1].y != m_recvAgent[m_curID - 1].ball.y)
                {
                    m_nLastDribblingID = 0;
                    bHoldBall = false;
                }
            }
        }

        /*
        //附加
        pfield->AddAgentDataOther(m_curID,&(inBuf[92]),84);*/
        break;
    }

    // 记录模式下，发送一份给记录程序
    if (m_ToAgent[m_curID - 1].bLogMode == true)
    {
        m_LogUdpSender[m_curID - 1].Send((char *)inBuf, inBuf[2]);
    }
}

int CCoachUDP::m_Piece2int(unsigned char *inTarg)
{
    short ret;
    ret = inTarg[0];
    ret <<= 8;
    ret &= 0xff00;
    ret |= ((int)inTarg[1] & 0x00ff);
    return ret;
}

void CCoachUDP::m_ag_Scan()
{
    // 获取所处的IP段
    /*char szHostName[128];
    struct hostent * pHost; //定义结构体 hostent
    if(gethostname(szHostName,128)==0)
    {
        pHost = gethostbyname(szHostName);
        for(int i=0;pHost!=NULL&&pHost->h_addr_list[i]!=NULL;i++)
        {
            LPCSTR IPAddress=inet_ntoa (*(struct in_addr *)pHost->h_addr_list[i]);
            CString ip(IPAddress);
            //获取ip前三个数字
            int cnt;
            int ret = ip.Find(".",0);
            while (1)
            {
                ret = ip.Find(".",ret+1);
                if (ret <0)
                {
                    break;
                }
                cnt = ret;
            }
            ip = ip.Left(cnt+1);
            ip += "255";
            //AfxMessageBox(ip);

            //在本地的每一个网卡上都扫描一遍agents
            for (int j=1;j<6;j++)
            {
                m_sendbuf[2] = j;	//ID
                m_sendbuf[3] = ROBST_PROBE;
                m_sendbuf[11] = m_calsum(m_sendbuf,11);	//sum

                Send(ip.GetBuffer(ip.GetLength()),20090+j,(char*)m_sendbuf,12);
            }
        }
    }*/
}

unsigned char CCoachUDP::m_calsum(unsigned char *inBuf, int inlen)
{
    unsigned char sum = 0;
    for (int i = 0; i < inlen; i++)
    {
        sum += inBuf[i];
    }
    return sum;
}

void CCoachUDP::UpdateCntList()
{
    if (m_pCntList != NULL)
    {
        for (int i = 0; i < m_nCntNum; i++)
        {
            CString strT;
            strT.Format("%d.%d.%d.%d", m_unknownCnt[i].ip[0], m_unknownCnt[i].ip[1], m_unknownCnt[i].ip[2],
                        m_unknownCnt[i].ip[3]);

            // m_pCntList->InsertItem(i,strT);
            m_pCntList->SetItemText(i, 0, strT);
            strT.Format("%d", m_unknownCnt[i].ID);
            m_pCntList->SetItemText(i, 1, strT);
            strT.Format("%d", m_unknownCnt[i].reccnt);
            m_pCntList->SetItemText(i, 2, strT);
        }
    }
}

bool CCoachUDP::m_CheckCredit(unsigned char *inIP)
{
    if (m_bEnIPFilter == false)
    {
        return true;
    }

    int i;
    for (i = 0; i < 5; i++)
    {
        if (memcmp(inIP, m_creditCnt[i].ip, 4) == 0)
        {
            // 在信任ip里存在该ip
            return true;
        }
    }
    return false;
}

void CCoachUDP::AllCredit()
{
    int n = m_nCntNum;
    if (n > 5)
    {
        n = 5;
    }

    for (int i = 0; i < n; i++)
    {
        m_creditCnt[i] = m_unknownCnt[i];
    }
}

void CCoachUDP::ClearCredit(int inIndex)
{
    if (inIndex >= 5 || inIndex < 0)
    {
        return;
    }

    m_creditCnt[inIndex].ip[0] = 0;
    m_creditCnt[inIndex].ip[1] = 0;
    m_creditCnt[inIndex].ip[2] = 0;
    m_creditCnt[inIndex].ip[3] = 0;
}

void CCoachUDP::AddCredit(int inUnknown, int inCredit)
{
    if (inUnknown >= m_nCntNum || inCredit >= 5 || inUnknown < 0 || inCredit < 0)
    {
        return;
    }
    m_creditCnt[inCredit] = m_unknownCnt[inUnknown];
}

void CCoachUDP::CopyDataSummaryTo(stDataSummary *inData)
{
    // 复制队员信息
    for (int i = 0; i < AG_NUM; i++)
    {
        memcpy(&(inData->agent[i]), &m_recvAgent[i], sizeof(uAgent));
    }
    inData->bPassKick = m_bPassKick;
    if (m_bPassKick == true)
    {
        inData->nCatchBeforKick_x = inData->posCatch.x;
        inData->nCatchBeforKick_y = inData->posCatch.y;
        inData->posPasserKickTo_x = posPasserKickTo_x;
        inData->posPasserKickTo_y = posPasserKickTo_y;
    }

    inData->nPassKickTime = nPassKickTime;
    m_bPassKick = false; // 复制完毕后复位（DataCenter复制完后会进行决策，所以不会漏掉这个信号）
    inData->posTouchBall.x = posTouchBall.x;
    inData->posTouchBall.y = posTouchBall.y;
    inData->bHoldBall = bHoldBall;
}

void CCoachUDP::AgentCmdMoveTo(int inID, int inTargetx, int inTargety, int inTargetAng, int inDist, bool inbField,
                               int inSpd)
{
    if (inID < 1 || inID > AG_NUM)
    {
        return;
    }
    m_ToAgent[inID - 1].MoveTo(inTargetx, inTargety, inTargetAng, inDist, inbField, inSpd);
    /*m_recvAgent[inID - 1].dest_x = inTargetx;
    m_recvAgent[inID - 1].dest_y = inTargety;*/
}

float CCoachUDP::m_Piece2float(unsigned char *inBuf)
{
    float fRetVal = 0.0f;

    memcpy(&fRetVal, inBuf, sizeof(float));

    return fRetVal;
}

void CCoachUDP::CheckAgentReply()
{
    for (int i = 0; i < AG_NUM; i++)
    {
        if (m_recvCnt[i] > 0)
        {
            m_recvLast[i] = m_recvCnt[i];
            // 数据包持续返回，则清零
            m_recvCnt[i] = 0;
        }
        else
        {
            // 数据包已经不返回了，则假定其已经离线
            m_recvAgent[i].online = false;
        }

        if (m_recvAgent[i].online == false || m_recvAgent[i].status == ROBST_ERR)
        {
            for (int j = 0; j < 10; j++)
            {
                m_recvAgent[i].obstacle[j].valid = false;
            }
        }
    }
}

void CCoachUDP::PrintInfo(CString inStr)
{
    if (pInfoList != NULL)
    {
        pInfoList->AddString(inStr);
        pInfoList->SetCurSel(pInfoList->GetCurSel() - 1);
    }
}
