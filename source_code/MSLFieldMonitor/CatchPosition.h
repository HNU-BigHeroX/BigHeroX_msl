// 计算接球点
#pragma once
#include "MSL_Structs.h"
class CCatchPosition
{
  public:
    CCatchPosition();
    ~CCatchPosition();

    stDataSummary *pDataSummary;

    POINT posCatch[3]; // 计算三个接球位置

    int m_Ag_GetStatus(UINT inSt, uAgent *inalist);
    bool GetAtkPos(CPoint *inPos, uAgent *inAgent);
    void CalCatchPosition();
    bool Get_start_state(void);
};
