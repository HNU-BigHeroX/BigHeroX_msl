#include "stdafx.h"
#include "BallMoveEstimation.h"

// static bool bDispaly = true;  //是否显示调试窗口
static bool bDispaly = false; // 不显示调试窗口

// static cv::Mat image = cv::Mat::zeros(120, 180, CV_8UC3);
//
// static void beLine(cv::Mat img, cv::Point start, cv::Point end,int b,int g,int r)
// {
// 	int thickness = 1;
// 	int lineType = 8;
// 	line(img,
// 		start,
// 		end,
// 		cv::Scalar(b, g, r),
// 		thickness,
// 		lineType);
// }

CBallMoveEstimation::CBallMoveEstimation()
{
    pDataSummary = NULL;
    m_arBallLog = NULL;
    m_arDisplay = NULL;

    if (bDispaly == true)
    {
        /// Create Windows
        // 		cv::namedWindow("CBallMoveEstimation", 0);
        // 		cv::resizeWindow("CBallMoveEstimation", 540, 360);
        // 		imshow("CBallMoveEstimation", image);
    }
}

CBallMoveEstimation::~CBallMoveEstimation()
{
    ReleaseData();
}

void CBallMoveEstimation::Init(int inLen)
{
    ReleaseData();
    // 	m_arBallLog = new cv::Point[inLen];
    // 	m_arDisplay = new cv::Point[inLen];
    // 	for (int i = 0; i < inLen;i++)
    // 	{
    // 		m_arBallLog[i].x = 0;
    // 		m_arBallLog[i].y = 0;
    // 		m_arDisplay[i].x = 0;
    // 		m_arDisplay[i].y = 0;
    // 	}
    m_nBufLen = inLen;
    m_nNum = 0;
}

void CBallMoveEstimation::ReleaseData()
{
    m_nBufLen = 0;
    m_nNum = 0;
    if (m_arBallLog != NULL)
    {
        delete[] m_arBallLog;
        m_arBallLog = NULL;
    }

    if (m_arDisplay != NULL)
    {
        delete[] m_arDisplay;
        m_arDisplay = NULL;
    }
}

void CBallMoveEstimation::Add(int inX, int inY)
{
    // 将所有数据往后移
    for (int i = m_nNum - 1; i > 0; i--)
    {
        m_arBallLog[i].x = m_arBallLog[i - 1].x;
        m_arBallLog[i].y = m_arBallLog[i - 1].y;
    }

    // 新加记录放在第一个位置
    m_arBallLog[0].x = inX;
    m_arBallLog[0].y = inY;

    if (m_nNum < m_nBufLen)
    {
        m_nNum++;
    }

    if (bDispaly == true)
    {
        CvDisplay();
    }
}

double CBallMoveEstimation::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    double ret = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    ret = (int)sqrt(ret);
    return ret;
}

CPoint CBallMoveEstimation::m_Loca_Ray(int inXbase, int inYbase, int inXtarg, int inYtarg, int inDist)
{
    CPoint newpoint;
    int srcDist = (int)m_CalDist(inXbase, inYbase, inXtarg, inYtarg);
    if (srcDist == 0)
    {
        newpoint.x = inXbase;
        newpoint.y = inYbase;
        return newpoint;
    }
    int dx = (int)((double)(inXtarg - inXbase) * inDist / srcDist);
    int dy = (int)((double)(inYtarg - inYbase) * inDist / srcDist);
    newpoint.x = inXbase + dx;
    newpoint.y = inYbase + dy;

    return newpoint;
}

// 计算两个角度的夹角
int CBallMoveEstimation::m_CalIncludedAngle(int inAngle1, int inAngle2)
{
    int retAngle = 0;
    int largeAngle, smallAngle;
    if (inAngle1 > inAngle2)
    {
        largeAngle = inAngle1;
        smallAngle = inAngle2;
    }
    else
    {
        largeAngle = inAngle2;
        smallAngle = inAngle1;
    }

    int includAngle_1 = largeAngle - smallAngle;
    int includAngle_2 = smallAngle + 360 - largeAngle;

    if (includAngle_1 < includAngle_2)
    {
        retAngle = includAngle_1;
    }
    else
    {
        retAngle = includAngle_2;
    }
    return retAngle;
}

