#include "TargetingState.h"
#include "BattleState.h"
#include "IApplication.h"
#include "Monster.h"
#include "GraphicsManager.h"

using StoneRing::TargetingState;
using StoneRing::BattleState;
using StoneRing::ICharacter;
using StoneRing::IApplication;

void TargetingState::Init(BattleState *pParent, Targetable targetable, bool bDefaultMonsters)
{
    m_target_sprite = GraphicsManager::CreateSprite("Battle/Target");
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
	m_pParent->TargetChanged();
}

bool TargetingState::IsDone() const
{
    return m_bDone;
}

void TargetingState::HandleKeyDown(const clan::InputEvent &key)
{
    switch (key.id)
    {


    }
}

void TargetingState::HandleButtonUp(const StoneRing::IApplication::Button& button)
{
    switch(button)
    {
        case IApplication::BUTTON_CONFIRM:
            m_bDone = true;
            break;
        case IApplication::BUTTON_CANCEL:
            m_pParent->CancelTargeting();
            m_bDone = true;
            break;
    }
}

void TargetingState::HandleButtonDown(const IApplication::Button& button)
{
}

void TargetingState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
    if(axis == IApplication::AXIS_VERTICAL)
    {
        if(dir == IApplication::AXIS_UP)
        {
            SelectUpTarget();
        }
        else if(dir == IApplication::AXIS_DOWN)
        {
            SelectDownTarget();
        }
    }
    else
    {
        if(dir == IApplication::AXIS_LEFT)
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
        else if(dir == IApplication::AXIS_RIGHT)
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


void TargetingState::HandleKeyUp(const clan::InputEvent &key)
{

}

bool TargetingState::SelectRightGroup()
{
    if (m_pParent->MonstersOnLeft())
        m_pParent->m_targets.selected.m_pGroup = m_pParty;
    else
        m_pParent->m_targets.selected.m_pGroup = m_pParent->m_monsters;


    m_pParent->m_targets.m_bSelectedGroup = true;
	m_pParent->TargetChanged();
    return true;
}
bool TargetingState::SelectLeftGroup()
{
    if (m_pParent->MonstersOnLeft())
        m_pParent->m_targets.selected.m_pGroup = m_pParent->m_monsters;
    else
        m_pParent->m_targets.selected.m_pGroup = m_pParty;


    m_pParent->m_targets.m_bSelectedGroup = true;
	m_pParent->TargetChanged();
    return true;
}
bool TargetingState::SelectFromRightGroup()
{
    if (m_pParent->MonstersOnLeft())
    {
        m_pParent->m_targets.selected.m_pTarget = m_pParty->GetCharacter(0);
        m_pParent->m_targets.m_bSelectedGroup = false;
		m_pParent->TargetChanged();
        return true;
    }
    else
    {
    for(uint i=0;i<m_pParent->m_monsters->GetCharacterCount();i++)
    {
        if(can_target(m_pParent->m_monsters->GetCharacter(i))){
            m_pParent->m_targets.selected.m_pTarget = m_pParent->m_monsters->GetCharacter(i);
            m_pParent->m_targets.m_bSelectedGroup = false;
			m_pParent->TargetChanged();
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
			m_pParent->TargetChanged();
            return true;
        }
    }

    }
    else
    {
        m_pParent->m_targets.selected.m_pTarget = m_pParty->GetCharacter(0);
        m_pParent->m_targets.m_bSelectedGroup = false;
		m_pParent->TargetChanged();
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
								m_pParent->TargetChanged();
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
								m_pParent->TargetChanged();
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
							m_pParent->TargetChanged();
                            return;
                        }
                    }
                }
                currentCellY_down++;

            }
        }
        else
        {
            Character* pCharacter = dynamic_cast<Character*>(target);
            int current_index = 0;
            for(int i=0;i<m_pParty->GetCharacterCount();i++)
            {
                if(pCharacter == m_pParty->GetCharacter(i))
                {
                    current_index = i;
                    break;
                }
            }
            current_index++;
            if(current_index == m_pParty->GetCharacterCount())
                current_index = 0;
            m_pParent->m_targets.selected.m_pTarget = m_pParty->GetCharacter(current_index);
			m_pParent->TargetChanged();
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
							m_pParent->TargetChanged();
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
            Character* pCharacter = dynamic_cast<Character*>(target);
            int current_index = 0;
            for(int i=0;i<m_pParty->GetCharacterCount();i++)
            {
                if(pCharacter == m_pParty->GetCharacter(i))
                {
                    current_index = i;
                    break;
                }
            }
            current_index--;
            if(current_index == -1)
                current_index = m_pParty->GetCharacterCount() -1 ;
            m_pParent->m_targets.selected.m_pTarget = m_pParty->GetCharacter(current_index);
			m_pParent->TargetChanged();
            return;
        }
    }

    return;
}


void TargetingState::Draw(const clan::Rect &screenRect,clan::Canvas& GC)
{

    m_target_sprite.update(1); // TODO: Do I need to provide actual ms here?
    

    uint playercount = m_pParty->GetCharacterCount();
    for (uint nPlayer = 0; nPlayer < playercount; nPlayer++)
    {

        ICharacter * pCharacter = m_pParty->GetCharacter(nPlayer);
        if ((m_pParent->m_targets.m_bSelectedGroup && m_pParent->m_targets.selected.m_pGroup == m_pParty)
                || m_pParent->m_targets.selected.m_pTarget == pCharacter)
        {
            clan::Rectf rect = m_pParent->get_character_rect(pCharacter);
            m_target_sprite.set_scale(-1.0,1.0);
            m_target_sprite.draw(GC,rect.right ,rect.top);
                                //(rect.top - (m_target_sprite.get_height()/2));
        }
    }


    for (uint i = 0; i < m_pParent->m_monsters->GetCharacterCount(); i++)
    {
        Monster * pMonster = dynamic_cast<Monster*>( m_pParent->m_monsters->GetCharacter(i));
        if ((m_pParent->m_targets.m_bSelectedGroup && m_pParent->m_targets.selected.m_pGroup == m_pParent->m_monsters)
                || m_pParent->m_targets.selected.m_pTarget == pMonster)
        {
        	if(!pMonster->GetToggle(ICharacter::CA_ALIVE)) continue;
            clan::Rectf rect = m_pParent->get_character_rect(pMonster);
            clan::Sprite sprite = pMonster->GetCurrentSprite();
            m_target_sprite.set_scale(1.0,1.0);
            m_target_sprite.draw(GC,rect.left, rect.top );
                                // rect.top - (m_target_sprite.get_height()/2.0f) + sprite.get_height()/2.0f);
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
void TargetingState::Update()
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
