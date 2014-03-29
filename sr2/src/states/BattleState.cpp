#include "BattleState.h"
#include "MonsterRef.h"
#include "CharacterManager.h"
#include "GraphicsManager.h"
#include "IApplication.h"
#include "MonsterGroup.h"
#include "Monster.h"
#include "Party.h"
#include "BattleMenu.h"
#include "AnimationState.h"
#include "BattleConfig.h"
#include "MenuBox.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include "SoundManager.h"


#define ENABLE_ANIMATIONS 1

using namespace StoneRing;
using namespace Steel;

//
namespace StoneRing {
#if 0 	
	class SpiteSort: public std::binary_function<BattleState::Sprite, BattleState::Sprite, bool> {
	public:
		SpriteSort(BattleState& state):m_state(state){
		}
		virtual ~SpriteSort(){}
		bool operator()(ICharacter* a, ICharacter* b) const {
			int a_value = a->GetBattlePos().y * m_state.m_monster_rect.get_width() + a->GetBattlePos().x;
			int b_value = b->GetBattlePos().y * m_state.m_monster_rect.get_width() + b->GetBattlePos().x;
			if(m_state.m_combat_state == BattleState::TARGETING && !m_state.m_targets.m_bSelectedGroup){
				if(m_state.m_targets.selected.m_pTarget == a){
					return false;
				}else if(m_state.m_targets.selected.m_pTarget == b){
					return true;
				}
			}else if(a == m_state.m_initiative[m_state.m_cur_char]){
				a_value = (a->GetBattlePos().y + 1) * m_state.m_monster_rect.get_width();
			}else if(b == m_state.m_initiative[m_state.m_cur_char]){
				b_value = (b->GetBattlePos().y + 1) * m_state.m_monster_rect.get_width();
			}
			return a_value < b_value;
		}
	private:
		BattleState& m_state;
	};
	
#endif
class StatusEffectPainter : public Visitor<StatusEffect*> {
public:
	StatusEffectPainter( clan::Canvas& gc ): m_i( 0 ), m_gc( gc ) {m_last_render_time = clan::System::get_time();}
	virtual ~StatusEffectPainter() {}

	void SetShadow( clan::Colorf shadow ) {
		m_shadow = shadow;
	}
	void SetSpacing( clan::Pointf spacing ) {
		m_spacing = spacing;
	}
	void SetStart( clan::Pointf point ) {
		m_start = point;
	}
	void SetHeight( int height ) {
		m_height = height;
	}
	void SetMultiplier( int m ) {
		m_mult = m;
	}

	virtual void Visit( StatusEffect* pEffect ) {
		
		clan::Sprite sprite = pEffect->GetIcon();
		sprite.update(clan::System::get_time() -  m_last_render_time);
		sprite.set_alpha( 0.5f );
		m_last_render_time = clan::System::get_time();
		int sprites_per_col = m_height / ( sprite.get_height() + m_spacing.y );
		int col = m_i / sprites_per_col;
		int row = m_i % sprites_per_col;
		clan::Pointf origin( m_start.x + col * ( m_spacing.x + sprite.get_width() ) * m_mult,
																				m_start.y + row * ( m_spacing.y + sprite.get_height() ) );
		clan::Sizef size( sprite.get_width(), sprite.get_height() );
		clan::Rectf box( origin, size );
		clan::Rectf shadowbox = box;
		shadowbox.translate( clan::Pointf( 2, 2 ) );

		// clan::Draw::fill(m_gc, shadowbox, m_shadow);
		sprite.draw( m_gc, origin.x, origin.y );
		m_i++;
	}

private:
	int m_i;
	int m_height;
	int m_mult;
	uint16_t m_last_render_time;
	clan::Colorf m_shadow;
	clan::Pointf m_start;
	clan::Pointf m_spacing;
	clan::Canvas & m_gc;
};
}
#ifdef WIN32
const BattleState::SpriteTicket BattleState::UNDEFINED_SPRITE_TICKET = 0xBEEFBEEF;
#else
const BattleState::SpriteTicket BattleState::UNDEFINED_SPRITE_TICKET = std::numeric_limits<int>::min();
#endif

BattleState::BattleState(){
#ifndef NDEBUG
	m_bShowSpriteRects = false;
#endif
}
BattleState::~BattleState(){
}

void BattleState::SetConfig( BattleConfig* config ) {
	m_config = config;
}

void BattleState::init( const std::vector<MonsterRef*>& monsters, int cellRows, int cellColumns, bool isBoss, const std::string & backdrop, bool scripted ) {
	m_monsters = new MonsterParty();
	m_bScriptedBattle = scripted;
	m_nRows = cellRows;
	m_nColumns = cellColumns;

	m_cur_char = 0;
	m_nRound = 0;

	m_bDoneAfterRound = false;
	m_bPlayerWon = false;
	m_bBossBattle = isBoss;
	// uint bottomrow = m_nRows - 1;

	m_last_render_time = clan::System::get_time();
	for ( std::vector<MonsterRef*>::const_iterator it = monsters.begin();
							it != monsters.end(); it++ ) {
		MonsterRef *pRef = *it;
		uint count = pRef->GetCount();

		for ( int y = 0;y < pRef->GetRows();y++ ) {
			for ( int x = 0;x < pRef->GetColumns();x++ ) {
				if ( count > 0 ) {
					Monster * pMonster = CharacterManager::CreateMonster( pRef->GetName() );
					pMonster->SetCellX( pRef->GetCellX() + x );
					pMonster->SetCellY( pRef->GetCellY() + y );

					pMonster->SetCurrentSprite( GraphicsManager::CreateMonsterSprite( pMonster->GetName(),  "idle" ) ); // fuck, dude

					m_monsters->AddMonster( pMonster );
					pMonster->Invoke();
				}

				if ( --count == 0 ) break;
			}
			if ( count == 0 ) break;
		}

		if ( count > 0 ) throw clan::Exception( "Couldn't fit all monsters in their rows and columns" );
	}
	m_backdrop = GraphicsManager::GetBackdrop( backdrop );
	
	m_darkModes.clear();
}

void BattleState::init( const MonsterGroup &group, const std::string &backdrop, bool scripted ) {
	m_bBossBattle = false;
	m_bPlayerWon = false;
	const std::vector<MonsterRef*> & monsters = group.GetMonsters();

	init( monsters, group.GetCellRows(), group.GetCellColumns(), false, backdrop, scripted );
}

void BattleState::add_participant_sprites() {
	m_sprite_mutex.lock();
	m_sprites.clear();
	for(auto it = m_initiative.begin(); it != m_initiative.end(); it++){
		Sprite sprite((*it)->GetCurrentSprite());
		sprite.SetEnabled(true);
		m_sprites.push_back(sprite);
	}
	m_sprite_mutex.unlock();
}


void BattleState::ClearDarkMode(int mode){
	m_darkModes.erase(mode);
}

void BattleState::SetDarkMode(int mode,float r, float g, float b, float a){
	m_darkModes[mode] = clan::Colorf(r,g,b,a);
}

bool BattleState::playerWon() const{
	return m_bPlayerWon;
}

void BattleState::set_positions_to_loci() {
	for ( int i=0;i<m_initiative.size();i++) {
		clan::Pointf pos = get_character_locus_rect( m_initiative[i] ).get_center();
		m_sprites[i].SetPosition(pos);
		m_sprites[i].SetOffset(clan::Pointf(0,0));
	}
	m_group_offsets.clear();
	m_shadow_offsets.clear();
}

