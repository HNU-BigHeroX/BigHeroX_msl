// SerialCom.cpp: implementation of the CSerialCom class.
// For Vorager 2.0
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SerialCom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 串口接收线程
UINT ComMonitor(LPVOID pParam)
{
    OVERLAPPED m_os = {0};
    m_os.hEvent = NULL;
    CSerialCom *pCom = (CSerialCom *)pParam;
    DWORD dwMaskFlag;
    DWORD dwCommEvent;
    DWORD dwByteToRead;
    DWORD dwRes;
    BYTE rechar[1] = {0xff};
    bool bWaitingOnStatusHandle = false;

    dwMaskFlag = EV_RXCHAR | EV_CTS;
    if (!SetCommMask(pCom->m_hCom, dwMaskFlag))
    {
        return 0;
    }
    m_os.hEvent = CreateEvent(NULL, true, false, NULL);
    if (m_os.hEvent == NULL)
        return 0;

    while (true)
    {

        if (!WaitCommEvent(pCom->m_hCom, &dwCommEvent, &m_os))
        {
            if (GetLastError() == ERROR_IO_PENDING)
            {
                bWaitingOnStatusHandle = true;
            }
            else
                break;
        }
        else
        {
            bWaitingOnStatusHandle = false;
        }
        if (bWaitingOnStatusHandle)
        {
            dwRes = WaitForSingleObject(m_os.hEvent, INFINITE);
            GetCommMask(pCom->m_hCom, &dwCommEvent);
            if ((dwCommEvent & EV_RXCHAR) == EV_RXCHAR)
            {
                do
                {
                    COMSTAT comstat;
                    DWORD dwError = 0;

                    ClearCommError(pCom->m_hCom, &dwError, &comstat);
                    if (comstat.cbInQue == 0)
                        break;
                    ReadFile(pCom->m_hCom, rechar, 1, &dwByteToRead, &m_os);
                    if (pCom->m_pCmd != NULL)
                        pCom->m_pCmd->Parse(rechar, 1);
                } while (true);
            }
            if (dwRes == WAIT_OBJECT_0)
            {
                if (!GetOverlappedResult(pCom->m_hCom, &m_os, &dwByteToRead, false))
                {
                    do
                    {
                        COMSTAT comstat;
                        DWORD dwError = 0;

                        ClearCommError(pCom->m_hCom, &dwError, &comstat);
                        if (comstat.cbInQue == 0)
                            break;
                        ReadFile(pCom->m_hCom, rechar, 1, &dwByteToRead, &m_os);
                        if (dwByteToRead > 0)
                        {
                            if (pCom->m_pCmd != NULL)
                                pCom->m_pCmd->Parse(rechar, 1);
                        }
                        else
                        {
                            CloseHandle(m_os.hEvent);
                            return 0;
                        }

                    } while (true);
                }
            }
        }
    }
    CloseHandle(m_os.hEvent);
    return 0;
}

// 串口发送线程
UINT SendingThread(LPVOID pParam)
{
    CSerialCom *pCom = (CSerialCom *)pParam;
    POSITION pos;
    UINT cnt = 0; // 延时计数

    while (pCom->IsRuning)
    {
        pos = pCom->m_cmdlist.GetHeadPosition();
        if (pos != NULL)
        {
            CMDBUF *pCbuf = (CMDBUF *)pCom->m_cmdlist.GetNext(pos);

            // 如果发送回合未结束，则等待
            while (pCom->bSending == TRUE)
            {
                Sleep(10);
                /*cnt ++;
                if (cnt > 20)*/
                break;
            }
            cnt = 0;

            // 发送缓冲列表里的指令
            pCom->ComSend(pCbuf->pCmdBuf, pCbuf->nLen);

            // 释放缓冲列表里已发送指令占用的资源
            pCom->m_cmdlist.RemoveHead();
            delete[] pCbuf->pCmdBuf;
            delete pCbuf;
        }
        Sleep(5);
    }
    return 0L;
}

CSerialCom::CSerialCom()
{
    // 默认串口属性
    m_baudrate = CBR_19200;
    m_bytesize = 8;
    m_stopbits = ONESTOPBIT;
    m_parity = NOPARITY;

    bSending = FALSE;
}

