#include "AnimationState.h"
#include "Animation.h"
#include "IApplication.h"
#include "BattleState.h"


using StoneRing::AnimationState;


AnimationState::AnimationState(BattleState& parent,
                               ICharacterGroup* casterGroup,
                               ICharacterGroup* targetGroup,
                               ICharacter* caster,
                               ICharacter* target):
        m_parent(parent),m_pCasterGroup(casterGroup),m_pTargetGroup(targetGroup),m_pCaster(caster),m_pTarget(target),m_pAnim(NULL),m_bDone(false)
{
}

AnimationState::~AnimationState()
{
}

void AnimationState::Init(Animation* pAnimation)
{
    m_pAnim = pAnimation;
}

CL_Point AnimationState::GetFocusOrigin(const SpriteMovement::Focus& focus, ICharacter * pTarget)
{
    CL_Point point;
    switch (focus.meFocusType)
    {
    case SpriteMovement::SCREEN:
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = IApplication::GetInstance()->GetScreenWidth() / 2;
            break;
        case SpriteMovement::TOWARDS:
            break;
        case SpriteMovement::AWAY:
            break;
        case SpriteMovement::LEFT:
            point.x = 0;
            break;
        case SpriteMovement::RIGHT:
            point.x = IApplication::GetInstance()->GetScreenWidth();
            break;
        }
        switch (focus.meFocusY)
        {
        case SpriteMovement::Y_CENTER:
            point.y = IApplication::GetInstance()->GetScreenHeight() / 2;
            break;
        case SpriteMovement::TOP:
            point.y = 0;
            break;
        case SpriteMovement::BOTTOM:
            point.y = IApplication::GetInstance()->GetScreenHeight();
            break;

        }
        break;
    case SpriteMovement::CASTER:
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = m_parent.get_character_rect(m_pCaster).get_center().x;
            break;
        case SpriteMovement::TOWARDS:

            break;
        case SpriteMovement::AWAY:
            break;
        case SpriteMovement::LEFT:
            point.x = m_parent.get_character_rect(m_pCaster).get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = m_parent.get_character_rect(m_pCaster).get_top_right().x;
            break;
        }
        switch (focus.meFocusY)
        {
        case SpriteMovement::Y_CENTER:
            point.y =  m_parent.get_character_rect(m_pCaster).get_center().y;
            break;
        case SpriteMovement::TOP:
            point.y =  m_parent.get_character_rect(m_pCaster).get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  m_parent.get_character_rect(m_pCaster).get_bottom_left().y;
            break;
        }
        break;
    case SpriteMovement::TARGET:
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = m_parent.get_character_rect(pTarget).get_center().x;
            break;
        case SpriteMovement::TOWARDS:

            break;
        case SpriteMovement::AWAY:
            break;
        case SpriteMovement::LEFT:
            point.x = m_parent.get_character_rect(pTarget).get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = m_parent.get_character_rect(pTarget).get_top_right().x;
            break;
        }
        switch (focus.meFocusY)
        {
        case SpriteMovement::Y_CENTER:
            point.y =  m_parent.get_character_rect(m_pCaster).get_center().y;
            break;
        case SpriteMovement::TOP:
            point.y =  m_parent.get_character_rect(m_pCaster).get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  m_parent.get_character_rect(m_pCaster).get_bottom_left().y;
            break;
        }
        break;
    case SpriteMovement::CASTER_GROUP:
    {
        CL_Rect rect = m_parent.get_group_rect(m_pCasterGroup);
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = rect.get_center().x;
            break;
        case SpriteMovement::LEFT:
            point.x = rect.get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = rect.get_top_right().x;
            break;

        }

        switch (focus.meFocusY)
        {
        case SpriteMovement::Y_CENTER:
            point.y = rect.get_center().y;
            break;
        case SpriteMovement::TOP:
            point.y = rect.get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y = rect.get_bottom_left().y;
            break;
        }

        break;
    }
    case SpriteMovement::TARGET_GROUP:
    {
        CL_Rect rect = m_parent.get_group_rect(m_pTargetGroup);
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = rect.get_center().x;
            break;
        case SpriteMovement::LEFT:
            point.x = rect.get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = rect.get_top_right().x;
            break;

        }

        switch (focus.meFocusY)
        {
        case SpriteMovement::Y_CENTER:
            point.y = rect.get_center().y;
            break;
        case SpriteMovement::TOP:
            point.y = rect.get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y = rect.get_bottom_left().y;
            break;
        }

        break;
    }
    case SpriteMovement::CASTER_LOCUS:
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = m_parent.get_character_locus_rect(m_pCaster).get_center().x;
            break;
        case SpriteMovement::TOWARDS:

            break;
        case SpriteMovement::AWAY:
            break;
        case SpriteMovement::LEFT:
            point.x = m_parent.get_character_locus_rect(m_pCaster).get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = m_parent.get_character_locus_rect(m_pCaster).get_top_right().x;
            break;
        }
        switch (focus.meFocusY)
        {
        case SpriteMovement::Y_CENTER:
            point.y =  m_parent.get_character_locus_rect(m_pCaster).get_center().y;
            break;
        case SpriteMovement::TOP:
            point.y =  m_parent.get_character_locus_rect(m_pCaster).get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  m_parent.get_character_locus_rect(m_pCaster).get_bottom_left().y;
            break;
        }
        break;
    case SpriteMovement::TARGET_LOCUS:
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = m_parent.get_character_locus_rect(pTarget).get_center().x;
            break;
        case SpriteMovement::TOWARDS:

            break;
        case SpriteMovement::AWAY:
            break;
        case SpriteMovement::LEFT:
            point.x = m_parent.get_character_locus_rect(pTarget).get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = m_parent.get_character_locus_rect(pTarget).get_top_right().x;
            break;
        }
        switch (focus.meFocusY)
        {
        case SpriteMovement::Y_CENTER:
            point.y =  m_parent.get_character_locus_rect(m_pCaster).get_center().y;
            break;
        case SpriteMovement::TOP:
            point.y =  m_parent.get_character_locus_rect(m_pCaster).get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  m_parent.get_character_locus_rect(m_pCaster).get_bottom_left().y;
            break;
        }
        break;
    }

    return point;
}


