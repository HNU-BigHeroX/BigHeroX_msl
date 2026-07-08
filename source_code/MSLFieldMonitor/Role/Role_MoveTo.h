#pragma once
#include "IRolePlay.h"
class CRole_MoveTo : public IRolePlay
{
  public:
    CRole_MoveTo();
    ~CRole_MoveTo();
    void Reset();
    void Play(stDataSummary *inDataSummary, uRole *inRole);

    bool isCross(int Line1_x1, int Line1_y1, int Line1_x2, int Line1_y2, int Line2_x1, int Line2_y1, int Line2_x2,
                 int Line2_y2);
};
