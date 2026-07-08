// HSIParam.h: HSIParam struct.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSIPARAM_H__D49445AD_77F2_4D3C_A216_5D4B9F054787__INCLUDED_)
#define AFX_HSIPARAM_H__D49445AD_77F2_4D3C_A216_5D4B9F054787__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HSI_TARGET_NUM 6

struct HSIParam
{
    int Hue_max, Hue_min, Satu_min, Satu_max, I_min, I_max;
    int x, y, sum;

    void Clear()
    {
        x = y = sum = 0;
    }

    void ReSet()
    {
        Hue_max = Hue_min = Satu_min = Satu_max = I_min = I_max = 0;
        x = y = sum = 0;
    }
};

#endif // !defined(AFX_HSIPARAM_H__D49445AD_77F2_4D3C_A216_5D4B9F054787__INCLUDED_)