void BattleState::run_turn(){
	ICharacter * iCharacter = m_initiative[m_cur_char];
	Character * pChar = dynamic_cast<Character*>( iCharacter );

	add_participant_sprites();
	set_positions_to_loci();

	iCharacter->StatusEffectRound();

	if ( pChar != NULL ) {
		ParameterList params;
		params.push_back( ParameterListItem( "$Character", pChar ) );
		params.push_back( ParameterListItem( "$Round", static_cast<int>( m_nRound ) ) );

		pChar->GetBattleMenu()->SetEnableConditionParams( params, pChar );
					
		pChar->GetBattleMenu()->Init();
		m_combat_state = BATTLE_MENU;
		m_menu_stack.push( pChar->GetBattleMenu() );
	} else {
		Monster * pMonster = dynamic_cast<Monster*>( m_initiative[m_cur_char] );
		assert( pMonster != NULL ); // has to be a monster...
		// Figure out what the monster will do here.
		m_combat_state = DISPLAY_ACTION;
		ParameterList params;
		// Supply a handle to the character in question;
		params.push_back( ParameterListItem( "$Character", pMonster ) );
		params.push_back( ParameterListItem( "$Round", static_cast<int>( m_nRound ) ) );
		pMonster->Round( params );
	}

}
void BattleState::set_shadow_offset(ICharacter* ch, const clan::Pointf& offset){
	m_shadow_offsets[get_sprite_for_char(ch)] = offset;
}

clan::Pointf BattleState::get_shadow_offset(ICharacter *ch){
	auto it = m_shadow_offsets.find(get_sprite_for_char(ch));
	if(it != m_shadow_offsets.end()){
		return it->second;
	}else{
		return clan::Pointf(0.0f,0.0f);
	}
}

void BattleState::next_turn() {
	if ( m_bDoneAfterRound ) {
		m_bDone = true;
	}

	m_combat_state = NEXT_TURN;
}

void BattleState::pick_next_character() {
	ICharacter * iChar;
	do{
		++m_cur_char;
		if(m_cur_char >= m_initiative.size()){
			m_cur_char = 0;
			m_nRound++;
		}
		iChar = m_initiative[m_cur_char];
		Character *pChar = dynamic_cast<Character*>( iChar );
		if(pChar){
			ParameterList params;
			params.push_back( ParameterListItem( "$Character", pChar ) );
			params.push_back( ParameterListItem( "$Round", static_cast<int>( m_nRound ) ) );

			pChar->GetBattleMenu()->SetEnableConditionParams( params, pChar );
			//pCharacter->GetBattleMenu()->Init();
			if(!pChar->GetBattleMenu()->HasEnabledOptions())
				continue;
		}
	}while(!iChar->GetToggle( ICharacter::CA_CAN_ACT )
							|| !iChar->GetToggle( ICharacter::CA_ALIVE ) );

}

bool characterInitiativeSort( const ICharacter* pChar1, const ICharacter* pChar2 ) {
	return pChar1->GetInitiative() > pChar2->GetInitiative();
}

void BattleState::roll_initiative() {
	Party * party = IApplication::GetInstance()->GetParty();

	for ( uint i = 0;i < party->GetCharacterCount();i++ ) {
		ICharacter * pChar = party->GetCharacter( i );
		pChar->RollInitiative();
		m_initiative.push_back( pChar );
	}

	for ( uint i = 0;i < m_monsters->GetCharacterCount();i++ ) {
		m_monsters->GetCharacter( i )->RollInitiative();
		m_initiative.push_back( m_monsters->GetCharacter( i ) );
	}

	// Okay they're all in the deque, now we just need to sort
	// it's ass
	std::sort( m_initiative.begin(), m_initiative.end(), characterInitiativeSort );

#ifndef NDEBUG
	std::cerr << "Initiative Order:" << std::endl;
	for ( std::deque<ICharacter*>::const_iterator iter = m_initiative.begin(); iter != m_initiative.end();
							iter++ ) {
		std::cerr << '\t' << ( *iter )->GetName() << " @ " << ( *iter )->GetInitiative() << std::endl;
	}
#endif
}

bool BattleState::IsDone() const {
	return m_bDone;
}

void BattleState::HandleButtonUp( const IApplication::Button& button ) {
	switch ( button ) {
		case IApplication::BUTTON_CONFIRM:
			if ( m_combat_state == BATTLE_MENU ) {

				BattleMenuOption * pOption = m_menu_stack.top()->GetSelectedOption();
				if ( pOption ) {
					ParameterList params;
					// Supply a handle to the character in question
					Character *pChar = dynamic_cast<Character*>( m_initiative[m_cur_char] );
					params.push_back( ParameterListItem( "$Character", pChar ) );
					params.push_back( ParameterListItem( "$Round", static_cast<int>( m_nRound ) ) );

					if ( pOption->Enabled( params, pChar ) ) {
						pOption->Select( m_menu_stack, params, pChar );
						// TODO: If we can detect that the command went through (was not cancelled)
						// then we could just call FinishTurn right here and not make the scripts call it...
					} else {
						// Play bbzt sound
						SoundManager::PlayEffect( SoundManager::EFFECT_BAD_OPTION );
					}
				}
			} else {
				// Play bbzt sound
				SoundManager::PlayEffect( SoundManager::EFFECT_BAD_OPTION );
			}
			break;
		case IApplication::BUTTON_CANCEL:
			if ( m_combat_state == BATTLE_MENU ) {
				if ( m_menu_stack.size() > 1 ) {
					SoundManager::PlayEffect( SoundManager::EFFECT_CANCEL );
					m_menu_stack.pop();
					// TODO: Should I "Deselect" here??
				}
			}
			break;


	}
}

void BattleState::HandleButtonDown( const IApplication::Button& button ) {

}

void BattleState::HandleAxisMove( const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos ) {
	if(m_eState == COMBAT){
		if ( axis == IApplication::AXIS_VERTICAL ) {
			if ( dir == IApplication::AXIS_DOWN ) {
				if ( m_combat_state == BATTLE_MENU )
					m_menu_stack.top()->SelectDown();
			} else if ( dir == IApplication::AXIS_UP ) {
				if ( m_combat_state == BATTLE_MENU )
					m_menu_stack.top()->SelectUp();
			}
		}
	}
}

void BattleState::HandleKeyDown( const clan::InputEvent &key ) {

}

void BattleState::HandleKeyUp( const clan::InputEvent &key ) {
#ifndef NDEBUG
	if(key.id == clan::keycode_m){
		m_bShowSpriteRects = !m_bShowSpriteRects;
	}
#endif
}

void BattleState::Draw( const clan::Rect &screenRect, clan::Canvas& GC ) {
	assert( m_draw_method != NULL );

	if ( m_eState == TRANSITION_IN ) {
		float passed = ( float )( clan::System::get_time() - m_startup_time ) / 1000.0f;
#ifdef __APPLE__
        passed = 1.0f;
#endif
		if ( passed >= 1.0f ) {
			m_eState = COMBAT;
			next_turn();
			pick_next_character();
			run_turn();
			( this->*m_draw_method )( screenRect, GC );
		} else {
			if ( m_transition == FLIP_ZOOM ||
								m_transition == FLIP_ZOOM_SPIN ||
								m_transition == FADE_IN
						) {
				//  clan::Sizef screenSize = screenRect.get_size() * passed;
				// Create a texture to draw to
				/*
				clan::Texture texture( GC, screenRect.get_width(), screenRect.get_height(), clan::cl_rgba );
				clan::FrameBuffer framebuffer( GC );
				// Bind the framebuffer to the texture
				framebuffer.attach_color_buffer( 0, texture );
				// Draw to this framebuffer instead of default
				GC.set_frame_buffer( framebuffer );
				( this->*m_draw_method )( screenRect, GC );
				GC.reset_frame_buffer();
				*/
				clan::FrameBuffer fb(GC.get_gc());
				clan::Texture2D tex(GC.get_gc(), screenRect.get_width(), screenRect.get_height());
				fb.attach_color(0,tex);
				clan::Canvas canvas(GC,fb);
				draw_battle( screenRect, canvas );
				//m_backdrop.draw(canvas, screenRect);
				// Make a sprite with the screen contents as it's sole frame
				clan::Sprite sprite( GC );
				// Have to get a texture2D to put in the sprite
				//clan::Texture2D tex ( canvas.get_gc(), canvas.get_pixeldata() );
				sprite.add_frame ( tex );
				
				if ( m_transition == FLIP_ZOOM || m_transition == FLIP_ZOOM_SPIN ) {
					clan::Rectf destRect(( screenRect.get_width() - ( screenRect.get_width() * passed ) ) / 2.0,
																							( screenRect.get_height() - ( screenRect.get_height() * passed ) ) / 2.0,
																							( screenRect.get_width() *passed ), ( screenRect.get_height() * passed ) );
					//image.set_scale(passed,passed);
					if ( m_transition == FLIP_ZOOM_SPIN )
						sprite.set_angle( clan::Angle::from_degrees( passed * 360.0f ) );
					sprite.draw( GC, destRect );
				} else if ( m_transition == FADE_IN ) {
					sprite.set_alpha( passed );
					sprite.draw( GC, 0, 0 );
				}
			}
		}
	}else{
		( this->*m_draw_method )( screenRect, GC );
	}
	if(m_eState == COMBAT && m_combat_state == NEXT_TURN){
		m_combat_state = BEGIN_TURN;
		run_turn();	
	}
}