bool AnimationState::IsDone() const
{
    return m_bDone;
}

void AnimationState::HandleKeyDown(const CL_InputEvent &key)
{
}

void AnimationState::HandleKeyUp(const CL_InputEvent &key)
{
}

void AnimationState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    uint passed = CL_System::get_time() - m_phase_start_time;
    float percentage = (float)passed / (float)(*m_phase_iterator)->GetDurationMs();
    if (percentage >= 1.0f)
    {
        NextPhase();
        percentage = (float)passed / (float)(*m_phase_iterator)->GetDurationMs();
    }

    for (std::list<SpriteAnimation*>::const_iterator iter = (*m_phase_iterator)->GetSpriteAnimationsBegin();
            iter != (*m_phase_iterator)->GetSpriteAnimationsEnd(); iter++)
    {
        SpriteAnimation* anim = *iter;
        if (anim->HasSpriteMovement())
        {
            SpriteMovement * movement = anim->GetSpriteMovement();
            if (movement->ForEachTarget() && (*m_phase_iterator)->InParallel())
            {
                for (uint i=0;i<m_pTargetGroup->GetCharacterCount();i++)
                {
                    ICharacter * pTarget = m_pTargetGroup->GetCharacter(i);
                    CL_Point origin = GetFocusOrigin(movement->GetInitialFocus(), pTarget);
                    CL_Point dest;
                    if(movement->HasEndFocus()){
                          dest = GetFocusOrigin(movement->GetEndFocus(), pTarget);
                    }

                    if (anim->HasSpriteStub())
                    {
                    }
                    else if (anim->HasSpriteRef())
                    {
                        CL_Sprite sprite = anim->GetSpriteRef()->CreateSprite();
                    }
                    else if (anim->HasBattleSprite())
                    {
                        // Have parent move this thing
                    }
                }
            }
        }
    }


}

bool AnimationState::LastToDraw() const // Should we continue drawing more states?
{
    return false;
}

bool AnimationState::DisableMappableObjects() const // Should the app move the MOs?
{
    return false;
}

void AnimationState::MappableObjectMoveHook() // Do stuff right after the mappable object movement
{
}

void AnimationState::NextPhase()
{
    (*++m_phase_iterator)->Execute();
    m_phase_start_time = CL_System::get_time();

    for (std::list<SpriteAnimation*>::const_iterator iter = (*m_phase_iterator)->GetSpriteAnimationsBegin();
            iter != (*m_phase_iterator)->GetSpriteAnimationsEnd(); iter++)
    {
        SpriteAnimation* animation = *iter;
        if (animation->HasAlterSprite())
        {
            // Alter any sprites on the parent now
        }

        if (animation->HasBattleSprite())
        {
            // Change battle sprite using the parent now
            // to GetWhich
        }
    }
}

void AnimationState::Start()
{
    m_phase_iterator = m_pAnim->GetPhasesBegin();
    NextPhase();

}

void AnimationState::SteelInit      (SteelInterpreter *)
{
}

void AnimationState::SteelCleanup   (SteelInterpreter *)
{
}

void AnimationState::Finish() // Hook to clean up or whatever after being popped
{
}
