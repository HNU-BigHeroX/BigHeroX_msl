#pragma once
#include "IRolePlay.h"
class CRole_Manual : public IRolePlay
{
  public:
    CRole_Manual();
    ~CRole_Manual();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
