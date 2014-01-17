#ifndef SR2_BATTLE_STATE_H
#define SR2_BATTLE_STATE_H

#include "State.h"
#include "sr_defines.h"
#include "MonsterGroup.h"
#include <ClanLib/core.h>
#include <deque>
#include <stack>
#include <map>
#include "BattleMenuOption.h"
#include "TargetingState.h"
#include "BattleConfig.h"


namespace StoneRing {

class MonsterRef;
class Monster;
class MonsterParty;
class IBattleAction;
class ICharacter;
class MonsterSort;
class AnimationState;


class BattleState : public State {
public:
	BattleState();
	virtual ~BattleState();
	void init( const MonsterGroup& monsters, const std::string &backdrop, bool scripted = false );
	void init( const std::vector<MonsterRef*>& monsters, int cellRows, int cellColumns, bool isBoss, const std::string & backdrop, bool scripted = false );
	virtual bool IsDone() const;
	virtual void HandleButtonUp( const IApplication::Button& button );
	virtual void HandleButtonDown( const IApplication::Button& button );
	virtual void HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos );
	virtual void HandleKeyDown( const clan::InputEvent &key );
	virtual void HandleKeyUp( const clan::InputEvent &key );
	virtual void Draw( const clan::Rect &screenRect, clan::Canvas& GC );
	virtual bool LastToDraw() const; // Should we continue drawing more states?
	virtual bool DisableMappableObjects() const; // Should the app move the MOs?
	virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
	virtual void Start();
	virtual void SteelInit( SteelInterpreter* );
	virtual void SteelCleanup( SteelInterpreter * );
	virtual void Finish(); // Hook to clean up or whatever after being popped

	void SetDarkMode(int mode, float r, float g, float b, float a);
	void ClearDarkMode(int mode);
	void SetConfig( BattleConfig* config );
	bool playerWon() const;
	typedef int SpriteTicket;
	static const SpriteTicket UNDEFINED_SPRITE_TICKET;
private:

	void StartTargeting();
	void FinishTurn();
	// Go back to menu, they decided not to proceed with this option
	void CancelTargeting();
	void FinishTargeting();
	void TargetChanged();
	// These return true if one is selected, false if
	// there is nothing in that direction
	/*
	bool SelectTargetOnLeft();
	bool SelectTargetOnRight();
	bool SelectFromLeftGroup();
	bool SelectFromRightGroup();
	bool SelectLeftGroup();
	bool SelectRightGroup();
	void SelectUpTarget();
	void SelectDownTarget();
	*/
	bool MonstersOnLeft();
	enum eState {
		TRANSITION_IN,
		BATTLE_START,
		COMBAT,
		BATTLE_REWARDS,
		TRANSITION_OUT
	};

	enum eTransition {
		FLIP_ZOOM,
		FLIP_ZOOM_SPIN,
		FADE_IN
	};

	enum eCombatState {
		BEGIN_TURN,
		BATTLE_MENU,
		TARGETING,
		DISPLAY_ACTION,
		FINISHING_TURN,
		NEXT_TURN
	};

	enum eDisplayOrder {
		DISPLAY_ORDER_NO_DISPLAY=0,
		DISPLAY_ORDER_POSTBACKDROP=1,
		DISPLAY_ORDER_PREMONSTERS=2,
		DISPLAY_ORDER_POSTMONSTERS=4,
		DISPLAY_ORDER_PRE_PLAYERS=8,
		DISPLAY_ORDER_POST_PLAYERS=16,
		DISPLAY_ORDER_PRE_SPRITES=32,
		DISPLAY_ORDER_POST_SPRITES=64,
		DISPLAY_ORDER_PRE_DISPLAYS=128,
		DISPLAY_ORDER_POST_DISPLAYS=256,
		DISPLAY_ORDER_PRE_MENUS=512,
		DISPLAY_ORDER_POST_MENUS=1024,
		DISPLAY_ORDER_FINAL=2048
	};

	class Command {
	public:
		Command( IBattleAction *pAction, bool groupTarget );
		~Command();

