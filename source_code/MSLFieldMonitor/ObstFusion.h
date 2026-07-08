// 障碍物数据融合
#pragma once

#include "resource.h"
#include "MSL_Structs.h"
#include "cv.h"
#include "highgui.h"

class CObstFusion
{
  private:
    bool opencv_debug;

  public:
    CObstFusion();
    ~CObstFusion();

    HBITMAP hBitmap1;
    HBITMAP hBitmap2;

    IplImage *hBitmapToIpl(HBITMAP hBmp);
    bool m_Obst_RemoveOutOfField(obstacle *inObstAr);
    int m_CalDist(int inx1, int iny1, int inx2, int iny2);
    void circle_maskGA(float R, int *Gcmask_x, int *Gcmask_y, float *Gcmask_w, int *Gcmask_lo);
    void bool_max_connectivity_analyze2_n(void);
    void bool_max_connectivity_analyze2_n_(void);
    bool segmentsIntr(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2, int &x, int &y);
    void Fusion(stDataSummary *inDataSummary);

    // 障碍物交线数组
    obstacle *m_fusObst;
};