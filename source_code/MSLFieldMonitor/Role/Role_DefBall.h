#pragma once
#include "IRolePlay.h"
class CRole_DefBall : public IRolePlay
{
  public:
    CRole_DefBall();
    ~CRole_DefBall();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
