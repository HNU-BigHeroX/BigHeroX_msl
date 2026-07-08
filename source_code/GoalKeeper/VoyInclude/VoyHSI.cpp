// VoyHSI.cpp: implementation of the VoyHSI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoyHSI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VoyHSI::VoyHSI()
{
}

VoyHSI::~VoyHSI()
{
}

BOOL VoyHSI::Load(CString filename, HSISet *setting)
{
    if (setting == NULL)
    {
        return FALSE;
    }
    CFileFind finder;
    if (finder.FindFile(filename) == FALSE)
    {
        AfxMessageBox("该文件不存在！");
        return FALSE;
    }

    CFile mFile(filename, CFile::modeRead);

    if (mFile.GetLength() != sizeof(HSISet))
        return FALSE;

    mFile.Read(setting, sizeof(HSISet));
    mFile.Close();

    return TRUE;
}

void VoyHSI::Save(CString filename, HSISet setting)
{
    CFileFind finder;
    if (finder.FindFile(filename) == TRUE)
    {
        if (MessageBox(NULL, "该文件已存在，是否覆盖？", "提示", MB_OKCANCEL) == IDCANCEL)
        {
            return;
        }
    }

    // 将日期填进去
    SYSTEMTIME st;
    GetLocalTime(&st);
    setting.year = st.wYear;
    setting.month = st.wMonth;
    setting.day = st.wDay;
    setting.hour = st.wHour;
    setting.minute = st.wMinute;
    setting.second = st.wSecond;

    CFile mFile(filename, CFile::modeWrite | CFile::modeCreate);
    mFile.Write(&setting, sizeof(HSISet));
    mFile.Flush();
    mFile.Close();
}

void VoyHSI::SaveToTxt(CString filename, HSISet setting)
{
    CFile mFile(filename, CFile::modeWrite | CFile::modeCreate);
    CString tosave;
    tosave.Format("[Teammate] T_Hmax:%d,T_Hmin:%d,T_Smax:%d,T_Smin:%d,T_Imax:%d,T_Imin:%d", setting.teammate.Hue_max,
                  setting.teammate.Hue_min, setting.teammate.Satu_max, setting.teammate.Satu_min,
                  setting.teammate.I_max, setting.teammate.I_min);
    mFile.Write(tosave, strlen(tosave));
    mFile.Flush();
    mFile.Close();
}

void VoyHSI::Load(CString filename, HSISet *setting, CListBox *inList)
{
    BOOL bload = Load(filename, setting);

    // printf
    if (inList != NULL && bload == TRUE)
    {
        CString info("--- 阈值文件读取 ---");
        inList->AddString(info);

        // info.Format("队友色标	H:%d-%d S:%d-%d I:%d-%d",);

        info.Format("黑色障碍	H:%d-%d S:%d-%d I:%d-%d", setting->black.Hue_max, setting->black.Hue_min,
                    setting->black.Satu_max, setting->black.Satu_min, setting->black.I_max, setting->black.I_min);
        inList->AddString(info);

        info.Format("绿色场地	H:%d-%d S:%d-%d I:%d-%d", setting->green.Hue_max, setting->green.Hue_min,
                    setting->green.Satu_max, setting->green.Satu_min, setting->green.I_max, setting->green.I_min);
        inList->AddString(info);

        info.Format("白色场线	H:%d-%d S:%d-%d I:%d-%d", setting->whitelines.Hue_max, setting->whitelines.Hue_min,
                    setting->whitelines.Satu_max, setting->whitelines.Satu_min, setting->whitelines.I_max,
                    setting->whitelines.I_min);
        inList->AddString(info);

        info.Format("球（全景）	H:%d-%d S:%d-%d I:%d-%d", setting->overball.Hue_max, setting->overball.Hue_min,
                    setting->overball.Satu_max, setting->overball.Satu_min, setting->overball.I_max,
                    setting->overball.I_min);
        inList->AddString(info);

        info.Format("队友色标	H:%d-%d S:%d-%d I:%d-%d", setting->teammate.Hue_max, setting->teammate.Hue_min,
                    setting->teammate.Satu_max, setting->teammate.Satu_min, setting->teammate.I_max,
                    setting->teammate.I_min);
        inList->AddString(info);

        info.Format("对手对标	H:%d-%d S:%d-%d I:%d-%d", setting->opponent.Hue_max, setting->opponent.Hue_min,
                    setting->opponent.Satu_max, setting->opponent.Satu_min, setting->opponent.I_max,
                    setting->opponent.I_min);
        inList->AddString(info);

        info.Format("--- %d年%d月%d日 %2d:%2d:%2d ---", setting->year, setting->month, setting->day, setting->hour,
                    setting->minute, setting->second);
        inList->AddString(info);

        inList->SetCurSel(inList->GetCount() - 1);
    }
}