		ICharacter *GetActor()const;
		ICharacter *GetTarget()const;
		bool GroupTarget()const;
		void Execute();
	private:
		ICharacter *m_pActor;
		ICharacter *m_pTarget;
		bool m_bGroupTarget;
		IBattleAction *m_pAction;

	};

	class BattleManager {
	public:
		BattleManager( BattleState& parent );
		~BattleManager();
		void StartPlayerTurn( Character* pC );
		void FinishedTargeting( Character* pC );
	private:
		BattleState & m_parent;

	};

	class Display {
	public:
		enum eDisplayType {
			DISPLAY_DAMAGE,
			DISPLAY_MP,
			DISPLAY_MISS,
			DISPLAY_CRITICAL
		};

		Display( BattleState& parent, eDisplayType type, int damage, ICharacter* pICharacter );
		~Display();

		void start();
		void update();
		void draw( clan::Canvas& GC );
		bool expired() const;

	private:
		BattleState& m_parent;
		ICharacter * m_pTarget;
		eDisplayType m_eDisplayType;
		int m_amount;
		clan::ubyte64 m_start_time;
		float m_complete;
	};

	class Sprite {
	public:
		Sprite( clan::Sprite sprite );
		~Sprite();

		int ZOrder() const;
		void SetPosition( const clan::Pointf& pos );
		clan::Pointf Position(bool with_offsets=false)const;
		void SetZOrder( int z );
		void Draw( clan::Canvas& gc );
		bool Enabled() const;
		void SetEnabled( bool enabled );
		void SetOffset( const clan::Pointf& pos);
		clan::Pointf GetOffset() const;
		clan::Rectf Rect(bool with_offsets=false)const;
		clan::Sprite GetSprite() const;
		void SetSprite(clan::Sprite sprite);
		bool operator<(const Sprite& other)const;
	private:
		clan::Sprite m_sprite;
		clan::Pointf m_pos;
		clan::Pointf m_offset;
		int m_zorder;
		bool m_enabled;
	};


	typedef void ( BattleState::* DrawMethod )( const clan::Rectf &, clan::Canvas& );
	//bool SortByBattlePos(const ICharacter* d1, const ICharacter* d2)const;

	void update_character_sprites();
	void draw_transition_in( const clan::Rectf &screenRect, clan::Canvas& GC );
	void draw_start( const clan::Rectf &screenRect, clan::Canvas& GC );
	void draw_battle( const clan::Rectf &screenRect, clan::Canvas& GC );
	void draw_shadows( const clan::Rectf &screenRect, clan::Canvas& GC );
	void draw_sprites( clan::Canvas& GC );
	void draw_monsters ( const clan::Rectf &monsterRect, clan::Canvas& GC);
	void draw_players ( const clan::Rectf &monsterRect, clan::Canvas& GC);
	void draw_status( const clan::Rectf &screenRect, clan::Canvas& GC );
	void draw_menus( const clan::Rectf &screenrect, clan::Canvas& GC );
	void draw_targets( const clan::Rectf &screenrect, clan::Canvas& GC );
	void draw_displays( clan::Canvas& GC );
	void draw_darkness( eDisplayOrder mode,  const clan::Rectf &screenRect, clan::Canvas& GC );
	void draw_status_effects( clan::Canvas& GC );

	clan::Sprite current_sprite( ICharacter *pCharacter ) const;

	// int is a handle

