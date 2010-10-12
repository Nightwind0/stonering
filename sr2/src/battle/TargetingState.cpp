#include "TargetingState.h"
#include "BattleState.h"
#include "IApplication.h"
#include "Monster.h"
#include "GraphicsManager.h"

using StoneRing::TargetingState;
using StoneRing::BattleState;
using StoneRing::ICharacter;

void TargetingState::Init(BattleState *pParent, Targetable targetable, bool bDefaultMonsters)
{
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();

    m_target_sprite = pGraphicsManager->CreateSprite("Battle/Target");
    m_pParent = pParent;
    m_targetable = targetable;
    m_bDefaultMonsters = bDefaultMonsters;
    m_pParent->StartTargeting();

    m_pParty = IApplication::GetInstance()->GetParty();
    if (bDefaultMonsters)
    {
        if (m_pParent->MonstersOnLeft())
        {
            if (may_target_single())
                ChangeState(SELECT_SINGLE_LEFT);
            else
                ChangeState(SELECT_LEFT_GROUP);
        }
    }
    else
    {

        if (m_pParent->MonstersOnLeft())
        {
            if (may_target_single())
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
        SelectRightGroup();
        break;
    case SELECT_LEFT_GROUP:
        SelectLeftGroup();
        break;
    case SELECT_SINGLE_LEFT:
        SelectFromLeftGroup();
        break;
    case SELECT_SINGLE_RIGHT:
        SelectFromRightGroup();
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


    }
}

void TargetingState::HandleButtonUp(const IApplication::Button& button)
{
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	    m_bDone = true;
	    break;
    }
}

void TargetingState::HandleButtonDown(const IApplication::Button& button)
{
}

void TargetingState::HandleAxisMove(const IApplication::Axis& axis, float pos)
{
    if(axis == IApplication::AXIS_VERTICAL)
    {
	if(pos == -1.0)
	{
	    SelectUpTarget();
	}
	else if(pos == 1.0)
	{
	    SelectDownTarget();
	}
    }
    else
    {
	if(pos == -1.0)
	{
	    if (m_state == SELECT_SINGLE_LEFT)
	    {
		if (!SelectTargetOnLeft() && may_target_group())
		{
		    ChangeState(SELECT_LEFT_GROUP);
		}
	    }
	    else if (m_state == SELECT_SINGLE_RIGHT)
	    {
		if (!SelectTargetOnLeft() && may_target_single())
		{
		    ChangeState(SELECT_SINGLE_LEFT);
		}
	    }
	    else if (m_state == SELECT_RIGHT_GROUP && may_target_single())
	    {
		ChangeState(SELECT_SINGLE_RIGHT);
	    }
	}
	else if(pos == 1.0)
	{
	    if (m_state == SELECT_SINGLE_LEFT)
	    {
		if (!SelectTargetOnRight() && may_target_single())
		{
		    ChangeState(SELECT_SINGLE_RIGHT);
		}
	    }
	    else if (m_state == SELECT_SINGLE_RIGHT)
	    {
		if (!SelectTargetOnRight() && may_target_group())
		{
		    ChangeState(SELECT_RIGHT_GROUP);
		}
	    }
	    else if (m_state == SELECT_LEFT_GROUP && may_target_single())
	    {
		ChangeState(SELECT_SINGLE_LEFT);
	    }
	}
    }
}


void TargetingState::HandleKeyUp(const CL_InputEvent &key)
{

}

