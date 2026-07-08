// ÇòµÄÔË¶¯Ô¤²â
#pragma once
#include "MSL_Structs.h"
#include <cv.h>
#include <highgui.h>

class CBallMoveEstimation
{
  public:
    CBallMoveEstimation();
    ~CBallMoveEstimation();
    void Estimation();

    stDataSummary *pDataSummary;

    CvPoint *m_arBallLog;
    CvPoint *m_arDisplay;
    int m_nBufLen;
    int m_nNum;
    CPoint pntDirect;
    CvPoint pntDisDirect;
    double m_CalDist(int inx1, int iny1, int inx2, int iny2);
    int m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg);
    CPoint m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist);
    CPoint m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng);

    void Init(int inLen);
    void ReleaseData();
    void Add(int inX, int inY);
    void CvDisplay();
    int m_CalIncludedAngle(int inAngle1, int inAngle2);
};