bool BattleState::LastToDraw() const {
	return false;
}

bool BattleState::DisableMappableObjects() const {
	return true;
}

void BattleState::Update() {
}

void BattleState::StartTargeting() {
	m_targets.selected.m_pTarget = NULL;
	m_targets.m_bSelectedGroup = false;
	m_combat_state = TARGETING;
}

void BattleState::FinishTargeting() {
	m_combat_state = DISPLAY_ACTION;
}

void BattleState::Start() {
	m_draw_method = &BattleState::draw_battle;
	m_eState = TRANSITION_IN;

	const eTransition transitions[] = {FADE_IN,FLIP_ZOOM,FLIP_ZOOM_SPIN};

	int which_trans = rand() % (sizeof(transitions)/sizeof(eTransition));
	m_transition = transitions[which_trans];
	m_bDone = false;

#if 0
	m_pStatuFont = pGraphicsManager->GetFont( GraphicsManager::BATTLE_STATUS, GraphicsManager::HP );
	m_pStatusMPFont =  pGraphicsManager->GetFont( GraphicsManager::BATTLE_STATUS, GraphicsManager::MP );
	m_pStatusBPFont =  pGraphicsManager->GetFont( GraphicsManager::BATTLE_STATUS, GraphicsManager::BP );
	m_pStatusGeneralFont =  pGraphicsManager->GetFont( GraphicsManager::BATTLE_STATUS, GraphicsManager::GENERAL );
	m_pStatusBadFont = pGraphicsManager->GetFont( GraphicsManager::BATTLE_STATUS, GraphicsManager::BAD );
#endif

	m_generalFont = GraphicsManager::GetFont( GraphicsManager::BATTLE_STATUS, "general" );
	m_hpFont = GraphicsManager::GetFont( GraphicsManager::BATTLE_STATUS, "hp" );
	m_mpFont = GraphicsManager::GetFont( GraphicsManager::BATTLE_STATUS, "mp" );
	m_bpFont = GraphicsManager::GetFont( GraphicsManager::BATTLE_STATUS, "bp" );
	m_charNameFont = GraphicsManager::GetFont( GraphicsManager::BATTLE_STATUS, "name" );

	m_bp_gradient = GraphicsManager::GetGradient( GraphicsManager::BATTLE_STATUS, "bp_bar" );
	m_bp_box = GraphicsManager::GetRect( GraphicsManager::BATTLE_STATUS, "bp_box" );

	m_bp_border = GraphicsManager::GetColor( GraphicsManager::BATTLE_STATUS, "bp_border" );
	;

	m_player_rect = GraphicsManager::GetRect( GraphicsManager::BATTLE_STATUS, "player" );
	m_monster_rect = GraphicsManager::GetRect( GraphicsManager::BATTLE_STATUS, "monster" );
	m_statusRect = GraphicsManager::GetRect( GraphicsManager::BATTLE_STATUS, "status" );
	m_popupRect = GraphicsManager::GetRect( GraphicsManager::BATTLE_POPUP_MENU, "popup" );
	m_status_text_rect = GraphicsManager::GetRect( GraphicsManager::BATTLE_STATUS, "text" );


	m_status_effect_shadow_color = GraphicsManager::GetColor( GraphicsManager::BATTLE_STATUS, "status_effect_shadow" );
	m_status_effect_spacing = GraphicsManager::GetPoint( GraphicsManager::BATTLE_STATUS, "status_effect_spacing" );
	m_bp_gradient = GraphicsManager::GetGradient( GraphicsManager::BATTLE_STATUS, "bp_bar" );
	init_or_release_players( false );

	m_bDone = false;
	roll_initiative();
	add_participant_sprites();	
	set_positions_to_loci();	



	//next_turn();

	std::string music = "Music/Battle";
	if ( isBossBattle() ) music = "Music/Boss";
	// TODO: Is final boss?
	SoundManager::PushMusic();
	SoundManager::SetMusic( music );
	
	m_startup_time = clan::System::get_time();
}

void BattleState::init_or_release_players( bool bRelease ) {
	Party * pParty = IApplication::GetInstance()->GetParty();

	uint count = pParty->GetCharacterCount();
	for ( uint nPlayer = 0;nPlayer < count; nPlayer++ ) {
		Character * pCharacter = dynamic_cast<Character*>( pParty->GetCharacter( nPlayer ) );
		assert( pCharacter );
		std::string name = pCharacter->GetName();
		if ( !bRelease ) { 
			// TODO: Should have the character set its own sprite based on its state
			std::string state = "idle";
			if(!pCharacter->GetToggle(ICharacter::CA_ALIVE))
				state = "dead";
			pCharacter->SetCurrentSprite( GraphicsManager::CreateCharacterSprite( name, state ) ); // bullshit
		}
	}
}


void BattleState::Finish() {

	for ( uint i = 0; i < m_monsters->GetCharacterCount();i++ )
		delete m_monsters->GetCharacter( i );

	delete m_monsters;

	m_displays.clear();
	m_group_offsets.clear();
	m_sprite_mutex.lock();
	m_sprites.clear();
	m_sprite_mutex.unlock();
	m_initiative.clear();

	init_or_release_players( true );
	SoundManager::PopMusic();
}

void BattleState::draw_darkness( eDisplayOrder mode, const clan::Rectf &screenRect, clan::Canvas& GC ) {
	std::map<int,clan::Colorf>::const_iterator findIt = m_darkModes.find(mode);
	if(findIt != m_darkModes.end()){
		GC.fill_rect( screenRect, findIt->second );
	}
}

void BattleState::draw_transition_in( const clan::Rectf &screenRect, clan::Canvas& GC ) {
}

void BattleState::draw_start( const clan::Rectf &screenRect, clan::Canvas& GC ) {
}