bool TargetingState::SelectRightGroup()
{
    if (m_pParent->MonstersOnLeft())
        m_pParent->m_targets.selected.m_pGroup = m_pParty;
    else
        m_pParent->m_targets.selected.m_pGroup = m_pParent->m_monsters;


    m_pParent->m_targets.m_bSelectedGroup = true;
    return true;
}
bool TargetingState::SelectLeftGroup()
{
    if (m_pParent->MonstersOnLeft())
        m_pParent->m_targets.selected.m_pGroup = m_pParent->m_monsters;
    else
        m_pParent->m_targets.selected.m_pGroup = m_pParty;


    m_pParent->m_targets.m_bSelectedGroup = true;
    return true;
}
bool TargetingState::SelectFromRightGroup()
{
    if (m_pParent->MonstersOnLeft())
    {
        m_pParent->m_targets.selected.m_pTarget = m_pParty->GetCharacter(0);
        m_pParent->m_targets.m_bSelectedGroup = false;
        return true;
    }
    else
    {
       for(uint i=0;i<m_pParent->m_monsters->GetCharacterCount();i++)
       {
           if(can_target(m_pParent->m_monsters->GetCharacter(i))){
            m_pParent->m_targets.selected.m_pTarget = m_pParent->m_monsters->GetCharacter(i);
            m_pParent->m_targets.m_bSelectedGroup = false;
            return true;
           }
       }
    }

    return false;
}
bool TargetingState::SelectFromLeftGroup()
{
    if (m_pParent->MonstersOnLeft())
    {

       for(uint i=0;i<m_pParent->m_monsters->GetCharacterCount();i++)
       {
           if(can_target(m_pParent->m_monsters->GetCharacter(i))){
            m_pParent->m_targets.selected.m_pTarget = m_pParent->m_monsters->GetCharacter(i);
            m_pParent->m_targets.m_bSelectedGroup = false;
            return true;
           }
       }

    }
    else
    {
        m_pParent->m_targets.selected.m_pTarget = m_pParty->GetCharacter(0);
        m_pParent->m_targets.m_bSelectedGroup = false;
        return true;
    }
    return false;
}
bool TargetingState::SelectTargetOnLeft()
{
    if (m_pParent->m_targets.m_bSelectedGroup)
    {
        // Pick top one in this group
    }
    else
    {
        ICharacter * target = m_pParent->m_targets.selected.m_pTarget;
        Monster * pMonster = dynamic_cast<Monster*>(target);
        if (pMonster != NULL)
        {
            // It's a monster... find out where it is
            uint cellX = pMonster->GetCellX();
            uint cellY = pMonster->GetCellY();

            // Can't go left from here
            if (cellX == 0) return false;

            int currentCellX = cellX - 1;
            bool found = false;
            while (!found && currentCellX >=0)
            {
                int currentCellY_up = cellY;
                int currentCellY_down = cellY;

                // Or, so that one can continue if the other is maxed out
                while (currentCellY_up >=0 || currentCellY_down < m_pParent->m_nRows)
                {
                    for (std::deque<ICharacter*>::const_iterator iter = m_pParent->m_initiative.begin(); iter != m_pParent->m_initiative.end(); iter++)
                    {
                        Monster * pMonster = dynamic_cast<Monster*>(*iter);
                        if (pMonster != NULL && can_target(pMonster))
                        {
                            if (pMonster->GetCellX() == currentCellX && (pMonster->GetCellY() == currentCellY_up || pMonster->GetCellY() == currentCellY_down))
                            {
                                m_pParent->m_targets.selected.m_pTarget = pMonster;
                                return true;
                            }
                        }
                    }


                    currentCellY_up--;
                    currentCellY_down++;

                }

                currentCellX--;

            }

        }
        else
        {
            // It's a player
            // TODO: This..
            // for now, we can only deal with one player
            return false;
        }
    }

    return false;
}
bool TargetingState::SelectTargetOnRight()
{
    if (m_pParent->m_targets.m_bSelectedGroup)
    {
        // Pick top one in this group
    }
    else
    {
        ICharacter * target = m_pParent->m_targets.selected.m_pTarget;
        Monster * pMonster = dynamic_cast<Monster*>(target);
        if (pMonster != NULL)
        {
            // It's a monster... find out where it is
            uint cellX = pMonster->GetCellX();
            uint cellY = pMonster->GetCellY();

            uint currentCellX = cellX + 1;
            bool found = false;
            while (!found && currentCellX < m_pParent->m_nColumns)
            {
                uint currentCellY_up = cellY;
                uint currentCellY_down = cellY;

                // Or, so that one can continue if the other is maxed out
                while (currentCellY_up >0 || currentCellY_down < m_pParent->m_nRows)
                {
                    for (std::deque<ICharacter*>::const_iterator iter = m_pParent->m_initiative.begin(); iter != m_pParent->m_initiative.end(); iter++)
                    {
                        Monster * pMonster = dynamic_cast<Monster*>(*iter);
                        if (pMonster != NULL && can_target(pMonster))
                        {
                            if (pMonster->GetCellX() == currentCellX && (pMonster->GetCellY() == currentCellY_up || pMonster->GetCellY() == currentCellY_down))
                            {
                                m_pParent->m_targets.selected.m_pTarget = pMonster;
                                return true;
                            }
                        }
                    }

                    if (currentCellY_up > 0)
                        currentCellY_up--;

                    currentCellY_down++;

                }

                currentCellX++;

            }

        }
        else
        {
            // It's a player
            // TODO: This..
            // for now, we can only deal with one player
            return false;
        }
    }

    return false;
}
void TargetingState::SelectDownTarget()
{
    if (m_pParent->m_targets.m_bSelectedGroup)
    {
        // Pick top one in this group
    }
    else
    {
        ICharacter * target = m_pParent->m_targets.selected.m_pTarget;
        Monster * pMonster = dynamic_cast<Monster*>(target);
        if (pMonster != NULL)
        {
            // It's a monster... find out where it is
            uint cellX = pMonster->GetCellX();
            uint cellY = pMonster->GetCellY();
            uint currentCellY_down = cellY + 1;

            while (currentCellY_down < m_pParent->m_nRows)
            {
                for (std::deque<ICharacter*>::const_iterator iter = m_pParent->m_initiative.begin(); iter != m_pParent->m_initiative.end(); iter++)
                {
                    Monster * pMonster = dynamic_cast<Monster*>(*iter);
                    if (pMonster != NULL && can_target(pMonster))
                    {
                        if (pMonster->GetCellX() == cellX && pMonster->GetCellY() == currentCellY_down)
                        {
                            m_pParent->m_targets.selected.m_pTarget = pMonster;
                            return;
                        }
                    }
                }
                currentCellY_down++;

            }
        }
        else
        {
            // It's a player
            // TODO: This..
            // for now, we can only deal with one player
            return;
        }
    }

    return;
}
void TargetingState::SelectUpTarget()
{

    if (m_pParent->m_targets.m_bSelectedGroup)
    {
        // Pick top one in this group
    }
    else
    {
        ICharacter * target = m_pParent->m_targets.selected.m_pTarget;
        Monster * pMonster = dynamic_cast<Monster*>(target);
        if (pMonster != NULL)
        {
            // It's a monster... find out where it is
            uint cellX = pMonster->GetCellX();
            uint cellY = pMonster->GetCellY();

            if (cellY == 0) return;

            uint currentCellY_up = cellY -1;

            while (currentCellY_up >= 0)
            {
                for (std::deque<ICharacter*>::const_iterator iter = m_pParent->m_initiative.begin(); iter != m_pParent->m_initiative.end(); iter++)
                {
                    Monster * pMonster = dynamic_cast<Monster*>(*iter);
                    if (pMonster != NULL && can_target(pMonster))
                    {
                        if (pMonster->GetCellX() == cellX && pMonster->GetCellY() == currentCellY_up)
                        {
                            m_pParent->m_targets.selected.m_pTarget = pMonster;
                            return;
                        }
                    }
                }

                if(currentCellY_up >0)
                    currentCellY_up--;

            }
        }
        else
        {
            // It's a player
            // TODO: This..
            // for now, we can only deal with one player
            return;
        }
    }

    return;
}


