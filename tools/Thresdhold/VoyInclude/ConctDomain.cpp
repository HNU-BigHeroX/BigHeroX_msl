// ConctDomain.cpp: implementation of the CConctDomain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConctDomain.h"

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
    _Range = 2;
}

CConctDomain::~CConctDomain()
{
    _toExp.RemoveAll();
    _result.RemoveAll();
}

BOOL CConctDomain::_Expend(int x, int y)
{
    CPoint pnt;
    for (int i = x - _Range; i < x + _Range + 1; i++)
    {
        for (int j = y - _Range; j < y + _Range + 1; j++)
        {
            // 符合条件的像素点添加到扩展列表中
            if (GetPoint(i, j, map) == _IDVal)
            {
                pnt.x = i;
                pnt.y = j;
                _toExp.AddTail(pnt);

                // 当前域信息累计
                _CurDomain.x += i;
                _CurDomain.y += j;
                _CurDomain.sum++;

                // 扫描过的点，一律标记上_IDVal+1
                SetPoint(map, i, j, _IDVal + 1);
            }
        }
    }
    return FALSE;
}

BOOL CConctDomain::Process(int inMode)
{
    CPoint p;
    DomainNode temp;

    // 清空前一次结果
    _result.RemoveAll();
    _toExp.RemoveAll();

    for (int i = 0; i < nWidth; i++)
    {
        for (int j = 0; j < nHeight; j++)
        {
            if (GetPoint(i, j, map) == _IDVal)
            {
                // 从该点开始扩张
                _Expend(i, j);

                // 如果扩展表非空，从表头取出一个点进行扩展
                while (_toExp.GetCount() > 0)
                {
                    p = _toExp.GetHead();
                    _Expend(p.x, p.y);
                    _toExp.RemoveHead();
                }

                if (_CurDomain.sum > 0)
                {
                    // 一个连通域扫描完了，保存当前域信息
                    _CurDomain.x /= _CurDomain.sum;
                    _CurDomain.y /= _CurDomain.sum;

                    // 像素多的连通域在前边
                    POSITION pos;
                    pos = _result.GetHeadPosition();
                    while (pos != NULL)
                    {
                        temp = _result.GetAt(pos);
                        if (_CurDomain.sum > temp.sum)
                        {
                            // 当前域比temp大，应该在其前边
                            //_result.GetPrev(pos);
                            _result.InsertBefore(pos, _CurDomain);

                            _CurDomain.x = 0;
                            _CurDomain.y = 0;
                            _CurDomain.sum = 0;
                            break;
                        }
                        _result.GetNext(pos);
                    }

                    if (_CurDomain.sum == 0)
                    {
                        // 从前面break出来的，继续break出去
                        break;
                    }

                    //_result为空表或者表中没有比当前小的连通域，加在表尾
                    _result.AddTail(_CurDomain);
                    _CurDomain.x = 0;
                    _CurDomain.y = 0;
                    _CurDomain.sum = 0;
                }
            }
        }
    }
    // 恢复map
    for (int k = 0; k < nWidth * nHeight; k++)
    {
        map[k] = srcmap[k];
    }
    return TRUE;
}

int CConctDomain::GetCount()
{
    return _result.GetCount();
}

void CConctDomain::Mark(BYTE *rgbBuf)
{
    RGBTRIPLE clr;
    clr.rgbtRed = color.rgbtBlue;
    clr.rgbtGreen = color.rgbtGreen;
    clr.rgbtBlue = color.rgbtRed;

    // int s = 1000000;
    // int t = 0;

    DomainNode node;
    POSITION pos;
    pos = _result.GetHeadPosition();
    while (pos != NULL)
    {
        node = _result.GetNext(pos);
        DrawFocus(rgbBuf, node.x, node.y, &clr);

        /*t ++;
        CString str;
        str.Format("第%d个，像素%d,上一像素%d",t,node.sum,s);
        if (node.sum > s)
        {
            AfxMessageBox(str);
        }
        s = node.sum;*/
    }
}

void CConctDomain::SetRange(int inRge)
{
    _Range = inRge;
}

Domain CConctDomain::GetDomainAt(int index)
{
    Domain ret;
    ret.x = 0;
    ret.y = 0;
    ret.sum = 0;

    if (_result.GetCount() == 0)
    {
        return ret;
    }

    if (index > _result.GetCount() && index < 0)
    {
        return ret;
    }

    int i = index;
    POSITION pos = _result.GetHeadPosition();
    while (pos != NULL)
    {
        if (i == 0)
        {
            // 找到了，将其换算到DIB的坐标
            ret = _result.GetAt(pos);

            return ret;
        }
        _result.GetNext(pos);
        i--;
    }
    return ret;
}

Domain CConctDomain::GetDomainNear(int x, int y)
{
    Domain ret;
    ret.x = x;
    ret.y = y;
    ret.sum = 0;
    if (_result.GetCount() == 0)
    {
        return ret;
    }

    Domain temp;
    ret = _result.GetHead();
    int dist = (ret.x - x) * (ret.x - x) + (ret.y - y) * (ret.y - y);
    int d;

    POSITION pos = _result.GetHeadPosition();
    while (pos != NULL)
    {
        temp = _result.GetAt(pos);
        d = (temp.x - x) * (temp.x - x) + (temp.y - y) * (temp.y - y);

        if (d < dist)
        {
            if (d != 0)
            {
                // 将当前节点作为返回值
                dist = d;
                ret = temp;
            }
        }
        _result.GetNext(pos);
    }
    return ret;
}
