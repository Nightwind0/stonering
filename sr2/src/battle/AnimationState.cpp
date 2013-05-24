#include "AnimationState.h"
#include "Animation.h"
#include "IApplication.h"
#include "BattleState.h"
#include "WeaponType.h"
#include "SoundManager.h"
#include "ICharacterGroup.h"
#include "GraphicsManager.h"
#include "sr_defines.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace StoneRing;

class AnimationRunner: public SteelRunner<AnimationState> {
public:
	AnimationRunner( SteelInterpreter* pInterpreter, AnimationState* pState )
		: SteelRunner<AnimationState>( pInterpreter, pState, &AnimationState::FunctorCompleted ) {
	}
	virtual ~AnimationRunner() {
	}
	virtual void run() {
		// TODO: Should I support an AstScript here too, like the normal runner does?
		try {
			if( m_pFunctor ) {

				m_result = m_pFunctor->Call( m_pInterpreter, SteelType::Container() );
			}
#if 0
			std::cerr << "Cut scene functor finished. Waiting for tasks to finish." << std::endl;
#endif

			// wait for all tasks to finish
			m_callee->WaitFinishedEvent();
		} catch( Steel::SteelException ex ) {
			// TODO: On the main thread do an exception screen
			std::cerr << "Exception in　animation: " << ex.getMessage() << " on line " << ex.getLine() << std::endl;

		}

		( m_callee->*m_callback )();
	}

};


AnimationState::AnimationState( BattleState& parent ):
	m_parent( parent ), m_pCasterGroup( NULL ), m_pTargetGroup( NULL ), m_pCaster( NULL ), m_pTarget( NULL ), m_pAnim( NULL ), m_bDone( false ) {

}


AnimationState::~AnimationState() {
}

void AnimationState::Init( Animation* pAnimation,
						   ICharacterGroup* casterGroup,  
							ICharacterGroup* targetGroup,
							ICharacter* caster,
							ICharacter* target,
							Equipment::eSlot hand
 						) {
	m_pAnim = pAnimation;
	m_functor_mode = false;
	m_bDone = false;
	m_pTargetGroup = targetGroup;
	m_pCasterGroup = casterGroup;
	m_pCaster = caster;
	m_pTarget = target;
	m_hand = hand;
	if( m_pTarget == NULL ) m_pTarget = m_pCaster;
}

void AnimationState::Init( SteelType::Functor pFunctor ) {
	m_functor = pFunctor;
	m_functor_mode = true;
	m_bDone = false;
}

void AnimationState::AddTask( AnimationState::Task* task ) {
	std::cout << "Adding task: " << task->GetName() << std::endl;
	m_task_mutex.lock();
	m_tasks.push_back( task );
	m_task_mutex.unlock();
}


BattleState::SpriteTicket AnimationState::AddSprite( CL_Sprite sprite ) {
	return m_parent.add_sprite( sprite );
}


CL_Rectf AnimationState::GetCharacterRect( ICharacter* ichar ) {
	return m_parent.get_character_rect( ichar );
}
CL_Rectf AnimationState::GetGroupRect( ICharacterGroup* igroup ) {
	return m_parent.get_group_rect( igroup );
}
CL_Rectf AnimationState::GetSpriteRect( BattleState::SpriteTicket sprite ) {
	return m_parent.get_sprite_rect( sprite );
}
void AnimationState::SetSpritePos( BattleState::SpriteTicket sprite, const CL_Pointf& pt ) {
	m_parent.set_sprite_pos( sprite, pt );
}



