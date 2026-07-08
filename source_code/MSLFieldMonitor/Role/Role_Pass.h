#pragma once
#include "IRolePlay.h"
class CRole_Pass : public IRolePlay
{
  public:
    CRole_Pass();
    ~CRole_Pass();

    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
