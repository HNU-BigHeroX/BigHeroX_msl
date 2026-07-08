// RefClient.cpp: implementation of the CRefClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RefClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRefClient::CRefClient()
{
    pRec = NULL;
    m_lastflag = '0';
    // pfield = NULL;
    Self = CYAN;

    m_nLenofStr = 0;
    m_eGameFlow = BeforeGame;

    pDataCenter = NULL;
    m_bConnected = false;
}

CRefClient::~CRefClient()
{
}

void CRefClient::Received(void *inbuf, int inlen)
{
    pRec = (char *)inbuf;
    if (inlen < 2)
    {
        Parse(*pRec);
    }
    else
    {
        ////////////////////////////
        // Ö±½ÓÏÔÊ¾×Ö·û´®
        /*char* arInfo = new char[inlen + 1];
        memcpy(arInfo, pRec, inlen);
        arInfo[inlen] = '\0';
        CString str(arInfo);
        printf(str);
        delete[]arInfo;*/
        //////////////////////////////////////
        for (int i = 0; i < inlen; i++)
        {
            Parse(pRec[i]);
        }
    }
    // 	m_strMessage.Format("rec %s ,%d",pRec,inlen);
    // 	printf(m_strMessage);
}

void CRefClient::Parse(char inchar)
{
    // message
    if (m_nLenofStr > 0)
    {
        CString strTmp;
        strTmp.Format("%c", inchar);
        m_strMessage += strTmp;
        m_nLenofStr--;
        if (m_nLenofStr == 0)
        {
            m_strMessage = L"[MSG]:" + m_strMessage;
            printf(m_strMessage);
        }
        return;
    }

    switch (inchar)
    {
    case COMM_START:
        //	printf("COMM_START");
        //	Ref_Start();
        pDataCenter->ChangeMatchModeTo(MATCH_START);
        break;
    case COMM_STOP:
        printf(L"COMM_STOP");
        pDataCenter->ChangeMatchModeTo(MATCH_STOP);
        break;

    case COMM_HALT:
        printf(L"COMM_HALT");
        pDataCenter->ChangeMatchModeTo(MATCH_STOP);
        break;

    case COMM_READY:
        printf(L"COMM_READY");
        break;

    case COMM_PARKING:
        printf(L"COMM_PARKING");
        if (m_eGameFlow != EndGame)
        {
            pDataCenter->ChangeMatchModeTo(MATCH_PARK_IN);
        }
        break;

        // game flow commands
    case COMM_FIRST_HALF:
        printf(L"COMM_FIRST_HALF");
        m_eGameFlow = FirstHalf;
        break;
    case COMM_HALF_TIME:
        printf(L"COMM_HALF_TIME");
        m_eGameFlow = HalfTime;
        break;
    case COMM_SECOND_HALF:
        printf(L"COMM_SECOND_HALF");
        m_eGameFlow = SecondHalf;
        break;
    case COMM_END_GAME:
        printf(L"COMM_END_GAME");
        m_eGameFlow = EndGame;
        break;
    case COMM_CANCEL:
        printf(L"COMM_CANCEL");
        break;

        // goal status
    case COMM_GOAL_MAGENTA:
        printf(L"COMM_GOAL_MAGENTA");
        switch (Self)
        {
        case CYAN:
            break;
        case MAGENTA:
            break;
        }
        break;
    case COMM_GOAL_CYAN:
        printf(L"COMM_GOAL_CYAN");
        switch (Self)
        {
        case CYAN:
            break;
        case MAGENTA:
            break;
        }
        break;
    case COMM_SUBGOAL_MAGENTA:
        printf(L"COMM_SUBGOAL_MAGENTA");
        switch (Self)
        {
        case CYAN:
            break;
        case MAGENTA:
            break;
        }
        break;
    case COMM_SUBGOAL_CYAN:
        printf(L"COMM_SUBGOAL_CYAN");
        switch (Self)
        {
        case CYAN:
            break;
        case MAGENTA:
            break;
        }
        break;

        // game flow commands
    case COMM_RESTART:
        printf(L"COMM_RESTART");
        break;

    case COMM_KICKOFF_MAGENTA:
        printf(L"COMM_KICKOFF_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Anti_Kickoff();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_KICKOFF_READY);
            break;
        case MAGENTA:
            // pfield->REF_KickOff();
            pDataCenter->ChangeMatchModeTo(MATCH_KICKOFF_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_KICKOFF_CYAN:
        printf(L"COMM_KICKOFF_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_KickOff();
            pDataCenter->ChangeMatchModeTo(MATCH_KICKOFF_READY);
            break;
        case MAGENTA:
            // pfield->REF_Anti_Kickoff();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_KICKOFF_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_FREEKICK_MAGENTA:
        printf(L"COMM_FREEKICK_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Anti_Freekick();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_FREEKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_Freekick();
            pDataCenter->ChangeMatchModeTo(MATCH_FREEKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_FREEKICK_CYAN:
        printf(L"COMM_FREEKICK_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Freekick();
            pDataCenter->ChangeMatchModeTo(MATCH_FREEKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_Anti_Freekick();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_FREEKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_GOALKICK_MAGENTA:
        printf(L"COMM_GOALKICK_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Anti_Goalkick();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_FREEKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_Goalkick();
            pDataCenter->ChangeMatchModeTo(MATCH_FREEKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_GOALKICK_CYAN:
        printf(L"COMM_GOALKICK_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Goalkick();
            pDataCenter->ChangeMatchModeTo(MATCH_FREEKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_Anti_Goalkick();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_FREEKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_THROWIN_MAGENTA:
        printf(L"COMM_THROWIN_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Anti_Throwin();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_FREEKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_Throwin();
            pDataCenter->ChangeMatchModeTo(MATCH_FREEKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_THROWIN_CYAN:
        printf(L"COMM_THROWIN_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Throwin();
            pDataCenter->ChangeMatchModeTo(MATCH_FREEKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_Anti_Throwin();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_FREEKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_CORNER_MAGENTA:
        printf(L"COMM_CORNER_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Anti_CornerKick();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_CORNERKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_CornerKick();
            pDataCenter->ChangeMatchModeTo(MATCH_CORNERKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_CORNER_CYAN:
        printf(L"COMM_CORNER_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_CornerKick();
            pDataCenter->ChangeMatchModeTo(MATCH_CORNERKICK_READY);
            break;
        case MAGENTA:
            // pfield->REF_Anti_CornerKick();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_CORNERKICK_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_PENALTY_MAGENTA:
        printf(L"COMM_PENALTY_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Anti_Penalty();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_PENALTY_READY);
            break;
        case MAGENTA:
            // pfield->REF_Penalty();
            pDataCenter->ChangeMatchModeTo(MATCH_PENALTY_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_PENALTY_CYAN:
        printf(L"COMM_PENALTY_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->REF_Penalty();
            pDataCenter->ChangeMatchModeTo(MATCH_PENALTY_READY);
            break;
        case MAGENTA:
            // pfield->REF_Anti_Penalty();
            pDataCenter->ChangeMatchModeTo(MATCH_COUNTER_PENALTY_READY);
            break;
        }
        m_lastflag = inchar;
        break;

    case COMM_DROPPED_BALL:
        printf(L"COMM_DROPPED_BALL");
        // pfield->REF_DroppedBall();
        pDataCenter->ChangeMatchModeTo(MATCH_DROPPEDBALL_READY);
        m_lastflag = inchar;
        break;
    case COMM_DUMMY:
        printf(L"COMM_DUMMY");
        break;

    // other
    case 'W':
        m_strMessage.Format("W");
        m_nLenofStr = 8;
        break;

    case 'R':
        m_strMessage.Format("R");
        m_nLenofStr = 8;
        break;

    default:
        static CString str;
        str.Format("[unknown] %c", inchar);
        printf(str);
        break;
    }

    // pfield->Invalidate();
}

/************************************************************************/
/*                            REF_START                                 */
/************************************************************************/
void CRefClient::Ref_Start()
{
    switch (m_lastflag)
    {
    case COMM_KICKOFF_MAGENTA:
        printf(L"START_KICKOFF_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Anti_Kickoff();
            break;
        case MAGENTA:
            // pfield->START_Kickoff();
            // pDataCenter->ChangeMatchModeTo(MATCH_KICKOFF_START);
            break;
        }
        break;

    case COMM_KICKOFF_CYAN:
        printf(L"START_KICKOFF_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Kickoff();
            // pDataCenter->ChangeMatchModeTo(MATCH_KICKOFF_START);
            break;
        case MAGENTA:
            // pfield->START_Anti_Kickoff();
            break;
        }
        break;

    case COMM_FREEKICK_MAGENTA:
        printf(L"START_FREEKICK_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Anti_Freekick();
            break;
        case MAGENTA:
            // pfield->START_Freekick();
            break;
        }
        break;

    case COMM_FREEKICK_CYAN:
        printf(L"START_FREEKICK_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Freekick();
            break;
        case MAGENTA:
            // pfield->START_Anti_Freekick();
            break;
        }
        break;
    case COMM_GOALKICK_MAGENTA:
        printf(L"START_GOALKICK_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Anti_Goalkick();
            break;
        case MAGENTA:
            // pfield->START_Goalkick();
            break;
        }
        break;

    case COMM_GOALKICK_CYAN:
        printf(L"START_GOALKICK_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Goalkick();
            break;
        case MAGENTA:
            // pfield->START_Anti_Goalkick();
            break;
        }
        break;

    case COMM_THROWIN_MAGENTA:
        printf(L"START_THROWIN_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Anti_Throwin();
            break;
        case MAGENTA:
            // pfield->START_Throwin();
            break;
        }
        break;

    case COMM_THROWIN_CYAN:
        printf(L"START_THROWIN_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Throwin();
            break;
        case MAGENTA:
            // pfield->START_Anti_Throwin();
            break;
        }
        break;

    case COMM_CORNER_MAGENTA:
        printf(L"START_CORNER_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Anti_CornerKick();
            break;
        case MAGENTA:
            // pfield->START_CornerKick();
            break;
        }
        break;

    case COMM_CORNER_CYAN:
        printf(L"START_CORNER_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->START_CornerKick();
            break;
        case MAGENTA:
            // pfield->START_Anti_CornerKick();
            break;
        }
        break;

    case COMM_PENALTY_MAGENTA:
        printf(L"START_PENALTY_MAGENTA");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Anti_Penalty();
            break;
        case MAGENTA:
            // pfield->START_Penalty();
            break;
        }
        break;

    case COMM_PENALTY_CYAN:
        printf(L"START_PENALTY_CYAN");
        switch (Self)
        {
        case CYAN:
            // pfield->START_Penalty();
            break;
        case MAGENTA:
            // pfield->START_Anti_Penalty();
            break;
        }
        break;

    case COMM_DROPPED_BALL:
        printf(L"START_DROPPED_BALL");
        // pfield->START_DroppedBall();
        break;

    default:
        break;
    }
    // pfield->Invalidate();
}

void CRefClient::isDisconnected()
{
}

void CRefClient::ConnectSucceed()
{
    m_bConnected = true;
}

void CRefClient::PlayerOut(CString inTeam, int inID, int inSec)
{
    if (inTeam == "Cyan")
    {
        if (Self == CYAN)
        {
            // pfield->PR_Out(inID);
        }
    }
    else
    {
        if (Self == MAGENTA)
        {
            // pfield->PR_Out(inID);
        }
    }
}

void CRefClient::PlayerIn(CString inTeam, int inID)
{
    if (inTeam == "Cyan")
    {
        if (Self == CYAN)
        {
            // pfield->PR_Ready(inID);
        }
    }
    else
    {
        if (Self == MAGENTA)
        {
            // pfield->PR_Ready(inID);
        }
    }
}

void CRefClient::PlayerSubstitution(CString inTeam, int inIn, int inOut)
{
    if (inTeam == "Cyan")
    {
        if (Self == CYAN)
        {
            /*pfield->PR_Ready(inIn);
            pfield->PR_Out(inOut);*/
        }
    }
    else
    {
        if (Self == MAGENTA)
        {
            /*pfield->PR_Ready(inIn);
            pfield->PR_Out(inOut);*/
        }
    }
}