	SpriteTicket add_sprite( clan::Sprite sprite );
	void set_sprite_pos( SpriteTicket nSprite, const clan::Pointf& pos );
	clan::Sprite get_sprite( SpriteTicket nSprite );
	void remove_sprite( SpriteTicket nSprite );
	clan::Rectf get_sprite_rect( SpriteTicket nSprite );
	void set_offset(SpriteTicket sprite, const clan::Pointf& offset);
	clan::Pointf get_offset(SpriteTicket sprite);
	void set_offset(ICharacterGroup* pGroup,const clan::Pointf& offset);
	clan::Pointf get_offset(ICharacterGroup* pGroup);
	void init_or_release_players( bool bRelease = false );
	void set_positions_to_loci();
	void add_participant_sprites();
	void roll_initiative();
	void next_turn();
	void run_turn();
	void pick_next_character();
	void check_for_death();
	bool end_conditions();
	void win();
	void lose();
	void death_animation( Monster* pMonster );
	void move_character( ICharacter* character, clan::Pointf point );
	ICharacterGroup* group_for_character( ICharacter* );
	SpriteTicket get_sprite_for_char( ICharacter * )const;
	clan::Rectf  get_group_rect( ICharacterGroup* group )const;
	clan::Rectf  get_character_rect( const ICharacter* pCharacter )const;
	clan::Rectf get_character_locus_rect( const ICharacter* pCharacter )const;
	clan::Sizef get_character_size( const ICharacter* )const;
	clan::Pointf get_character_locus( const ICharacter* pCharacter )const;
	clan::Pointf get_monster_locus( const Monster* pMonster )const;
	clan::Pointf get_player_locus( uint n )const;
	ICharacter* get_next_character( const ICharacterGroup* pGroup, const ICharacter* pCharacter )const;
	ICharacter* get_prev_character( const ICharacterGroup* pGroup, const ICharacter* pCharacter )const;


	/* Battle BIFs */
	SteelType selectTargets( bool single, bool group, bool defaultMonsters );
	SteelType finishTurn();
	// if they back out and want to go back to the battle menu
	SteelType cancelOption();
	SteelType animation(SteelType::Functor functor);
	SteelType doTargetedAnimation( SteelType::Handle pIActor,  SteelType::Handle pITarget, int hand, SteelType::Handle hAnim );
	SteelType doCharacterAnimation( SteelType::Handle pIActor, SteelType::Handle hAnim );
	SteelType createDisplay( int damage, SteelType::Handle pICharacter, int display_type );
	// returns monster group or party as array of character handles
	SteelType getCharacterGroup( bool monsters );
	SteelType getAllCharacters();

	SteelType getMonsterDamageCategory( SteelType::Handle hMonster );


	SteelType flee();
	SteelType isBossBattle();
	SteelType darkMode( int order, double r, double g, double b, double a );
	SteelType clearDarkMode(int order);



	eState m_eState;
	DrawMethod m_draw_method;
	MonsterParty* m_monsters;
	clan::Image m_backdrop;
	clan::Rectf m_monster_rect;
	clan::Rectf m_player_rect;
	clan::Rectf m_bp_box;
	clan::Rect m_status_text_rect;
	clan::Pointf m_status_effect_spacing;
	clan::Gradient m_bp_gradient;
	std::map<int,clan::Colorf> m_darkModes;
	clan::Colorf m_status_effect_shadow_color;
	clan::Colorf m_bp_border;
	clan::ubyte64 m_startup_time;
	Font m_mpFont;
	Font m_bpFont;
	Font m_hpFont;
	Font m_generalFont;
	Font m_charNameFont;
	eTransition m_transition;
	eCombatState m_combat_state;
	BattleMenuStack m_menu_stack;
	TargetingState m_targeting_state;
	struct Targets {
		union {
			ICharacter * m_pTarget;
			ICharacterGroup * m_pGroup;
		}selected;
		bool m_bSelectedGroup;
	}m_targets;
	bool m_bDone;
	bool m_bDoneAfterRound;
	clan::Rectf m_statusRect;
	clan::Rectf m_popupRect;
	uint m_nRows;
	uint m_nColumns;
	std::deque<ICharacter*> m_initiative;
	uint m_nRound;
	uint m_cur_char;
	bool m_bBossBattle;
	bool m_bPlayerWon;
	bool m_bScriptedBattle;
	std::list<Display> m_displays;
	std::vector<Sprite> m_sprites;
	std::map<ICharacterGroup*,clan::Pointf> m_group_offsets;
	clan::Mutex m_sprite_mutex;
	uint64_t m_last_render_time;
	BattleConfig * m_config;
#ifndef NDEBUG
	bool m_bShowSpriteRects;
#endif

	friend class BattleState::Display;
	friend class TargetingState;
	friend class BattleManager;
	friend class AnimationState;
};

}
	#endif
