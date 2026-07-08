#include "stdafx.h"
#include "ObstFusion_ZWJ.h"

static obstacle ObstSrc[50];
static enemy BufEnemy[20];    // 对障碍物信息进行缓存
static int nBufEnemyNum = 0;  // 缓存障碍物的数量
static enemy ResultEnemy[20]; // 追踪合并的障碍物数组
static int nNumResult = 0;    // 追踪合并的障碍物数量
CObstFusion_ZWJ::CObstFusion_ZWJ()
{
    arTrackEnemy = new enemy[100];
    arTrackCount = new int[100];
    m_ResetObstSrc();
    bEnable = true;
}

CObstFusion_ZWJ::~CObstFusion_ZWJ()
{
    delete[] arTrackEnemy;
    delete[] arTrackCount;
}

void CObstFusion_ZWJ::m_ResetObstSrc()
{
    for (int i = 0; i < 50; i++)
    {
        memset(&ObstSrc[i], 0, sizeof(obstacle));
        ObstSrc[i].valid = false;
    }

    nNumTrackEnemy = 0;
    for (i = 0; i < 100; i++)
    {
        arTrackCount[i] = 0;
    }

    for (i = 0; i < 20; i++)
    {
        BufEnemy[i].size = 0;
    }
}

void CObstFusion_ZWJ::Fusion(stDataSummary *inDataSummary)
{
    // 队员传来的障碍物数据读取自(inDataSummary->agent[i].obstacle[j])
    // 融合后的障碍物数据请写入(inDataSummary->t_enemy[i]) 数量inDataSummary->NumOfemy
    uAgent *pAgent = inDataSummary->agent;

    // 1、敌方数组大小清零
    inDataSummary->NumOfemy = 0;
    memset(inDataSummary->t_enemy, 0, sizeof(enemy) * 20);
    m_ResetObstSrc();

    // 2、从共享数据取出所有障碍物
    int nObstSrcNum = 0;
    for (int i = 0; i < AG_NUM; i++)
    {
        if (pAgent[i].online == true && pAgent[i].status != ROBST_ERR)
        {
            for (int j = 0; j < 10; j++)
            {
                if (pAgent[i].obstacle[j].valid == true)
                {
                    memcpy(&(ObstSrc[nObstSrcNum]), &(pAgent[i].obstacle[j]), sizeof(obstacle));
                    nObstSrcNum++;
                }
            }
        }
    }

    if (nObstSrcNum <= 0)
    {
        // 没有障碍物
        return;
    }

    // 计算每个障碍物到它的提供者的距离
    for (i = 0; i < nObstSrcNum; i++)
    {
        ObstSrc[i].DistToOwner =
            m_CalDist(ObstSrc[i].x, ObstSrc[i].y, pAgent[ObstSrc[i].ID].x, pAgent[ObstSrc[i].ID].y);

        // 将距离观察者很远的障碍物剔除
        /*if (ObstSrc[i].DistToOwner > 1000)
            ObstSrc[i].valid = false;*/
    }

    // 对每个障碍物进行类似障碍物的剔除
    int nNearDist = 50; // 相互距离小于这个的认为是同一个障碍物
    for (i = 0; i < nObstSrcNum; i++)
    {
        if (ObstSrc[i].valid == true)
        {
            // i障碍物有效，遍历后面的障碍物，寻找跟其距离较近的障碍物并进行融合
            for (int j = i + 1; j < nObstSrcNum; j++)
            {
                if (ObstSrc[j].valid == true)
                {
                    // j障碍物有效，则将其与i障碍物进行距离计算，查看是否可以被融合
                    if (m_CalDist(ObstSrc[i].x, ObstSrc[i].y, ObstSrc[j].x, ObstSrc[j].y) <= nNearDist)
                    {
                        // i障碍物和j障碍物足够近了，对其进行融合
                        if (ObstSrc[i].DistToOwner < ObstSrc[j].DistToOwner)
                        {
                            // i障碍物与观察者比j障碍物近，剔除j保留i
                            ObstSrc[j].valid = false;
                        }
                        else
                        {
                            // i障碍物与观察者比j障碍物远，剔除i保留j
                            ObstSrc[i].valid = false;
                            // i被剔除了，该退出此循环了
                            break;
                        }
                    }
                }
            }
        }
    }

    // 将融合后的障碍物复制到共享数组中去
    int nTmpEnemyNum = 0;
    for (i = 0; i < nObstSrcNum; i++)
    {
        if (ObstSrc[i].valid == true)
        {
            inDataSummary->t_enemy[nTmpEnemyNum].x = ObstSrc[i].x;
            inDataSummary->t_enemy[nTmpEnemyNum].y = ObstSrc[i].y;
            inDataSummary->t_enemy[nTmpEnemyNum].size = ObstSrc[i].size;
            inDataSummary->t_enemy[nTmpEnemyNum].clst_id = ObstSrc[i].ID;
            nTmpEnemyNum++;
            if (nTmpEnemyNum >= 20)
            {
                // enemy超限
                break;
            }
        }
    }
    inDataSummary->NumOfemy = nTmpEnemyNum;
}

