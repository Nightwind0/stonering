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
	m_pParent->SelectFirstTarget(bDefaultMonsters);

	if(bDefaultMonsters)
	{
		
	}
}

bool TargetingState::IsDone() const
{
	return m_bDone;
}

void TargetingState::HandleKeyDown(const CL_InputEvent &key)
{
	switch(key.id)
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
	switch(key.id)
	{
	case CL_KEY_LEFT:
	case CL_KEY_RIGHT:
		break;
	}
}

void TargetingState::Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
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

  