#pragma once
#include "IRolePlay.h"
class CRole_Attack : public IRolePlay
{
  public:
    CRole_Attack();
    ~CRole_Attack();

    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
