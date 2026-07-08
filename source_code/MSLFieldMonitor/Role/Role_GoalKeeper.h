#pragma once
#include "IRolePlay.h"
class CRole_GoalKeeper : public IRolePlay
{
  public:
    CRole_GoalKeeper();
    ~CRole_GoalKeeper();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
