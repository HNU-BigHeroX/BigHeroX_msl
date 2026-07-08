#pragma once
#include "IRolePlay.h"
class CRole_Defence : public IRolePlay
{
  public:
    CRole_Defence();
    ~CRole_Defence();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
