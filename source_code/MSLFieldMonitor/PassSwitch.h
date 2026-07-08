// »»±ßÓÃµÄ
#pragma once
class CPassSwitch
{
  public:
    CPassSwitch();
    ~CPassSwitch();

    int nOppoToOwnX;
    int nOwnToOppoX;
    bool resInOwnField;

    int nLeftFieldY;
    int nRightFieldY;
    bool resLeftField;

    int nOurPenletyOutHeight;
    int nOurPenletyOutWidth;
    int nOurPenletyInHeight;
    int nOurPenletyInWidth;
    bool resInOurPenlety;

    bool isInOwnField(int inX);
    bool isInLeftField(int inY);
    bool isInOurPenalty(int inX, int inY);
    void Reset();
};