CL_Pointf AnimationState::GetFocusOrigin( const SpriteMovement::Focus& focus, ICharacter * pTarget ) {
	CL_Pointf point( 0, 0 );
	switch( focus.meFocusType ) {
		case SpriteMovement::SCREEN:
			switch( focus.meFocusX ) {
				default:
				case SpriteMovement::X_CENTER:
					point.x = IApplication::GetInstance()->GetDisplayRect().get_width() / 2;
					break;
				case SpriteMovement::TOWARDS:
					break;
				case SpriteMovement::AWAY:
					break;
				case SpriteMovement::LEFT:
					point.x = 0;
					break;
				case SpriteMovement::RIGHT:
					point.x = IApplication::GetInstance()->GetDisplayRect().get_width() / 2;
					break;
			}
			switch( focus.meFocusY ) {
				default:
				case SpriteMovement::Y_CENTER:
					point.y = IApplication::GetInstance()->GetDisplayRect().get_height() / 2;
					break;
				case SpriteMovement::TOP:
					point.y = 0;
					break;
				case SpriteMovement::BOTTOM:
					point.y = IApplication::GetInstance()->GetDisplayRect().get_height() / 2;
					break;

			}
			break;
		case SpriteMovement::CASTER: {
			CL_Rectf rect =  m_parent.get_character_rect( m_pCaster );
			switch( focus.meFocusX ) {
				default:
				case SpriteMovement::X_CENTER:
					point.x = rect.get_center().x;
					break;

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
			switch( focus.meFocusY ) {
				default:
				case SpriteMovement::Y_CENTER:
					point.y =  rect.get_center().y;
					break;

				case SpriteMovement::TOP:
					point.y =  rect.get_top_left().y;
					break;
				case SpriteMovement::BOTTOM:
					point.y =  rect.get_bottom_left().y;
					break;
			}
			break;
		}
		case SpriteMovement::TARGET: {
			CL_Rectf rect = m_parent.get_character_rect( m_pTarget );
			switch( focus.meFocusX ) {
				default:
				case SpriteMovement::X_CENTER:
					point.x = rect.get_center().x;
					break;

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
			switch( focus.meFocusY ) {
				default:
				case SpriteMovement::Y_CENTER:
					point.y =  rect.get_center().y;
					break;

				case SpriteMovement::TOP:
					point.y =  rect.get_top_left().y;
					break;
				case SpriteMovement::BOTTOM:
					point.y =  rect.get_bottom_left().y;
					break;
			}
			break;
		}
		case SpriteMovement::CASTER_GROUP: {
			CL_Rectf rect = m_parent.get_group_rect( m_pCasterGroup );
			switch( focus.meFocusX ) {
				default:
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

			switch( focus.meFocusY ) {
				default:
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
		case SpriteMovement::TARGET_GROUP: {
			CL_Rectf rect = m_parent.get_group_rect( m_pTargetGroup );
			switch( focus.meFocusX ) {
				default:
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

			switch( focus.meFocusY ) {
				default:
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
		case SpriteMovement::CASTER_LOCUS: {
			CL_Rectf rect =  m_parent.get_character_locus_rect( m_pCaster );
			switch( focus.meFocusX ) {
				default:
				case SpriteMovement::X_CENTER:
					point.x = rect.get_center().x;
					break;

				case SpriteMovement::LEFT:
					point.x = rect.get_top_left().x;
					break;
				case SpriteMovement::RIGHT:
					point.x = rect.get_top_right().x;
					break;
				case SpriteMovement::TOWARDS:

					point.x = rect.get_center().x;
					break;
			}
			switch( focus.meFocusY ) {


				case SpriteMovement::TOP:
					point.y =  rect.get_top_left().y;
					break;
				case SpriteMovement::BOTTOM:
					point.y =  rect.get_bottom_left().y;
					break;
				default:
				case SpriteMovement::Y_CENTER:
					point.y =  rect.get_center().y;
					break;
			}
			break;
		}
		case SpriteMovement::TARGET_LOCUS: {
			CL_Rectf rect =  m_parent.get_character_locus_rect( m_pTarget );
			switch( focus.meFocusX ) {

				case SpriteMovement::X_CENTER:
					point.x = rect.get_center().x;
					break;

				case SpriteMovement::LEFT:
					point.x = rect.get_top_left().x;
					break;
				case SpriteMovement::RIGHT:
					point.x = rect.get_top_right().x;
					break;
				default:
				case SpriteMovement::TOWARDS:
					point.x = rect.get_center().x;
					break;

			}
			switch( focus.meFocusY ) {


				case SpriteMovement::TOP:
					point.y =  rect.get_top_left().y;
					break;
				case SpriteMovement::BOTTOM:
					point.y =  rect.get_bottom_left().y;
					break;
				default:
				case SpriteMovement::Y_CENTER:
					point.y =  rect.get_center().y;
					break;
			}
			break;
		}
	}

	return point;
}


bool AnimationState::IsDone() const {
	return m_bDone;
}


void AnimationState::WaitFinishedEvent() {
	while( !m_tasks.empty() ) {
		start( m_tasks.front() );
		waitFor( m_tasks.front() );
		m_task_mutex.lock();
		if(!m_tasks.empty())
			m_tasks.erase( m_tasks.begin() );
		m_task_mutex.unlock();
	}
}



void AnimationState::HandleKeyDown( const CL_InputEvent &key ) {
}

void AnimationState::HandleKeyUp( const CL_InputEvent &key ) {
}

void AnimationState::move_sprite( ICharacter* pActor, ICharacter* pTarget, SpriteAnimation* anim, SpriteMovement* movement, float percentage ) {
	if( percentage > 1.0f ) percentage = 1.0f;
	if( percentage == 0.0f ) percentage = 0.000001f;
	CL_Pointf origin = GetFocusOrigin( movement->GetInitialFocus(), pActor );
// std::cout << "Origin is " << origin_i.x << ',' << origin_i.y << std::endl;

	CL_Pointf current = origin;

	CL_Sprite sprite;

	// TODO: Now take whatever it is, and display it at 'current'

	if( anim->GetSpriteTicket() != BattleState::UNDEFINED_SPRITE_TICKET )
		sprite = m_parent.get_sprite( anim->GetSpriteTicket() );


	if( anim->HasBattleSprite() ) {
		switch( anim->GetBattleSprite()->GetWho() ) {
			case WHO_CASTER:
				sprite = m_pCaster->GetCurrentSprite( false );
				break;
			case WHO_TARGET:
				sprite = m_pTarget->GetCurrentSprite( false );
				break;
		}
	}

// 	enum eMovementScriptType { SPRITE_ROTATION, SPRITE_SCALE, SPRITE_PITCH, SPRITE_YAW, CIRCLE_RADIUS, AMPLITUDE };

	float rotation = movement->Rotation();
	float scale;
	float scale_y;
	sprite.get_scale( scale, scale_y );
	float pitch = 0.0f;
	float yaw = 0.0f;
	float radius = movement->circleRadius();
	float amplitude = movement->Amplitude();
	double alpha = sprite.get_alpha();
	float scriptRotation = 0.0f;

	if( movement->hasMovementScript( SpriteMovement::SPRITE_ROTATION ) ) {
		scriptRotation = ( double )movement->executeMovementScript( SpriteMovement::SPRITE_ROTATION, percentage );
	}
	if( movement->hasMovementScript( SpriteMovement::SPRITE_SCALE ) ) {
		scale *= ( double )movement->executeMovementScript( SpriteMovement::SPRITE_SCALE, percentage );
	}
	if( movement->hasMovementScript( SpriteMovement::SPRITE_PITCH ) ) {
		pitch = ( double )movement->executeMovementScript( SpriteMovement::SPRITE_PITCH, percentage );
	}
	if( movement->hasMovementScript( SpriteMovement::SPRITE_YAW ) ) {
		yaw = ( double )movement->executeMovementScript( SpriteMovement::SPRITE_YAW, percentage );
	}
	if( movement->hasMovementScript( SpriteMovement::CIRCLE_RADIUS ) ) {
		radius = ( double )movement->executeMovementScript( SpriteMovement::CIRCLE_RADIUS, percentage );
	}
	if( movement->hasMovementScript( SpriteMovement::AMPLITUDE ) ) {
		amplitude = ( double )movement->executeMovementScript( SpriteMovement::AMPLITUDE, percentage );
	}
	if( movement->hasMovementScript( SpriteMovement::ALPHA ) ) {
		alpha *= ( double )movement->executeMovementScript( SpriteMovement::ALPHA, percentage );
	}


	// Rotation
	// TODO: Switch to away/toward

	bool clockwise = true;

	double degrees = percentage * movement->Rotation();

	if( scriptRotation != 0.0f )
		degrees = scriptRotation;

	if( ( !pActor->IsMonster() && m_parent.MonstersOnLeft() ) ||
					pActor->IsMonster() && !m_parent.MonstersOnLeft() ) {
		degrees = 0 - degrees;

		if( movement->circleDirection() == SpriteMovement::ROTATE_TOWARDS )
			clockwise = false;

	}


	if( movement->circleDirection() == SpriteMovement::COUNTERCLOCKWISE )
		clockwise = false;


	CL_Angle angle = CL_Angle::from_degrees( degrees );
	CL_Angle yaw_angle = CL_Angle::from_radians( yaw );
	CL_Angle pitch_angle = CL_Angle::from_radians( pitch );
	// TODO: Mechanism to affect ALL this character's sprites in case it changes
	sprite.set_angle( angle );
	sprite.set_angle_yaw( yaw_angle );
	sprite.set_angle_pitch( pitch_angle );
	sprite.set_alpha( alpha );
	sprite.set_scale( scale, scale );

	float completion = movement->Completion();
	percentage *= completion;

	if( movement->HasEndFocus() ) {
		CL_Pointf dest = GetFocusOrigin( movement->GetEndFocus(), pActor );
		//(1-p)*A + p*B
		//current = (1.0f - percentage) * origin + percentage * dest;
		switch( movement->GetMovementStyle() ) {
			case SpriteMovement::STRAIGHT:
				current = origin * ( 1.0f - percentage ) + dest * percentage;
				break;
			case SpriteMovement::XONLY:
				current.x = origin.x * ( 1.0f - percentage ) + dest.x * percentage;
				break;
			case SpriteMovement::YONLY:
				current.y = origin.y * ( 1.0f - percentage ) + dest.y * percentage;
				break;
			case SpriteMovement::CIRCLE: {
				float angle_deg = 0.0f;
				if( !movement->hasMovementScript( SpriteMovement::CIRCLE_ANGLE ) ) {
					if( clockwise )
						angle_deg = movement->circleStartAngle() + ( percentage ) * movement->circleDegrees();
					else angle_deg = movement->circleStartAngle() - ( percentage ) * movement->circleDegrees();
				} else {
					angle_deg = ( double )movement->executeMovementScript( SpriteMovement::CIRCLE_ANGLE, percentage );
				}
				if( !movement->hasMovementScript( SpriteMovement::CIRCLE_RADIUS ) ) {
					radius = movement->circleRadius() + percentage * movement->circleGrowth(); // spiral powers
				}
				float angle = ( CL_PI / 180.0f ) * angle_deg;
				CL_Pointf cpoint( cos( angle ), sin( angle ) );
				current = dest +  cpoint * radius; // C + R * (cos A, sin A)
				break;
			}
			case SpriteMovement::SINE: {
				CL_Pointf d, c;
				d.x = dest.x - origin.x;
				d.y = dest.y - origin.y;
				float l = sqrt( ( double )d.x * d.x + d.y + d.y );
				d.x /=  l;
				d.y /= l;
				float w = 2.0f * CL_PI * movement->Periods(); // make this 1/2 to arc up

				current.x = percentage * dest.x + ( 1.0f - percentage ) * origin.x + amplitude * d.y * sin( w * percentage );
				current.y = percentage * dest.y + ( 1.0f - percentage ) * origin.y - amplitude * d.x * sin( w * percentage );
				break;
			}
			case SpriteMovement::ARC_OVER: {
				CL_Pointf d, c;
				d.x = dest.x - origin.x;
				d.y = dest.y - origin.y;
				float l = sqrt( ( double )d.x * d.x + d.y + d.y );
				d.x /=  l;
				d.y /= l;
				float w = 2.0f * CL_PI * 0.5f; // make this 1/2 to arc up

				current.x = percentage * dest.x + ( 1.0f - percentage ) * origin.x + amplitude * d.y * sin( w * percentage );
				current.y = percentage * dest.y + ( 1.0f - percentage ) * origin.y + amplitude * d.x * sin( w * percentage );
				break;
			}
			case SpriteMovement::ARC_UNDER: {
				CL_Pointf d, c;
				d.x = dest.x - origin.x;
				d.y = dest.y - origin.y;
				float l = sqrt( ( double )d.x * d.x + d.y + d.y );
				d.x /=  l;
				d.y /= l;
				float w = 2.0f * CL_PI * 0.5f; // make this 1/2 to arc up

				current.x = percentage * dest.x + ( 1.0f - percentage ) * origin.x + amplitude * d.y * sin( w * percentage );
				current.y = percentage * dest.y + ( 1.0f - percentage ) * origin.y - amplitude * d.x * sin( w * percentage );
				break;
			}
			default:
				break;
		}
	} else {
		// move a set distance in direction
		CL_Pointf direction;
		SpriteMovement::eMovementDirection dir = movement->GetMovementDirection();

		if( dir == SpriteMovement::MOVE_AWAY ) {
			if( m_parent.MonstersOnLeft() ) {
				if( m_pCaster->IsMonster() )
					dir = SpriteMovement::W;
				else dir = SpriteMovement::E;
			} else {
				if( m_pCaster->IsMonster() )
					dir = SpriteMovement::E;
				else dir = SpriteMovement::W;
			}
		} else if( dir == SpriteMovement::MOVE_TOWARDS ) {
			if( !m_parent.MonstersOnLeft() ) {
				if( m_pCaster->IsMonster() )
					dir = SpriteMovement::W;
				else dir = SpriteMovement::E;
			} else {
				if( m_pCaster->IsMonster() )
					dir = SpriteMovement::E;
				else dir = SpriteMovement::W;
			}
		}

		switch( dir ) {
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

		direction *= ( percentage * movement->Distance() );

		current = origin + direction;

	}

	if( movement->Invert() ) {
		float diff_x = origin.x - current.x;
		float diff_y = origin.y - current.y;
		current.x += diff_x * 2;
		current.y += diff_y * 2;
	}

	if( anim->GetSpriteTicket() != BattleState::UNDEFINED_SPRITE_TICKET ) {
		sprite = m_parent.get_sprite( anim->GetSpriteTicket() );
		m_parent.set_sprite_pos( anim->GetSpriteTicket(), current );
	}

	if( anim->HasBattleSprite() ) {
		// Have parent move this thing
		CL_Pointf point;
		point.x = current.x;
		point.y = current.y;
		switch( anim->GetBattleSprite()->GetWho() ) {
			case WHO_CASTER:
				m_pCaster->SetBattlePos( point );
				break;
			case WHO_TARGET:
				m_pTarget->SetBattlePos( point );
				break;
		}
	}


}
/*
void AnimationState::move_character(ICharacter* character, SpriteAnimation* anim, SpriteMovement* movement, float percentage)
{
    if(percentage > 1.0f) percentage = 1.0f;
    CL_Point origin_i = GetFocusOrigin(movement->GetInitialFocus(), character);
// std::cout << "Origin is " << origin_i.x << ',' << origin_i.y << std::endl;
    CL_Pointf origin(origin_i.x,origin_i.y);
    CL_Point dest_i;
    CL_Pointf current = origin;

    // Rotation
    // TODO: Switch to away/toward
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


}
*/

void AnimationState::Draw( const CL_Rect& screenRect, CL_GraphicContext& GC ) {
	if( m_functor_mode ) {
		draw_functor( screenRect, GC );
	} else {
		draw( screenRect, GC );
	}
}


void AnimationState::draw( const CL_Rect &screenRect, CL_GraphicContext& GC ) {
	if( !m_bDone ) {
		uint passed = CL_System::get_time() - m_phase_start_time;
		float percentage = ( float )passed / ( float )( *m_phase_iterator )->GetDurationMs();
		bool draw = true;
		if( percentage >= 1.0f ) {
			EndPhase();
			draw = NextPhase();
			if( draw ) {
				StartPhase();
				percentage = ( float )passed / ( float )( *m_phase_iterator )->GetDurationMs();
			}

		}

		if( draw ) {
			for( std::list<Phase::PhaseComponent>::const_iterator iter = ( *m_phase_iterator )->GetPhaseComponentsBegin();
								iter != ( *m_phase_iterator )->GetPhaseComponentsEnd(); iter++ ) {
				if( iter->m_bAnimation ) {
					SpriteAnimation* anim = iter->m_animation;

					if( !anim->ShouldSkip() && anim->HasSpriteMovement() ) {
						SpriteMovement * movement = anim->GetSpriteMovement();
						if( movement->ForEachTarget() && ( *m_phase_iterator )->InParallel() ) {
							for( uint i = 0; i < m_pTargetGroup->GetCharacterCount(); i++ ) {
								ICharacter * pTarget = m_pTargetGroup->GetCharacter( i );
								move_sprite( pTarget, m_pCaster, anim, movement, percentage );
							}

						} else {
							move_sprite( m_pCaster, m_pTarget, anim, movement, percentage );
						}
					}
					if( !anim->ShouldSkip() && anim->HasAlterSprite() ) {
						apply_alter_sprite( anim->GetAlterSprite() );
					}

				}
			}
		}
	}
	CL_System::sleep( 10 );
}

void AnimationState::draw_functor( const CL_Rect& screenRect, CL_GraphicContext& GC ) {
	bool notasks = false;
	for( int i = m_tasks.size() - 1; i >= 0; i-- ) {
		Task * pTask = m_tasks[i];
		pTask->update( m_pInterpreter );
		if( pTask->finished() ) {

			pTask->finish( m_pInterpreter );
			// Do something with waitFor?
			std::cout << "Removing task: " << m_tasks[i]->GetName() << '@' << std::hex << m_tasks[i] << std::endl;
			m_task_mutex.lock();
			m_tasks.erase( m_tasks.begin() + i );
			notasks = m_tasks.empty();
			m_task_mutex.unlock();
			m_finished_task_mutex.lock();
			m_finished_tasks.insert( pTask );
			m_finished_task_mutex.unlock();
			//m_wait_event.set();
		}
	}
}


bool AnimationState::LastToDraw() const { // Should we continue drawing more states?
	return false;
}

bool AnimationState::DisableMappableObjects() const { // Should the app move the MOs?
	return true;
}

void AnimationState::MappableObjectMoveHook() { // Do stuff right after the mappable object movement
}

bool AnimationState::NextPhase() {

	m_phase_iterator++;
	if( m_phase_iterator == m_pAnim->GetPhasesEnd() ) {
		// Done
		m_bDone = true;
		return false;
	}

	return true;
}

void AnimationState::EndPhase() {
	for( std::list<Phase::PhaseComponent>::const_iterator iter = ( *m_phase_iterator )->GetPhaseComponentsBegin();
						iter != ( *m_phase_iterator )->GetPhaseComponentsEnd(); iter++ ) {
		if( iter->m_bAnimation ) {
			SpriteAnimation* animation = iter->m_animation;
			if( animation->GetSpriteTicket() != BattleState::UNDEFINED_SPRITE_TICKET ) {
				m_parent.remove_sprite( animation->GetSpriteTicket() );
				animation->SetSpriteTicket( BattleState::UNDEFINED_SPRITE_TICKET );
			}
		}
	}
}

void AnimationState::apply_alter_sprite( AlterSprite* pAlterSprite ) {
	CL_Sprite sprite;
	// Alter any sprites on the parent now
	switch( pAlterSprite->GetWho() ) {
		case WHO_CASTER:
			sprite = m_pCaster->GetCurrentSprite( true );
			break;
		case WHO_TARGET:
			sprite = m_pTarget->GetCurrentSprite( true );
			break;
	}

	float scale;
	float alpha;
	sprite.get_scale( scale, scale );
	alpha = sprite.get_alpha();
	CL_Colorf color = sprite.get_color();
	switch( pAlterSprite->GetAlter() ) {
		case AlterSprite::HIDE:
			alpha = 0.0f;
			break;
		case AlterSprite::SMALLER_SIZE:
			sprite.set_scale( 1.0f / 1.5f * scale, 1.0 / 1.5f * scale );
			break;
		case AlterSprite::LARGER_SIZE:
			sprite.set_scale( 1.5f * scale, 1.5f * scale );
			break;
		case AlterSprite::HALF_SIZE:
			sprite.set_scale( 0.5f * scale, 0.5f * scale );
			break;
		case AlterSprite::DOUBLE_SIZE:
			sprite.set_scale( 2.0f * scale, 2.0f * scale );
			break;
		case AlterSprite::NEGATIVE:
			// TODO:
			//sprite.set_color(color * CL_Colorf(0.5f,0.5f,0.5f));
			break;
		case AlterSprite::X_FLIP:
			// TODO:
		case AlterSprite::Y_FLIP:
			// TODO:
			break;
		case AlterSprite::GRAYSCALE:
			sprite.set_color( CL_Colorf( 0.7f, 0.7f, 0.7f ) );
			break;
		case AlterSprite::GREENSCALE:
			sprite.set_color( CL_Colorf( 0.0f, 1.0f, 0.0f ) );
			break;
		case AlterSprite::REDSCALE:
			sprite.set_color( CL_Colorf( 1.0f, 0.0f, 0.0f ) );
			break;
		case AlterSprite::BLUESCALE:
			sprite.set_color( CL_Colorf( 0.0f, 0.0f, 1.0f ) );
			break;
		case AlterSprite::RESET:
			sprite.set_color( CL_Colorf( 1.0f, 1.0f, 1.0f ) );
			sprite.set_scale( 1.0f, 1.0f );
			alpha = 1.0f;
			break;
	}

	sprite.set_alpha( alpha );
}

void AnimationState::StartPhase() {
	Phase * phase = *m_phase_iterator;
	phase->Execute();
	m_phase_start_time = CL_System::get_time();

	for( std::list<Phase::PhaseComponent>::const_iterator iter = ( *m_phase_iterator )->GetPhaseComponentsBegin();
						iter != ( *m_phase_iterator )->GetPhaseComponentsEnd(); iter++ ) {
		if( iter->m_bAnimation ) {
			SpriteAnimation* animation = iter->m_animation;
			if( animation->HasAlterSprite() ) {
				apply_alter_sprite( animation->GetAlterSprite() );
			}

			if( animation->HasBattleSprite() ) {
				// Change battle sprite using the parent now
				// to GetWhich
			}

			if( animation->HasSpriteStub() ) {
				// Create sprite on parent state
				// keep pointer to it around
				SpriteStub* stub = animation->GetSpriteStub();
				// Monsters don't have weapons for now. TODO: Don't assume this
				if( !m_pCaster->IsMonster() ) {
					Character * pCharacter = dynamic_cast<Character*>( m_pCaster );
					Equipment::eSlot slot = Equipment::EHAND;
					switch(stub->Which()){
						case SpriteStub::MAIN:
							slot = Equipment::EHAND;
							break;
						case SpriteStub::OFF:
							slot = Equipment::EOFFHAND;
							break;
						case SpriteStub::DEFAULT:
							slot = m_hand;
							break;
					}
					Equipment* equipment = pCharacter->GetEquipment( slot );
					Weapon* pWeapon = dynamic_cast<Weapon*>( equipment );
					if( pWeapon ) {
						animation->Unskip();
						WeaponType* pType = pWeapon->GetWeaponType();
						animation->SetSpriteTicket( m_parent.add_sprite( pType->GetSprite() ) );
					} else {
						animation->Skip();
					}
				} else {
					animation->Skip();
				}

			}

			if( animation->HasSpriteRef() ) {
				animation->SetSpriteTicket( m_parent.add_sprite( animation->GetSpriteRef()->CreateSprite() ) );
			}
		} else {
			SoundManager::PlaySound( iter->m_soundplay->GetSound() );
		}
	}

	return;
}

void AnimationState::Start() {
	m_bDone = false;
	if( m_functor_mode ) {
		m_pRunner->setFunctor( m_functor );
		m_steel_thread.start( m_pRunner );
	} else {
		m_phase_iterator = m_pAnim->GetPhasesBegin();
		StartPhase();
	}
}

void AnimationState::FunctorCompleted() {
	m_bDone = true;
}


void AnimationState::SteelInit( SteelInterpreter *pInterpreter ) {
	if( m_functor_mode ) {
		using namespace Steel;
		m_pInterpreter = pInterpreter;
		m_pRunner = new AnimationRunner( pInterpreter, this );
		pInterpreter->pushScope();
		pInterpreter->addFunction( "sine_wave", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::sine_wave ) );
		pInterpreter->addFunction( "arc_over", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::arc_over ) );
		pInterpreter->addFunction( "arc_under", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::arc_under ) );
		pInterpreter->addFunction( "createSprite", "anim", new SteelFunctor1Arg<AnimationState, const std::string&>( this, &AnimationState::createSprite ) );
		pInterpreter->addFunction( "getCharacterSprite", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Handle>( this, &AnimationState::getCharacterSprite ) );
		pInterpreter->addFunction( "getWeaponSprite", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::getWeaponSprite ) );
		pInterpreter->addFunction( "removeSprite", "anim", new SteelFunctor1Arg<AnimationState, int>( this, &AnimationState::removeSprite ) );
		pInterpreter->addFunction( "getCharacterLocale", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::getCharacterLocale ) );
		pInterpreter->addFunction( "getGroupLocale", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::getGroupLocale ) );
		pInterpreter->addFunction( "getScreenLocale", "anim", new SteelFunctor1Arg<AnimationState, int>( this, &AnimationState::getScreenLocale ) );
		pInterpreter->addFunction( "getSpriteLocale", "anim", new SteelFunctor2Arg<AnimationState, int, int>( this, &AnimationState::getSpriteLocale ) );
		pInterpreter->addFunction( "setLocaleOffset", "anim", new SteelFunctor3Arg<AnimationState, SteelType::Handle, int, int>( this, &AnimationState::setLocaleOffset ) );
		pInterpreter->addFunction( "createPath", "anim", new SteelFunctor4Arg<AnimationState, SteelType::Handle, SteelType::Handle, SteelType::Functor, double>( this, &AnimationState::createPath ) );
		pInterpreter->addFunction( "changePathStart", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::changePathStart ) );
		pInterpreter->addFunction( "changePathEnd", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::changePathEnd ) );

		pInterpreter->addFunction( "setPathSpeedFunction", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Functor>( this, &AnimationState::setPathSpeedFunction ) );
		pInterpreter->addFunction( "setPathCompletion", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, double>( this, &AnimationState::setPathCompletion ) );
		pInterpreter->addFunction( "setPathFlags", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::setPathFlags ) );

		pInterpreter->addFunction( "moveSprite", "anim", new SteelFunctor2Arg<AnimationState, int, SteelType::Handle>( this, &AnimationState::moveSprite ) );
		pInterpreter->addFunction( "createRotation", "anim", new SteelFunctor3Arg<AnimationState, SteelType::Functor, double, int>( this, &AnimationState::createRotation ) );
		pInterpreter->addFunction( "rotateSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::rotateSpriteTimed ) );
		pInterpreter->addFunction( "movePathTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::moveSpriteTimed ) );

		pInterpreter->addFunction( "moveSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::moveSpriteTimed ) );



		pInterpreter->addFunction( "syncTo", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::syncTo ) );
		pInterpreter->addFunction( "rotateSprite", "anim", new SteelFunctor2Arg<AnimationState, int, SteelType::Handle>( this, &AnimationState::rotateSprite ) );
		pInterpreter->addFunction( "createShaker", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Functor, int>( this, &AnimationState::createShaker ) );
		pInterpreter->addFunction( "shake", "anim",new SteelFunctor3Arg<AnimationState, SteelType::Handle, SteelType::Handle, double>( this, &AnimationState::shake ) );
		pInterpreter->addFunction( "createFade", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Functor>( this, &AnimationState::createFade ) );
		pInterpreter->addFunction( "fadeSprite", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::fadeSprite ) );
		pInterpreter->addFunction( "createColorizer", "anim", new SteelFunctor3Arg<AnimationState, SteelType::Functor, SteelType::Functor, SteelType::Functor>( this, &AnimationState::createColorizer ) );

		pInterpreter->addFunction( "colorizeSprite", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::colorizeSprite ) );
		pInterpreter->addFunction( "startAfter", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Handle>( this, &AnimationState::startAfter ) );
		pInterpreter->addFunction( "doAfter", "anim", new SteelFunctor2Arg<AnimationState, double, SteelType::Functor>( this, &AnimationState::doAfter ) );
		pInterpreter->addFunction( "doFunction", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Functor>( this, &AnimationState::doFunction ) );
		pInterpreter->addFunction( "pause", "anim", new SteelFunctor1Arg<AnimationState, double>( this, &AnimationState::pause ) );
		pInterpreter->addFunction( "chainTasks", "anim", new SteelFunctor1Arg<AnimationState, const Steel::SteelArray&>( this, &AnimationState::chainTasks ) );
		pInterpreter->addFunction( "waitFor", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Handle>( this, &AnimationState::waitFor ) );
		pInterpreter->addFunction( "waitForAll", "anim", new SteelFunctor1Arg<AnimationState, const Steel::SteelArray&>( this, &AnimationState::waitForAll ) );
		pInterpreter->addFunction( "start", "anim", new SteelFunctor1Arg<AnimationState, SteelType::Handle>( this, &AnimationState::start ) );
		pInterpreter->addFunction( "startAll", "anim", new SteelFunctor1Arg<AnimationState, const Steel::SteelArray&>( this, &AnimationState::startAll ) );

		SteelConst( pInterpreter, "$_TOP_LEFT", ( int )Locale::TOP_LEFT );
		SteelConst( pInterpreter, "$_TOP_RIGHT", ( int )Locale::TOP_RIGHT );
		SteelConst( pInterpreter, "$_TOP_CENTER", ( int )Locale::TOP_CENTER );
		SteelConst( pInterpreter, "$_MIDDLE_LEFT", ( int )Locale::MIDDLE_LEFT );
		SteelConst( pInterpreter, "$_MIDDLE_RIGHT", ( int )Locale::MIDDLE_RIGHT );
		SteelConst( pInterpreter, "$_CENTER", ( int )Locale::CENTER );
		SteelConst( pInterpreter, "$_BOTTOM_LEFT", ( int )Locale::BOTTOM_LEFT );
		SteelConst( pInterpreter, "$_BOTTOM_RIGHT", ( int )Locale::BOTTOM_RIGHT );
		SteelConst( pInterpreter, "$_BOTTOM_CENTER", ( int )Locale::BOTTOM_CENTER );
		SteelConst( pInterpreter, "$_MOVE_NORMAL", ( int )Path::NORMAL );
		SteelConst( pInterpreter, "$_MOVE_NO_VERTICAL", ( int )Path::NO_VERTICAL );
		SteelConst( pInterpreter, "$_MOVE_NO_HORIZONTAL", ( int )Path::NO_HORIZONTAL );


		SteelConst( pInterpreter, "$_AXIS_ROLL", ( int )Rotation::ROLL );
		SteelConst( pInterpreter, "$_AXIS_YAW", ( int )Rotation::YAW );
		SteelConst( pInterpreter, "$_AXIS_PITCH", ( int )Rotation::PITCH );
	}
}