CSerialCom::~CSerialCom()
{
    IsRuning = FALSE;

    // 清除缓冲列表里的数据
    POSITION pos = m_cmdlist.GetHeadPosition();
    while (pos != NULL)
    {
        m_pTempCmd = (CMDBUF *)m_cmdlist.GetNext(pos);
        delete[] m_pTempCmd->pCmdBuf;
        delete m_pTempCmd;
    }
    m_cmdlist.RemoveAll();

    Sleep(50);
}

BOOL CSerialCom::Create(int inCom)
{
    if (TRUE == IsRuning)
    {
        if (inCom == m_com)
        { // 重复打开同一串口
            return FALSE;
        }
        else
        { // 未关闭原来的串口
            CloseHandle(m_hCom);
        }
    }
    m_com = inCom;

    // 建立串口
    CString strCom;
    strCom.Format("COM%d", inCom); // 生成一个串口号字符串

    // 用串口号字符串打开串口并返回打开的串口句柄
    m_hCom = CreateFile(strCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (m_hCom == INVALID_HANDLE_VALUE) // 打开串口失败的处理
    {
        m_hCom = NULL;
        return false;
    }

    DCB dcb; // 设备配置对象，下面是对串口的一般配置

    COMMTIMEOUTS m_CommTimeouts;
    m_CommTimeouts.ReadIntervalTimeout = 1000;
    m_CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
    m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
    m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
    m_CommTimeouts.WriteTotalTimeoutConstant = 1000;
    if (!SetCommTimeouts(m_hCom, &m_CommTimeouts))
    {
        CloseHandle(m_hCom);
        m_hCom = NULL;
        return false;
    }

    GetCommState(m_hCom, &dcb); // 将配置对象设置到打开的串口
    dcb.BaudRate = m_baudrate;  // 设置串口波特率
    dcb.ByteSize = m_bytesize;  // 设置传输的字节数
    dcb.StopBits = m_stopbits;  // 设置停止位
    dcb.Parity = m_parity;      // 设置校验方式

    // 将配置对象设置到打开的串口，如果该失败，清除句柄关闭串口
    if (!SetCommState(m_hCom, &dcb))
    {
        CloseHandle(m_hCom);
        m_hCom = NULL;
        return false;
    }

    IsRuning = TRUE;

    // 开启串口接收线程
    AfxBeginThread(ComMonitor, (LPVOID)this, THREAD_PRIORITY_NORMAL);

    // 开启串口发送线程
    AfxBeginThread(SendingThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);

    return true;
}

void CSerialCom::Send(const void *pBuffer, const int iLength)
{
    m_pTempCmd = new CMDBUF;
    m_pTempCmd->pCmdBuf = new UCHAR[iLength];
    memcpy(m_pTempCmd->pCmdBuf, pBuffer, iLength);
    _ASSERTE(_CrtCheckMemory());
    m_pTempCmd->nLen = iLength;

    m_cmdlist.AddTail(m_pTempCmd);
}

void CSerialCom::ComSend(const void *pBuffer, const int iLength)
{
    if (m_hCom == NULL)
        return;

    bSending = TRUE;

    OVERLAPPED m_os = {0};
    DWORD dwByteWrite;

    if (!WriteFile(m_hCom, pBuffer, iLength, &dwByteWrite, &m_os))
        DWORD iErr = GetLastError();
    // Sleep(100);						//延时100ms

    DWORD dwNumberOfBytesTransferred;

    GetOverlappedResult(m_hCom, &m_os, &dwNumberOfBytesTransferred, TRUE);

    // bSending = FALSE;
}

void CSerialCom::Close()
{
    // 关闭串口
    if (m_hCom != NULL)
        CloseHandle(m_hCom);

    IsRuning = FALSE;
    m_hCom = NULL;
}

void CSerialCom::SetCmd(CVoyCmd *pCmd)
{
    m_pCmd = pCmd;
    if (m_pCmd != NULL)
    {
        m_pCmd->m_pPhy = this;
    }
}

void CSerialCom::SetBaudRate(int inBaud)
{
}
