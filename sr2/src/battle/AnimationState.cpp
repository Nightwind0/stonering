#include "AnimationState.h"
#include "Animation.h"
#include "IApplication.h"
#include "BattleState.h"

using namespace StoneRing;


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
    CL_Point point(0,0);
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
    case SpriteMovement::CASTER:{
	CL_Rect rect =  m_parent.get_character_rect(m_pCaster);
        switch (focus.meFocusX)
        {

        case SpriteMovement::X_CENTER:
            point.x = rect.get_center().x;
            break;
	default:
        case SpriteMovement::TOWARDS:
	    point.x = rect.get_top_left().x;
            break;
        case SpriteMovement::AWAY:
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
            point.y =  rect.get_center().y;
            break;
	default:
        case SpriteMovement::TOP:
            point.y =  rect.get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  rect.get_bottom_left().y;
            break;
        }
        break;
    }
    case SpriteMovement::TARGET:
    {
	CL_Rect rect = m_parent.get_character_rect(m_pTarget);
        switch (focus.meFocusX)
        {

        case SpriteMovement::X_CENTER:
            point.x = rect.get_center().x;
            break;
	default:
        case SpriteMovement::TOWARDS:
	    point.x = rect.get_center().x;
            break;
        case SpriteMovement::AWAY:
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
            point.y =  rect.get_center().y;
            break;
	default:
        case SpriteMovement::TOP:
            point.y =  rect.get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  rect.get_bottom_left().y;
            break;
        }
        break;
    }
    case SpriteMovement::CASTER_GROUP:
    {
        CL_Rect rect = m_parent.get_group_rect(m_pCasterGroup);
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = rect.get_center().x;
            break;
	default:
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
	default:
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
	default:
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
	default:
        case SpriteMovement::TOP:
            point.y = rect.get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y = rect.get_bottom_left().y;
            break;
        }

        break;
    }
    case SpriteMovement::CASTER_LOCUS:{
	CL_Rect rect =  m_parent.get_character_locus_rect(m_pCaster);
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x = rect.get_center().x;
            break;

        case SpriteMovement::AWAY:
            break;
        case SpriteMovement::LEFT:
            point.x = rect.get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = rect.get_top_right().x;
            break;
	case SpriteMovement::TOWARDS:
	default:
	    point.x = rect.get_top_left().x;
            break;
        }
        switch (focus.meFocusY)
        {

	    default:
        case SpriteMovement::TOP:
            point.y =  rect.get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  rect.get_bottom_left().y;
            break;

	case SpriteMovement::Y_CENTER:
            point.y =  rect.get_center().y;
            break;
        }
        break;
    }
    case SpriteMovement::TARGET_LOCUS:{
	CL_Rect rect =  m_parent.get_character_locus_rect(m_pTarget);
        switch (focus.meFocusX)
        {
        case SpriteMovement::X_CENTER:
            point.x =rect.get_center().x;
            break;

        case SpriteMovement::AWAY:
            break;
        case SpriteMovement::LEFT:
            point.x = rect.get_top_left().x;
            break;
        case SpriteMovement::RIGHT:
            point.x = rect.get_top_right().x;
            break; 
	default:
	case SpriteMovement::TOWARDS:
	    point.x = rect.get_top_left().x;
            break;
	    
        }
        switch (focus.meFocusY)
        {

	default:
        case SpriteMovement::TOP:
            point.y =  rect.get_top_left().y;
            break;
        case SpriteMovement::BOTTOM:
            point.y =  rect.get_bottom_left().y;
            break;

	case SpriteMovement::Y_CENTER:
            point.y =  rect.get_center().y;
            break;
        }
        break;
    }
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

