#pragma once
#include "IRolePlay.h"
class CRole_Stop : public IRolePlay
{
  public:
    CRole_Stop();
    ~CRole_Stop();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
