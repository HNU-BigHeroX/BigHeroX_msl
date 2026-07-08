#pragma once
#include <MATH.H>
class IDepthMap
{
  public:
    IDepthMap();
    ~IDepthMap();

    // data
    int m_nWidth;
    int m_nHeight;
    int *m_pMapData;

    // display
    unsigned char *m_pRGBData;
    BITMAPINFO m_bmi;

    void m_Release();
    void InitData(int inWidth, int inHeight);
    void DisplayToWnd(CWnd *inpWnd, int inX, int inY, int inDestWidth, int inDestHeight);
    int m_CalDist(int inx1, int iny1, int inx2, int iny2);
    // 将矩阵数值重置到指定值
    void ResetMat(int inValue);
    void AddPeak(int inX, int inY, int inPeakVal, int inStep);
    // 文字输出
    void DrawText(CWnd *inpWnd, int inX, int inY, CString inText, int inCharNum);
    void AddSunken(int inEdgeValue, int inSlopeLength);
};
