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
	m_task_mutex.lock();
	add_task(task);
	m_task_mutex.unlock();
}


BattleState::SpriteTicket AnimationState::AddSprite( clan::Sprite sprite ) {
	return m_parent.add_sprite( sprite );
}


clan::Rectf AnimationState::GetCharacterRect( ICharacter* ichar ) const {
	return m_parent.get_character_rect( ichar );
}
clan::Rectf AnimationState::GetGroupRect( ICharacterGroup* igroup ) const {
	return m_parent.get_group_rect( igroup );
}
clan::Rectf AnimationState::GetSpriteRect( BattleState::SpriteTicket sprite ) const {
	return m_parent.get_sprite_rect( sprite );
}
void AnimationState::SetSpritePos( BattleState::SpriteTicket sprite, const clan::Pointf& pt ) {
	m_parent.set_sprite_pos( sprite, pt );
}



clan::Pointf AnimationState::GetFocusOrigin( const SpriteMovement::Focus& focus, ICharacter * pTarget ) {
	clan::Pointf point( 0, 0 );
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
			clan::Rectf rect =  m_parent.get_character_rect( m_pCaster );
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
			clan::Rectf rect = m_parent.get_character_rect( m_pTarget );
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
			clan::Rectf rect = m_parent.get_group_rect( m_pCasterGroup );
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
			clan::Rectf rect = m_parent.get_group_rect( m_pTargetGroup );
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
			clan::Rectf rect =  m_parent.get_character_locus_rect( m_pCaster );
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
			clan::Rectf rect =  m_parent.get_character_locus_rect( m_pTarget );
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
	bool empty = false;
	m_task_mutex.lock();
	while( !m_tasks.empty() ) {
		start( m_tasks.front() );
		waitFor( m_tasks.front() );
		if(!m_tasks.empty())
			m_tasks.erase( m_tasks.begin() );
	}
	m_task_mutex.unlock();	
}



void AnimationState::HandleKeyDown( const clan::InputEvent &key ) {
}

void AnimationState::HandleKeyUp( const clan::InputEvent &key ) {
}