void VoyHSI::SaveHSIToINI(CString inFilename, HSISet *setting, CListBox *inList)
{
    CFileFind finder;
    if (finder.FindFile(inFilename) == TRUE)
    {
        if (MessageBox(NULL, "该文件已存在，是否覆盖？", "提示", MB_OKCANCEL) == IDCANCEL)
        {
            return;
        }
    }

    CString strValue;
    // 将日期填进去
    SYSTEMTIME st;
    GetLocalTime(&st);
    setting->year = st.wYear;
    setting->month = st.wMonth;
    setting->day = st.wDay;
    setting->hour = st.wHour;
    setting->minute = st.wMinute;
    setting->second = st.wSecond;
    strValue.Format(" %d-%d-%d %d:%d:%d", setting->year, setting->month, setting->day, setting->hour, setting->minute,
                    setting->second);
    WritePrivateProfileString("Time Log", "Thresdhold", strValue, inFilename);

    if (inList != NULL)
    {
        inList->AddString("阈值保存时间：");
        inList->AddString(strValue);
        inList->SetCurSel(inList->GetCount() - 1);
    }

    // black
    strValue.Format("%d", setting->black.Hue_max);
    WritePrivateProfileString("Black Thresdhold", "Hue_Max", strValue, inFilename);
    strValue.Format("%d", setting->black.Hue_min);
    WritePrivateProfileString("Black Thresdhold", "Hue_Min", strValue, inFilename);
    strValue.Format("%d", setting->black.Satu_max);
    WritePrivateProfileString("Black Thresdhold", "Satu_Max", strValue, inFilename);
    strValue.Format("%d", setting->black.Satu_min);
    WritePrivateProfileString("Black Thresdhold", "Satu_Min", strValue, inFilename);
    strValue.Format("%d", setting->black.I_max);
    WritePrivateProfileString("Black Thresdhold", "Int_Max", strValue, inFilename);
    strValue.Format("%d", setting->black.I_min);
    WritePrivateProfileString("Black Thresdhold", "Int_Min", strValue, inFilename);

    // green
    strValue.Format("%d", setting->green.Hue_max);
    WritePrivateProfileString("Green Thresdhold", "Hue_Max", strValue, inFilename);
    strValue.Format("%d", setting->green.Hue_min);
    WritePrivateProfileString("Green Thresdhold", "Hue_Min", strValue, inFilename);
    strValue.Format("%d", setting->green.Satu_max);
    WritePrivateProfileString("Green Thresdhold", "Satu_Max", strValue, inFilename);
    strValue.Format("%d", setting->green.Satu_min);
    WritePrivateProfileString("Green Thresdhold", "Satu_Min", strValue, inFilename);
    strValue.Format("%d", setting->green.I_max);
    WritePrivateProfileString("Green Thresdhold", "Int_Max", strValue, inFilename);
    strValue.Format("%d", setting->green.I_min);
    WritePrivateProfileString("Green Thresdhold", "Int_Min", strValue, inFilename);

    // whitelines
    strValue.Format("%d", setting->whitelines.Hue_max);
    WritePrivateProfileString("WhiteLine Thresdhold", "Hue_Max", strValue, inFilename);
    strValue.Format("%d", setting->whitelines.Hue_min);
    WritePrivateProfileString("WhiteLine Thresdhold", "Hue_Min", strValue, inFilename);
    strValue.Format("%d", setting->whitelines.Satu_max);
    WritePrivateProfileString("WhiteLine Thresdhold", "Satu_Max", strValue, inFilename);
    strValue.Format("%d", setting->whitelines.Satu_min);
    WritePrivateProfileString("WhiteLine Thresdhold", "Satu_Min", strValue, inFilename);
    strValue.Format("%d", setting->whitelines.I_max);
    WritePrivateProfileString("WhiteLine Thresdhold", "Int_Max", strValue, inFilename);
    strValue.Format("%d", setting->whitelines.I_min);
    WritePrivateProfileString("WhiteLine Thresdhold", "Int_Min", strValue, inFilename);

    // overball
    strValue.Format("%d", setting->overball.Hue_max);
    WritePrivateProfileString("OverBall Thresdhold", "Hue_Max", strValue, inFilename);
    strValue.Format("%d", setting->overball.Hue_min);
    WritePrivateProfileString("OverBall Thresdhold", "Hue_Min", strValue, inFilename);
    strValue.Format("%d", setting->overball.Satu_max);
    WritePrivateProfileString("OverBall Thresdhold", "Satu_Max", strValue, inFilename);
    strValue.Format("%d", setting->overball.Satu_min);
    WritePrivateProfileString("OverBall Thresdhold", "Satu_Min", strValue, inFilename);
    strValue.Format("%d", setting->overball.I_max);
    WritePrivateProfileString("OverBall Thresdhold", "Int_Max", strValue, inFilename);
    strValue.Format("%d", setting->overball.I_min);
    WritePrivateProfileString("OverBall Thresdhold", "Int_Min", strValue, inFilename);

    // teammate
    strValue.Format("%d", setting->teammate.Hue_max);
    WritePrivateProfileString("TeamMate Thresdhold", "Hue_Max", strValue, inFilename);
    strValue.Format("%d", setting->teammate.Hue_min);
    WritePrivateProfileString("TeamMate Thresdhold", "Hue_Min", strValue, inFilename);
    strValue.Format("%d", setting->teammate.Satu_max);
    WritePrivateProfileString("TeamMate Thresdhold", "Satu_Max", strValue, inFilename);
    strValue.Format("%d", setting->teammate.Satu_min);
    WritePrivateProfileString("TeamMate Thresdhold", "Satu_Min", strValue, inFilename);
    strValue.Format("%d", setting->teammate.I_max);
    WritePrivateProfileString("TeamMate Thresdhold", "Int_Max", strValue, inFilename);
    strValue.Format("%d", setting->teammate.I_min);
    WritePrivateProfileString("TeamMate Thresdhold", "Int_Min", strValue, inFilename);

    // opponent
    strValue.Format("%d", setting->opponent.Hue_max);
    WritePrivateProfileString("Opponent Thresdhold", "Hue_Max", strValue, inFilename);
    strValue.Format("%d", setting->opponent.Hue_min);
    WritePrivateProfileString("Opponent Thresdhold", "Hue_Min", strValue, inFilename);
    strValue.Format("%d", setting->opponent.Satu_max);
    WritePrivateProfileString("Opponent Thresdhold", "Satu_Max", strValue, inFilename);
    strValue.Format("%d", setting->opponent.Satu_min);
    WritePrivateProfileString("Opponent Thresdhold", "Satu_Min", strValue, inFilename);
    strValue.Format("%d", setting->opponent.I_max);
    WritePrivateProfileString("Opponent Thresdhold", "Int_Max", strValue, inFilename);
    strValue.Format("%d", setting->opponent.I_min);
    WritePrivateProfileString("Opponent Thresdhold", "Int_Min", strValue, inFilename);
}

