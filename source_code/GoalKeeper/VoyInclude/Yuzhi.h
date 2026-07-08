// Yuzhi.h: interface for the Yuzhi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YUZHI_H__5FD72FB5_CB4A_4857_87C2_E6DD354DB1DA__INCLUDED_)
#define AFX_YUZHI_H__5FD72FB5_CB4A_4857_87C2_E6DD354DB1DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct Yuzhi
{
    int bh, bf, bs, bhx, bhs, bis, bix; // 球的HSI颜色
    int jh, jf, js, jhx, jhs, jis, jix; // 己方门的HSI颜色
    int dh, df, ds, dhx, dhs, dis, dix; // 对方门的HSI颜色
    int ch, cf, cs, chx, chs, cis, cix; // 场地
    int sh, ih;                         // 黑色s,黑色i
    int sw, iw;                         // 白色s,白色i
    double x, y, jiao, chang, benjix, benjiy, benjijiao;
    double ball_x, ball_y, ball_jiao, ball_chang, ball_zhong, quanbx, quanby; // 球的,x,y角度，长度
    double jdoor_x, jdoor_y, jdoor_jiao, jdoor_chang, jdoor_zhong, jdoor_zuo, jdoor_you,
        jdoor_kuan; // 自己们的x,y,角度，长度
    double ddoor_x, ddoor_y, ddoor_jiao, ddoor_chang, ddoor_zhong, ddoor_zuo, ddoor_you,
        ddoor_kuan; // 对方门的x,y,角度，长度
    int val_qiehuan;
};

#endif // !defined(AFX_YUZHI_H__5FD72FB5_CB4A_4857_87C2_E6DD354DB1DA__INCLUDED_)