void AnimationState::move_sprite( ICharacter* pActor, ICharacter* pTarget, SpriteAnimation* anim, SpriteMovement* movement, float percentage ) {
	if( percentage > 1.0f ) percentage = 1.0f;
	if( percentage == 0.0f ) percentage = 0.000001f;
	clan::Pointf origin = GetFocusOrigin( movement->GetInitialFocus(), pActor );
// std::cout << "Origin is " << origin_i.x << ',' << origin_i.y << std::endl;

	clan::Pointf current = origin;

	clan::Sprite sprite;

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


	clan::Angle angle = clan::Angle::from_degrees( degrees );
	clan::Angle yaw_angle = clan::Angle::from_radians( yaw );
	clan::Angle pitch_angle = clan::Angle::from_radians( pitch );
	// TODO: Mechanism to affect ALL this character's sprites in case it changes
	sprite.set_angle( angle );
	//sprite.set_angle_yaw( yaw_angle );
	//sprite.set_angle_pitch( pitch_angle );
	sprite.set_alpha( alpha );
	sprite.set_scale( scale, scale );

	float completion = movement->Completion();
	percentage *= completion;

	if( movement->HasEndFocus() ) {
		clan::Pointf dest = GetFocusOrigin( movement->GetEndFocus(), pActor );
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
				float angle = ( clan::PI / 180.0f ) * angle_deg;
				clan::Pointf cpoint( cos( angle ), sin( angle ) );
				current = dest +  cpoint * radius; // C + R * (cos A, sin A)
				break;
			}
			case SpriteMovement::SINE: {
				clan::Pointf d, c;
				d.x = dest.x - origin.x;
				d.y = dest.y - origin.y;
				float l = sqrt( ( double )d.x * d.x + d.y + d.y );
				d.x /=  l;
				d.y /= l;
				float w = 2.0f * clan::PI * movement->Periods(); // make this 1/2 to arc up

				current.x = percentage * dest.x + ( 1.0f - percentage ) * origin.x + amplitude * d.y * sin( w * percentage );
				current.y = percentage * dest.y + ( 1.0f - percentage ) * origin.y - amplitude * d.x * sin( w * percentage );
				break;
			}
			case SpriteMovement::ARC_OVER: {
				clan::Pointf d, c;
				d.x = dest.x - origin.x;
				d.y = dest.y - origin.y;
				float l = sqrt( ( double )d.x * d.x + d.y + d.y );
				d.x /=  l;
				d.y /= l;
				float w = 2.0f * clan::PI * 0.5f; // make this 1/2 to arc up

				current.x = percentage * dest.x + ( 1.0f - percentage ) * origin.x + amplitude * d.y * sin( w * percentage );
				current.y = percentage * dest.y + ( 1.0f - percentage ) * origin.y + amplitude * d.x * sin( w * percentage );
				break;
			}
			case SpriteMovement::ARC_UNDER: {
				clan::Pointf d, c;
				d.x = dest.x - origin.x;
				d.y = dest.y - origin.y;
				float l = sqrt( ( double )d.x * d.x + d.y + d.y );
				d.x /=  l;
				d.y /= l;
				float w = 2.0f * clan::PI * 0.5f; // make this 1/2 to arc up

				current.x = percentage * dest.x + ( 1.0f - percentage ) * origin.x + amplitude * d.y * sin( w * percentage );
				current.y = percentage * dest.y + ( 1.0f - percentage ) * origin.y - amplitude * d.x * sin( w * percentage );
				break;
			}
			default:
				break;
		}
	} else {
		// move a set distance in direction
		clan::Pointf direction;
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
		clan::Pointf point;
		point.x = current.x;
		point.y = current.y;
		switch( anim->GetBattleSprite()->GetWho() ) {
			case WHO_CASTER:
				m_parent.set_sprite_pos(m_parent.get_sprite_for_char(m_pCaster), point);
				break;
			case WHO_TARGET:
				m_parent.set_sprite_pos(m_parent.get_sprite_for_char(m_pTarget), point);				
				break;
		}
	}


}
/*
void AnimationState::move_character(ICharacter* character, SpriteAnimation* anim, SpriteMovement* movement, float percentage)
{
    if(percentage > 1.0f) percentage = 1.0f;
    clan::Point origin_i = GetFocusOrigin(movement->GetInitialFocus(), character);
// std::cout << "Origin is " << origin_i.x << ',' << origin_i.y << std::endl;
    clan::Pointf origin(origin_i.x,origin_i.y);
    clan::Point dest_i;
    clan::Pointf current = origin;

    // Rotation
    // TODO: Switch to away/toward
    double degrees = percentage * movement->Rotation();
    if((!character->IsMonster() && m_parent.MonstersOnLeft()) ||
        character->IsMonster() && !m_parent.MonstersOnLeft())
        degrees = 0 - degrees;


    clan::Angle angle = clan::Angle::from_degrees(degrees);
    // TODO: Mechanism to affect ALL this character's sprites in case it changes
    clan::Sprite sprite = character->GetCurrentSprite();
    sprite.set_angle(angle);

    float completion = movement->Completion();
    percentage *= completion;


}
*/

void AnimationState::Draw( const clan::Rect& screenRect, clan::Canvas& GC ) {
	if( m_functor_mode ) {
		draw_functor( screenRect, GC );
	} else {
		draw( screenRect, GC );
	}
}


void AnimationState::draw( const clan::Rect &screenRect, clan::Canvas& GC ) {
	if( !m_bDone ) {
		uint64_t passed = clan::System::get_time() - m_phase_start_time;
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
	clan::System::sleep( 10 );
}

void AnimationState::draw_functor( const clan::Rect& screenRect, clan::Canvas& GC ) {
	bool notasks = false;
	bool emergency = false;
	m_task_mutex.lock();
	for( auto it = std::begin(m_tasks); it != std::end(m_tasks); it++ ){
		Task * pTask = *it;
		try{
			pTask->update( m_pInterpreter );
		}catch(Steel::SteelException ex){
			emergency = true;
			std::cerr << "Quitting task " << pTask->GetName() <<  " after SteelException: " << ex.getScript() << ':' << ex.getMessage() << std::endl;
		}catch(Steel::ParamMismatch pm){
			emergency = true;
			std::cerr << "Param mismatch within animation task update" << std::endl;
		}
		if( pTask->finished() || emergency ) {

			pTask->finish( m_pInterpreter );
			// Do something with waitFor?
			std::cout << "Finished task: " << pTask->GetName() << '@' << std::hex << pTask << std::endl;
			notasks = m_tasks.empty();
			std::cout << "Tasks left = " << m_tasks.size() << std::endl;
			//m_wait_event.set();
		}
	}
	m_task_mutex.unlock();
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
	clan::Sprite sprite;
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
	clan::Colorf color = sprite.get_color();
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
			//sprite.set_color(color * clan::Colorf(0.5f,0.5f,0.5f));
			break;
		case AlterSprite::X_FLIP:
			// TODO:
		case AlterSprite::Y_FLIP:
			// TODO:
			break;
		case AlterSprite::GRAYSCALE:
			sprite.set_color( clan::Colorf( 0.7f, 0.7f, 0.7f ) );
			break;
		case AlterSprite::GREENSCALE:
			sprite.set_color( clan::Colorf( 0.0f, 1.0f, 0.0f ) );
			break;
		case AlterSprite::REDSCALE:
			sprite.set_color( clan::Colorf( 1.0f, 0.0f, 0.0f ) );
			break;
		case AlterSprite::BLUESCALE:
			sprite.set_color( clan::Colorf( 0.0f, 0.0f, 1.0f ) );
			break;
		case AlterSprite::RESET:
			sprite.set_color( clan::Colorf( 1.0f, 1.0f, 1.0f ) );
			sprite.set_scale( 1.0f, 1.0f );
			alpha = 1.0f;
			break;
	}

	sprite.set_alpha( alpha );
}

