// path.cpp: implementation of the Cpath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include "VoyVideo.h"
#include "path.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cpath::Cpath()
{
}

Cpath::~Cpath()
{
}

void Cpath::GetDefault(Yuzhi *yu)
{
    char fileBuffer[5010] = {0}, rBuffer[5010] = {0};
    int rBSize;
    CFile in;
    CFileDialog inDlg(TRUE, NULL, NULL, NULL, _T("自定义文件类型 (*.dat)|*.dat|所有文件 (*.*)|*.*||"), NULL);
    inDlg.m_ofn.lpstrTitle = _T("打开自定义文件");
    inDlg.m_ofn.lpstrFile = fileBuffer;
    inDlg.m_ofn.nMaxFile = 5000;
    if (inDlg.DoModal() == IDOK)
    {
        if (in.Open(inDlg.GetPathName(), CFile::modeRead))
        {
            rBSize = in.GetLength();
            // 读数据
            in.Read(rBuffer, rBSize);
            CString zz(rBuffer);
            AfxMessageBox(zz);

            yu->bh = Getfig('b', 'h', rBuffer);
            yu->bf = Getfig('b', 'f', rBuffer);
            yu->bs = Getfig('b', 's', rBuffer);
            yu->ch = Getfig('c', 'h', rBuffer);
            yu->cf = Getfig('c', 'f', rBuffer);
            yu->cs = Getfig('c', 's', rBuffer);
            yu->sh = Getfig('s', 'h', rBuffer);
            yu->ih = Getfig('i', 'h', rBuffer);
            yu->sw = Getfig('s', 'w', rBuffer);
            yu->iw = Getfig('i', 'w', rBuffer);
            if (yu->val_qiehuan == 0)
            {
                yu->dh = Getfig('l', 'h', rBuffer);
                yu->df = Getfig('l', 'f', rBuffer);
                yu->ds = Getfig('l', 's', rBuffer);
                yu->jh = Getfig('h', 'h', rBuffer);
                yu->jf = Getfig('h', 'f', rBuffer);
                yu->js = Getfig('h', 's', rBuffer);
            }
            else
            {
                yu->dh = Getfig('h', 'h', rBuffer);
                yu->df = Getfig('h', 'f', rBuffer);
                yu->ds = Getfig('h', 's', rBuffer);
                yu->jh = Getfig('l', 'h', rBuffer);
                yu->jf = Getfig('l', 'f', rBuffer);
                yu->js = Getfig('l', 's', rBuffer);
            }
        }
    }
}

int Cpath::Getfig(char a, char b, char sz[])
{
    int o;
    int p;
    int q;
    int x;
    int n;
    for (n = 0; n <= 200; n++)
    {
        if (a == sz[n - 1] && b == sz[n])
        {
            if (sz[n + 2] == ' ')
                o = 0;
            else
                o = (sz[n + 2] - 48) * 100;
            if (sz[n + 3] == ' ')
                p = 0;
            else
                p = (sz[n + 3] - 48) * 10;
            q = (sz[n + 4] - 48);
            x = o + p + q;
        }
    }
    return x;
}

void Cpath::starwrite()
{
    f.Open("mydata.txt", CFile::modeCreate | CFile::modeWrite);
}

void Cpath::write(CString str)
{
    f.WriteString(str);
}
