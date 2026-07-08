#pragma once
#include "ITactic.h"
class CTac_Test_MoveARound : public ITactic
{
  public:
    CTac_Test_MoveARound();
    ~CTac_Test_MoveARound();

    void Reset();
    void PlanTac(stTactics *outTactics);
    void Start();

    int m_nStep;      // 有限状态机变量
    int m_nIndexHead; // 当前接球点下标
    void HeadTo(int inIndex);
    bool bSomeOneOutOfField();
    bool bEveryOneArrived();
    bool bSomeOneInPosition(int inX, int inY);
};
