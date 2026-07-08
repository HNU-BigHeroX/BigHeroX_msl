#pragma once
#include "IRolePlay.h"
class CRole_PassMove : public IRolePlay
{
  public:
    CRole_PassMove();
    ~CRole_PassMove();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
