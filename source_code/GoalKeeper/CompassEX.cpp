// CompassEX.cpp: implementation of the CCompassEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoyVideo.h"
#include "CompassEX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompassEX::CCompassEX()
{
    pBeh = NULL;
    m_bFrmStart = false;
    m_chLast = 0;
    m_nRecIndex = 0;

    m_iXField = 0;
    m_iYField = 0;
    pAglOut = NULL;
}

CCompassEX::~CCompassEX()
{
}

void CCompassEX::ParseByte(BYTE *inChar)
{
    if (m_nRecIndex == 0)
    {
        if (*inChar == 0xaa)
        {
            m_RecBuf[m_nRecIndex] = *inChar;
            m_nRecIndex++;
        }
    }
    else
    {
        // 缓冲数据
        m_RecBuf[m_nRecIndex] = *inChar;
        m_nRecIndex++;
        if (m_nRecIndex >= CMDLEN)
        {
            // one frame rec complete
            m_nRecIndex = 0;
            m_ParseFrm();
        }
    }
}

void CCompassEX::m_ParseFrm()
{
    switch (m_RecBuf[4])
    {
    case 0xa0:
        m_printf("连续输出模式");
        return;

        break;
    case 0xa1:
        m_printf("单次输出模式");
        return;
        break;

    case 0xf5:
        m_printf("罗盘转动标定 开始");
        return;
        break;

    case 0xf8:
        m_printf("罗盘转动标定 结束");
        return;
        break;

    case 0xf7:
        m_printf("保存数据 成功");
        return;
        break;

    case 0xa5:
        m_printf("允许设置罗盘零点指向");
        return;
        break;

    case 0xe5:
        m_printf("磁北修正");
        return;
        break;

    case 0xe8:
        m_printf("清除磁北修正");
        return;
        break;

    case 0xee:
        m_printf("指令执行 失败...");
        return;
        break;

    default:
        break;
    }

    // xf
    m_wTemp = (WORD)m_RecBuf[5];
    m_wTemp <<= 8;
    m_wTemp &= 0xff00;
    m_wTemp |= (0x00ff & (WORD)m_RecBuf[6]);
    memcpy(&m_sTemp, &m_wTemp, 2);
    m_iXField = m_sTemp;

    // yf
    m_wTemp = (WORD)m_RecBuf[7];
    m_wTemp <<= 8;
    m_wTemp &= 0xff00;
    m_wTemp |= (0x00ff & (WORD)m_RecBuf[8]);
    memcpy(&m_sTemp, &m_wTemp, 2);
    m_iYField = m_sTemp;

    // angle
    m_nAngle = 0;
    m_nAngle = (WORD)m_RecBuf[9];
    m_nAngle <<= 8;
    m_nAngle &= 0xff00;
    m_nAngle |= (0x00ff & (WORD)m_RecBuf[10]);

    FLOAT m_fAngle = m_nAngle;
    m_fAngle /= 10;

    m_strinfo.Format("磁北指向：%.1f 磁强:X%d Y%d", m_fAngle, m_iXField, m_iYField);
    m_printf(m_strinfo);

    pBeh->AfterUpdateAttitude(m_fAngle, m_iXField, m_iYField, 0, 0);
}

void CCompassEX::m_printf(CString instr)
{
    pAglOut->SetWindowText(instr);
}