void BattleState::draw_status( const clan::Rectf &screenRect, clan::Canvas& GC ) {
	Party * pParty = IApplication::GetInstance()->GetParty();

	MenuBox::Draw( GC, m_statusRect );

	clan::Rectf textRect = m_status_text_rect;

	//clan::Draw::box(GC,m_status_rect,clan::Colorf::red);
	int height_per_character = textRect.get_height() / pParty->GetCharacterCount();

	for ( uint p = 0; p < pParty->GetCharacterCount(); p++ ) {
		ICharacter * pChar = pParty->GetCharacter( p );
		std::ostringstream name;
		name << std::setw( 16 ) << std::left << pChar->GetName();
		std::ostringstream hp;
		hp << std::setw( 6 ) << (int)pChar->GetLerpAttribute( ICharacter::CA_HP ) << '/'
		<< pChar->GetAttribute( ICharacter::CA_MAXHP );
		
		if(m_combat_state == BATTLE_MENU && pChar == m_initiative[m_cur_char]){
			float percentage = float(clan::System::get_time() % 1000) / 1000.0f;
			m_generalFont.set_alpha(percentage);
		}

		m_generalFont.draw_text( GC, textRect.left,
					 				textRect.top + ( p * height_per_character ),
									name.str(), Font::TOP_LEFT );
		m_generalFont.set_alpha(1.0f);
		m_hpFont.draw_text( GC, textRect.get_width() / 3 + textRect.left,
										textRect.top + ( p* height_per_character )
											, hp.str(), Font::TOP_LEFT );
		std::ostringstream mp;
		mp << std::setw( 6 ) << (int)pChar->GetLerpAttribute( ICharacter::CA_MP ) << '/'
		<< pChar->GetAttribute( ICharacter::CA_MAXMP );
		m_mpFont.draw_text( GC, ( textRect.get_width() / 3 ) * 2 + textRect.left,
										textRect.top + ( p*height_per_character ), mp.str(), Font::TOP_LEFT
																				);

		clan::Rectf bp_box = m_bp_box;
		bp_box.top += textRect.top + ( p * height_per_character );
		bp_box.bottom += textRect.top + ( p * height_per_character );
		clan::Rectf bp_fill = bp_box;
		float percentage = pChar->GetLerpAttribute( ICharacter::CA_BP ) / pChar->GetAttribute( ICharacter::CA_MAXBP );
		bp_fill.right = bp_fill.left + ( percentage * bp_fill.get_width() );
		GC.fill_rect( bp_fill, m_bp_gradient );
		GC.draw_box( bp_box, m_bp_border );
	}
}

void BattleState::draw_battle( const clan::Rectf &screenRect, clan::Canvas& GC ) {
	GC.push_cliprect(screenRect);
	m_backdrop.draw( GC, screenRect );
	update_character_sprites();
	draw_status( screenRect, GC );

	draw_darkness( DISPLAY_ORDER_PRE_SPRITES,screenRect, GC );
	draw_shadows( screenRect, GC );
	draw_sprites( GC );
	draw_monsters( m_monster_rect, GC ); // draw various things around monsters
	draw_players( m_player_rect, GC );
	draw_status_effects( GC );
	draw_darkness( DISPLAY_ORDER_POST_SPRITES,screenRect, GC );


	draw_darkness( DISPLAY_ORDER_PRE_DISPLAYS, screenRect, GC );
	draw_displays( GC );
	draw_darkness( DISPLAY_ORDER_POST_DISPLAYS, screenRect, GC );

	draw_darkness( DISPLAY_ORDER_PRE_MENUS,screenRect, GC );
	if ( m_eState == COMBAT &&  m_combat_state == BATTLE_MENU ) {
		draw_menus( screenRect, GC );
	}

	draw_darkness( DISPLAY_ORDER_POST_MENUS,screenRect, GC );

	draw_darkness( DISPLAY_ORDER_FINAL, screenRect, GC );
	m_last_render_time = clan::System::get_time();
	GC.pop_cliprect();
}

void BattleState::draw_status_effects( clan::Canvas& GC ) {
	for ( std::deque<ICharacter*>::iterator iter = m_initiative.begin();
							iter != m_initiative.end(); iter++ ) {
		ICharacter * pICharacter = *iter;
		Character * pCharacter = dynamic_cast<Character*>( pICharacter );

		StatusEffectPainter painter( GC );
		painter.SetShadow( m_status_effect_shadow_color );
		painter.SetSpacing( m_status_effect_spacing );
		if ( pCharacter != NULL ) {
			clan::Rectf rectf = get_character_rect( pICharacter );

			painter.SetStart( rectf.get_top_right() );
			painter.SetHeight( rectf.get_height() );
			painter.SetMultiplier( 1 );

		} else {
			// Monster
			if ( !pICharacter->GetToggle( ICharacter::CA_ALIVE ) ||
								!pICharacter->GetToggle( ICharacter::CA_VISIBLE ) )
				continue;
			clan::Rectf rectf = get_character_rect( pICharacter );
			painter.SetHeight( rectf.get_height() );
			painter.SetStart( rectf.get_top_left() );
			painter.SetMultiplier( -1 );
		}

		pICharacter->IterateStatusEffects( painter );

	}
}


BattleState::Display::Display( BattleState& parent, BattleState::Display::eDisplayType type, int damage, ICharacter* pICharacter )
		: m_parent( parent ), m_eDisplayType( type ), m_amount( -damage ) {
	m_pTarget = dynamic_cast<ICharacter*>( pICharacter );
}
BattleState::Display::~Display() {
}

void BattleState::Display::start() {
	m_start_time = clan::System::get_time();
	m_complete = 0.0f;
}

bool BattleState::Display::expired() const {
	return m_complete >= 1.0f;
}

void BattleState::Display::update() {
	int elapsed = clan::System::get_time() - m_start_time;

	m_complete = ( double )elapsed / 2000.0;
}
void BattleState::Display::draw( clan::Canvas& GC ) {
	std::ostringstream stream;
	Font font;
	Font shadow_font;

	GraphicsManager::DisplayFont displayFont;


	switch ( m_eDisplayType ) {
		case DISPLAY_CRITICAL:
			stream << "CRITICAL ";
			// fallthrough
		case DISPLAY_DAMAGE:
			if ( m_amount >= 0 ) {
				displayFont = GraphicsManager::DISPLAY_HP_POSITIVE;
				stream << m_amount;
			} else {
				displayFont = GraphicsManager::DISPLAY_HP_NEGATIVE;
				stream << -m_amount;
			}
			break;
		case DISPLAY_MP:
			if ( m_amount >= 0 ) {
				displayFont = GraphicsManager::DISPLAY_MP_POSITIVE;
				stream << m_amount;
			} else {
				displayFont = GraphicsManager::DISPLAY_MP_NEGATIVE;
				stream << -m_amount;
			}
			break;
		case DISPLAY_MISS:
			displayFont = GraphicsManager::DISPLAY_MISS;
			stream << "MISS";
			break;

	}

	font = GraphicsManager::GetFont( GraphicsManager::GetFontName( displayFont ) );
	shadow_font = GraphicsManager::GetFont( GraphicsManager::GetFontName( GraphicsManager::DISPLAY_FONT_SHADOW ) );

	font.set_alpha( 1.0f - m_complete );
	shadow_font.set_alpha( 1.0f - m_complete );

	clan::Rect rect;
	Monster *pMonster = dynamic_cast<Monster*>( m_pTarget );

	if ( pMonster ) {
		rect = m_parent.get_character_rect( pMonster );
	} else {
		Party * pParty = IApplication::GetInstance()->GetParty();
		for ( uint n = 0;n < pParty->GetCharacterCount();n++ ) {
			if ( m_pTarget == pParty->GetCharacter( n ) ) {
				rect = m_parent.get_character_rect( pParty->GetCharacter( n ) );
				break;
			}
		}
	}

	std::string value( stream.str() );

	clan::Point pos = rect.get_top_right();

	pos.y += font.get_font_metrics( GC ).get_height();
	pos.x -= font.get_text_size( GC, value ).width;

	shadow_font.draw_text( GC, pos.x - 2, pos.y - 2, value );
	font.draw_text( GC, pos.x, pos.y, value );


}

BattleState::Sprite::Sprite( clan::Sprite sprite ): m_sprite( sprite ), m_enabled(true),m_zorder(0) {
}

BattleState::Sprite::~Sprite() {
}


bool BattleState::Sprite::operator<( const BattleState::Sprite& other ) const {
	clan::Pointf bottom_right = Rect().get_bottom_right(); 
	return bottom_right.y + m_offset.y + m_zorder < other.Rect().get_bottom_right().y + other.m_offset.y + other.m_zorder;
}

void BattleState::Sprite::SetPosition( const clan::Pointf& pos ) {
	m_pos = pos;
}