int CObstFusion_ZWJ::m_CalDist(int inx1, int iny1, int inx2, int iny2)
{
    double ret = (inx1 - inx2) * (inx1 - inx2) + (iny1 - iny2) * (iny1 - iny2);
    ret = (int)sqrt(ret);
    return (int)ret;
}

static enemy enemy_NotSort[20];
static enemy tmpEnemy;
void CObstFusion_ZWJ::SortEnemyDistToOurGoal(stDataSummary *inDataSummary)
{
    if (inDataSummary->NumOfemy == 0)
    {
        return;
    }
    // 拷贝数据
    memcpy(enemy_NotSort, inDataSummary->t_enemy, 20 * sizeof(enemy));

    int nMinDist = 5000;
    int nTmpDist = 0;
    int nMinIndex = 0;
    // 冒泡法排序
    for (int k = 0; k < inDataSummary->NumOfemy; k++)
    {
        nMinDist = 5000;
        for (int i = k; i < inDataSummary->NumOfemy; i++)
        {
            // 找出当前队长与球门距离最近的障碍物，并复制到队首（k是队首）
            nTmpDist = m_CalDist(enemy_NotSort[i].x, enemy_NotSort[i].y, 0, 600);
            if (nTmpDist < nMinDist)
            {
                nMinDist = nTmpDist;
                nMinIndex = i;
            }
        }

        // 复制（k是队首）
        if (nMinIndex > k)
        {
            memcpy(&tmpEnemy, &enemy_NotSort[k], sizeof(enemy));
            memcpy(&enemy_NotSort[k], &enemy_NotSort[nMinIndex], sizeof(enemy));
            memcpy(&enemy_NotSort[nMinIndex], &tmpEnemy, sizeof(enemy));
        }
    }

    // 将排序结果复制回去
    memcpy(inDataSummary->t_enemy, enemy_NotSort, 20 * sizeof(enemy));
}

static int nDistTrack = 100;
static int nMaxTrackCount = 20;
static int nValidThreshold = 7;
// 对已追踪列表里的单个障碍物进行分析，查看其是否仍然值得追踪
void CObstFusion_ZWJ::UpdateTrackEnemy(int inTrackIndex)
{
    // 对障碍物进行挨个计数更新
    bool bGetTracked = false;
    int nNewPosIndex = 0;
    for (int i = 0; i < nBufEnemyNum; i++)
    {
        if (BufEnemy[i].size > 0) // 需要确认标记有效
        {
            if (m_CalDist(arTrackEnemy[inTrackIndex].x, arTrackEnemy[inTrackIndex].y, BufEnemy[i].x, BufEnemy[i].y) <
                nDistTrack)
            {
                nNewPosIndex = i;
                bGetTracked = true;
                break;
            }
        }
    }

    if (bGetTracked == true)
    {
        // 已经跟踪上了，跟踪计数累加
        arTrackEnemy[inTrackIndex].x = BufEnemy[nNewPosIndex].x;
        arTrackEnemy[inTrackIndex].y = BufEnemy[nNewPosIndex].y;
        arTrackCount[inTrackIndex]++;
        if (arTrackCount[inTrackIndex] > nMaxTrackCount)
        {
            arTrackCount[inTrackIndex] = nMaxTrackCount;
        }
        // 对新选中的已经被跟踪的障碍物进行标记（标记这个已经跟踪，方便最后对未被跟踪的目标进行新添加）
        BufEnemy[nNewPosIndex].size = 0;
    }
    else
    {
        // 未跟踪上，跟踪计数减一，若减少到零则，对其进行清除
        arTrackCount[inTrackIndex] -= 1;
        if (arTrackCount[inTrackIndex] <= 0)
        {
            arTrackCount[inTrackIndex] = 0;
            RemoveTrack(inTrackIndex);
        }
    }
}

// 移除某个追踪目标障碍物
void CObstFusion_ZWJ::RemoveTrack(int inTrackIndex)
{
    if (inTrackIndex > nNumTrackEnemy)
    {
        return;
    }

    for (int i = inTrackIndex; i < nNumTrackEnemy - 1; i++)
    {
        memcpy(&(arTrackEnemy[i]), &(arTrackEnemy[i + 1]), sizeof(enemy));
        memcpy(&(arTrackCount[i]), &(arTrackCount[i + 1]), sizeof(int));
    }
    nNumTrackEnemy -= 1;
}

