// RadarScan.cpp: implementation of the CRadarScan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RadarScan.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRadarScan::CRadarScan()
{
    m_pMap = NULL;

    // 扫描结果
    resultx = new int[144];
    resulty = new int[144];
    distance = new int[144];

    // 射线点阵表
    rayx = new int[144 * 240];
    rayy = new int[144 * 240];

    // 初始化
    for (int x2 = -72; x2 < 72; x2++)
        for (int y2 = 0; y2 < 240; y2++)
        {
            rayx[(x2 + 72) * 240 + y2] = sin(x2 * 2.5 / 57.4) * y2 + 320;
            rayy[(x2 + 72) * 240 + y2] = cos(x2 * 2.5 / 57.4) * y2 + 240;
        }

    // 射线连通域
    midline = new int[144];
    clockwise = new int[144];
    anticlockwise = new int[144];
    sumoflines = new int[144];
    m_nNumofSector = 0;

    // 中心点
    m_cenx = 320;
    m_ceny = 240;
}

CRadarScan::~CRadarScan()
{
    delete[] resultx;
    delete[] resulty;
    delete[] distance;
    delete[] rayx;
    delete[] rayy;

    delete[] midline;
    delete[] clockwise;
    delete[] anticlockwise;
    delete[] sumoflines;
}

void CRadarScan::Scan(UCHAR *inmap, int inWidth, int inHeight, UCHAR inVal, int inSpan)
{
    m_pMap = inmap;
    m_nWidth = inWidth;
    m_nHeight = inHeight;
    if (m_pMap == NULL)
    {
        return;
    }
    // 144条射线扫描
    for (int i = 0; i < 144; i += inSpan)
    {
        count = 0;
        for (int j = 20; j < 240; j++)
        {
            index = i * 240 + j;
            if (m_pMap[(rayy[index]) * m_nWidth + rayx[index]] == inVal)
            {
                count++;
                if (count > 3)
                {
                    resultx[i] = rayx[index];
                    resulty[i] = rayy[index];
                    distance[i] = j;
                    break;
                }
            }
            if (j == 239)
            {
                // 扫到射线末端还没跳出循环，说明没扫描到
                resultx[i] = -1;
                resulty[i] = -1;
                distance[i] = 240;
            }
            // 调试画点
            // m_pMap[(rayy[index])*m_nWidth+rayx[index]] = inVal+1;
            //////////////////////////////////////////////////////////////////////////
        }
    }

    // 第二阶段，扫描
}

BOOL CRadarScan::Line(UCHAR *inBuf, int inIndex, UCHAR red, UCHAR green, UCHAR blue)
{
    if (distance[inIndex] < 0)
    {
        return FALSE;
    }
    int loc;

    for (index = 0; index < distance[inIndex]; index++)
    {
        loc = rayy[inIndex * 240 + index] * m_nWidth * 3 + rayx[inIndex * 240 + index] * 3;
        inBuf[loc] = blue;
        inBuf[loc + 1] = green;
        inBuf[loc + 2] = red;
    }
    return TRUE;
}

void CRadarScan::GetPoint(int inIndex, int inDistance, CPoint *inpnt)
{
    inpnt->x = rayx[inIndex * 240 + inDistance];
    inpnt->y = rayy[inIndex * 240 + inDistance];
}

BOOL CRadarScan::Sector()
{
    int linesum = 0;
    int num = 0;
    m_nNumofSector = 0;

    int mintoscan = 0;
    int maxtoscan = 143;
    // 先扫描零号线附近
    if (distance[0] != 240)
    {
        // 先扫零号线右侧
        for (int i = 0; i < 144; i++)
        {
            if (distance[i] < 240)
            {
                linesum += i;
                num++;
                anticlockwise[m_nNumofSector] = i;
            }
            else
            {
                break;
            }
        }

        // 扫描零号线左侧
        clockwise[m_nNumofSector] = 143;
        for (i = 143; i > anticlockwise[m_nNumofSector]; i--)
        {
            if (distance[i] < 240)
            {
                linesum += i;
                num++;
                clockwise[m_nNumofSector] = i;
            }
            else
            {
                break;
            }
        }

        if (num > 0)
        {
            midline[m_nNumofSector] = anticlockwise[m_nNumofSector] - (144 - clockwise[m_nNumofSector]);
            if (midline[m_nNumofSector] > 0)
            {
                midline[m_nNumofSector] = midline[m_nNumofSector] / 2;
            }
            else
            {
                midline[m_nNumofSector] = 143 + (midline[m_nNumofSector] / 2);
            }
            sumoflines[m_nNumofSector] = num;
            m_nNumofSector++;

            // 更新剩下的扫描区域
            mintoscan = anticlockwise[0];
            maxtoscan = clockwise[0];
        }
    }

    // 零号线扫描完毕，开始扫描剩余部分
    bool flag; // 标记是否进入了一个连通域
    while (maxtoscan > mintoscan)
    {
        linesum = 0;
        num = 0;
        flag = false;
        for (int i = mintoscan; i < maxtoscan; i++)
        {
            if (distance[i] < 240)
            {
                if (flag == false)
                {
                    anticlockwise[m_nNumofSector] = i;
                    flag = true;
                }
                linesum += i;
                num++; // 对有效射线进行累计
            }
            else
            {
                if (flag == true)
                {
                    // 判断是否离开了当前连通域
                    if (distance[i] == 240)
                    {
                        clockwise[m_nNumofSector] = i - 1;
                        sumoflines[m_nNumofSector] = num;
                        midline[m_nNumofSector] = linesum / num;
                        m_nNumofSector++;
                        mintoscan = i;
                        break; // 跳出for循环，进行下一个while循环
                    }
                }
            }
        }
        if (i == maxtoscan)
        {
            break;
        }
    }

    // 冒泡法对结果进行排序
    int index; // 记录最大连通域的索引
    int tempsum, tempmidline, tempclock, tempanticlock;

    for (int i = 0; i < m_nNumofSector; i++)
    {
        linesum = 0;
        // 开始对剩余部分冒泡
        for (int j = i; j < m_nNumofSector; j++)
        {
            if (sumoflines[j] > linesum)
            {
                linesum = sumoflines[j];
                index = j;
            }
        }
        // 将找出的最大值与i对换
        if (index != i)
        {
            // 将i索引值拷贝出来
            tempsum = sumoflines[i];
            tempmidline = midline[i];
            tempclock = clockwise[i];
            tempanticlock = anticlockwise[i];

            // index->i
            sumoflines[i] = sumoflines[index];
            midline[i] = midline[index];
            clockwise[i] = clockwise[index];
            anticlockwise[i] = anticlockwise[index];

            // temp->index
            sumoflines[index] = tempsum;
            midline[index] = tempmidline;
            clockwise[index] = tempclock;
            anticlockwise[index] = tempanticlock;
        }
    }

    return TRUE;
}

int CRadarScan::GetNumofSector()
{
    return m_nNumofSector;
}

double CRadarScan::CalAngle(int inx, int iny)
{
    double dx = inx - m_cenx;
    double dy = iny - m_ceny;
    angle = atan(dx / dy) * 180 / 3.14;

    if (angle < 0)
    {
        if (dx > 0)
        {
            angle = -angle;
        }
        else
        {
            angle = 180 - angle;
        }
    }
    else
    {
        if (dx > 0)
        {
            angle = 180 - angle;
        }
        else
        {
            angle = 360 - angle;
        }
    }
    return angle;
}
