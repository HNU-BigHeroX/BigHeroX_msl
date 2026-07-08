#include "stdafx.h"
#include "PassSwitch.h"

CPassSwitch::CPassSwitch()
{
    Reset();
}

CPassSwitch::~CPassSwitch()
{
}

bool CPassSwitch::isInOwnField(int inX)
{
    if (inX < nOppoToOwnX)
    {
        resInOwnField = true;
    }
    if (inX > nOwnToOppoX)
    {
        resInOwnField = false;
    }
    return resInOwnField;
}

bool CPassSwitch::isInLeftField(int inY)
{
    if (inY < nRightFieldY)
    {
        resLeftField = false;
    }
    if (inY > nLeftFieldY)
    {
        resLeftField = true;
    }
    return resLeftField;
}

void CPassSwitch::Reset()
{
    nOppoToOwnX = 750;
    nOwnToOppoX = 950;
    resInOwnField = false;

    nLeftFieldY = 700;
    nRightFieldY = 500;
    resLeftField = true;

    nOurPenletyOutHeight = 350;
    nOurPenletyOutWidth = 800;
    nOurPenletyInHeight = 250;
    nOurPenletyInWidth = 700;
    resInOurPenlety = false;
}

bool CPassSwitch::isInOurPenalty(int inX, int inY)
{
    if (inX > nOurPenletyOutHeight || inY < (600 - nOurPenletyOutWidth / 2) || inY > (600 + nOurPenletyOutWidth / 2))
    {
        resInOurPenlety = false;
    }

    if (inX < nOurPenletyInHeight && inY > (600 - nOurPenletyInWidth / 2) && inY < (600 + nOurPenletyInWidth / 2))
    {
        resInOurPenlety = true;
    }

    return resInOurPenlety;
}
