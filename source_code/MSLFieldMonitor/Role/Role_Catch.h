#pragma once
#include "IRolePlay.h"
class CRole_Catch : public IRolePlay
{
  public:
    CRole_Catch();
    ~CRole_Catch();

    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