CPoint CBallMoveEstimation::m_Loca_Rotate(int inx, int iny, int inCx, int inCy, int inAng)
{
    CPoint newpoint;
    if (inAng % 360 == 0)
    {
        newpoint.x = inx;
        newpoint.y = iny;
        return newpoint;
    }

    // 将角度换算成数值
    double vAng = (double)inAng * 3.14159 / 180;

    newpoint.x = (int)((inx - inCx) * cos(vAng) + (iny - inCy) * sin(vAng) + inCx);
    newpoint.y = (int)(-(inx - inCx) * sin(vAng) + (iny - inCy) * cos(vAng) + inCy);

    return newpoint;
}

int CBallMoveEstimation::m_Loca_CalAngle(int inXbase, int inYbase, int inXtarg, int inYtarg)
{
    double retAngle;
    if (inXbase != inXtarg)
    {
        // 分母不为零
        int dx = inXtarg - inXbase;
        double dy = inYtarg - inYbase;
        retAngle = atan(dy / dx);

        // 换算成角度
        retAngle = retAngle * 180 / 3.14158;

        // 象限的修正
        if (retAngle < 0)
        {
            if (dy > 0)
            {
                retAngle += 180;
            }
        }
        else
        {
            if (dy < 0)
            {
                retAngle += 180;
            }
        }

        // 修正
        while (retAngle < 0)
        {
            retAngle += 360;
        }
        while (retAngle > 360)
        {
            retAngle -= 360;
        }

        // 修正
        if (dy == 0)
        {
            if (dx > 0)
            {
                retAngle = 0;
            }
            else
            {
                retAngle = 180;
            }
        }
    }
    else
    {
        // inXbase == inXtarg
        if (inYtarg > inYbase)
        {
            retAngle = 90;
        }
        else
        {
            retAngle = 270;
        }
    }

    return (int)retAngle;
}

