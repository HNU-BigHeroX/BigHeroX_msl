// TxtArray.cpp: implementation of the CTxtArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TxtArray.h"
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTxtArray::CTxtArray()
{
    pList = NULL;
    pbAssign = NULL;

    m_nReadLen = 0;
}

CTxtArray::~CTxtArray()
{
    if (pbAssign != NULL)
    {
        delete[] pbAssign;
    }
}

int CTxtArray::ReadFromFile(CString inFilename, int *inArr, int inLen)
{
    if ((access(inFilename, 0)) == -1)
    {
        CString str("文件不存在！");
        str = inFilename + str;

        AfxMessageBox(str);
        return 0;
    }
    CStdioFile mFile;
    CFileException mExcept;
    BOOL res = mFile.Open(inFilename, CFile::modeRead, &mExcept);
    if (res == FALSE)
    {
        CString str("文件读取失败！");
        str = inFilename + str;

        AfxMessageBox(str);
        return 0;
    }
    CString strRead;
    CString strTmp;
    int pos = 0;
    int index, val;
    int listIndex = 0;
    m_nReadLen = 0;
    if (pList != NULL)
    {
        pList->DeleteAllItems();
    }
    // 读取文件
    while (mFile.ReadString(strRead))
    {
        // 查找“：”
        pos = strRead.Find("=");
        if (pos < 0)
        {
            // 数组长度
            pos = strRead.Find("Len:");
            if (pos >= 0)
            {
                strTmp = strRead.Right(strRead.GetLength() - pos - 4);
                m_nReadLen = atoi(strTmp);
                if (m_nReadLen <= 0)
                {
                    return 0;
                }

                if (m_nReadLen > inLen)
                {
                    AfxMessageBox("将要读取的数组比存储数值长，将按储存长度为准");
                    m_nReadLen = inLen;
                }

                // 是否读取数值的标记变量
                if (pbAssign != NULL)
                {
                    delete[] pbAssign;
                    pbAssign = NULL;
                }
                pbAssign = new bool[m_nReadLen];
                for (int i = 0; i < m_nReadLen; i++)
                {
                    pbAssign[i] = false;
                }
            }
        }
        else
        {
            // 读取数值
            strTmp = strRead.Left(pos);
            index = atoi(strTmp);
            strTmp = strRead.Right(strRead.GetLength() - pos - 1);
            val = atoi(strTmp);
            if (index < m_nReadLen)
            {
                inArr[index] = val;
                pbAssign[index] = true;
            }

            // 显示
            if (pList != NULL)
            {
                strTmp.Format("%d", index);
                pList->InsertItem(listIndex, strTmp);
                //	pList->SetItemText(listIndex,0,strTmp);
                strTmp.Format("%d", val);
                pList->SetItemText(listIndex, 1, strTmp);
                listIndex++;
            }
        }
    }
    mFile.Close();

    // 后期处理
    bool bRead = false;
    for (int i = 0; i < m_nReadLen; i++)
    {
        if (pbAssign[i] == true)
        {
            bRead = true;
        }
    }
    if (bRead)
    {
        AfterRead(inArr, m_nReadLen);
    }
    return m_nReadLen;
}

bool CTxtArray::WriteFile(CString inFilename, UCHAR *inArr, int inLen)
{
    CStdioFile mFile;
    CFileException mExcept;
    mFile.Open(inFilename, CFile::modeCreate | CFile::modeWrite, &mExcept);
    CString string;
    string.Format("Len:%d\r\n", inLen);
    mFile.WriteString(string);
    for (int i = 0; i < inLen; i++)
    {
        string.Format("%d=%d\r\n", i, inArr[i]);
        mFile.WriteString(string);
    }
    mFile.Close();
    return true;
}

bool CTxtArray::WriteIntFile(CString inFilename, int *inArr, int inLen)
{
    CStdioFile mFile;
    CFileException mExcept;
    // 打开
    mFile.Open(inFilename, CFile::modeCreate | CFile::modeWrite, &mExcept);
    CString string;
    // 写入长度
    string.Format("Len:%d\n", inLen);
    mFile.WriteString(string);
    // 写入内容
    for (int i = 0; i < inLen; i++)
    {
        string.Format("%d=%d\n", i, inArr[i]);
        mFile.WriteString(string);
    }
    // 关闭
    mFile.Close();
    return true;
}

void CTxtArray::AfterRead(void *inArr, int inLen)
{
    int lastIndex = 0;
    int step = 0;
    int *pArr = (int *)inArr;
    for (int i = 1; i < m_nReadLen; i++)
    {
        if (pbAssign[i] == true)
        {
            // 将i到last这段值进行插值
            for (int j = lastIndex; j <= i; j++)
            {
                pArr[j] = pArr[lastIndex] + (pArr[i] - pArr[lastIndex]) * (j - lastIndex) / (i - lastIndex);
            }
            lastIndex = i;
        }
    }
}
