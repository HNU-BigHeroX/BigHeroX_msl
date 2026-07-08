// 张万杰版障碍物数据融合
#pragma once

#include "MSL_Structs.h"
#include "cv.h"
#include "highgui.h"

class CObstFusion_ZWJ
{
  public:
    CObstFusion_ZWJ();
    ~CObstFusion_ZWJ();

    int m_CalDist(int inx1, int iny1, int inx2, int iny2);
    void Fusion(stDataSummary *inDataSummary);
    void SortEnemyDistToOurGoal(stDataSummary *inDataSummary);

    enemy *arTrackEnemy;
    int *arTrackCount;
    int nNumTrackEnemy;
    void RemoveTrack(int inTrackIndex);
    void UpdateTrackEnemy(int inTrackIndex);
    void AddTrackEnemy(enemy *inTmpEnemyToAdd);

    bool bEnable;
    void TrackEnemy(stDataSummary *inDataSummary);
    void SetResult(stDataSummary *inDataSummary);
    void FindObstHoldBallInNoSeen(stDataSummary *inDataSummary);

  protected:
    void m_ResetObstSrc();
};
