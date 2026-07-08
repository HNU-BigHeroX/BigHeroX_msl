// RadarScan.h: interface for the CRadarScan class.
//
//////////////////////////////////////////////////////////////////////
/*************************************************
  http://robot.up-tech.com
  Author: 徐鑫鑫      Version: 1.1       Date: 2008/10/20
  Email:  zwj_uptech@126.com
  Description:    // 射线连通域扫描
  Others:         // 1.1 增加平面坐标到极坐标的角度换算函数
*************************************************/
#if !defined(AFX_RADARSCAN_H__D5EB3725_8A38_4333_9105_D8A3BAACA39B__INCLUDED_)
#define AFX_RADARSCAN_H__D5EB3725_8A38_4333_9105_D8A3BAACA39B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRadarScan
{
  public:
    double CalAngle(int inx, int iny);
    int GetNumofSector();
    void GetPoint(int inIndex, int inDistance, CPoint *inpnt);
    BOOL Line(UCHAR *inBuf, int inIndex, UCHAR red, UCHAR green, UCHAR blue);
    void Scan(UCHAR *inmap, int inWidth, int inHeight, UCHAR inVal, int inSpan);
    // 独立射线扫描结果
    int *resultx;
    int *resulty;
    int *distance;

    // 射线连通域扫描结果
    int *midline;
    int *clockwise;
    int *anticlockwise;
    int *sumoflines;
    BOOL Sector();

    // 中心坐标
    int m_cenx;
    int m_ceny;
    double angle;
    double dx;
    double dy;

    CRadarScan();
    virtual ~CRadarScan();

  protected:
    int *rayx; // 射线点阵表的x值
    int *rayy; // 射线点阵表的y值
    UCHAR *m_pMap;
    int m_nWidth;
    int m_nHeight;
    int index;
    int count;

    // 扇面连通域扫描结果
    int m_nNumofSector;
};

#endif // !defined(AFX_RADARSCAN_H__D5EB3725_8A38_4333_9105_D8A3BAACA39B__INCLUDED_)