void TargetingState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{

    m_target_sprite.update();
    

    uint playercount = m_pParty->GetCharacterCount();
    for (uint nPlayer = 0; nPlayer < playercount; nPlayer++)
    {

        ICharacter * pCharacter = m_pParty->GetCharacter(nPlayer);
        if ((m_pParent->m_targets.m_bSelectedGroup && m_pParent->m_targets.selected.m_pGroup == m_pParty)
                || m_pParent->m_targets.selected.m_pTarget == pCharacter)
        {
	    CL_Rect rect = m_pParent->get_character_rect(pCharacter);
            m_target_sprite.draw(GC,
                                 static_cast<int>(rect.left -   (m_target_sprite.get_width()/2)),
                                 static_cast<int>(rect.top - (m_target_sprite.get_height()/2)));
        }
    }


    for (uint i = 0; i < m_pParent->m_monsters->GetCharacterCount(); i++)
    {
        Monster * pMonster = dynamic_cast<Monster*>( m_pParent->m_monsters->GetCharacter(i));
        if ((m_pParent->m_targets.m_bSelectedGroup && m_pParent->m_targets.selected.m_pGroup == m_pParent->m_monsters)
                || m_pParent->m_targets.selected.m_pTarget == pMonster)
        {
	    CL_Rect rect = m_pParent->get_character_rect(pMonster);
            CL_Sprite sprite = pMonster->GetCurrentSprite();
            m_target_sprite.draw(GC,rect.left-   (m_target_sprite.get_width()/2.0f),
                                 rect.top - (m_target_sprite.get_height()/2.0f) + sprite.get_height()/2.0f);
        }
    }
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

bool TargetingState::can_target(ICharacter*pTarget)const
{
    Monster * pMonster = dynamic_cast<Monster*>(pTarget);

    if(pMonster != NULL)
    {
        return pMonster->GetToggle(ICharacter::CA_VISIBLE);
    }
    else
    {
        return true;
    }
}