static int nDistRange = 50;
static int nCalMax = 10;
void CBallMoveEstimation::Estimation()
{
    if (pDataSummary == NULL)
    {
        // 指针为空
        return;
    }

    if (m_nNum < nCalMax)
    {
        // 采样数据还不够
        pDataSummary->posPassToEnemy[0].x = 500;
        pDataSummary->posPassToEnemy[0].y = 600;
        pDataSummary->nNumOfPassToEnemy = 0;
        pDataSummary->nNumOfPassToTeammate = 0;
        return;
    }

    if (pDataSummary->NumOfemy <= 0)
    {
        // 未检测到障碍物
        pDataSummary->posPassToEnemy[0].x = 500;
        pDataSummary->posPassToEnemy[0].y = 600;
        pDataSummary->nNumOfPassToEnemy = 0;
        return;
    }

    // 读取最新一个点
    int start_x = m_arBallLog[0].x;
    int start_y = m_arBallLog[0].y;
    pntDirect.x = start_x;
    pntDirect.y = start_y;
    int dist = 0;

    for (int i = 1; i < nCalMax; i++)
    {
        dist = m_CalDist(start_x, start_y, m_arBallLog[i].x, m_arBallLog[i].y);
        if (dist > nDistRange)
        {
            // 与起始点距离足够远，计算朝向角
            // pntDirect = m_Loca_Ray(start_x, start_y, m_arBallLog[i].x, m_arBallLog[i].y, dist);
            pntDirect = m_Loca_Rotate(m_arBallLog[i].x, m_arBallLog[i].y, start_x, start_y, 180);
            break;
        }
    }

    // 查看是否有计算出滚动方向
    if (pntDirect.x == start_x && pntDirect.y == start_y)
    {
        // 球移动距离太短，没有计算出移动方向
        pDataSummary->posPassToEnemy[0].x = 500;
        pDataSummary->posPassToEnemy[0].y = 600;
        pDataSummary->nNumOfPassToEnemy = 0;
        pDataSummary->nNumOfPassToTeammate = 0;
        return;
    }

    // 已经计算出运动朝向，计算球滚动方向角
    int BallAngle = m_Loca_CalAngle(start_x, start_y, pntDirect.x, pntDirect.y);

    /////////////////////////////////////////////////////////////////////////////////
    if (pDataSummary->NumOfemy > 0)
    {
        // 接着计算球到几个障碍物的朝向角及障碍物朝向角与球滚动方向角的夹角
        int nNumOfEnemy = 5;
        if (pDataSummary->NumOfemy < 5)
        {
            nNumOfEnemy = pDataSummary->NumOfemy;
        }
        int emenyAngle[5];
        int IncludedAngleWithObst[5];
        for (i = 0; i < nNumOfEnemy; i++)
        {
            emenyAngle[i] = m_Loca_CalAngle(start_x, start_y, pDataSummary->t_enemy[i].x, pDataSummary->t_enemy[i].y);
            IncludedAngleWithObst[i] = m_CalIncludedAngle(BallAngle, emenyAngle[i]);
        }
        // 从障碍物夹角里挑选夹角最小的障碍物index
        int nIndex = 0;
        int nMinIncludeAngle = 360;
        for (i = 0; i < nNumOfEnemy; i++)
        {
            if (IncludedAngleWithObst[i] < nMinIncludeAngle)
            {
                nMinIncludeAngle = IncludedAngleWithObst[i];
                nIndex = i;
            }
        }

        // 对共享数据中的球滚向障碍物坐标进行赋值
        if (nMinIncludeAngle < 30)
        {
            // 最小夹角小于一定角度才是有传球嫌疑对象
            pDataSummary->posPassToEnemy[0].x = pDataSummary->t_enemy[nIndex].x;
            pDataSummary->posPassToEnemy[0].y = pDataSummary->t_enemy[nIndex].y;
            pDataSummary->nNumOfPassToEnemy = 1;
        }
        else
        {
            pDataSummary->nNumOfPassToEnemy = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    // 接着计算球到几个队友的朝向角及队友朝向角与球滚动方向角的夹角
    int TeammateAngle[5];
    int IncludedAngleWithTeammate[5];
    int nAgentIndex[5];
    int nNumOfTeammate = 0;
    for (i = 1; i < AG_NUM; i++)
    {
        if (pDataSummary->agent[i].online == true)
        {
            TeammateAngle[nNumOfTeammate] =
                m_Loca_CalAngle(start_x, start_y, pDataSummary->agent[i].x, pDataSummary->agent[i].y);
            IncludedAngleWithTeammate[nNumOfTeammate] = m_CalIncludedAngle(BallAngle, TeammateAngle[nNumOfTeammate]);
            nAgentIndex[nNumOfTeammate] = i;
            nNumOfTeammate++;
        }
    }
    if (nNumOfTeammate > 0)
    {
        // 从障碍物夹角里挑选夹角最小的队友index
        int nIndex = 0;
        int nMinIncludeAngle = 360;
        for (int i = 0; i < nNumOfTeammate; i++)
        {
            if (IncludedAngleWithTeammate[i] < nMinIncludeAngle)
            {
                nMinIncludeAngle = IncludedAngleWithTeammate[i];
                nIndex = nAgentIndex[i];
            }
        }

        // 对共享数据中的球滚向队友坐标进行赋值
        if (nMinIncludeAngle < 30)
        {
            // 最小夹角小于一定角度才能有接球嫌疑
            pDataSummary->posPassToTeammate.x = pDataSummary->agent[nIndex].x;
            pDataSummary->posPassToTeammate.y = pDataSummary->agent[nIndex].y;
            pDataSummary->nNumOfPassToTeammate = 1;
        }
        else
        {
            pDataSummary->nNumOfPassToTeammate = 0;
        }
    }
    else
    {
        pDataSummary->nNumOfPassToTeammate = 0;
    }
}

void CBallMoveEstimation::CvDisplay()
{
    // 背景涂黑
    // image = cv::Scalar(0, 0, 0);
    // 画球轨迹
    for (int i = 0; i < m_nNum; i++)
    {
        m_arDisplay[i].x = m_arBallLog[i].x / 10;
        m_arDisplay[i].y = 120 - m_arBallLog[i].y / 10;

        if (i > 0)
        {
            //			beLine(image, m_arDisplay[i - 1], m_arDisplay[i], 255 - i * 2,0 , 255 - i * 2);
        }
    }
    pntDisDirect.x = pntDirect.x / 10;
    pntDisDirect.y = 120 - pntDirect.y / 10;
    // 画球运动方向
    //	beLine(image, m_arDisplay[0], pntDisDirect, 0, 255, 0);
    // 显示
    //	imshow("CBallMoveEstimation", image);
}