clan::Rectf BattleState::Sprite::Rect(bool with_offsets) const { 
	clan::Sizef size(m_sprite.get_size().width,m_sprite.get_size().height);
	if(with_offsets)
		return clan::Rectf(m_pos.x+m_offset.x-size.width/2.0f,m_pos.y+m_offset.x-size.height/2.0f,size);
	else
		return clan::Rectf(m_pos.x-size.width/2.0f,m_pos.y-size.height/2.0f,size);
		
		
	//return clan::Rectf(m_pos,size);
}

clan::Pointf BattleState::Sprite::Position(bool with_offsets)const {
	if(with_offsets)
		return m_pos + m_offset;
	else
		return m_pos;
}
void BattleState::Sprite::Draw( clan::Canvas& gc) {
	m_sprite.set_alignment( clan::origin_center );
	clan::Rectf rect = Rect();
	clan::Pointf point = rect.get_center();
	m_sprite.draw( gc, point.x + m_offset.x, point.y + m_offset.y );
}

int BattleState::Sprite::ZOrder() const {
	return m_zorder;
}

void BattleState::Sprite::SetZOrder( int z ) {
	m_zorder = z;
}

bool BattleState::Sprite::Enabled() const {
	return m_enabled;
}

void BattleState::Sprite::SetEnabled( bool enabled ) {
	m_enabled = enabled;
}

clan::Sprite BattleState::Sprite::GetSprite() const {
	return m_sprite;
}

void BattleState::Sprite::SetSprite(clan::Sprite sprite){
	m_sprite = sprite;
}

clan::Pointf BattleState::Sprite::GetOffset() const {
	return m_offset;
}

void BattleState::Sprite::SetOffset(const clan::Pointf& pt){
	m_offset = pt;
}

int BattleState::add_sprite( clan::Sprite sprite ) {
	m_sprite_mutex.lock();
	int index = m_sprites.size();
	sprite.set_alignment( clan::origin_center );
	Sprite mysprite( sprite );


	mysprite.SetEnabled(true);
	m_sprites.push_back( mysprite );
	m_sprite_mutex.unlock();
	return index;
}

void BattleState::set_sprite_pos( SpriteTicket nSprite, const clan::Pointf& pos ) {
	assert(nSprite != UNDEFINED_SPRITE_TICKET);
	assert(nSprite < m_sprites.size());
	m_sprite_mutex.lock();
	m_sprites[nSprite].SetPosition( pos );
	m_sprite_mutex.unlock();
}


clan::Sprite BattleState::get_sprite( SpriteTicket nSprite )  {
	clan::MutexSection l(&m_sprite_mutex);
	assert(nSprite != UNDEFINED_SPRITE_TICKET);
	assert(nSprite < m_sprites.size());
	return m_sprites[nSprite].GetSprite();
}

void BattleState::remove_sprite( SpriteTicket nSprite ) {
	// If an animation asks us to remove a sprite, we actually don't, we just disable it.
	// at the end of the turn, it'll get removed
	if ( nSprite <  m_sprites.size() ) {
		m_sprite_mutex.lock();
		m_sprites[nSprite].SetEnabled( false );
		m_sprite_mutex.unlock();
	}
}

clan::Rectf BattleState::get_sprite_rect( BattleState::SpriteTicket nSprite ) {
	clan::MutexSection l(&m_sprite_mutex);
	return m_sprites[nSprite].Rect();
}


void BattleState::draw_shadows( const clan::Rectf &screenRect, clan::Canvas& GC ){
	for(int i = 0; i<m_initiative.size(); i++){
		if(m_sprites[i].Enabled()){
			ICharacter * c = m_initiative[i]; 
			clan::Pointf shadow_off;
			auto it = m_shadow_offsets.find(i);
			if(it != m_shadow_offsets.end()){
				shadow_off = it->second;
			}
			bool turn = (m_cur_char == i);
			float alpha = c->GetCurrentSprite().get_alpha();
			clan::Rectf rect = get_character_rect(c);
			clan::Pointf pt = rect.get_bottom_left();
			pt += shadow_off;
			pt.x += rect.get_width() / 2.0f;
			pt.y -= rect.get_height() / 15.0f; // TODO: Make this a resource config, and be able to override it per character
			clan::Shape2D shape;
			std::vector<clan::Vec2f> shadowTris;
			shape.add_ellipse(pt.x,pt.y,rect.get_width() / 2.0f, rect.get_width() / 2.0f * 0.33f);
			shape.get_triangles(shadowTris);
			float blue = 0.0f;
			if(turn){
				blue = 2.0f * (float(clan::System::get_time() %
				1000) / 1000.0f);
				if(blue > 1.0f){
					blue = 1.0f - (blue-1.0f);
				}
			}
			GC.fill_triangles(shadowTris,clan::Colorf(0.0f,blue,blue,0.25f * alpha));
		}
		//add_ellipse (float center_x, float center_y, float radius_x, float radius_y, bool reverse=false)
		//GC.fill_circle(
	}
}

void BattleState::draw_sprites( clan::Canvas& GC ) {
	m_sprite_mutex.lock();
	auto sprites = m_sprites; // Copy sprites so I can maintain their order... must be a better way?
	m_sprite_mutex.unlock(); // because I copied them 
	std::sort(std::begin(sprites),std::end(sprites));
	for ( int i = 0;i < sprites.size();i++ ) {
		if ( sprites[i].Enabled() ) {
			sprites[i].GetSprite().update(clan::System::get_time()-m_last_render_time);
#ifndef NDEBUG
			if(m_bShowSpriteRects){
				clan::Rectf rect = sprites[i].Rect(true);
				GC.draw_box(rect,clan::Colorf(0.0f,1.0f,1.0f));
			}
#endif
			sprites[i].Draw( GC );
		}
	}
}

# if 0 
bool SortByBattlePos( const ICharacter* d1, const ICharacter* d2 ) {

	clan::Pointf pos1, pos2;
	pos1 = d1->GetBattlePos();
	pos2 = d2->GetBattlePos();


	return pos1.y * IApplication::GetInstance()->GetDisplayRect().get_width() + pos1.x <
								pos2.y * IApplication::GetInstance()->GetDisplayRect().get_width() + pos2.x;
}
#endif


void BattleState::update_character_sprites(){
	clan::Pointf monster_group_offset;
	clan::Pointf player_group_offset;
	const clan::Pointf empty(0.0,0.0);
	
	if(m_group_offsets.count(m_monsters)){
		monster_group_offset = m_group_offsets[m_monsters];
	}
	if(m_group_offsets.count(IApplication::GetInstance()->GetParty())){
		player_group_offset = m_group_offsets[IApplication::GetInstance()->GetParty()];
	}
	
	for ( uint i = 0; i < m_initiative.size(); i++ ) {

		ICharacter * pChar = m_initiative[i];
		assert(pChar);
	
		m_sprite_mutex.lock();
		// ICharacter *iCharacter = m_monsters->GetCharacter(i);
		if(pChar->IsMonster()){
			m_sprites[i].SetEnabled( pChar->GetToggle ( ICharacter::CA_VISIBLE) );
			if(monster_group_offset != empty){
				m_sprites[i].SetOffset(monster_group_offset);
			}
		}else{
			if(player_group_offset != empty){
				m_sprites[i].SetOffset(player_group_offset);
			}			
		}
		m_sprites[i].SetSprite(current_sprite(m_initiative[i]));

		clan::Sprite sprite = m_sprites[i].GetSprite();
		m_sprite_mutex.unlock();		

		if ( !pChar->GetToggle( ICharacter::CA_DRAW_STILL ) &&  pChar->GetToggle( ICharacter::CA_ALIVE ) )
			sprite.update(clan::System::get_time()-m_last_render_time);
	}
}