void AnimationState::StartPhase() {
	Phase * phase = *m_phase_iterator;
	phase->Execute();
	m_phase_start_time = clan::System::get_time();

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
						animation->SetSpriteTicket( m_parent.add_sprite( pWeapon->GetSprite() ) );
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
		pInterpreter->addFunction( "getCharacterLocus", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::getCharacterLocus ) );



		pInterpreter->addFunction( "setPathSpeedFunction", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, SteelType::Functor>( this, &AnimationState::setPathSpeedFunction ) );
		pInterpreter->addFunction( "setPathCompletion", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, double>( this, &AnimationState::setPathCompletion ) );
		pInterpreter->addFunction( "setPathFlags", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Handle, int>( this, &AnimationState::setPathFlags ) );

		pInterpreter->addFunction( "moveSprite", "anim", new SteelFunctor2Arg<AnimationState, int, SteelType::Handle>( this, &AnimationState::moveSprite ) );
		pInterpreter->addFunction( "createRotation", "anim", new SteelFunctor4Arg<AnimationState, SteelType::Functor, double,double, int>( this, &AnimationState::createRotation ) );
		pInterpreter->addFunction( "createStretch", "anim", new SteelFunctor2Arg<AnimationState, SteelType::Functor, SteelType::Functor>( this, &AnimationState::createStretch ) );
		pInterpreter->addFunction( "stretchSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::stretchSpriteTimed ) );

		pInterpreter->addFunction( "rotateSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::rotateSpriteTimed ) );
		pInterpreter->addFunction( "movePathTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::moveSpriteTimed ) );

		pInterpreter->addFunction( "moveSpriteTimed", "anim", new SteelFunctor3Arg<AnimationState, int, SteelType::Handle, double>( this, &AnimationState::moveSpriteTimed ) );
/*
 * 		SteelType setSpriteRotation(int sprite, double radians);
		SteelType flipSprite(int sprite, bool flip_x, bool flip_y);
		SteelType scaleSprite(int sprite, double scale_x, double scale_y);
		SteelType setSpriteLocation(int sprite, SteelType::Handle hLocale);
		SteelType closestCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTarget);
		SteelType nearCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner);
		SteelType farCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner); 		
		*/
		pInterpreter->addFunction( "setSpriteRotation", "anim", new SteelFunctor2Arg<AnimationState,int,double>(this, &AnimationState::setSpriteRotation) );
		pInterpreter->addFunction( "flipSprite", "anim", new SteelFunctor3Arg<AnimationState,int,bool,bool>(this,&AnimationState::flipSprite) );
		pInterpreter->addFunction( "scaleSprite","anim", new SteelFunctor3Arg<AnimationState,int,double,double>(this,&AnimationState::scaleSprite) );
		pInterpreter->addFunction( "setSpriteLocation", "anim", new SteelFunctor2Arg<AnimationState,int,SteelType::Handle>(this,&AnimationState::setSpriteLocation) );
		pInterpreter->addFunction( "closestCorner", "anim", new SteelFunctor2Arg<AnimationState,SteelType::Handle,SteelType::Handle>(this,&AnimationState::closestCorner) );
		pInterpreter->addFunction( "nearCorner", "anim", new SteelFunctor3Arg<AnimationState,SteelType::Handle, SteelType::Handle,int>(this,&AnimationState::nearCorner) );
		pInterpreter->addFunction( "farCorner", "anim", new SteelFunctor3Arg<AnimationState,SteelType::Handle, SteelType::Handle,int>(this,&AnimationState::farCorner) );
		
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

		pInterpreter->addFunction( "createOrbit", "anim", new SteelFunctor5Arg<AnimationState, SteelType::Functor, SteelType::Functor,double, double, bool>(this, &AnimationState::createOrbit) );
		pInterpreter->addFunction( "orbitSprite", "anim", new SteelFunctor3Arg<AnimationState,int,SteelType::Handle,SteelType::Handle>(this, &AnimationState::orbitSprite) );
		pInterpreter->addFunction( "orbitSpriteTimed", "anim", new SteelFunctor4Arg<AnimationState,int,SteelType::Handle,SteelType::Handle,double>(this, &AnimationState::orbitSpriteTimed) );
		
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
		pInterpreter->popScope();
		delete m_pRunner;
		m_pRunner = nullptr;
	}
}

clan::Pointf AnimationState::GetGroupOffset( ICharacterGroup* igroup ) const {
	return m_parent.get_offset( igroup );
}

BattleState::SpriteTicket AnimationState::GetSpriteForChar( ICharacter* iChar ) {
	return m_parent.get_sprite_for_char( iChar );
}

clan::Pointf AnimationState::GetSpriteOffset( BattleState::SpriteTicket sprite ) const {
	return m_parent.get_offset( sprite );
}

void AnimationState::SetGroupOffset( ICharacterGroup* igroup, const clan::Pointf& pt ) {
	m_parent.set_offset( igroup, pt );
}

void AnimationState::SetSpriteOffset( BattleState::SpriteTicket sprite, const clan::Pointf& pt ) {
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
		clan::Sprite       m_sprite;
		AnimationState& m_state;
	} functor( sprite_ref, *this );
	clan::Event event;
	IApplication::GetInstance()->RunOnMainThread( event, &functor );
	event.wait();
	SteelType var;
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
	Character * pCharacter = Steel::GrabHandle<Character*>(iCharacter);
	assert(pCharacter);
	Equipment* equipment = pCharacter->GetEquipment( static_cast<Equipment::eSlot>(hand) );
	Weapon* pWeapon = dynamic_cast<Weapon*>( equipment );
	if( pWeapon ) { // TODO: Cache this and don't add it twice.
		int sprite = m_parent.add_sprite( pWeapon->GetSprite() );
		var.set( sprite );	
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

SteelType AnimationState::getCharacterLocus( SteelType::Handle iCharacter, int corner ) {
	Locale* locale = new Locale( Locale::CHARACTER_LOCUS, ( Locale::Corner )corner );
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
	locale->SetOffset( clan::Point( x, y ) );
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

SteelType AnimationState::createRotation( SteelType::Functor functor, double degrees, double start_deg, int axis ) {
	Rotation* rotation = new Rotation();
	rotation->m_axis = ( Rotation::Axis )axis;
	rotation->m_degrees = degrees;
	rotation->m_start_degrees = start_deg;
	rotation->m_functor = functor;
	m_handles.push_back( rotation );
	SteelType var;
	var.set( rotation );
	return var;
}

SteelType AnimationState::createStretch( SteelType::Functor width_f, SteelType::Functor height_f) {
	Stretch* stretch = new Stretch();
	stretch->m_width_functor = width_f;
	stretch->m_height_functor = height_f;

	m_handles.push_back( stretch );
	SteelType var;
	var.set( stretch );
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

SteelType AnimationState::closestCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale){
	Locale* reference = Steel::GrabHandle<Locale*>(iReferenceLocale);
	Locale* target = Steel::GrabHandle<Locale*>(iTargetLocale);
	clan::Pointf refPt = GetPosition(*reference);
	clan::Pointf targetPt = GetPosition(*target);
	SteelType ret;
		
	
	// This isn't a true "closest" as it doesn't consider the middle points
	bool targetOnLeft = false;
	bool targetAbove = false;
	
	if(targetPt.x < refPt.x)
		targetOnLeft = true;
	
	if(targetPt.y < refPt.y)
		targetAbove = true;
	
	if(targetOnLeft && targetAbove)
		ret.set(Locale::BOTTOM_LEFT);
	else if(targetOnLeft && !targetAbove)
		ret.set(Locale::TOP_LEFT);
	else if(targetAbove)
		ret.set(Locale::BOTTOM_RIGHT);
	else{
		ret.set(Locale::TOP_RIGHT);
	}
	
	return ret;
}

SteelType AnimationState::nearCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner){
	Locale* reference = Steel::GrabHandle<Locale*>(iReferenceLocale);
	Locale* target = Steel::GrabHandle<Locale*>(iTargetLocale);
	clan::Pointf refPt = GetPosition(*reference);
	clan::Pointf targetPt = GetPosition(*target);
	SteelType ret;
	
	
	bool targetOnLeft = false;
	
	if(targetPt.x < refPt.x)
		targetOnLeft = true;
	
	switch(corner){
		case Locale::BOTTOM_LEFT:
			ret.set(Locale::BOTTOM_RIGHT);
			break;
		case Locale::MIDDLE_LEFT:
			ret.set(Locale::MIDDLE_RIGHT);
			break;
		case Locale::TOP_LEFT:
			ret.set(Locale::TOP_RIGHT);
			break;
	}
	
	return ret;
}

SteelType AnimationState::farCorner(SteelType::Handle iReferenceLocale, SteelType::Handle iTargetLocale, int corner){
	Locale* reference = Steel::GrabHandle<Locale*>(iReferenceLocale);
	Locale* target = Steel::GrabHandle<Locale*>(iTargetLocale);
	clan::Pointf refPt = GetPosition(*reference);
	clan::Pointf targetPt = GetPosition(*target);
	SteelType ret;
	
	
	bool targetOnLeft = false;
	
	if(targetPt.x < refPt.x)
		targetOnLeft = true;
	
	switch(corner){
		case Locale::BOTTOM_RIGHT:
			ret.set(Locale::BOTTOM_LEFT);
			break;
		case Locale::MIDDLE_RIGHT:
			ret.set(Locale::MIDDLE_LEFT);
			break;
		case Locale::TOP_RIGHT:
			ret.set(Locale::TOP_LEFT);
			break;
	}
	
	return ret;	
}// Returns the corner on the opposite
	


SteelType AnimationState::setSpriteRotation(int sprite, double radians){
	GetSprite(sprite).set_angle(clan::Angle::from_radians(radians));
	return SteelType();
}
SteelType AnimationState::flipSprite(int sprite,bool flip_x, bool flip_y){
	float cur_x, cur_y;
	GetSprite(sprite).get_scale(cur_x,cur_y);
	
	if(flip_x)
		cur_x = -cur_x;
	if(flip_y)
		cur_y = -cur_y;
	
	GetSprite(sprite).set_scale(cur_x,cur_y);
	return SteelType();
}
SteelType AnimationState::scaleSprite(int sprite, double x, double y){
	GetSprite(sprite).set_scale(x,y);
	return SteelType();
}
SteelType AnimationState::setSpriteLocation(int sprite, SteelType::Handle hLocale){
	Locale * pLoc = Steel::GrabHandle<Locale*>( hLocale );
	m_parent.set_sprite_pos( sprite, GetPosition(*pLoc) );
	return SteelType();
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

SteelType AnimationState::createOrbit(SteelType::Functor radius_functor, SteelType::Functor speed_functor, double start_degrees, double deg, bool clockwise){
	Orbit * orb = new Orbit();
	orb->m_radius_functor = radius_functor;
	orb->m_speed_functor = speed_functor;
	orb->m_degrees = deg;
	orb->m_start_angle = start_degrees;
	orb->m_clockwise = clockwise;
	SteelType var;
	var.set(orb);
	m_handles.push_back(orb);
	return var;
}
SteelType AnimationState::orbitSprite(int sprite,SteelType::Handle hOrbit, SteelType::Handle hLocale){
	Orbit * orb = Steel::GrabHandle<Orbit*>( hOrbit );
	Locale * locale = Steel::GrabHandle<Locale*> ( hLocale );
	OrbitTask * task = new OrbitTask ( *this );
	task->init(*orb, *locale);
	task->SetSprite(sprite);
	SteelType var;
	var.set ( task );
	m_handles.push_back ( task );
	return var;
}
SteelType AnimationState::orbitSpriteTimed(int sprite, SteelType::Handle hOrbit, SteelType::Handle hLocale, double seconds){
	Orbit * orb = Steel::GrabHandle<Orbit*>( hOrbit );
	Locale * locale = Steel::GrabHandle<Locale*> ( hLocale );
	orb->m_duration = seconds;
	TimedOrbitTask * task = new TimedOrbitTask( *this );
	task->init(*orb, *locale);
	task->SetSprite( sprite );
	SteelType var;
	var.set ( task );
	m_handles.push_back(task);
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

SteelType AnimationState::stretchSpriteTimed( int sprite, SteelType::Handle hStretch, double seconds ) {
	Stretch * stretch  = Steel::GrabHandle<Stretch*>( hStretch );
	stretch->m_duration = seconds;
	TimedStretchTask * task = new TimedStretchTask( *this );
	std::cout << "TimedStretchTask created: " << std::hex << task << std::endl;
	task->init( *stretch );
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
	clan::System::sleep( seconds * 1000.0 );
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
		AddTask( pTask );
	}
	while( !pTask->expired() ) {
	}
	//std::cout << "Done waiting for: " << pTask->GetName() << std::endl;
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
	}
	SteelType var;
	var.set( task );
	return var;
}

void AnimationState::add_task( AnimationState::Task* task ) {
	m_task_mutex.lock();
	if(!task->expired() && !task->started()){
		std::cout << "Adding task: " << task->GetName() << '@' << std::hex << task <<" to " << std::dec << m_tasks.size() << " existing tasks" << std::endl;
		m_tasks.push_back(task);
		task->start( m_pInterpreter );
	}
	m_task_mutex.unlock();
}


SteelType AnimationState::startAll( const Steel::SteelArray& alltasks ) {
	m_task_mutex.lock();
	for( int i = 0; i < alltasks.size(); i++ ) {
		Task * task = Steel::GrabHandle<Task*>( alltasks[i] );
		if( !task->started() ) {
			add_task(task);
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
	for( std::list<SteelType::IHandle*>::const_iterator it = m_handles.begin(); it != m_handles.end(); it++ ) {
		std::cout << std::hex << ( long )*it << std::endl;
		delete *it;
	}
	m_handles.clear();
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

void AnimationState::Locale::SetOffset( const clan::Point& offset ) {
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

clan::Sprite AnimationState::GetSprite( BattleState::SpriteTicket sprite ) {
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


clan::Pointf AnimationState::Task::get_position( const AnimationState::Locale& locale) const {
		return m_state.GetPosition(locale);
}
clan::Pointf AnimationState::Task::get_mid_point( const clan::Pointf& start, const clan::Pointf& end, float p ) {
	return start * ( 1.0f - p ) + end * p;
}

// M = S * (1-p) + E * p
// Solve for p
// M / E

clan::Pointf AnimationState::GetPosition( const AnimationState::Locale& locale ) const {
	clan::Rectf rect;
	switch( locale.GetType() ) {
		case Locale::CHARACTER:
			rect = GetCharacterRect( locale.GetChar() );
			break;
		case Locale::CHARACTER_LOCUS:
			rect = m_parent.get_character_locus_rect( locale.GetChar() );
			break;
		case Locale::SPRITE:
			rect = GetSpriteRect( locale.GetSprite() );
			break;
		case Locale::SCREEN:
			rect = IApplication::GetInstance()->GetDisplayRect();
			break;
		case Locale::GROUP:
			rect = GetGroupRect( locale.GetGroup() );
			break;
	}

	clan::Pointf point;

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
	m_start_time = clan::System::get_time();
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
	clan::Pointf start = get_position( m_path->m_start );
	clan::Pointf true_end = get_position( m_path->m_end );

	//clan::Pointf end = (start + true_end) * m_completion;
	clan::Pointf end = get_mid_point( start, true_end, m_path->m_completion );


	if( m_path->m_flags & Path::NO_HORIZONTAL ) {
		end.x = start.x;
	}

	if( m_path->m_flags & Path::NO_VERTICAL ) {
		end.y = start.y;
	}

	float p = percentage();
	float elapsed = float( clan::System::get_time() - m_start_time );
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
	clan::Pointf adj_pos = m_cur_pos;
	adj_pos += clan::Pointf( -( end.y - start.y ), ( end.x - start.x ) ).normalize() * float(dis);
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
	clan::Pointf start = get_position( m_path->m_start );
	clan::Pointf end = get_position( m_path->m_end );
	if( m_path->m_flags & Path::NO_HORIZONTAL ) {
		end.x = start.x;
	}

	if( m_path->m_flags & Path::NO_VERTICAL ) {
		end.y = start.y;
	}

	clan::Pointf pos = get_mid_point( start, end, percentage() );
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

void AnimationState::TimedStretchTask::init( const Stretch& stretch ) {
	m_stretch = stretch;
}

void AnimationState::TimedStretchTask::start( SteelInterpreter* ) {
	m_start_time = clan::System::get_time();
}

void AnimationState::TimedStretchTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float width = ( double )m_stretch.m_width_functor->Call( pInterpreter, params );
	float height = ( double )m_stretch.m_height_functor->Call ( pInterpreter, params );
	clan::Sprite sprite = m_state.GetSprite( m_sprite );
	sprite.set_scale(width,height);
}



void AnimationState::TimedStretchTask::cleanup() {
	m_state.GetSprite( m_sprite ).set_scale(1.0,1.0);
}

void AnimationState::RotationTask::init( const Rotation& rot ) {
	m_rotation = rot;
	m_completion_degrees = m_rotation.m_degrees;
}

void AnimationState::RotationTask::start( SteelInterpreter* ) {
	m_degrees = m_rotation.m_start_degrees;
	m_last_time = clan::System::get_time();
}

void AnimationState::RotationTask::update( SteelInterpreter* pInterpreter ) {
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	float speed = ( double )m_rotation.m_functor->Call( pInterpreter, params );
	float delta = speed * float( clan::System::get_time() - m_last_time );
	m_degrees += fabs( delta );
	if( m_degrees > m_rotation.m_degrees )
		delta -=  m_degrees - m_rotation.m_degrees; // lessen the delta by how much we overshot
	clan::Sprite sprite = m_state.GetSprite( m_sprite );
	switch( m_rotation.m_axis ) {
		case Rotation::PITCH:
			//sprite.rotate_pitch( clan::Angle::from_degrees( delta ) );
			break;
		case Rotation::YAW:
			//sprite.rotate_yaw( clan::Angle::from_degrees( delta ) );
			break;
		case Rotation::ROLL:
			sprite.rotate( clan::Angle::from_degrees( delta ) );
			break;
	}
	m_last_time = clan::System::get_time();
}

bool AnimationState::RotationTask::finished() {
	return percentage() >= 1.0f;
}


float AnimationState::RotationTask::_percentage() const {
	return  m_degrees / m_rotation.m_degrees;
}

void AnimationState::RotationTask::cleanup() {

	switch( m_rotation.m_axis ) {
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( clan::Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::YAW:
			//m_state.GetSprite( m_sprite ).set_angle_yaw( clan::Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::PITCH:
			//m_state.GetSprite( m_sprite ).set_angle_pitch( clan::Angle::from_degrees( 0.0f ) );
			break;
	}
}


void AnimationState::TimedRotationTask::init( const Rotation& rot ) {
	m_rotation = rot;
}

void AnimationState::TimedRotationTask::start( SteelInterpreter* pInterpreter ) {
	TimedTask::start( pInterpreter );
	m_degrees = 0;
}
void AnimationState::TimedRotationTask::update( SteelInterpreter* pInterpreter ) {
	float speed = 0.1f;
	if( m_rotation.m_functor ) {
		SteelType::Container params;
		SteelType p;
		p.set( percentage() ); // or _percentage?
		params.push_back( p );
		speed = ( double )m_rotation.m_functor->Call( pInterpreter, params );
	}
	
	float delta = speed * float( clan::System::get_time() - m_last_time );
	m_degrees += abs( delta );
	if( m_degrees > m_rotation.m_degrees )
		delta -=  m_degrees - m_rotation.m_degrees; // lessen the delta by how much we overshot
	
#ifndef NDEBUG
	//std::cout << "Angle is " << angle << std::endl;
#endif
	switch( m_rotation.m_axis ) {
		case Rotation::YAW:
			//m_state.GetSprite( m_sprite ).set_angle_yaw( clan::Angle::from_degrees( angle ) );
			break;
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( clan::Angle::from_degrees( m_degrees ) );
			break;
		case Rotation::PITCH:
			//m_state.GetSprite( m_sprite ).set_angle_pitch( clan::Angle::from_degrees( angle ) );
			break;
	}
}
bool AnimationState::TimedRotationTask::finished() {
	return TimedTask::finished();
}

void AnimationState::TimedRotationTask::cleanup() {
	switch( m_rotation.m_axis ) {
		case Rotation::ROLL:
			m_state.GetSprite( m_sprite ).set_angle( clan::Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::YAW:
			//m_state.GetSprite( m_sprite ).set_angle_yaw( clan::Angle::from_degrees( 0.0f ) );
			break;
		case Rotation::PITCH:
			//m_state.GetSprite( m_sprite ).set_angle_pitch( clan::Angle::from_degrees( 0.0f ) );
			break;
	}
}


void AnimationState::OrbitTask::init(const Orbit & orbit, const Locale& locale){
	m_orbit = orbit;
	m_origin = locale;
}
void AnimationState::OrbitTask::start(SteelInterpreter* pInterpreter){
	m_degrees = m_start_degrees;
	m_last_time = clan::System::get_time();	
}
void AnimationState::OrbitTask::update(SteelInterpreter* pInterpreter){
	const clan::Pointf origin = m_state.GetPosition(m_origin);
	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	assert(m_orbit.m_radius_functor);
	assert(m_orbit.m_speed_functor);
	float radius = ( double ) m_orbit.m_radius_functor->Call(pInterpreter, params );
	float speed = ( double )m_orbit.m_speed_functor->Call( pInterpreter, params );
	float delta = speed * float( clan::System::get_time() - m_last_time );
	m_degrees += fabs( delta );
	if( m_degrees > m_orbit.m_degrees )
		delta -=  m_degrees - m_orbit.m_degrees; // lessen the delta by how much we overshot	
	float angle = ( clan::PI / 180.0f ) * m_degrees;
	clan::Pointf cpoint( cos( angle ), sin( angle ) );
	clan::Pointf current = origin +  cpoint * radius; // C + R * (cos A, sin A)		
	m_state.SetSpritePos(m_sprite,current);
	m_last_time =  clan::System::get_time();
}
float AnimationState::OrbitTask::_percentage() const {
	return m_degrees / m_orbit.m_degrees;
}
bool AnimationState::OrbitTask::finished(){
	return (m_degrees >= m_orbit.m_degrees);
}
void AnimationState::OrbitTask::cleanup(){
}
void AnimationState::TimedOrbitTask::init( const AnimationState::Orbit& rot, const AnimationState::Locale& origin ) {
	m_orbit = rot;
	m_locale = origin;
}

void AnimationState::TimedOrbitTask::start( SteelInterpreter* pInterpreter) {
	StoneRing::AnimationState::TimedTask::start(pInterpreter);
	m_degrees = m_orbit.m_start_angle;
}

void AnimationState::TimedOrbitTask::update( SteelInterpreter* pInterpreter ) {
	float speed = 0.1f;
	const clan::Pointf origin = m_state.GetPosition(m_locale);	

	SteelType::Container params;
	SteelType p;
	p.set( percentage() ); // or _percentage?
	params.push_back( p );
	assert(m_orbit.m_speed_functor);
	assert(m_orbit.m_radius_functor);
	speed = ( double )m_orbit.m_speed_functor->Call( pInterpreter, params );
	
	float radius = (double)m_orbit.m_radius_functor->Call( pInterpreter, params );
	
	float delta = speed * float( clan::System::get_time() - m_last_time );
	m_degrees += abs( delta );

	float angle = ( clan::PI / 180.0f ) * m_degrees;
	clan::Pointf cpoint( cos( angle ), sin( angle ) );
	clan::Pointf current = origin +  cpoint * radius; // C + R * (cos A, sin A)		
	m_state.SetSpritePos(m_sprite,current);		
}

void AnimationState::TimedOrbitTask::cleanup() {
	StoneRing::AnimationState::Task::cleanup();
}

bool AnimationState::TimedOrbitTask::finished() {
	return percentage() >= 1.0;
}

void AnimationState::TimedStretchTask::SetDuration( float duration ) {
	m_duration = duration;
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
	clan::Vec2<float> dir = clan::Vec2<float>(m_dir.ToScreenVector()) * motion;
	clan::Pointf dir_pt( dir.x, dir.y );
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
			m_state.SetSpriteOffset( m_locale.GetSprite(), clan::Pointf(0.0f,0.0f) );
			break;
		case Locale::CHARACTER:
			m_state.SetSpriteOffset( m_state.GetSpriteForChar(m_locale.GetChar()),clan::Pointf(0.0f,0.0f) );
			break;
		case Locale::GROUP:
			m_state.SetGroupOffset( m_locale.GetGroup(), clan::Pointf(0.0f,0.0f) );
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

	m_state.GetSprite( m_sprite ).set_color( clan::Colorf( r, g, b,
																																																					m_state.GetSprite( m_sprite ).get_alpha()
																																																			) );
}

void AnimationState::ColorizeTask::cleanup() {
	m_state.GetSprite( m_sprite ).set_color( clan::Colorf( 1.0f, 1.0f, 1.0f ) );
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



