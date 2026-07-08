// TemMapFile.cpp: implementation of the CTemMapFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TemMapFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTemMapFile::CTemMapFile()
{
}

CTemMapFile::~CTemMapFile()
{
}

void CTemMapFile::SaveToFile(int *inmap, int insize, CString filename)
{
    CFileFind finder;
    if (finder.FindFile(filename) == TRUE)
    {
        if (MessageBox(NULL, "该文件已存在，是否覆盖？", "提示", MB_OKCANCEL) == IDCANCEL)
        {
            return;
        }
    }

    /*/将日期填进去
    SYSTEMTIME st;
    GetLocalTime(&st);
    setting.year = st.wYear;
    setting.month = st.wMonth;
    setting.day = st.wDay;
    setting.hour = st.wHour;
    setting.minute = st.wMinute;
    setting.second = st.wSecond;*/

    CFile mFile(filename, CFile::modeWrite | CFile::modeCreate);
    mFile.Write(inmap, 640 * 480 * 4);
    mFile.Flush();
    mFile.Close();
}

void CTemMapFile::Load(CString filename, int *inmap, int insize)
{
    if (inmap == NULL)
    {
        return;
    }
    CFileFind finder;
    if (finder.FindFile(filename) == FALSE)
    {
        AfxMessageBox("场线匹配模板文件不存在！");
        return;
    }

    CFile mFile(filename, CFile::modeRead);

    if (mFile.GetLength() != (DWORD)insize)
    {
        AfxMessageBox("场线匹配模板数据异常！");
        return;
    }

    mFile.Read(inmap, insize);
    mFile.Close();
}
