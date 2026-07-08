#pragma once
#include "IRolePlay.h"
class CRole_FocusOnBall : public IRolePlay
{
  public:
    CRole_FocusOnBall();
    ~CRole_FocusOnBall();

    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);

    bool isBlock(int inAgentIndex);
};
