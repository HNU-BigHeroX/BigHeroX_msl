#pragma once
#include "IRolePlay.h"
class CRole_CatchMove : public IRolePlay
{
  public:
    CRole_CatchMove();
    ~CRole_CatchMove();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