void BattleState::draw_monsters( const clan::Rectf &monsterRect, clan::Canvas& GC ) {
	if(m_combat_state != TARGETING) return; // I only do extra stuff during targeting...
	
	for ( uint i = 0; i < m_initiative.size(); i++ ) {
		if(!m_initiative[i]->IsMonster())
			continue;
		Monster *pMonster = dynamic_cast<Monster*>(m_initiative[i]);
		assert(pMonster);

		clan::Pointf center = m_sprites[i].Position();

		if ( m_combat_state == TARGETING ) {
			if (( m_targets.m_bSelectedGroup && m_targets.selected.m_pGroup == m_monsters )
							|| ( !m_targets.m_bSelectedGroup && pMonster == m_targets.selected.m_pTarget ) ) {
				clan::FontMetrics metrics = m_charNameFont.get_font_metrics( GC );
				clan::Sizef textSize = m_charNameFont.get_text_size( GC, pMonster->GetName() );
				clan::Pointf textPoint = clan::Pointf( center.x - textSize.width / 2, get_character_rect( pMonster ).get_top_left().y );

				if ( textPoint.y - metrics.get_height() < 0 ) {
					// Gonna have to draw below the monster
					textPoint = clan::Pointf( center.x - textSize.width / 2,
											get_character_rect( pMonster ).get_bottom_left().y + metrics.get_height() );
				}

				m_charNameFont.draw_text( GC, textPoint, pMonster->GetName(), Font::DEFAULT );
			}
		}


	}
}

clan::Sprite BattleState::current_sprite( ICharacter* iCharacter ) const {
	clan::Sprite  sprite = iCharacter->GetCurrentSprite( true );
	return sprite;
}


void BattleState::draw_players( const clan::Rectf &playerRect, clan::Canvas& GC ) {

}

clan::Rectf  BattleState::get_group_rect( ICharacterGroup* group ) const {
	MonsterParty * monsterParty = dynamic_cast<MonsterParty*>( group );

	if ( monsterParty != NULL ) {
		return m_monster_rect;
	} else {
		return m_player_rect;
	}
}

clan::Pointf BattleState::get_monster_locus( const Monster * pMonster )const {
	clan::Pointf point;
	const uint cellWidth = m_monster_rect.get_width() / m_nColumns;
	const uint cellHeight = m_monster_rect.get_height() / m_nRows;
	clan::Sprite  sprite = const_cast<Monster*>( pMonster )->GetCurrentSprite();


	point.x = m_monster_rect.left + pMonster->GetCellX() * cellWidth + ( cellWidth  / 2 );
	point.y = m_monster_rect.top + pMonster->GetCellY() * cellHeight + ( cellHeight / 2 );
	return point;
}

clan::Pointf BattleState::get_player_locus( uint nPlayer )const {
	clan::Pointf point;
	const uint partySize = IApplication::GetInstance()->GetParty()->GetCharacterCount();
	double each_player = m_player_rect.get_width() / partySize;
	// TODO: Get the spacing from config
	point.x = m_player_rect.left + ( nPlayer ) * each_player + ( each_player / 2.0 );
	point.y = m_player_rect.top + ( nPlayer ) * 64;
	return point;
}

clan::Pointf BattleState::get_character_locus( const ICharacter* pCharacter ) const {
	clan::Point pointf;
	const Monster * pMonster = dynamic_cast<const Monster*>( pCharacter );
	if ( pMonster != NULL ) {
		return get_monster_locus( pMonster );
	} else {
		Party * pParty = IApplication::GetInstance()->GetParty();
		uint playercount = pParty->GetCharacterCount();
		for ( uint i = 0;i < playercount;i++ ) {
			ICharacter * iCharacter = pParty->GetCharacter( i );

			if ( pCharacter == iCharacter ) {
				return get_player_locus( i );
			}
		}
	}
	assert( 0 );
	return clan::Pointf( 0.0, 0.0 );
}

clan::Sizef  BattleState::get_character_size( const ICharacter* pCharacter ) const {
#if 0 
	const Monster * pMonster = dynamic_cast<const Monster*>( pCharacter );
	if ( pMonster != NULL ) {
		return const_cast<Monster*>( pMonster )->GetCurrentSprite().get_size();
	} else {
		return clan::Sizef( 60, 120 );
	}
#else
	return const_cast<ICharacter*>(pCharacter)->GetCurrentSprite().get_size();
#endif
}


clan::Rectf BattleState::get_character_rect( const ICharacter* pCharacter )const {
	clan::Rectf rect;
	int n = 0;
	for(int i=0;i<m_initiative.size();i++){
		if(m_initiative[i] == pCharacter){
			n = i;
			break;
		}
	}
	clan::Pointf point = m_sprites[n].Position(true);
	clan::Sizef size = get_character_size( pCharacter );

	//rect.set_top_left( clan::Pointf( point.x - size.width / 2.0f, point.y - size.height / 2.0f ) );
	rect = clan::Rectf(point.x - size.width / 2.0f,point.y - size.height / 2.0f, size);
	/* rect.top = point.x - size.width / 2.0f;
	 rect.left = point.y - size.height / 2.0f;
	 rect.bottom = rect.top + size.height;
	 rect.right = rect.left + size.width; */
	//rect.set_size( size );


	return rect;
}

clan::Rectf BattleState::get_character_locus_rect( const ICharacter* pCharacter )const {
	clan::Rectf rect;
	clan::Pointf point = get_character_locus( pCharacter );
	clan::Sizef size = get_character_size( pCharacter );
	rect.set_top_left( clan::Pointf( point.x - size.width / 2.0f, point.y - size.height / 2.0f ) );
	rect.set_size( get_character_size( pCharacter ) );


	/*
	std::cout << pCharacter->GetName() <<" locus " << '(' << rect.left << ',' << rect.top << ')' << '('
	<< rect.right << ',' << rect.bottom <<')' << 'c'
	<< rect.get_center().x << ',' << rect.get_center().y << std::endl;
	*/
	return rect;
}


void BattleState::draw_displays( clan::Canvas& GC ) {
	for ( std::list<Display>::iterator iter = m_displays.begin();iter != m_displays.end();iter++ ) {
		iter->update();
		iter->draw( GC );
	}

	// m_displays.remove_if(Display::has_expired);
	m_displays.remove_if( std::mem_fun_ref( &Display::expired ) );
}


