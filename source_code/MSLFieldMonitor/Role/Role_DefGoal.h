#pragma once
#include "IRolePlay.h"
class CRole_DefGoal : public IRolePlay
{
  public:
    CRole_DefGoal();
    ~CRole_DefGoal();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
