// SpreadVis.cpp: implementation of the CSpreadVis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpreadVis.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpreadVis::CSpreadVis()
{
    m_pParam = NULL;
    m_pImageBuf = NULL;
    m_nCntrX = m_nCntrY = m_nWidth = m_nHeight = 0;
    flag = false;
    found = false;

    // 颜色初始化
    m_FocusClr.rgbtBlue = 0;
    m_FocusClr.rgbtGreen = 0xff;
    m_FocusClr.rgbtRed = 0xff;

    m_TargClr.rgbtRed = m_TargClr.rgbtGreen = m_TargClr.rgbtBlue = 0;
    m_BgClr.rgbtBlue = m_BgClr.rgbtGreen = m_BgClr.rgbtRed = 0xff;
}

CSpreadVis::~CSpreadVis()
{
}

// 引入图像数据位置
void CSpreadVis::ImageBuf(int Width, int Height, BYTE *Buf)
{
    m_nHeight = Height;
    m_nWidth = Width;
    m_pImageBuf = Buf;
}

void CSpreadVis::SetParam(YUVParam *inParam)
{
    m_pParam = inParam;
}

// 设置象素点颜色值
BOOL CSpreadVis::SetColor(int x, int y, RGBTRIPLE rgb)
{

    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    m_pImageBuf[y * m_nWidth * 3 + x * 3] = rgb.rgbtBlue;
    m_pImageBuf[y * m_nWidth * 3 + x * 3 + 1] = rgb.rgbtGreen;
    m_pImageBuf[y * m_nWidth * 3 + x * 3 + 2] = rgb.rgbtRed;

    return TRUE;
}

// 获取像素点颜色值
BOOL CSpreadVis::GetColor(int x, int y, RGBTRIPLE *prgb)
{
    if (x > m_nWidth - 1 || y > m_nHeight - 1 || x < 0 || y < 0)
        return FALSE;

    prgb->rgbtBlue = m_pImageBuf[y * m_nWidth * 3 + x * 3];
    prgb->rgbtGreen = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 1];
    prgb->rgbtRed = m_pImageBuf[y * m_nWidth * 3 + x * 3 + 2];

    return TRUE;
}

// 标记点绘制
void CSpreadVis::DrawFocus(long x, long y, int inRange, RGBTRIPLE *prgb)
{
    // 绘制标记
    for (int i = -inRange; i < inRange; i++)
    {
        SetColor(x + i, y, *prgb);
        SetColor(x, y + i, *prgb);
    }
}

