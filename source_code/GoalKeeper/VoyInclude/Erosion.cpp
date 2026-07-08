// Erosion.cpp: implementation of the CErosion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Erosion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CErosion::CErosion()
{
    _tWidth = _tHeight = 0;
    _tMap = NULL;
}

CErosion::~CErosion()
{
}

BOOL CErosion::Process()
{
    // 检查参数是否正确
    if (nWidth <= 0 || nHeight <= 0 || map == NULL || srcmap == NULL)
    {
        return FALSE;
    }

    // 检查图像尺寸是否变化，以确定是否对tMap进行重建
    if (_tWidth != nWidth || _tHeight != nHeight)
    {
        delete _tMap;
        _tMap = new UCHAR[nWidth * nHeight];
        _tWidth = nWidth;
        _tHeight = nHeight;
    }

    // 将上次操作结果复制到tMap中，作为此次处理的对象
    for (int t = 0; t < nWidth * nHeight; t++)
    {
        _tMap[t] = map[t];
    }

    // 腐蚀处理
    for (int i = 0; i < nWidth; i++)
    {
        for (int j = 0; j < nHeight; j++)
        {
            if (Cluster(i, j) == true)
            {
                SetPoint(map, i, j, _IDVal);
            }
            else
            {
                SetPoint(map, i, j, _IDVal - 1);
            }
        }
    }
    return TRUE;
}

// 判断一个标记点是否被与他同样的标记所包围着
bool CErosion::Cluster(int x, int y)
{
    bool ret = true;
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            if (GetPoint(i, j, _tMap) != _IDVal)
            {
                ret = false;
                break;
            }
        }
        if (ret == false)
        {
            break;
        }
    }
    return ret;
}