void AnimationState::move_character(ICharacter* character, SpriteAnimation* anim, SpriteMovement* movement, float percentage)
{
    if(percentage > 1.0f) percentage = 1.0f;
    CL_Point origin_i = GetFocusOrigin(movement->GetInitialFocus(), character);
   // std::cout << "Origin is " << origin_i.x << ',' << origin_i.y << std::endl;
    CL_Pointf origin(origin_i.x,origin_i.y);
    CL_Point dest_i;
    CL_Pointf current = origin;
    
    // Rotation
    double degrees = percentage * movement->Rotation();
    if((!character->IsMonster() && m_parent.MonstersOnLeft()) || 
	character->IsMonster() && !m_parent.MonstersOnLeft())    
	degrees = 0 - degrees;

    
    CL_Angle angle = CL_Angle::from_degrees(degrees);
    // TODO: Mechanism to affect ALL this character's sprites in case it changes
    CL_Sprite sprite = character->GetCurrentSprite();
    sprite.set_angle(angle);
  
    float completion = movement->Completion();
    percentage *= completion;
    if (movement->HasEndFocus())
    {
        dest_i = GetFocusOrigin(movement->GetEndFocus(), character);
        CL_Pointf dest(dest_i.x,dest_i.y);
	  //(1-p)*A + p*B
        //current = (1.0f - percentage) * origin + percentage * dest;
        switch (movement->GetMovementStyle())
        {
        case SpriteMovement::STRAIGHT:
            current = origin * (1.0f - percentage) + dest * percentage;
            break;
	case SpriteMovement::XONLY:
	    current.x = origin.x * (1.0f - percentage) + dest.x * percentage;
	    break;
	case SpriteMovement::YONLY:
	    current.y = origin.y * (1.0f - percentage) + dest.y * percentage;
	    break;
        case SpriteMovement::SINE:
        {
            CL_Pointf d,c;
            d.x = dest.x - origin.x;
            d.y = dest.y - origin.y;
            float l = sqrt((double)d.x*d.x + d.y+d.y);
            d.x /=  l;
            d.y /= l;
            float w = 2.0f * CL_PI * movement->Periods(); // make this 1/2 to arc up

            current.x = percentage*dest.x + (1.0f-percentage)*origin.x + movement->Amplitude()*d.y*sin(w*percentage);
            current.y = percentage*dest.y + (1.0f-percentage)*origin.y - movement->Amplitude()*d.x*sin(w*percentage);
            break;
        }
        case SpriteMovement::ARC_OVER:
        {
            CL_Pointf d,c;
            d.x = dest.x - origin.x;
            d.y = dest.y - origin.y;
            float l = sqrt((double)d.x*d.x + d.y+d.y);
            d.x /=  l;
            d.y /= l;
            float w = 2.0f * CL_PI *0.5f; // make this 1/2 to arc up

            current.x = percentage*dest.x + (1.0f-percentage)*origin.x + movement->Amplitude()*d.y*sin(w*percentage);
            current.y = percentage*dest.y + (1.0f-percentage)*origin.y - movement->Amplitude()*d.x*sin(w*percentage);
            break;
        }
        default:
            break;
        }
    }
    else
    {
        // move a set distance in direction
        CL_Pointf direction;
        SpriteMovement::eMovementDirection dir = movement->GetMovementDirection();

        if(dir == SpriteMovement::MOVE_AWAY)
        {
            if(m_parent.MonstersOnLeft())
            {
                if(m_pCaster->IsMonster())
                    dir = SpriteMovement::W;
                else dir = SpriteMovement::E;
            }
            else
            {
                if(m_pCaster->IsMonster())
                    dir = SpriteMovement::E;
                else dir = SpriteMovement::W;
            }
        }
        else if(dir == SpriteMovement::MOVE_TOWARDS)
        {
            if(!m_parent.MonstersOnLeft())
            {
                if(m_pCaster->IsMonster())
                    dir = SpriteMovement::W;
                else dir = SpriteMovement::E;
            }
            else
            {
                if(m_pCaster->IsMonster())
                    dir = SpriteMovement::E;
                else dir = SpriteMovement::W;
            }
        }

        switch (dir)
        {
        case SpriteMovement::N:
            direction.x = 0.0f;
            direction.y = -1.0f;
            break;
        case SpriteMovement::S:
            direction.x = 0.0f;
            direction.y = 1.0f;
            break;
        case SpriteMovement::E:
            direction.x = 1.0f;
            direction.y = 0.0f;
            break;
        case SpriteMovement::W:
            direction.x = -1.0f;
            direction.y = 0.0f;
            break;
        case SpriteMovement::SE:
            direction.y = 1.0f;
            direction.x = 1.0f;
            break;
        case SpriteMovement::SW:
            direction.x = - 1.0f;
            direction.y = 1.0f;
            break;
        case SpriteMovement::NE:
            direction.x = 1.0f;
            direction.y = -1.0f;
            break;
        case SpriteMovement::NW:
            direction.x = - 1.0f;
            direction.y = - 1.0f;
            break;

        }

        direction *= percentage;
        current = origin * direction;


    }
    
    if(movement->Invert())
    {
	std::cout << "Inverted ";
	float diff_x = origin.x - current.x;
	float diff_y = origin.y - current.y;
	current.x += diff_x*2;
	current.y += diff_y*2;
    }
    // TODO: Now take whatever it is, and display it at 'current'
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
        CL_Point point;
        point.x = current.x;
        point.y = current.y;
        switch (anim->GetBattleSprite()->GetWho())
        {
        case CASTER:
            m_pCaster->SetBattlePos ( point );
            break;
        case TARGET:
            m_pTarget->SetBattlePos ( point );
            break;
        }
    }
}


void AnimationState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    if (!m_bDone)
    {
        uint passed = CL_System::get_time() - m_phase_start_time;
        float percentage = (float)passed / (float)(*m_phase_iterator)->GetDurationMs();
        bool draw = true;
        if (percentage >= 1.0f)
        {
            draw = NextPhase();
            if(draw)
            {
                StartPhase();
                percentage = (float)passed / (float)(*m_phase_iterator)->GetDurationMs();
            }

        }

        if (draw)
        {
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
                            move_character(pTarget,anim,movement,percentage);
                        }

                    }else{
                        move_character(m_pCaster,anim,movement,percentage);
                    }
                }
            }
        }
    }
    CL_System::sleep(10);
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

bool AnimationState::NextPhase()
{

    m_phase_iterator++;
    if (m_phase_iterator == m_pAnim->GetPhasesEnd())
    {
        // Done
        m_bDone = true;
        return false;
    }

    return true;
}
void AnimationState::StartPhase()
{
    Phase * phase = *m_phase_iterator;
    phase->Execute();
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

    return;
}

void AnimationState::Start()
{
    m_phase_iterator = m_pAnim->GetPhasesBegin();
    StartPhase();
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
