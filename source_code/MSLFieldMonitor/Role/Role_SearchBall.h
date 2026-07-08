#pragma once
#include "IRolePlay.h"
class CRole_SearchBall : public IRolePlay
{
  public:
    CRole_SearchBall();
    ~CRole_SearchBall();

    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);
};