void BattleState::draw_menus( const clan::Rectf &screenrect, clan::Canvas& GC ) {
	BattleMenu * pMenu = m_menu_stack.top();
	clan::Rectf menu_rect = m_popupRect;
	menu_rect.translate( GraphicsManager::GetMenuInset() );
	menu_rect.shrink( GraphicsManager::GetMenuInset().x, GraphicsManager::GetMenuInset().y );

	pMenu->SetRect( menu_rect );

	if ( pMenu->GetType() == BattleMenu::POPUP ) {
		MenuBox::Draw( GC, m_popupRect, false );
		pMenu->Draw( GC );
	}
}
void BattleState::SteelInit( SteelInterpreter* pInterpreter ) {
	pInterpreter->pushScope();

	static SteelFunctor3Arg<BattleState, bool, bool, bool> fn_selectTargets( this, &BattleState::selectTargets );
	static SteelFunctorNoArgs<BattleState> fn_finishTurn( this, &BattleState::finishTurn );
	static SteelFunctorNoArgs<BattleState> fn_cancelOption( this, &BattleState::cancelOption );
	static SteelFunctor3Arg<BattleState, int, SteelType::Handle, int> fn_createDisplay( this, &BattleState::createDisplay );
	static SteelFunctor1Arg<BattleState, bool> fn_getCharacterGroup( this, &BattleState::getCharacterGroup );
	static SteelFunctorNoArgs<BattleState> fn_getAllCharacters( this, &BattleState::getAllCharacters );
	static SteelFunctor1Arg<BattleState, SteelType::Handle> fn_getMonsterDamageCategory( this, &BattleState::getMonsterDamageCategory );

	static SteelFunctorNoArgs<BattleState> fn_flee( this, &BattleState::flee );
	static SteelFunctorNoArgs<BattleState> fn_isBossBattle( this, &BattleState::isBossBattle );
	static SteelFunctor1Arg<BattleState, int> fn_clearDarkMode( this, &BattleState::clearDarkMode );
	static SteelFunctor5Arg<BattleState, int, double, double, double, double> fn_darkColor( this, &BattleState::darkMode );

	pInterpreter->addFunction("animation","battle",new SteelFunctor1Arg<BattleState,SteelType::Functor>(this,&BattleState::animation));
	
	SteelConst( pInterpreter, "$_DISP_DAMAGE", Display::DISPLAY_DAMAGE );
	SteelConst( pInterpreter, "$_DISP_MP", Display::DISPLAY_MP );
	SteelConst( pInterpreter, "$_DISP_MISS", Display::DISPLAY_MISS );
	SteelConst( pInterpreter, "$_DISP_CRITICAL", Display::DISPLAY_CRITICAL );
	SteelConst( pInterpreter, "$_POST_BACKDROP", DISPLAY_ORDER_POSTBACKDROP );
	SteelConst( pInterpreter, "$_PRE_MONSTERS", DISPLAY_ORDER_PREMONSTERS );
	SteelConst( pInterpreter, "$_POST_MONSTERS", DISPLAY_ORDER_POSTMONSTERS );
	SteelConst( pInterpreter, "$_PRE_PLAYERS", DISPLAY_ORDER_PRE_PLAYERS );
	SteelConst( pInterpreter, "$_POST_PLAYERS", DISPLAY_ORDER_POST_PLAYERS );
	SteelConst( pInterpreter, "$_PRE_SPRITES", DISPLAY_ORDER_PRE_SPRITES );
	SteelConst( pInterpreter, "$_POST_SPRITES", DISPLAY_ORDER_POST_SPRITES );
	SteelConst( pInterpreter, "$_PRE_DISPLAYS", DISPLAY_ORDER_PRE_DISPLAYS );
	SteelConst( pInterpreter, "$_POST_DISPLAYS", DISPLAY_ORDER_POST_DISPLAYS );
	SteelConst( pInterpreter, "$_PRE_MENUS", DISPLAY_ORDER_PRE_MENUS );
	SteelConst( pInterpreter, "$_POST_MENUS", DISPLAY_ORDER_POST_MENUS );
	SteelConst( pInterpreter, "$_FINAL_DRAW", DISPLAY_ORDER_FINAL );

	pInterpreter->addFunction( "selectTargets", "battle", new SteelFunctor3Arg<BattleState, bool, bool, bool>( this, &BattleState::selectTargets ) );
	pInterpreter->addFunction( "finishTurn", "battle", new SteelFunctorNoArgs<BattleState>( this, &BattleState::finishTurn ) );
	pInterpreter->addFunction( "cancelOption", "battle", new SteelFunctorNoArgs<BattleState>( this, &BattleState::cancelOption ) );
	pInterpreter->addFunction( "createDisplay", "battle", new SteelFunctor3Arg<BattleState, int, SteelType::Handle, int>( this, &BattleState::createDisplay ) );
	pInterpreter->addFunction( "getCharacterGroup", "battle", new SteelFunctor1Arg<BattleState, bool>( this, &BattleState::getCharacterGroup ) );
	pInterpreter->addFunction( "getAllCharacters", "battle", new SteelFunctorNoArgs<BattleState>( this, &BattleState::getAllCharacters ) );
	pInterpreter->addFunction( "getMonsterDamageCategory", "battle", new SteelFunctor1Arg<BattleState, SteelType::Handle>( this, &BattleState::getMonsterDamageCategory ) );
	//pInterpreter->addFunction("getSkill","battle",new SteelFunctor2Arg<BattleState,SteelType::Handle,const std::string&>(this,&BattleState::getSkill));
	pInterpreter->addFunction( "clearDarkMode", "battle", Steel::create_functor(this,&BattleState::clearDarkMode) );
	pInterpreter->addFunction( "flee", "battle", Steel::create_functor( this, &BattleState::flee ) );
	pInterpreter->addFunction( "isBossBattle", "battle", new SteelFunctorNoArgs<BattleState>( this, &BattleState::isBossBattle ) );
	//pInterpreter->addFunction( "clearDarkMode", "battle", new SteelFunctor1Arg<BattleState, int>( this, &BattleState::clearDarkMode ) );
	pInterpreter->addFunction( "darkMode", "battle", Steel::create_functor( this, &BattleState::darkMode ) );
	m_config->SetupForBattle();
}

void BattleState::SteelCleanup( SteelInterpreter* pInterpreter ) {
	//pInterpreter->removeFunctions( "battle" );
	pInterpreter->popScope();
	m_config->TeardownForBattle();
}

ICharacter* BattleState::get_next_character( const ICharacterGroup* pParty, const ICharacter* pCharacter ) const {
	const int party_count = pParty->GetCharacterCount();
	for ( int i = 0;i < party_count;i++ ) {
		if ( pParty->GetCharacter( i ) == pCharacter ) {
			if ( i + 1 == party_count ) {
				return pParty->GetCharacter( 0 );
			} else {
				return pParty->GetCharacter( i + 1 );
			}
		}
	}

	assert( 0 );
	return NULL;
}

ICharacter* BattleState::get_prev_character( const ICharacterGroup* pParty, const ICharacter* pCharacter ) const {
	const int party_count = pParty->GetCharacterCount();
	for ( int i = 0;i < party_count;i++ ) {
		if ( pParty->GetCharacter( i ) == pCharacter ) {
			if ( i - 1 < 0 ) {
				return pParty->GetCharacter( party_count - 1 );
			} else {
				return pParty->GetCharacter( i - 1 );
			}
		}

	}

	assert( 0 );
	return NULL;
}

#if 0
void BattleState::SelectNextTarget() {
	Monster * pMonster = dynamic_cast<Monster*>( m_targets.selected.m_pTarget );
	if ( pMonster != NULL ) {
		m_targets.selected.m_pTarget = get_next_character( m_monsters, pMonster );
	} else {
		Character * pCharacter = dynamic_cast<Character*>( m_targets.selected.m_pTarget );
		const Party * pParty = dynamic_cast<Party*>( IApplication::GetInstance()->GetParty() );
		m_targets.selected.m_pTarget = get_next_character( pParty, pCharacter );
	}
}

void BattleState::SelectPreviousTarget() {
	Monster * pMonster = dynamic_cast<Monster*>( m_targets.selected.m_pTarget );
	if ( pMonster != NULL ) {
		// We have a monster
		m_targets.selected.m_pTarget = get_prev_character( m_monsters, pMonster );
	} else {
		Character * pCharacter = dynamic_cast<Character*>( m_targets.selected.m_pTarget );
		const Party * pParty = dynamic_cast<Party*>( IApplication::GetInstance()->GetParty() );
		m_targets.selected.m_pTarget = get_prev_character( pParty, pCharacter );
	}
}

void BattleState::SelectLeftTarget() {
//
}

void BattleState::SelectRightTarget() {

}


void BattleState::SelectUpTarget() {

}
void BattleState::SelectDownTarget() {


}

#endif

BattleState::SpriteTicket BattleState::get_sprite_for_char( ICharacter * ichar )const{
	for(int i=0;i<m_initiative.size();i++){
		if(m_initiative[i] == ichar)
			return i;
	}
	assert(0);
	return UNDEFINED_SPRITE_TICKET;
}


ICharacterGroup* BattleState::group_for_character( ICharacter* pICharacter ) {
	if ( pICharacter->IsMonster() ) return m_monsters;
	else return IApplication::GetInstance()->GetParty();
}

bool BattleState::MonstersOnLeft() {
	return true;
}