// 添加某个追踪障碍物
void CObstFusion_ZWJ::AddTrackEnemy(enemy *inTmpEnemyToAdd)
{
    if (nNumTrackEnemy < 100)
    {
        memcpy(&arTrackEnemy[nNumTrackEnemy], inTmpEnemyToAdd, sizeof(enemy));
        arTrackCount[nNumTrackEnemy] = 1;
        nNumTrackEnemy++;
    }
}

// 开启全局障碍物追踪模式
void CObstFusion_ZWJ::TrackEnemy(stDataSummary *inDataSummary)
{
    if (bEnable == false)
    {
        nNumTrackEnemy = 0;
        return;
    }

    // 将障碍物数据缓存下来
    memcpy(BufEnemy, inDataSummary->t_enemy, 20 * sizeof(enemy));
    nBufEnemyNum = inDataSummary->NumOfemy;

    for (int i = 0; i < inDataSummary->NumOfemy; i++)
    {
        if (BufEnemy[i].size <= 0)
        {
            BufEnemy[i].size = 1;
        }
    }

    // 对已经跟踪上的障碍物进行处理
    for (i = 0; i < nNumTrackEnemy; i++)
    {
        UpdateTrackEnemy(i);
    }
    // 对未处理的障碍物进行新增操作
    for (i = 0; i < 20; i++)
    {
        if (BufEnemy[i].size > 0)
        {
            // 新增障碍物
            AddTrackEnemy(&BufEnemy[i]);
        }
    }

    //(不需要对坐标接近的障碍物进行合并，因为在障碍物数据更新时，会将跟踪后的障碍物size设为0，另一个影子障碍物会计数逐减并消失)

    // 复制障碍物结果对不符合阈值条件的障碍物进行剔除
    nNumResult = 0;
    for (i = 0; i < nNumTrackEnemy; i++)
    {
        if (arTrackCount[i] > nValidThreshold)
        {
            memcpy(&ResultEnemy[nNumResult], &arTrackEnemy[i], sizeof(enemy));
            nNumResult++;
        }
    }
    // 追踪后的结果放置在ResultEnemy数组里
}

// 开启全局障碍物追踪模式
void CObstFusion_ZWJ::SetResult(stDataSummary *inDataSummary)
{
    if (bEnable == false)
    {
        return;
    }
    for (int i = 0; i < nNumResult; i++)
    {
        memcpy(&(inDataSummary->t_enemy[i]), &(ResultEnemy[i]), sizeof(enemy));
    }
    inDataSummary->NumOfemy = nNumResult;
}

void CObstFusion_ZWJ::FindObstHoldBallInNoSeen(stDataSummary *inDataSummary)
{
    int nMinDist = 5000;
    int nMinObstIndex = -1;
    int nTarget_x = inDataSummary->cGlobalBall.x;
    int nTarget_y = inDataSummary->cGlobalBall.y;
    if (inDataSummary->bNoOneSeeBall == true)
    {
        int nInDistObstHoldBallToBall = m_CalDist(nTarget_x, nTarget_y, inDataSummary->nObstHoldBallInNoSeen_x,
                                                  inDataSummary->nObstHoldBallInNoSeen_y);
        if (nInDistObstHoldBallToBall < 200)
        {
            nTarget_x = inDataSummary->nObstHoldBallInNoSeen_x;
            nTarget_y = inDataSummary->nObstHoldBallInNoSeen_y;
        }
    }
    for (int i = 0; i < inDataSummary->NumOfemy; i++)
    {
        int nDistToBall = m_CalDist(inDataSummary->t_enemy[i].x, inDataSummary->t_enemy[i].y, nTarget_x, nTarget_y);
        if (nDistToBall < nMinDist)
        {
            nMinDist = nDistToBall;
            nMinObstIndex = i;
        }
    }
    if (nMinObstIndex > 0 && nMinObstIndex < 20)
    {
        inDataSummary->nObstHoldBallInNoSeen_x = inDataSummary->t_enemy[nMinObstIndex].x;
        inDataSummary->nObstHoldBallInNoSeen_y = inDataSummary->t_enemy[nMinObstIndex].y;
    }
    else
    {
        inDataSummary->nObstHoldBallInNoSeen_x = inDataSummary->cGlobalBall.x;
        inDataSummary->nObstHoldBallInNoSeen_y = inDataSummary->cGlobalBall.y;
    }
}
