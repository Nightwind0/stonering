#include "TargetingState.h"
#include "BattleState.h"

using StoneRing::TargetingState;
using StoneRing::BattleState;

void TargetingState::Init(BattleState *pParent, Targetable targetable, bool bDefaultMonsters)
{
    m_pParent = pParent;
    m_targetable = targetable;
    m_bDefaultMonsters = bDefaultMonsters;
    m_pParent->StartTargeting();


    if (bDefaultMonsters)
    {
        if (m_pParent->MonstersOnLeft())
        {
            if (targetable & SINGLE)
                ChangeState(SELECT_SINGLE_LEFT);
            else
                ChangeState(SELECT_LEFT_GROUP);
        }
    }
    else
    {

        if (m_pParent->MonstersOnLeft())
        {
            if (targetable & SINGLE)
                ChangeState(SELECT_SINGLE_RIGHT);
            else
                ChangeState(SELECT_RIGHT_GROUP);
        }

    }
}

void TargetingState::ChangeState(State newState)
{
    switch (newState)
    {
    case SELECT_RIGHT_GROUP:
        m_pParent->SelectRightGroup();
        break;
    case SELECT_LEFT_GROUP:
        m_pParent->SelectLeftGroup();
        break;
    case SELECT_SINGLE_LEFT:
        m_pParent->SelectFromLeftGroup();
        break;
    case SELECT_SINGLE_RIGHT:
        m_pParent->SelectFromRightGroup();
        break;
    }

    m_state = newState;
}

bool TargetingState::IsDone() const
{
    return m_bDone;
}

void TargetingState::HandleKeyDown(const CL_InputEvent &key)
{
    switch (key.id)
    {
    case CL_KEY_DOWN:
        m_pParent->SelectNextTarget();
        break;
    case CL_KEY_UP:
        m_pParent->SelectPreviousTarget();
        break;


    }
}

void TargetingState::HandleKeyUp(const CL_InputEvent &key)
{
    switch (key.id)
    {
    case CL_KEY_LEFT:
        if (m_state == SELECT_SINGLE_RIGHT)
        {
            ChangeState(SELECT_SINGLE_LEFT);
        }
        else if (m_state == SELECT_RIGHT_GROUP)
        {
            if (m_targetable & SINGLE)
                ChangeState(SELECT_SINGLE_RIGHT);
            else
                ChangeState(SELECT_LEFT_GROUP);
        }
        else if (m_state == SELECT_SINGLE_LEFT)
        {
            if(m_targetable & GROUP)
                ChangeState(SELECT_LEFT_GROUP);
        }
        break;
    case CL_KEY_RIGHT:
        if(m_state == SELECT_SINGLE_RIGHT)
        {
            if(m_targetable & GROUP)
                ChangeState(SELECT_RIGHT_GROUP);
        }
        else if(m_state == SELECT_SINGLE_LEFT)
        {
            if(m_targetable & SINGLE)
                ChangeState(SELECT_SINGLE_RIGHT);
            else
                ChangeState(SELECT_RIGHT_GROUP);
        }
        else if(m_state == SELECT_LEFT_GROUP)
        {
            if(m_targetable & SINGLE)
                ChangeState(SELECT_SINGLE_LEFT);
            else
                ChangeState(SELECT_RIGHT_GROUP);
        }

        break;
    case CL_KEY_ENTER:
        m_bDone = true;
        break;
    }
}

void TargetingState::Draw(const CL_Rect &screenRect,CL_GraphicContext& pGC)
{
}

bool TargetingState::LastToDraw() const
{
    return true;
}

bool TargetingState::DisableMappableObjects() const
{
    return true;
}
void TargetingState::MappableObjectMoveHook()
{
}

void TargetingState::Start()
{
    m_bDone = false;
}

void TargetingState::SteelInit      (SteelInterpreter *)
{
}

void TargetingState::SteelCleanup   (SteelInterpreter *)
{
}

void TargetingState::Finish()
{
    m_pParent->FinishTargeting();
}


