// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4F6E1352_6C0A_4EDA_9ABA_F33DC500D9AC__INCLUDED_) // 控制版本
#define AFX_STDAFX_H__4F6E1352_6C0A_4EDA_9ABA_F33DC500D9AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once // 保证编译一次
#endif       // _MSC_VER > 1000

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers 包含一些很少用到的头文件

#define WINVER 0x0500 // 为了使“LoadCursor(NULL,IDC_HAND);”合法

#include <afxwin.h>   // MFC core and standard components
#include <afxext.h>   // MFC extensions
#include <afxdisp.h>  // MFC Automation classes
#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h> // MFC socket extensions

typedef unsigned long ULONG_PTR, *PULONG_PTR;
#include < gdiplus.h > // 负责系统与绘制程序之间的交互
using namespace Gdiplus;
#pragma comment(lib, "GdiPlus.lib")

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4F6E1352_6C0A_4EDA_9ABA_F33DC500D9AC__INCLUDED_)
