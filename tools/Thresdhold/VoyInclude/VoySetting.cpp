// VoySetting.cpp: implementation of the CVoySetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoySetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVoySetting::CVoySetting()
{
}

CVoySetting::~CVoySetting()
{
}

void CVoySetting::Save(CString filename, VoySet setting)
{
    CFile mFile(filename, CFile::modeWrite | CFile::modeCreate);
    mFile.Write(&setting, sizeof(VoySet));
    mFile.Flush();
    mFile.Close();
}

BOOL CVoySetting::Load(CString filename, VoySet *setting)
{
    CFileFind finder;
    if (finder.FindFile(filename) == FALSE)
        return FALSE;

    CFile mFile(filename, CFile::modeRead);

    if (mFile.GetLength() != sizeof(VoySet))
        return FALSE;

    mFile.Read(setting, sizeof(VoySet));
    mFile.Close();

    return TRUE;
}