void AnimationState::SteelCleanup( SteelInterpreter *pInterpreter ) {
	if( m_functor_mode ) {
		pInterpreter->removeFunctions( "anim" );
		pInterpreter->popScope();
	}
}

CL_Pointf AnimationState::GetGroupOffset( ICharacterGroup* igroup ) const {
	return m_parent.get_offset( igroup );
}

BattleState::SpriteTicket AnimationState::GetSpriteForChar( ICharacter* iChar ) {
	return m_parent.get_sprite_for_char( iChar );
}

CL_Pointf AnimationState::GetSpriteOffset( BattleState::SpriteTicket sprite ) const {
	return m_parent.get_offset( sprite );
}

void AnimationState::SetGroupOffset( ICharacterGroup* igroup, const CL_Pointf& pt ) {
	m_parent.set_offset( igroup, pt );
}

void AnimationState::SetSpriteOffset( BattleState::SpriteTicket sprite, const CL_Pointf& pt ) {
	m_parent.set_offset( sprite, pt );
}


SteelType AnimationState::sine_wave( double p ) {
	SteelType var;
	var.set( sin( p ) );
	return var;
}

SteelType AnimationState::arc_over( double p ) {
	SteelType var;
	var.set( sin( p * M_PI * 2.0f ) );
	return var;
}

