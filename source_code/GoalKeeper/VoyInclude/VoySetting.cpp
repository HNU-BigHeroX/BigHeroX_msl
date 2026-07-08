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

// DEL void CVoySetting::Save(CString filename, VoySet setting)
// DEL {
// DEL 	CFile mFile(filename, CFile::modeWrite|CFile::modeCreate);
// DEL 	mFile.Write(&setting,sizeof(VoySet));
// DEL 	mFile.Flush();
// DEL 	mFile.Close();
// DEL }

// DEL BOOL CVoySetting::Load(CString filename, VoySet *setting)
// DEL {
// DEL 	CFileFind finder;
// DEL 	if(finder.FindFile(filename) == FALSE)
// DEL 		return FALSE;
// DEL
// DEL 	CFile mFile(filename,CFile::modeRead);
// DEL
// DEL 	if(mFile.GetLength() != sizeof(VoySet))
// DEL 	return FALSE;
// DEL
// DEL 	mFile.Read(setting,sizeof(VoySet));
// DEL 	mFile.Close();
// DEL
// DEL 	return TRUE;
// DEL }

void CVoySetting::Save(CString filename, Voy3Set setting)
{
    CFile mFile(filename, CFile::modeWrite | CFile::modeCreate);
    mFile.Write(&setting, sizeof(Voy3Set));
    mFile.Flush();
    mFile.Close();
}

BOOL CVoySetting::Load(CString filename, Voy3Set *setting)
{
    CFileFind finder;
    if (finder.FindFile(filename) == FALSE)
        return FALSE;

    CFile mFile(filename, CFile::modeRead);

    if (mFile.GetLength() != sizeof(Voy3Set))
        return FALSE;

    mFile.Read(setting, sizeof(Voy3Set));
    mFile.Close();

    return TRUE;
}