BOOL VoyHSI::LoadHSIFromINI(CString inFilename, HSISet *setting, CListBox *inList)
{
    if (setting == NULL)
    {
        return FALSE;
    }
    CFileFind finder;
    if (finder.FindFile(inFilename) == FALSE)
    {
        AfxMessageBox("该文件不存在！");
        return FALSE;
    }
    // black
    setting->black.Hue_max = (int)GetPrivateProfileInt("Black Thresdhold", "Hue_Max", 255, inFilename);
    setting->black.Hue_min = (int)GetPrivateProfileInt("Black Thresdhold", "Hue_Min", 0, inFilename);
    setting->black.Satu_max = (int)GetPrivateProfileInt("Black Thresdhold", "Satu_Max", 255, inFilename);
    setting->black.Satu_min = (int)GetPrivateProfileInt("Black Thresdhold", "Satu_Min", 0, inFilename);
    setting->black.I_max = (int)GetPrivateProfileInt("Black Thresdhold", "Int_Max", 255, inFilename);
    setting->black.I_min = (int)GetPrivateProfileInt("Black Thresdhold", "Int_Min", 0, inFilename);

    // green
    setting->green.Hue_max = (int)GetPrivateProfileInt("Green Thresdhold", "Hue_Max", 255, inFilename);
    setting->green.Hue_min = (int)GetPrivateProfileInt("Green Thresdhold", "Hue_Min", 0, inFilename);
    setting->green.Satu_max = (int)GetPrivateProfileInt("Green Thresdhold", "Satu_Max", 255, inFilename);
    setting->green.Satu_min = (int)GetPrivateProfileInt("Green Thresdhold", "Satu_Min", 0, inFilename);
    setting->green.I_max = (int)GetPrivateProfileInt("Green Thresdhold", "Int_Max", 255, inFilename);
    setting->green.I_min = (int)GetPrivateProfileInt("Green Thresdhold", "Int_Min", 0, inFilename);

    // whiteline
    setting->whitelines.Hue_max = (int)GetPrivateProfileInt("WhiteLine Thresdhold", "Hue_Max", 255, inFilename);
    setting->whitelines.Hue_min = (int)GetPrivateProfileInt("WhiteLine Thresdhold", "Hue_Min", 0, inFilename);
    setting->whitelines.Satu_max = (int)GetPrivateProfileInt("WhiteLine Thresdhold", "Satu_Max", 255, inFilename);
    setting->whitelines.Satu_min = (int)GetPrivateProfileInt("WhiteLine Thresdhold", "Satu_Min", 0, inFilename);
    setting->whitelines.I_max = (int)GetPrivateProfileInt("WhiteLine Thresdhold", "Int_Max", 255, inFilename);
    setting->whitelines.I_min = (int)GetPrivateProfileInt("WhiteLine Thresdhold", "Int_Min", 0, inFilename);

    // overball
    setting->overball.Hue_max = (int)GetPrivateProfileInt("OverBall Thresdhold", "Hue_Max", 255, inFilename);
    setting->overball.Hue_min = (int)GetPrivateProfileInt("OverBall Thresdhold", "Hue_Min", 0, inFilename);
    setting->overball.Satu_max = (int)GetPrivateProfileInt("OverBall Thresdhold", "Satu_Max", 255, inFilename);
    setting->overball.Satu_min = (int)GetPrivateProfileInt("OverBall Thresdhold", "Satu_Min", 0, inFilename);
    setting->overball.I_max = (int)GetPrivateProfileInt("OverBall Thresdhold", "Int_Max", 255, inFilename);
    setting->overball.I_min = (int)GetPrivateProfileInt("OverBall Thresdhold", "Int_Min", 0, inFilename);

    // teammate
    setting->teammate.Hue_max = (int)GetPrivateProfileInt("TeamMate Thresdhold", "Hue_Max", 255, inFilename);
    setting->teammate.Hue_min = (int)GetPrivateProfileInt("TeamMate Thresdhold", "Hue_Min", 0, inFilename);
    setting->teammate.Satu_max = (int)GetPrivateProfileInt("TeamMate Thresdhold", "Satu_Max", 255, inFilename);
    setting->teammate.Satu_min = (int)GetPrivateProfileInt("TeamMate Thresdhold", "Satu_Min", 0, inFilename);
    setting->teammate.I_max = (int)GetPrivateProfileInt("TeamMate Thresdhold", "Int_Max", 255, inFilename);
    setting->teammate.I_min = (int)GetPrivateProfileInt("TeamMate Thresdhold", "Int_Min", 0, inFilename);

    // opponent
    setting->opponent.Hue_max = (int)GetPrivateProfileInt("Opponent Thresdhold", "Hue_Max", 255, inFilename);
    setting->opponent.Hue_min = (int)GetPrivateProfileInt("Opponent Thresdhold", "Hue_Min", 0, inFilename);
    setting->opponent.Satu_max = (int)GetPrivateProfileInt("Opponent Thresdhold", "Satu_Max", 255, inFilename);
    setting->opponent.Satu_min = (int)GetPrivateProfileInt("Opponent Thresdhold", "Satu_Min", 0, inFilename);
    setting->opponent.I_max = (int)GetPrivateProfileInt("Opponent Thresdhold", "Int_Max", 255, inFilename);
    setting->opponent.I_min = (int)GetPrivateProfileInt("Opponent Thresdhold", "Int_Min", 0, inFilename);

    char buffer[255];
    GetPrivateProfileString("Time Log", "Thresdhold", "", buffer, 255, inFilename);
    CString strTime(buffer);
    ;

    if (inList != NULL)
    {
        inList->AddString("");
        inList->AddString("[!]所读取的阈值记录时间：");
        inList->AddString(strTime);
        inList->AddString("");
        inList->SetCurSel(inList->GetCount() - 1);
    }
    return TRUE;
}