// 算法函数主体
BOOL CSpreadVis::Process(UINT inMode)
{
    if (m_pImageBuf == NULL || m_pParam == NULL || m_nWidth == 0 || m_nHeight == 0)
    {
        return FALSE;
    }

    for (Range = 1; Range < m_nMaxRange; Range++)
    {
        // 扫描左边缘
        for (Step = m_nCntrY - Range; Step < m_nCntrY + Range; Step++)
        {
            // 判断扫描出界条件
            if (m_nCntrX - Range < 0)
            {
                break;
            }

            if (Step < 0 || Step > m_nHeight - 1)
            { // 上下出界，跳到下一次循环
                continue;
            }

            // 取象素进行判断
            GetColor(m_nCntrX - Range, Step, &m_rgb);
            found |= m_Treat(m_nCntrX - Range, Step, &m_rgb, inMode);
        }

        // 扫描右边缘
        for (Step = m_nCntrY - Range; Step < m_nCntrY + Range; Step++)
        {
            // 判断扫描出界条件
            if (m_nCntrX + Range > m_nWidth)
            {
                break;
            }

            if (Step < 0 || Step > m_nHeight)
            { // 上下出界，跳到下一次循环
                continue;
            }

            // 取象素进行判断
            GetColor(m_nCntrX + Range, Step, &m_rgb);
            found |= m_Treat(m_nCntrX + Range, Step, &m_rgb, inMode);
        }

        // 扫描下边缘
        for (Step = m_nCntrX - Range; Step < m_nCntrX + Range; Step++)
        {
            // 判断扫描出界条件
            if (m_nCntrY - Range < 0)
            {
                break;
            }

            if (Step < 0 || Step > m_nWidth - 1)
            { // 出界，跳到下一次循环
                continue;
            }

            // 取象素进行判断
            GetColor(Step, m_nCntrY - Range, &m_rgb);
            found |= m_Treat(Step, m_nCntrY - Range, &m_rgb, inMode);
        }

        // 扫描上边缘
        for (Step = m_nCntrX - Range; Step < m_nCntrX + Range + 1; Step++)
        {
            // 判断扫描出界条件
            if (m_nCntrY + Range > m_nHeight)
            {
                break;
            }

            if (Step < 0 || Step > m_nWidth - 1)
            { // 出界，跳到下一次循环
                continue;
            }

            // 取象素进行判断
            GetColor(Step, m_nCntrY + Range, &m_rgb);

            found |= m_Treat(Step, m_nCntrY + Range, &m_rgb, inMode);
        }

        if (inMode > 0)
        {
            // 绘画中心点坐标
            m_rgb.rgbtBlue = 0xff;
            m_rgb.rgbtGreen = 0;
            m_rgb.rgbtRed = 0;

            DrawFocus(m_nCntrX, m_nCntrY, 5, &m_rgb);

            if (found == true)
            {
                DrawFocus(m_pParam->Vx, m_pParam->Vy, 320, &m_FocusClr);
                found = false;
                return true;
            }
        }
    }

    // 恢复初始状态
    return false;
}

bool CSpreadVis::m_Treat(int x, int y, RGBTRIPLE *prgb, UINT inMode)
{
    // 根据使能变量决定是否进行颜色空间转换
    if (m_pParam->Uen == TRUE)
    {
        tU = -2 * prgb->rgbtRed - 3 * prgb->rgbtGreen + 5 * prgb->rgbtBlue;
        if (tU > m_pParam->Umin && tU < m_pParam->Umax)
        {
            m_pParam->Ux = x;
            m_pParam->Uy = y;
            flag = true;
        }
    }

    if (m_pParam->Ven == TRUE)
    {
        tV = 5 * prgb->rgbtRed - 5 * prgb->rgbtGreen - 1 * prgb->rgbtBlue;
        if (m_pParam->Vmin < tV && tV < m_pParam->Vmax)
        {
            m_pParam->Vx = x;
            m_pParam->Vy = y;
            flag = true;
        }
    }

    if (m_pParam->Yen == TRUE)
    {
        tY = 0;
        if (m_pParam->Ymin < tY && tY < m_pParam->Ymax)
        {
            m_pParam->Yx = x;
            m_pParam->Yy = y;
            flag = true;
        }
    }

    if (flag == true)
    {
        // DrawFocus(x,y,&m_FocusClr);
        flag = false;
        return true;
    }
    else
    {
        if (inMode == 2)
        {
            SetColor(x, y, m_BgClr);
        }
        return false;
    }
}

void CSpreadVis::SetCenter(int x, int y)
{

    m_nCntrX = x;
    m_nCntrY = y;

    // 计算生长幅度
    if (m_nWidth == 0 || m_nHeight == 0)
    {
        // AfxMessageBox("设置中点前请先导入图像尺寸！");
        return;
    }

    if (m_nWidth > m_nHeight)
    { // 宽度决定生长幅度
        if (x > m_nWidth / 2)
        { // 中心点靠右，生长幅度到左边缘
            m_nMaxRange = x;
        }
        else
        { // 中心点靠左，生长幅度到右边缘
            m_nMaxRange = m_nWidth - x;
        }
    }
    else
    { // 高度决定生长幅度
        if (y > m_nHeight / 2)
        { // 中心点靠上，生长幅度到下边缘
            m_nMaxRange = y;
        }
        else
        { // 中心点靠下，生长幅度到上边缘
            m_nMaxRange = m_nHeight - m_nCntrY;
        }
    }
}