SteelType AnimationState::arc_under( double p ) {
	SteelType var;
	var.set( -sin( p * M_PI ) );
	return var;
}

SteelType AnimationState::createSprite( const std::string& sprite_ref ) {
	class SpriteFunctor : public IApplication::Functor {
	public:
		SpriteFunctor( const std::string& spriteRef, AnimationState& state )
			: m_spriteRef( spriteRef ), m_state( state ) {
		}
		virtual void operator()() {
			m_sprite = GraphicsManager::CreateSprite( m_spriteRef );
			m_ticket = m_state.AddSprite( m_sprite );
		}
		std::string     m_spriteRef;
		BattleState::SpriteTicket m_ticket;
		CL_Sprite       m_sprite;
		AnimationState& m_state;
	} functor( sprite_ref, *this );
	CL_Event event;
	IApplication::GetInstance()->RunOnMainThread( event, &functor );
	event.wait();
	SteelType var;
	m_added_sprites.insert( functor.m_ticket );
	var.set( functor.m_ticket );
	return var;
}

SteelType AnimationState::getCharacterSprite( SteelType::Handle iCharacter ) {
	ICharacter* ichar = Steel::GrabHandle<ICharacter*>( iCharacter );
	SteelType var;
	var.set( m_parent.get_sprite_for_char( ichar ) );
	return var;
}