void BattleState::FinishTurn() {
	m_combat_state = FINISHING_TURN;
	// Clear the menu stack
	while ( !m_menu_stack.empty() )
		m_menu_stack.pop();
	// TODO: Good time to check if either side is wiped out, etc
	// And, any dead monsters need to have ->Remove called on them
	ParameterList params;
	m_config->OnTurn( params );
	check_for_death();
	if ( !end_conditions() ) {
		pick_next_character();
		next_turn();
	}
}


// Go back to menu, they decided not to proceed with this option
void BattleState::CancelTargeting() {
	SoundManager::PlayEffect( SoundManager::EFFECT_CANCEL );
	m_targets.m_bSelectedGroup = false;
	m_targets.selected.m_pTarget = NULL;
}

bool BattleState::end_conditions() {
	Party * party = IApplication::GetInstance()->GetParty();
	// Now, see if the monsters are all dead
	bool anyAlive = false;
	for ( int i = 0;i < m_monsters->GetCharacterCount();i++ ) {
		if ( m_monsters->GetCharacter( i )->GetToggle( ICharacter::CA_ALIVE ) ) {
			anyAlive = true;
			break;
		}
	}

	if ( !anyAlive ) {
		// You win!
		// TODO: Win
		win();
		// return so you don't lose after you win
		return true;
	}

	anyAlive = false;

	for ( int i = 0;i < party->GetCharacterCount();i++ ) {
		if ( party->GetCharacter( i )->GetToggle( ICharacter::CA_ALIVE ) ) {
			anyAlive = true;
			break;
		}
	}

	if ( !anyAlive ) {
		lose();
		return true;
	}

	return false;
}


void BattleState::check_for_death() {

	for ( std::deque<ICharacter*>::const_iterator iter = m_initiative.begin();
							iter != m_initiative.end(); iter++ ) {
		Monster * pMonster = dynamic_cast<Monster*>( *iter );

		if ( pMonster ) {
			if ( !pMonster->GetToggle( ICharacter::CA_ALIVE ) && !pMonster->DeathAnimated() ) {
				pMonster->Die();
				death_animation( pMonster );
				pMonster->SetToggle( ICharacter::CA_VISIBLE, false );
			}
		} else {
			// Change PC sprite to dead one?
		}
	}
	// BRING OUT YOUR DEAD!
	// Ok. We don't do this because we need to leave the dead actually in the initiative, to do status effect rounds on them
	/*
	m_initiative.erase(std::remove_if(m_initiative.begin(), m_initiative.end(),
	      std::not1(std::bind2nd(std::mem_fun(&ICharacter::GetToggle),ICharacter::CA_ALIVE)))
	    ,m_initiative.end());
	*/
}

void BattleState::death_animation( Monster* pMonster ) {
	pMonster->MarkDeathAnimated();
}

void BattleState::lose() {
	m_bDone = true;


	ParameterList params;
	if(!m_bScriptedBattle)
		m_config->OnBattleLost( params );
}

void BattleState::win() {
	SoundManager::SetMusic( "Music/Fanfare" );
	m_bDone = true;
	m_bPlayerWon = true;
	ParameterList params;
	// All battle methods remain valid here
	m_config->OnBattleWon( params );
	Party * pParty = IApplication::GetInstance()->GetParty();

	// TODO: Maybe this should just be steel in OnBattleWon?
	for ( int i = 0;i < pParty->GetCharacterCount(); i++ )
		dynamic_cast<Character*>( pParty->GetCharacter( i ) )->RemoveBattleStatusEffects();
}

void BattleState::TargetChanged(){
}

/***** BIFS *****/
SteelType BattleState::selectTargets( bool single, bool group, bool defaultMonsters ) {
	m_combat_state = TARGETING;
	SteelType::Container targets;
	TargetingState::Targetable targetable;
	if ( single && group )
		targetable = TargetingState::EITHER;
	else if ( group )
		targetable = TargetingState::GROUP;
	else
		targetable = TargetingState::SINGLE;

	m_targeting_state.Init( this, targetable, defaultMonsters );
	IApplication::GetInstance()->RunState( &m_targeting_state );


	if ( m_targets.m_bSelectedGroup ) {

		for ( uint i = 0;i < m_targets.selected.m_pGroup->GetCharacterCount();i++ ) {
			if(!m_targets.selected.m_pGroup->GetCharacter(i)->GetToggle(ICharacter::CA_ALIVE))
				continue;
			SteelType ref;
			ref.set( m_targets.selected.m_pGroup->GetCharacter( i ) );
			targets.push_back( ref );
		}

	} else {
		SteelType ref;
		if ( m_targets.selected.m_pTarget ) {
			ref.set( m_targets.selected.m_pTarget );
			targets.push_back( ref );
		}
	}

	SteelType val;
	val.set( targets );
	return val;
}

SteelType BattleState::finishTurn() {
	FinishTurn(); 
	return SteelType();
}
// if they back out and want to go back to the battle menu
SteelType BattleState::cancelOption() {
	m_combat_state = BATTLE_MENU;
	SteelType var;
	var.set(-1);
	return var;
}

SteelType BattleState::clearDarkMode( int nOrder ) {
	ClearDarkMode(nOrder);
	return SteelType();
}

SteelType BattleState::darkMode( int nOrder, double r, double g, double b, double a ) {
	SetDarkMode(nOrder,r,g,b,a);
	return SteelType();
}

SteelType BattleState::animation( SteelType::Functor functor ) {
	AnimationState state(*this);
	state.Init(functor);
	IApplication::GetInstance()->RunState(&state);
	return SteelType();
}





SteelType BattleState::createDisplay( int damage, SteelType::Handle hICharacter, int display_type ) {
	//            Display(BattleState& parent,eDisplayType type,int damage,SteelType::Handle pICharacter);
	ICharacter* iChar = GrabHandle<ICharacter*>( hICharacter );
	if ( !iChar ) {
		throw TypeMismatch();
	}
	Display display( *this, static_cast<Display::eDisplayType>( display_type ), damage, iChar );
	display.start();
	m_displays.push_back( display );

	return SteelType();
}

SteelType BattleState::getCharacterGroup( bool monsters ) {
	SteelType::Container vector;
	ICharacterGroup * group;

	if ( monsters ) {
		group = m_monsters;
	} else {
		group = IApplication::GetInstance()->GetParty();
	}

	for ( uint i = 0;i < group->GetCharacterCount();i++ ) {
		SteelType handle;
		handle.set( group->GetCharacter( i ) );
		vector.push_back( handle );
	}
	SteelType array;
	array.set( vector );
	return array;
}

SteelType BattleState::getAllCharacters() {
	SteelType::Container vector;

	for ( uint i = 0;i < m_initiative.size();i++ ) {
		SteelType handle;
		handle.set( m_initiative[i] );
		vector.push_back( handle );
	}

	SteelType array;
	array.set( vector );
	return array;
}

SteelType BattleState::getMonsterDamageCategory( SteelType::Handle hMonster ) {
	SteelType result;
	Monster * pMonster = GrabHandle<Monster*>( hMonster );
	if ( !pMonster ) throw TypeMismatch();

	result.set( pMonster->GetDefaultDamageCategory() );

	return result;
}

SteelType BattleState::flee() {
	m_bDoneAfterRound = true;
	return SteelType();
}

SteelType BattleState::isBossBattle() {
	SteelType val;
	val.set( m_bBossBattle );

	return val;
}


void BattleState::set_offset( BattleState::SpriteTicket sprite, const clan::Pointf& offset ) {
	clan::MutexSection l(&m_sprite_mutex);
	m_sprites[sprite].SetOffset(offset);
}

clan::Pointf BattleState::get_offset( BattleState::SpriteTicket sprite ) {
	clan::MutexSection l(&m_sprite_mutex);
	return m_sprites[sprite].GetOffset();
}


void BattleState::set_offset( ICharacterGroup* pGroup, const clan::Pointf& offset ) {
	m_group_offsets[pGroup] = offset;
}

clan::Pointf BattleState::get_offset( ICharacterGroup* pGroup ) {
	return m_group_offsets[pGroup];
}



