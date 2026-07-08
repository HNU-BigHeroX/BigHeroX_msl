#pragma once
#include "IRolePlay.h"
class CRole_Penalty : public IRolePlay
{
  public:
    CRole_Penalty();
    ~CRole_Penalty();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