SteelType AnimationState::getWeaponSprite( SteelType::Handle iCharacter, int hand ) {
	SteelType var;
	Character * pCharacter = dynamic_cast<Character*>( m_pCaster );
	Equipment* equipment = pCharacter->GetEquipment( hand == SpriteStub::MAIN ? Equipment::EHAND : Equipment::EOFFHAND );
	Weapon* pWeapon = dynamic_cast<Weapon*>( equipment );
	if( pWeapon ) {
		WeaponType* pType = pWeapon->GetWeaponType();
		var.set( m_parent.add_sprite( pType->GetSprite() ) );
	} else {
		var.set( BattleState::UNDEFINED_SPRITE_TICKET );
	}

	return var;
}

SteelType AnimationState::removeSprite( int sprite ) {
	m_parent.remove_sprite( sprite );
	return SteelType();
}

SteelType AnimationState::getCharacterLocale( SteelType::Handle iCharacter, int corner ) {
	Locale* locale = new Locale( Locale::CHARACTER, ( Locale::Corner )corner );
	locale->SetIChar( Steel::GrabHandle<ICharacter*>( iCharacter ) );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::getGroupLocale( SteelType::Handle iGroup, int corner ) {
	Locale* locale = new Locale( Locale::GROUP, ( Locale::Corner )corner );
	locale->SetGroup( Steel::GrabHandle<ICharacterGroup*>( iGroup ) );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::getScreenLocale( int corner ) {
	Locale* locale = new Locale( Locale::SCREEN, ( Locale::Corner )corner );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::getSpriteLocale( int sprite, int corner ) {
	Locale* locale = new Locale( Locale::SPRITE, ( Locale::Corner )corner );
	SteelType var;
	var.set( locale );
	m_handles.push_back( locale );
	return var;
}

SteelType AnimationState::setLocaleOffset( SteelType::Handle hLocale, int x, int y ) {
	Locale * locale = Steel::GrabHandle<Locale*>( hLocale );
	locale->SetOffset( CL_Point( x, y ) );
	return SteelType();
}


SteelType AnimationState::createPath( SteelType::Handle hStartLocale, SteelType::Handle hEndLocale,
																																						SteelType::Functor functor, double pixels_per_ms ) {
	Path * path = new Path();
	path->m_start = *Steel::GrabHandle<Locale*>( hStartLocale );
	path->m_end = *Steel::GrabHandle<Locale*>( hEndLocale );
	path->m_functor = functor;
	path->m_speed = pixels_per_ms;
	m_handles.push_back( path );
	SteelType var;
	var.set( path );
	return var;
}

SteelType AnimationState::changePathStart( SteelType::Handle hPath, SteelType::Handle hStartLocale ) {
	Path * path = Steel::GrabHandle<Path*>( hPath );
	Locale* start = Steel::GrabHandle<Locale*>( hStartLocale );

	path->m_start = *start;
	return SteelType();
}

SteelType AnimationState::changePathEnd( SteelType::Handle hPath, SteelType::Handle hEndLocale ) {
	Path * path = Steel::GrabHandle<Path*>( hPath );
	Locale* end = Steel::GrabHandle<Locale*>( hEndLocale );

	path->m_start = *end;
	return SteelType();
}

SteelType AnimationState::setPathSpeedFunction( SteelType::Handle hPath, SteelType::Functor functor ) {
	Path* path = Steel::GrabHandle<Path*>( hPath );
	path->m_speed_functor = functor;
	return SteelType();
}

SteelType AnimationState::setPathCompletion( SteelType::Handle hPath, double completion ) {
	Path* path = Steel::GrabHandle<Path*>( hPath );
	path->m_completion = completion;
	return SteelType();
}

SteelType AnimationState::setPathFlags( SteelType::Handle hPath, int flags ) {
	Path* path = Steel::GrabHandle<Path*>( hPath );
	path->m_flags = flags;
	return SteelType();
}

SteelType AnimationState::moveSprite( int sprite, SteelType::Handle hpath ) {
	PathTask* task = new PathTask( *this );
	std::cout << "PathTask created: " << std::hex << task << std::endl;
	task->SetSprite( sprite );
	task->init( Steel::GrabHandle<Path*>( hpath ) );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::moveSpriteTimed( int sprite, SteelType::Handle hpath, double seconds ) {
	TimedPathTask* task = new TimedPathTask( *this );
	std::cout << "TimedPathTask created: " << std::hex << task << std::endl;
	task->SetSprite( sprite );
	task->SetDuration( seconds );
	task->init( Steel::GrabHandle<Path*>( hpath ) );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::createRotation( SteelType::Functor functor, double degrees, int axis ) {
	Rotation* rotation = new Rotation();
	rotation->m_axis = ( Rotation::Axis )axis;
	rotation->m_degrees = degrees;
	rotation->m_functor = functor;
	m_handles.push_back( rotation );
	SteelType var;
	var.set( rotation );
	return var;
}

SteelType AnimationState::syncTo( SteelType::Handle hTask, SteelType::Handle hWithTask ) {
	Task* task = Steel::GrabHandle<Task*>( hTask );
	Task* withTask = Steel::GrabHandle<Task*>( hWithTask );
	task->SyncTo( withTask );
	SteelType var;
	var.set( withTask );
	return var;
}

SteelType AnimationState::doAfter( double after, SteelType::Functor f ) {
	FunctionTask * task = new FunctionTask( *this );
	task->init( after, f );
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::doFunction( SteelType::Functor f ) {
	return doAfter( 0, f );
}



SteelType AnimationState::rotateSprite( int sprite, SteelType::Handle hRotation ) {
	Rotation * rot = Steel::GrabHandle<Rotation*>( hRotation );
	RotationTask * task = new RotationTask( *this );
	std::cout << "RotationTask created: " << std::hex << task << std::endl;
	task->init( *rot );
	task->SetSprite( sprite );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::rotateSpriteTimed( int sprite, SteelType::Handle hRotation, double seconds ) {
	Rotation * rot = Steel::GrabHandle<Rotation*>( hRotation );
	rot->m_duration = seconds;
	TimedRotationTask * task = new TimedRotationTask( *this );
	std::cout << "TimedRotationTask created: " << std::hex << task << std::endl;
	task->init( *rot );
	task->SetSprite( sprite );
	SteelType var;
	var.set( task );
	m_handles.push_back( task );
	return var;
}

SteelType AnimationState::createShaker( SteelType::Functor magnitude, int flags ) {
	Shaker * shaker = new Shaker();
	shaker->m_functor = magnitude;
	shaker->m_flags = flags;
	m_handles.push_back( shaker );
	SteelType var;
	var.set( shaker );
	return var;
}

SteelType AnimationState::shake( SteelType::Handle hlocale, SteelType::Handle hShaker, double seconds ) {
	Locale loc = *Steel::GrabHandle<Locale*>( hlocale );
	Shaker * shaker = Steel::GrabHandle<Shaker*>( hShaker );
	ShakerTask * task = new ShakerTask( *this );
	task->SetDuration( seconds );
	task->init( *shaker, loc );
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::createFade( SteelType::Functor functor ) {
	Fade * fade = new Fade();
	fade->m_functor = functor;
	m_handles.push_back( fade );
	SteelType var;
	var.set( fade );
	return var;
}

SteelType AnimationState::fadeSprite( int sprite, SteelType::Handle hFade, double seconds ) {
	Fade * fade = Steel::GrabHandle<Fade*>( hFade );
	FadeTask * task = new FadeTask( *this );
	task->SetSprite( sprite );
	task->SetDuration( seconds );
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::createSimpleColorizer( double r, double g, double b, double seconds ) {
	return SteelType();
}

SteelType AnimationState::createColorizer( SteelType::Functor r_func, SteelType::Functor g_func, SteelType::Functor b_func ) {
	Colorizer * col = new Colorizer();
	SteelType var;
	var.set( col );
	col->m_red = r_func;
	col->m_green = g_func;
	col->m_blue = b_func;
	m_handles.push_back( col );
	return var;
}

SteelType AnimationState::colorizeSprite( int sprite, SteelType::Handle hColorizer, double seconds ) {
	ColorizeTask * task = new ColorizeTask( *this );

	task->init( *Steel::GrabHandle<Colorizer*>( hColorizer ) );
	m_handles.push_back( task );
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::startAfter( SteelType::Handle htask, SteelType::Handle hnexttask ) {
	Task* task = Steel::GrabHandle<Task*>( htask );
	Task* next = Steel::GrabHandle<Task*>( hnexttask );
	task->SetNextTask( next );
	SteelType var;
	var.set( next );
	return var;
}

SteelType AnimationState::pause( double seconds ) {
	CL_System::sleep( seconds * 1000.0 );
	return SteelType();
}


SteelType AnimationState::chainTasks( const Steel::SteelArray& array ) {
	Task * lastTask = NULL;
	for( int i = 0; i < array.size(); i++ ) {
		SteelType::Handle htask = ( SteelType::Handle )array[i];
		Task * task = Steel::GrabHandle<Task*>( htask );
		if( lastTask ) {
			lastTask->SetNextTask( task );
		}

		lastTask = task;
	}
	SteelType var;
	var.set( lastTask ); // this way you can wait on the chainTasks for the last one to complete
	return var;
}

SteelType AnimationState::waitFor( SteelType::Handle waitOn ) {
	SteelType val;
	val.set( waitOn );
	Task * pTask = Steel::GrabHandle<Task*>( waitOn );
	std::cout << "Waiting on:" << pTask->GetName() << std::hex << pTask << std::endl;
#if 0
	std::cout << "Going to lock ftm" << std::endl;
	m_finished_task_mutex.lock();
	if( m_finished_tasks.find( pTask ) != m_finished_tasks.end() ) {
		std::cout << "Task already finished." << std::endl;
		m_finished_task_mutex.unlock();
		return val;
	}
	m_finished_task_mutex.unlock();
	std::cout << "Unlocked ftm after initial check" << std::endl;
#endif
	if( !pTask->started() ) {
		pTask->start( m_pInterpreter );
		AddTask( pTask );
	}
	while( true ) {
		m_finished_task_mutex.lock();
		if( m_finished_tasks.find( pTask ) != m_finished_tasks.end() ) {
			m_finished_task_mutex.unlock();
			break;
		}
		m_finished_task_mutex.unlock();
	}
	std::cout << "Done waiting for: " << pTask->GetName() << std::endl;
	return val;
}

SteelType AnimationState::waitForAll( const Steel::SteelArray& alltasks ) {
	startAll( alltasks ); // start in case some aren't started
	Steel::SteelArray remaining = alltasks;
	while( !remaining.empty() ) {
		waitFor( remaining.front() );
		remaining.pop_front();
	}
	return SteelType();
}

SteelType AnimationState::start( SteelType::Handle hTask ) {
	Task * task = Steel::GrabHandle<Task*>( hTask );
	if( !task->started() ) {
		AddTask( task );
		task->start( m_pInterpreter );
	}
	SteelType var;
	var.set( task );
	return var;
}

SteelType AnimationState::startAll( const Steel::SteelArray& alltasks ) {
	m_task_mutex.lock();
	for( int i = 0; i < alltasks.size(); i++ ) {
		Task * task = Steel::GrabHandle<Task*>( alltasks[i] );
		if( !task->started() ) {
			std::cout << "Starting task:" << task->GetName() << " @ " << std::hex << task << std::endl;
			m_tasks.push_back( task );
			task->start( m_pInterpreter );
		}
	}
	m_task_mutex.unlock();
	return SteelType();
}





void AnimationState::Finish() { // Hook to clean up or whatever after being popped
	std::cout << "AnimationState::Finish" << std::endl;
	m_pCaster = NULL;
	m_pTarget = NULL;
	m_pCasterGroup = NULL;
	m_pTargetGroup = NULL;
	m_task_mutex.lock();
	m_tasks.clear();
	for( std::set<Task*>::iterator it = m_finished_tasks.begin(); it != m_finished_tasks.end(); it++ ) {
		std::cout << "Finishing up task: " << ( *it )->GetName() << std::endl;
		( *it )->cleanup();
		( *it )->SyncTo( NULL );
	}
	m_finished_tasks.clear();
	for( std::list<SteelType::IHandle*>::const_iterator it = m_handles.begin(); it != m_handles.end(); it++ ) {
		std::cout << std::hex << ( long )*it << std::endl;
		delete *it;
	}
	m_handles.clear();
	for( std::set<BattleState::SpriteTicket>::const_iterator it = m_added_sprites.begin(); it != m_added_sprites.end(); it++ ) {
		m_parent.remove_sprite( *it );
	}
	m_task_mutex.unlock();
}



void AnimationState::Darken( int mode, float r, float g, float b, float a ) {
	m_parent.SetDarkMode( mode, r, g, b, a );
}

void AnimationState::ClearDark( int mode ) {
	m_parent.ClearDarkMode( mode );
}







/**
 *  Tasks
 *
 *
 * */

AnimationState::Locale::Locale() {

}

AnimationState::Locale::~Locale() {
}

AnimationState::Locale::Locale( AnimationState::Locale::Type type, AnimationState::Locale::Corner corner ) {
	m_type = type;
	m_corner = corner;
}

void AnimationState::Locale::SetGroup( ICharacterGroup* pGroup ) {
	m_target.as_group = pGroup;
}

void AnimationState::Locale::SetIChar( ICharacter* pChar ) {
	m_target.as_char = pChar;
	m_type = AnimationState::Locale::CHARACTER;
}

void AnimationState::Locale::SetSprite( BattleState::SpriteTicket sprite ) {
	m_target.as_sprite = sprite;
}

void AnimationState::Locale::SetOffset( const CL_Point& offset ) {
	m_offset = offset;
}

void AnimationState::Locale::SetType( AnimationState::Locale::Type type ) {
	m_type = type;
}

void AnimationState::Locale::SetCorner( AnimationState::Locale::Corner corner ) {
	m_corner = corner;
}

ICharacter* AnimationState::Locale::GetChar() const {
	assert( m_type == CHARACTER );
	return m_target.as_char;
}

ICharacterGroup* AnimationState::Locale::GetGroup() const {
	assert( m_type == GROUP );
	return m_target.as_group;
}

BattleState::SpriteTicket AnimationState::Locale::GetSprite() const {
	assert( m_type == SPRITE );
	return m_target.as_sprite;
}

CL_Sprite AnimationState::GetSprite( BattleState::SpriteTicket sprite ) {
	return m_parent.get_sprite( sprite );
}

float AnimationState::Task::percentage() const {
	float perc = 0.0f;
	if( m_sync_task ) {
		perc =  m_sync_task->percentage();
	} else {
		perc = _percentage();
	}
	// Multiplying a vector by zero moves it to the corner..
	if( perc == 0.0f ) {
		perc = 0.000001f;
	}
	
	if( perc > 1.0f){
		perc = 1.0f;
	}

	return perc;
}

CL_Pointf AnimationState::Task::get_mid_point( const CL_Pointf& start, const CL_Pointf& end, float p ) {
	return start * ( 1.0f - p ) + end * p;
}

// M = S * (1-p) + E * p
// Solve for p
// M / E

CL_Pointf AnimationState::Task::get_position( const AnimationState::Locale& locale ) const {
	CL_Rectf rect;
	switch( locale.GetType() ) {
		case Locale::CHARACTER:
			rect = m_state.GetCharacterRect( locale.GetChar() );
			break;
		case Locale::SPRITE:
			rect = m_state.GetSpriteRect( locale.GetSprite() );
			break;
		case Locale::SCREEN:
			rect = IApplication::GetInstance()->GetDisplayRect();
			break;
		case Locale::GROUP:
			rect = m_state.GetGroupRect( locale.GetGroup() );
			break;
	}

	CL_Pointf point;

	switch( locale.GetCorner() ) {
		case Locale::TOP_LEFT:
			point =  rect.get_top_left();
			break;
		case Locale::TOP_RIGHT:
			point = rect.get_top_right();
			break;
		case Locale::TOP_CENTER:
			point = rect.get_top_left();
			point.x += rect.get_width() / 2.0f;
			break;
		case Locale::MIDDLE_LEFT:
			point = rect.get_center();
			point.y -= rect.get_width() / 2.0f;
			break;
		case Locale::MIDDLE_RIGHT:
			point = rect.get_center();
			point.y += rect.get_width() / 2.0f;
			break;
		case Locale::CENTER:
			point = rect.get_center();
			break;
		case Locale::BOTTOM_LEFT:
			point = rect.get_bottom_left();
			break;
		case Locale::BOTTOM_RIGHT:
			point = rect.get_bottom_right();
			break;
		case Locale::BOTTOM_CENTER:
			point = rect.get_bottom_left();
			point.x += rect.get_width() / 2.0f;
			break;
	}

	point += locale.GetOffset();
	return point;
}

AnimationState::PathTask::~PathTask() {

}


void AnimationState::PathTask::init( Path* path ) {
	m_path = path;
	m_path->m_completion = 1.0f;
}


void AnimationState::PathTask::SetCompletion( float completion ) {
	m_path->m_completion = completion;
}

void AnimationState::PathTask::SetFlags( int flags ) {
	m_path->m_flags = flags;
}

void AnimationState::PathTask::SetSpeed( float pixels_per_ms ) {
	m_path->m_speed = pixels_per_ms;
}


void AnimationState::PathTask::SetStart( const AnimationState::Locale& start ) {
	m_path->m_start = start;
}

void AnimationState::PathTask::SetEnd( const AnimationState::Locale& end ) {
	m_path->m_end = end;
}


void AnimationState::PathTask::SetSpeedFunctor( SteelType::Functor functor ) {
	m_path->m_speed_functor = functor;
}

void AnimationState::PathTask::start( SteelInterpreter* pInterpreter ) {
	Task::start( pInterpreter );
	m_cur_pos = get_position( m_path->m_start );
	m_percentage_so_far = 0.0f;
	m_start_time = CL_System::get_time();
}

void AnimationState::PathTask::update( SteelInterpreter* pInterpreter ) {
	assert( m_path );
	float speed = m_path->m_speed;
	if( m_path->m_speed_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		speed = ( double )m_path->m_speed_functor->Call( pInterpreter, params );
	}
	CL_Pointf start = get_position( m_path->m_start );
	CL_Pointf true_end = get_position( m_path->m_end );

	//CL_Pointf end = (start + true_end) * m_completion;
	CL_Pointf end = get_mid_point( start, true_end, m_path->m_completion );


	if( m_path->m_flags & Path::NO_HORIZONTAL ) {
		end.x = start.x;
	}

	if( m_path->m_flags & Path::NO_VERTICAL ) {
		end.y = start.y;
	}

	float p = percentage();
	float elapsed = float( CL_System::get_time() - m_start_time );
	int pixels = int( speed * elapsed );
	float full_pixel_dist = start.distance( end );
	float increment_percent = 0.0f;
	bool done = false;
	if( full_pixel_dist )
		increment_percent = float( pixels ) / full_pixel_dist;

	if( p + increment_percent >= 1.0f ) {
		increment_percent = 1.0f - p;
		done = true;
	}

	m_cur_pos = get_mid_point( m_cur_pos, end, increment_percent );

	double dis = 1.0f;
	if( m_path->m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		dis += ( double )m_path->m_functor->Call( pInterpreter, params );
	}
	// TODO: Adjust Y component by function output
	CL_Pointf adj_pos = m_cur_pos;
	adj_pos += CL_Pointf( -( end.y - start.y ), ( end.x - start.x ) ).normalize() * dis;
	m_state.SetSpritePos( m_sprite, adj_pos );


	m_percentage_so_far = m_cur_pos.distance( start ) / full_pixel_dist;
	if( done ) m_percentage_so_far = 1.0f;
}

bool AnimationState::PathTask::finished() {
	return percentage() >= 1.0f;
}

float AnimationState::PathTask::_percentage() const {
	return m_percentage_so_far;
}


void AnimationState::TimedPathTask::init( Path* path ) {
	m_path = path;
}

void AnimationState::TimedPathTask::start( SteelInterpreter* pInterpreter ) {
	TimedTask::start( pInterpreter );
}

void AnimationState::TimedPathTask::update( SteelInterpreter* pInterpreter ) {
	assert( m_path );
	CL_Pointf start = get_position( m_path->m_start );
	CL_Pointf end = get_position( m_path->m_end );
	if( m_path->m_flags & Path::NO_HORIZONTAL ) {
		end.x = start.x;
	}

	if( m_path->m_flags & Path::NO_VERTICAL ) {
		end.y = start.y;
	}

	CL_Pointf pos = get_mid_point( start, end, percentage() );
	m_state.SetSpritePos( m_sprite, pos );
}

void AnimationState::TimedPathTask::SetStart( const Locale& start ) {
	m_path->m_start = start;
}

void AnimationState::TimedPathTask::SetFlags( int flags ) {
	m_path->m_flags = flags;
}

void AnimationState::TimedPathTask::SetEnd( const Locale& end ) {
	m_path->m_end = end;
}

void AnimationState::RotationTask::init( const Rotation& rot ) {
	m_rotation = rot;
}

void AnimationState::RotationTask::start( SteelInterpreter* ) {
	m_degrees = 0.0f;
	m_last_time = CL_System::get_time();
}

void AnimationState::RotationTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float speed = ( double )m_functor->Call( pInterpreter, params );
	float delta = speed * float( CL_System::get_time() - m_last_time );
	m_degrees += abs( delta );
	if( m_degrees > 1.0f )
		delta -=  m_degrees - 1.0f; // lessen the delta by how much we overshot
	CL_Sprite sprite = m_state.GetSprite( m_sprite );
	switch( m_rotation.m_axis ) {
		case Rotation::PITCH:
			sprite.rotate_pitch( CL_Angle::from_degrees( delta ) );
			break;
		case Rotation::YAW:
			sprite.rotate_yaw( CL_Angle::from_degrees( delta ) );
			break;
		case Rotation::ROLL:
			sprite.rotate( CL_Angle::from_degrees( delta ) );
			break;
	}
}

bool AnimationState::RotationTask::finished() {
	return percentage() >= 1.0f;
}


float AnimationState::RotationTask::_percentage() const {
	return  m_degrees / m_completion_degrees;
}

void AnimationState::RotationTask::cleanup() {

	switch( m_rotation.m_axis ) {
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( CL_Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::YAW:
			m_state.GetSprite( m_sprite ).set_angle_yaw( CL_Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::PITCH:
			m_state.GetSprite( m_sprite ).set_angle_pitch( CL_Angle::from_degrees( 0.0f ) );
			break;
	}
}


void AnimationState::TimedRotationTask::init( const Rotation& rot ) {
	m_rotation = rot;
}

void AnimationState::TimedRotationTask::start( SteelInterpreter* pInterpreter ) {
	TimedTask::start( pInterpreter );
}
void AnimationState::TimedRotationTask::update( SteelInterpreter* pInterpreter ) {
	float angle = 0.0f;
	if( m_rotation.m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		angle = ( double )m_rotation.m_functor->Call( pInterpreter, params );
	}
#ifndef NDEBUG
	//std::cout << "Angle is " << angle << std::endl;
#endif
	switch( m_rotation.m_axis ) {
		case Rotation::YAW:
			m_state.GetSprite( m_sprite ).set_angle_yaw( CL_Angle::from_degrees( angle ) );
			break;
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( CL_Angle::from_degrees( angle ) );
			break;
		case Rotation::PITCH:
			m_state.GetSprite( m_sprite ).set_angle_pitch( CL_Angle::from_degrees( angle ) );
			break;
	}
}
bool AnimationState::TimedRotationTask::finished() {
	return TimedTask::finished();
}

void AnimationState::TimedRotationTask::cleanup() {
	switch( m_rotation.m_axis ) {
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( CL_Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::YAW:
			m_state.GetSprite( m_sprite ).set_angle_yaw( CL_Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::PITCH:
			m_state.GetSprite( m_sprite ).set_angle_pitch( CL_Angle::from_degrees( 0.0f ) );
			break;
	}
}


void AnimationState::ShakerTask::init( const Shaker& shaker, const Locale& locale ) {
	m_locale = locale;
	m_shaker = shaker;
}
void AnimationState::ShakerTask::SetDuration( float duration ) {
	m_duration = duration;
}

void AnimationState::ShakerTask::start( SteelInterpreter* pInterpreter ) {
	TimedTask::start( pInterpreter );
	pick_rand_dir();
}

void AnimationState::ShakerTask::pick_rand_dir() {
	do {
		int d = rand() % 4;
		if( d == 0 )
			m_dir = Direction::NORTH;
		else if( d == 1 )
			m_dir = Direction::EAST;
		else if( d == 2 )
			m_dir = Direction::WEST;
		else m_dir = Direction::SOUTH;
	} while( !dir_legal() );
}

bool AnimationState::ShakerTask::dir_legal() const {
	if( m_shaker.m_flags & Shaker::NO_HORIZONTAL && ( m_dir == Direction::EAST || m_dir == Direction::WEST ) )
		return false;
	if( m_shaker.m_flags & Shaker::NO_VERTICAL && ( m_dir == Direction::NORTH || m_dir == Direction::SOUTH ) )
		return false;

	return true;
}

void AnimationState::ShakerTask::update( SteelInterpreter* pInterpreter ) {
	float motion = 0.0f;
	if( m_shaker.m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		motion = ( double )m_shaker.m_functor->Call( pInterpreter, params );
	}
	// TODO: Call a method which calls a method on the battle state
	// that sets an offset to either a sprite, a group, or the screen.
	CL_Vec2<float> dir = m_dir.ToScreenVector() * motion;
	CL_Pointf dir_pt( dir.x, dir.y );
	switch( m_locale.GetType() ) {
		case Locale::SPRITE:
			m_state.SetSpriteOffset( m_locale.GetSprite(), m_state.GetSpriteOffset( m_locale.GetSprite() ) + dir_pt );
			break;
		case Locale::CHARACTER:
			m_state.SetSpriteOffset( m_state.GetSpriteForChar( m_locale.GetChar() ),
																												m_state.GetSpriteOffset( m_state.GetSpriteForChar( m_locale.GetChar() ) ) + dir_pt );
			break;
		case Locale::GROUP:
			m_state.SetGroupOffset( m_locale.GetGroup(), m_state.GetGroupOffset( m_locale.GetGroup() ) + dir_pt );
			break;
	}

	if(m_osc){
		pick_opposite_dir();
	}else{
		pick_rand_dir();
	}
	m_osc = !m_osc;
}

void AnimationState::ShakerTask::pick_opposite_dir() {
	m_dir = m_dir.opposite();
}


void AnimationState::ShakerTask::finish( SteelInterpreter* pInterpreter ) {
	TimedTask::finish( pInterpreter );
	// TODO: Return battle state to zero offsets
}

void AnimationState::ShakerTask::cleanup(){
	switch( m_locale.GetType() ) {
		case Locale::SPRITE:
			m_state.SetSpriteOffset( m_locale.GetSprite(), CL_Pointf(0.0f,0.0f) );
			break;
		case Locale::CHARACTER:
			m_state.SetSpriteOffset( m_state.GetSpriteForChar(m_locale.GetChar()),CL_Pointf(0.0f,0.0f) );
			break;
		case Locale::GROUP:
			m_state.SetGroupOffset( m_locale.GetGroup(), CL_Pointf(0.0f,0.0f) );
			break;
	}	
}

void AnimationState::FadeTask::SetDuration( float duration ) {
	m_duration = duration;
}

void AnimationState::FadeTask::start( SteelInterpreter* pInterpreter ) {
	TimedTask::start( pInterpreter );
}

void AnimationState::FadeTask::update( SteelInterpreter* pInterpreter ) {
	float alpha = 1.0f - percentage();
	if( m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		alpha = ( double )m_functor->Call( pInterpreter, params );
	}
	m_state.GetSprite( m_sprite ).set_alpha( alpha );
}

bool AnimationState::FadeTask::finished() {
	return percentage() > 1.0f;
}

void AnimationState::FadeTask::cleanup() {
	m_state.GetSprite( m_sprite ).set_alpha( 1.0f );
}

void AnimationState::ColorizeTask::init( const Colorizer& colorizer ) {
	m_colorizer = colorizer;
}

void AnimationState::ColorizeTask::SetDuration( float duration ) {
	m_duration = duration;
}

void AnimationState::ColorizeTask::start( SteelInterpreter* ) {
}

void AnimationState::ColorizeTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float r = ( double )m_colorizer.m_red->Call( pInterpreter, params );
	float g = ( double )m_colorizer.m_green->Call( pInterpreter, params );
	float b = ( double )m_colorizer.m_blue->Call( pInterpreter, params );

	m_state.GetSprite( m_sprite ).set_color( CL_Colorf( r, g, b,
																																																					m_state.GetSprite( m_sprite ).get_alpha()
																																																			) );
}

void AnimationState::ColorizeTask::cleanup() {
	m_state.GetSprite( m_sprite ).set_color( CL_Colorf( 1.0f, 1.0f, 1.0f ) );
}

void AnimationState::DarkenTask::init( int mode, float duration, SteelType::Functor r,
																																							SteelType::Functor g, SteelType::Functor b, SteelType::Functor a ) {
	m_mode = mode;
	m_duration = duration;
	m_red = r;
	m_green = g;
	m_blue = b;
	m_alpha = a;
}

void AnimationState::DarkenTask::start( SteelInterpreter* ) {
}

void AnimationState::DarkenTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float r = ( double )m_red->Call( pInterpreter, params );
	float g = ( double )m_green->Call( pInterpreter, params );
	float b = ( double )m_blue->Call( pInterpreter, params );
	float a = ( double )m_alpha->Call( pInterpreter, params );

	m_state.Darken( m_mode, r, g, b, a );
}

void AnimationState::DarkenTask::cleanup() {
	m_state.ClearDark( m_mode );
}


void AnimationState::FunctionTask::init( double duration, SteelType::Functor f ) {
	SetDuration( duration );
	m_functor = f;
}

void AnimationState::FunctionTask::start( SteelInterpreter* pInterpreter ) {
	StoneRing::AnimationState::Task::start( pInterpreter );
}

void AnimationState::FunctionTask::update( SteelInterpreter* ) {

}

void AnimationState::FunctionTask::cleanup() {
	StoneRing::AnimationState::Task::cleanup();
}

void AnimationState::FunctionTask::finish( SteelInterpreter* pInterpreter ) {
	StoneRing::AnimationState::Task::finish( pInterpreter );
	m_functor->Call( pInterpreter, SteelType::Container() );
}



