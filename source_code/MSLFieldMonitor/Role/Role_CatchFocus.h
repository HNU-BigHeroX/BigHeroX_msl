#pragma once
#include "IRolePlay.h"
class CRole_CatchFocus : public IRolePlay
{
  public:
    CRole_CatchFocus();
    ~CRole_CatchFocus();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
