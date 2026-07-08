// ConctDomain.cpp: implementation of the CConctDomain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConctDomain.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConctDomain::CConctDomain()
{
    _CurDomain.x = 0;
    _CurDomain.y = 0;
    _CurDomain.sum = 0;
    _Range = 4;

    m_Nextx = 0;
    m_Nexty = 0;
    result = NULL;
    map = NULL;
    m_nNumofResult = 0;

    toscan = NULL;

    // debug
    debug_cnt = 0;
}

CConctDomain::~CConctDomain()
{
    DomainNode *p;
    while (result != NULL)
    {
        p = result;
        result = p->next;
        delete p;
    }

    if (map != NULL)
    {
        delete[] map;
    }
}

BOOL CConctDomain::_Expend(int x, int y) // 扫描该点周围的空点，并在空点周围扫描非空点
{
    // 因为是经过蒙版的，不需要对边缘区域进行特殊处理

    // 为提高效率，采用分段判断的方式，目的是减少乘法计算
    // 试图扫描下行
    flag = false;   // 标记数据指针是否有效
    if (y - 1 > -1) //>=0
    {
        for (i = x - 1; i < x + 1; i++)
        {
            if (i > 0 && i < m_nWidth)
            {
                if (flag == false)
                {
                    flag = true;
                    temp = (y - 1) * m_nWidth + i;
                    m_cursrcp = srcmap + temp;
                    m_curmapp = map + temp;
                }
            }
            else
            {
                flag = false;
            }
            if (flag == true)
            {
                if (*m_cursrcp != _IDVal)
                {
                    // 周围扫到一个非目标颜色的，检测该像素周围有无可扩展点
                    if (m_checkpoint(i, y - 1) == true)
                    {
                        return TRUE;
                    }
                }
            }
        }
    }

    // 扫描中排
    flag = false;
    {
        for (i = x - 1; i < x + 1; i++)
        {
            if (i > 0 && i < m_nWidth)
            {
                if (flag == false)
                {
                    flag = true;
                    temp = y * m_nWidth + i;
                    m_cursrcp = srcmap + temp;
                    m_curmapp = map + temp;
                }
            }
            else
            {
                flag = false;
            }
            if (flag == true)
            {
                if (*m_cursrcp != _IDVal)
                {
                    // 周围扫到一个非目标颜色的，检测该像素周围有无可扩展点
                    if (m_checkpoint(i, y) == true)
                    {
                        return TRUE;
                    }
                }
            }
        }
    }
    // 扫描上排
    flag = false;
    if (y + 1 < m_nHeight)
    {
        for (i = x - 1; i < x + 1; i++)
        {
            if (i > 0 && i < m_nWidth)
            {
                if (flag == false)
                {
                    flag = true;
                    temp = (y + 1) * m_nWidth + i;
                    m_cursrcp = srcmap + temp;
                    m_curmapp = map + temp;
                }
            }
            else
            {
                flag = false;
            }
            if (flag == true)
            {
                if (*m_cursrcp != _IDVal)
                {
                    // 周围扫到一个非目标颜色的，检测该像素周围有无可扩展点
                    if (m_checkpoint(i, y + 1) == true)
                    {
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

BOOL CConctDomain::Process(int inMode)
{
    int x, y;
    //////////////////////////////////////////////////////////////////////////
    // 第一阶段，找出色域边缘点

    UCHAR *lCurmapp = map;
    UCHAR *lCursrcp = srcmap;
    // 最底一行像素先特殊处理
    for (x = 0; x < m_nWidth; x++)
    {
        if (*lCursrcp == _IDVal)
        {
            *lCurmapp = _IDVal;
        }
        lCursrcp++;
        lCurmapp++;
    }

    // 扫描第二行到倒数第二行
    for (y = 1; y < m_nHeight - 1; y++)
    {
        temp = y * m_nWidth;
        lCurmapp = map + temp;
        lCursrcp = srcmap + temp;
        for (x = 0; x < m_nWidth; x++)
        {
            if (*lCursrcp == _IDVal)
            {
                // 查询此点是否边缘
                if (x == 0 || x == m_nWidth - 1)
                {
                    *lCurmapp = _IDVal;
                }
                else
                {
                    // （周围是否有非空点）
                    // 先扫左
                    UCHAR *tempp = lCursrcp - 1;
                    if (*tempp != _IDVal)
                    {
                        *lCurmapp = _IDVal;
                    }
                    else
                    {
                        // 右
                        tempp++;
                        tempp++;
                        if (*tempp != _IDVal)
                        {
                            *lCurmapp = _IDVal;
                        }
                    }
                    // 上三个点
                    if (*lCurmapp != _IDVal)
                    {
                        tempp = lCursrcp + m_nWidth - 1;
                        if (*tempp != _IDVal)
                        {
                            *lCurmapp = _IDVal;
                        }
                        else
                        {
                            tempp++;
                            if (*tempp != _IDVal)
                            {
                                *lCurmapp = _IDVal;
                            }
                            else
                            {
                                tempp++;
                                if (*tempp != _IDVal)
                                {
                                    *lCurmapp = _IDVal;
                                }
                            }
                        }
                    }

                    // 下三个点
                    if (*lCurmapp != _IDVal)
                    {
                        tempp = lCursrcp - m_nWidth - 1;
                        if (*tempp != _IDVal)
                        {
                            *lCurmapp = _IDVal;
                        }
                        else
                        {
                            tempp++;
                            if (*tempp != _IDVal)
                            {
                                *lCurmapp = _IDVal;
                            }
                            else
                            {
                                tempp++;
                                if (*tempp != _IDVal)
                                {
                                    *lCurmapp = _IDVal;
                                }
                            }
                        }
                    }
                }
            }
            lCursrcp++;
            lCurmapp++;
        }
    }

    // 扫描上面一行(指针已经移到那，不用再对指针进行处理)
    for (x = 0; x < m_nWidth; x++)
    {
        if (*lCursrcp == _IDVal)
        {
            *lCurmapp = _IDVal;
        }
        lCursrcp++;
        lCurmapp++;
    }
    //////////////////////////////////////////////////////////////////////////
    // 第二阶段，在边缘点中找出连续点
    // memcpy(map,srcmap,m_nWidth*m_nHeight);
    lCurmapp = map;
    DomainNode *tp = result;
    m_nNumofResult = 0;
    bool useup = false;   // 判断result链表是否耗尽
    bool found = false;   // 判断一轮扩散是否发现连续点
    DomainNode *tmpnode;  // 为操作toscan链表生成一个临时指针
    DomainNode *scannode; // 当前被扫描的节点
    UCHAR *scanp;         // 被扩张点在map的位置
    UCHAR *expp;          // scanp扩张时使用的位置指针

    for (y = 0; y < m_nHeight; y++)
    {
        for (x = 0; x < m_nWidth; x++)
        {
            if (*lCurmapp == _IDVal) // 扫描到一个轮廓点，由此展开搜索
            {
                // 往待扫描链表插入一个节点
                toscan = new DomainNode();
                toscan->x = x;
                toscan->y = y;
                toscan->next = NULL;
                // 为该连通域增加一个结果
                if (useup == true || tp == NULL)
                {
                    tp = new DomainNode();
                    tp->next = result;
                    result = tp;
                }
                else
                {
                    tp = tp->next;
                }
                m_nNumofResult++;
                // 新的结果节点申请好了，初始化一下，之后tp作为当前结果节点进行数据统计
                tp->left = tp->right = toscan->x;
                tp->bottom = tp->top = toscan->y;
                tp->x = tp->y = 0;
                tp->sum = 0;

                // 开始扩散
                while (toscan != NULL)
                {
                    // 从toscan链表提出一个点进行扩散
                    scannode = toscan;
                    toscan = toscan->next;
                    // 将该节点信息加入连通域统计
                    tp->x += scannode->x;
                    tp->y += scannode->y;
                    tp->sum++;

                    // 判断此点对结果范围尺寸的贡献
                    if (scannode->x < tp->left)
                    {
                        tp->left = scannode->x;
                    }
                    if (scannode->x > tp->right)
                    {
                        tp->right = scannode->x;
                    }
                    if (scannode->y < tp->bottom)
                    {
                        tp->bottom = scannode->y;
                    }
                    if (scannode->y > tp->top)
                    {
                        tp->top = scannode->y;
                    }

                    // 只对非边界的像素进行统计
                    if ((scannode->x > 0) && (scannode->x < m_nWidth - 1) && (scannode->y > 0) &&
                        (scannode->y < m_nHeight - 1))
                    {
                        // 计算扫描点对应的map位置指针
                        scanp = map + ((scannode->y) * m_nWidth + scannode->x);

                        // 扫描上侧三个点
                        expp = scanp + m_nWidth - 1;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x - 1;
                            tmpnode->y = scannode->y + 1;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }
                        expp++;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x;
                            tmpnode->y = scannode->y + 1;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }
                        expp++;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x + 1;
                            tmpnode->y = scannode->y + 1;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }

                        // 中间两侧的点
                        expp = scanp - 1;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x - 1;
                            tmpnode->y = scannode->y;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }
                        expp += 2;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x + 1;
                            tmpnode->y = scannode->y;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }

                        // 下侧三个点
                        expp = scanp - m_nWidth - 1;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x - 1;
                            tmpnode->y = scannode->y - 1;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }
                        expp++;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x;
                            tmpnode->y = scannode->y - 1;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }
                        expp++;
                        if (*expp == _IDVal)
                        {
                            // 将扩散点添加到toscan里
                            tmpnode = new DomainNode();
                            tmpnode->x = scannode->x + 1;
                            tmpnode->y = scannode->y - 1;
                            tmpnode->next = toscan;
                            toscan = tmpnode;
                            // 抹除当前标记
                            *expp = 0;
                        }
                    } // end if ((scannode->x > 0) && (scannode->x < m_nWidth-1) ……

                    // 扩展完这个节点，删除
                    delete scannode;
                } // end while (toscan != NULL)

                // 一个连通域扩散结束，开始对结果数据进行统计
                tp->x = tp->x / tp->sum;
                tp->y = tp->y / tp->sum;

                if (tp->next == NULL)
                {
                    useup = true;
                }

            } // end (*lCurmapp == _IDVal)	//扫描到一个轮廓点，由此展开搜索
            lCurmapp++;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    return TRUE;
}

int CConctDomain::GetCount()
{
    return m_nNumofResult;
}

void CConctDomain::Mark(BYTE *rgbBuf)
{
    /*RGBTRIPLE clr;
    clr.rgbtRed = color.rgbtBlue;
    clr.rgbtGreen = color.rgbtGreen;
    clr.rgbtBlue = color.rgbtRed;

    //int s = 1000000;
    //int t = 0;

    DomainNode node;
    POSITION pos;
    pos = _result.GetHeadPosition();
    while (pos != NULL)
    {
        node = _result.GetNext(pos);
        DrawFocus(rgbBuf,node.x,node.y,&clr);
    }*/
}

void CConctDomain::SetRange(int inRge)
{
    _Range = inRge;
}

DomainNode CConctDomain::GetDomainAt(int index)
{
    DomainNode ret;
    DomainNode *p = result;
    int sum = 0;
    for (int i = 0; i < m_nNumofResult; i++)
    {
        if (p->sum > sum)
        {
            sum = p->sum;
            ret = *p;
        }
        p = p->next;
    }

    return ret;
}

DomainNode CConctDomain::GetDomainNear(int x, int y)
{
    DomainNode ret;
    DomainNode *p = result;
    int dist_long = 0;
    int dist;
    for (int i = 0; i < m_nNumofResult; i++)
    {
        dist = int(sqrt((x - m_nWidth / 2) ^ 2 + (y - m_nHeight / 2) ^ 2));
        if (dist > dist_long)
        {
            dist_long = dist;
            ret = *p;
        }
        p = p->next;
    }

    return ret;
}

bool CConctDomain::RadScan(int orgx, int orgy, int drcx, int drcy, CPoint *inpnt)
{
    /*int m_rad = 5;
    int cnt = 0;
    int dx = abs(orgx - drcx);
    int dy = abs(orgy - drcy);
    if (dx == 0 && dy == 0)
    {
        return false;
    }

    if (dx > dy)
    {
        float slope = (float)(drcy - orgy)/(drcx - orgx);
        if (drcx < orgx)
        {
            //射线向左扫描
            int j;
            for (int i=orgx;i>0;i--)
            {
                j = orgy +(i-orgx)*slope;
                if (GetPoint(i,j,map) == _IDVal)
                {
                    if (inpnt !=NULL && cnt == 0)
                    {
                        inpnt->x = i;
                        inpnt->y = j;
                    }
                    cnt ++;
                    //沿射线方向超过m_rad个像素才认定是角旗的一部分
                    if (cnt > m_rad)
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            //射线向右扫描
            int j;
            for (int i=orgx;i<=320;i++)
            {
                j = orgy +(i-orgx)*slope;
                if (GetPoint(i,j,map) == _IDVal)
                {
                    if (inpnt !=NULL && cnt == 0)
                    {
                        inpnt->x = i;
                        inpnt->y = j;
                    }
                    cnt ++;
                    //沿射线方向超过m_rad个像素才认定是角旗的一部分
                    if (cnt > m_rad)
                    {
                        return true;
                    }
                }
            }
        }
    }
    else
    {
        float slope = (float)(drcx - orgx)/(drcy - orgy);
        if (drcy < orgy)
        {
            //射线向下扫描
            int i;
            for (int j=orgy;j>0;j--)
            {
                i = orgx + (j-orgy)*slope;
                if (GetPoint(i,j,map) == _IDVal)
                {
                    if (inpnt !=NULL && cnt == 0)
                    {
                        inpnt->x = i;
                        inpnt->y = j;
                    }
                    cnt ++;
                    //沿射线方向超过m_rad个像素才认定是角旗的一部分
                    if (cnt > m_rad)
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            //射线向上扫描
            int i;
            for (int j=orgy;j<=240;j++)
            {
                i = orgx + (j-orgy)*slope;
                if (GetPoint(i,j,map) == _IDVal)
                {
                    if (inpnt !=NULL && cnt == 0)
                    {
                        inpnt->x = i;
                        inpnt->y = j;
                    }
                    cnt ++;
                    //沿射线方向超过m_rad个像素才认定是角旗的一部分
                    if (cnt > m_rad)
                    {
                        return true;
                    }
                }
            }
        }
    }*/
    return false;
}

void CConctDomain::InitMap(UCHAR *inMap, int inWidth, int inHeight, UCHAR IDVal)
{
    // 检查参数合法性
    if (inMap == NULL || inWidth < 0 || inHeight < 0)
    {
        return;
    }

    srcmap = inMap;

    if (inWidth != m_nWidth || inHeight != m_nHeight)
    {
        delete map;
        map = new UCHAR[inWidth * inHeight];
        m_nWidth = inWidth;
        m_nHeight = inHeight;
    }

    // memcpy(map,srcmap,m_nWidth*m_nHeight);
    // 清空标记区
    memset(map, 0, m_nWidth * m_nHeight);

    _IDVal = IDVal;
}

bool CConctDomain::m_checkpoint(int inx, int iny) // 扫描在map中没有被标记过的有效点
{
    m_Nextx = m_Nexty = -1;
    // 先扫描下行
    bool ch_flag = false;
    if (iny - 1 > -1) //>=0
    {
        for (ch_i = inx - 1; ch_i < inx + 1; ch_i++)
        {
            if (ch_i >= 0 && ch_i < m_nWidth)
            {
                if (ch_flag == false)
                {
                    ch_temp = (iny - 1) * m_nWidth + i;
                    ch_cursrcp = srcmap + ch_temp;
                    ch_curmapp = map + ch_temp;
                    ch_flag = true;
                }
            }
            else
            {
                // 出界的时候标记变量
                ch_flag = false;
            }
            if (ch_flag == true)
            {
                // 合法坐标，判断是否是未被标记过的有效点
                if (*ch_cursrcp == _IDVal)
                {
                    // 判断是否被标记过
                    if (*ch_curmapp == 0)
                    {
                        // 发现符合条件的点！标记完返回！
                        m_Nextx = ch_i;
                        m_Nexty = iny - 1;
                        *ch_curmapp = 1;
                        return true;
                    }
                }
                ch_cursrcp++;
                ch_curmapp++;
            }
        }
    }

    // 扫描中行
    ch_flag = false;
    {
        for (ch_i = inx - 1; ch_i < inx + 1; ch_i++)
        {
            if (ch_i >= 0 && ch_i < m_nWidth)
            {
                if (ch_flag == false)
                {
                    ch_temp = (iny)*m_nWidth + i;
                    ch_cursrcp = srcmap + ch_temp;
                    ch_curmapp = map + ch_temp;
                    ch_flag = true;
                }
            }
            else
            {
                // 出界的时候标记变量
                ch_flag = false;
            }
            if (ch_flag == true)
            {
                // 合法坐标，判断是否是未被标记过的有效点
                if (*ch_cursrcp == _IDVal)
                {
                    // 判断是否被标记过
                    if (*ch_curmapp == 0)
                    {
                        // 发现符合条件的点！标记完返回！
                        m_Nextx = ch_i;
                        m_Nexty = iny;
                        *ch_curmapp = 1;
                        return true;
                    }
                }
                ch_cursrcp++;
                ch_curmapp++;
            }
        }
    }

    // 扫描上行
    ch_flag = false;
    if (iny + 1 < m_nHeight)
    {
        for (ch_i = inx - 1; ch_i < inx + 1; ch_i++)
        {
            if (ch_i >= 0 && ch_i < m_nWidth)
            {
                if (ch_flag == false)
                {
                    ch_temp = (iny + 1) * m_nWidth + i;
                    ch_cursrcp = srcmap + ch_temp;
                    ch_curmapp = map + ch_temp;
                    ch_flag = true;
                }
            }
            else
            {
                // 出界的时候标记变量
                ch_flag = false;
            }
            if (ch_flag == true)
            {
                // 合法坐标，判断是否是未被标记过的有效点
                if (*ch_cursrcp == _IDVal)
                {
                    // 判断是否被标记过
                    if (*ch_curmapp == 0)
                    {
                        // 发现符合条件的点！标记完返回！
                        m_Nextx = ch_i;
                        m_Nexty = iny + 1;
                        *ch_curmapp = 1;
                        return true;
                    }
                }
                ch_cursrcp++;
                ch_curmapp++;
            }
        }
    }

    // 扫描完了还没有发现可扩展点，返回false
    return false;
}

bool CConctDomain::m_checkpoint(UCHAR *inpData)
{
    if (*inpData == _IDVal)
    {
        // 检测周围是否
    }
    return false;
}

BOOL CConctDomain::ShowMap(BYTE *rgbBuf, int inWidth, int inHeight)
{
    if (rgbBuf == NULL || inWidth != m_nWidth || inHeight != m_nHeight)
    {
        return FALSE;
    }
    UCHAR *lCurmapp = map;
    UCHAR *lCursrcp = rgbBuf;
    for (int i = 0; i < m_nWidth * m_nHeight; i++)
    {
        if (*lCurmapp == _IDVal)
        {
            *lCursrcp = 0xff;
            *(lCursrcp + 1) = 0xff;
            *(lCursrcp + 2) = 0x0;
        }
        lCurmapp++;
        lCursrcp += 3;
    }
    return TRUE;
}

void CConctDomain::Dilation(int inTimes)
{
    bool flag = false; // true 为进入色域  false 为离开色域
    UCHAR *tempp;
    // 扫描第二行到倒数第二行
    for (int y = 1; y < m_nHeight - 1; y++)
    {
        temp = y * m_nWidth;
        lCursrcp = srcmap + temp;
        flag = false;
        for (int x = 1; x < m_nWidth - 1; x++)
        {
            if (*lCursrcp == _IDVal)
            {
                if (flag == false)
                {
                    // 进入色域了，向左扩一点
                    tempp = lCursrcp - 1;
                    *(tempp) = _IDVal;
                    flag = true;
                }
            }
            else
            {
                if (flag == true)
                {
                    // 已经离开色域，此点作为右边缘，标记
                    *lCursrcp = _IDVal;
                    flag = false;
                }
            }
            if (x == m_nWidth - 2)
            {
                // 右边倒数第二点，特殊处理
                if (*lCursrcp == _IDVal)
                {
                    // 右边扩一点
                    tempp = lCursrcp + 1;
                    *(tempp) = _IDVal;
                    flag = false;
                }
            }
            lCursrcp++;
        }
    }

    // 纵向的膨胀
    for (int x = 0; x < m_nWidth - 1; x++)
    {
        flag = false;
        lCursrcp = srcmap + x;
        for (y = 1; y < m_nHeight - 1; y++)
        {
            if (*lCursrcp == _IDVal)
            {
                if (flag == false)
                {
                    // 进色域，将下方点扩
                    tempp = lCursrcp - m_nWidth;
                    *tempp = _IDVal;
                    flag = true;
                }
            }
            else
            {
                if (flag == true)
                {
                    // 离开色域，此点作为上边缘，标记
                    *lCursrcp = _IDVal;
                    flag = false;
                }
            }
            if (y == m_nHeight - 2)
            {
                if (*lCursrcp == _IDVal)
                {
                    // 离开色域，此点作为上边缘，标记
                    *tempp = _IDVal;
                }
            }
            lCursrcp += m_nWidth;
        }
    }
}

void CConctDomain::Erosion(int inMode)
{
    bool flag = false; // true 为进入色域  false 为离开色域
    UCHAR *tempp;
    // 扫描第二行到倒数第二行
    for (int y = 1; y < m_nHeight - 1; y++)
    {
        temp = y * m_nWidth;
        lCursrcp = srcmap + temp;
        flag = false;
        for (int x = 1; x < m_nWidth; x++)
        {
            if (*lCursrcp == _IDVal)
            {
                if (flag == false)
                {
                    // 进入色域了，清掉这个点
                    tempp = lCursrcp;
                    *(tempp) = 0;
                    flag = true;
                }
            }
            else
            {
                if (flag == true)
                {
                    // 已经离开色域，此点作为右边缘，标记
                    tempp = lCursrcp - 1;
                    *(tempp) = 0;
                    flag = false;
                }
            }
            if (x == m_nWidth - 1)
            {
                // 右边倒数第一点，特殊处理
                if (*lCursrcp == _IDVal)
                {
                    // 抠掉
                    tempp = lCursrcp;
                    *(tempp) = 0;
                    flag = false;
                }
            }
            lCursrcp++;
        }
    }

    // 纵向的腐蚀
    for (int x = 0; x < m_nWidth - 1; x++)
    {
        flag = false;
        lCursrcp = srcmap + x;
        for (y = 1; y < m_nHeight - 1; y++)
        {
            if (*lCursrcp == _IDVal)
            {
                if (flag == false)
                {
                    // 进色域，扣掉该点
                    tempp = lCursrcp;
                    *tempp = 0;
                    flag = true;
                }
            }
            else
            {
                if (flag == true)
                {
                    // 离开色域，此点作为上边缘，标记
                    tempp = lCursrcp - m_nWidth;
                    *tempp = 0;
                    flag = false;
                }
            }
            if (y == m_nHeight - 1)
            {
                if (*lCursrcp == _IDVal)
                {
                    // 离开色域，此点作为上边缘，标记
                    *tempp = 0;
                }
            }
            lCursrcp += m_nWidth;
        }
    }
}

void CConctDomain::DrawFocus(BYTE *rgbBuf, long x, long y, RGBTRIPLE *prgb)
{
    if (x < 0 || x > m_nWidth - 1 || y < 0 || y > m_nHeight - 1)
    {
        return;
    }
    // 横向画线
    int range = 10;

    int minx = x - range;
    if (minx < 0)
    {
        minx = 0;
    }
    int maxx = x + range;
    if (maxx > m_nWidth - 1)
    {
        maxx = m_nWidth - 1;
    }

    BYTE *currgb = rgbBuf + y * m_nWidth * 3 + (minx) * 3;
    for (int z = minx; z < maxx; z++)
    {
        *currgb = prgb->rgbtBlue;
        currgb++;
        *currgb = prgb->rgbtGreen;
        currgb++;
        *currgb = prgb->rgbtRed;
        currgb++;
    }

    // 纵向画线
    int miny = y - range;
    if (miny < 0)
    {
        miny = 0;
    }
    int maxy = y + range;
    if (maxy > m_nHeight - 1)
    {
        maxy = m_nHeight - 1;
    }

    currgb = rgbBuf + (miny)*m_nWidth * 3 + x * 3;
    for (z = miny; z < maxy; z++)
    {
        *currgb = prgb->rgbtBlue;
        currgb++;
        *currgb = prgb->rgbtGreen;
        currgb++;
        *currgb = prgb->rgbtRed;
        currgb++;
        currgb += m_nWidth * 3 - 3;
    }
}

void CConctDomain::Combo(UCHAR *inMap, int inWidth, int inHeight, UCHAR IDVal)
{
    InitMap(inMap, inWidth, inHeight, IDVal);
    Process(0);
}
