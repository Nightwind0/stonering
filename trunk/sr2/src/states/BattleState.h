#ifndef SR2_BATTLE_STATE_H
#define SR2_BATTLE_STATE_H

#include "State.h"
#include "sr_defines.h"
#include "MonsterGroup.h"
#include <ClanLib/core.h>
#include <deque>
#include <stack>
#include "BattleMenuOption.h"
#include "TargetingState.h"
#include "BattleConfig.h"

namespace StoneRing{

    class MonsterRef;
    class Monster;
    class MonsterParty;
    class IBattleAction;
    class ICharacter;
    class AnimationState;


    class BattleState : public State
    {
    public:
        void init(const MonsterGroup& monsters, const std::string &backdrop);
	void init(const std::vector<MonsterRef*>& monsters, int cellRows, int cellColumns, bool isBoss, const std::string & backdrop);
        virtual bool IsDone() const;
	virtual void HandleButtonUp(const IApplication::Button& button);
	virtual void HandleButtonDown(const IApplication::Button& button);
	virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void SteelInit(SteelInterpreter*);
        virtual void SteelCleanup   (SteelInterpreter *);
        virtual void Finish(); // Hook to clean up or whatever after being popped
        
	void SetConfig(BattleConfig* config);
	typedef int SpriteTicket;
	static const SpriteTicket UNDEFINED_SPRITE_TICKET;
    private:

	void StartTargeting();
	void FinishTurn();
	// Go back to menu, they decided not to proceed with this option
	void CancelTargeting();
	void FinishTargeting();
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
        enum eState
        {
            TRANSITION_IN,
            BATTLE_START,
            COMBAT,
            BATTLE_REWARDS,
            TRANSITION_OUT
        };

        enum eCombatState
        {
            BATTLE_MENU,
            TARGETING,
            DISPLAY_ACTION
        };
	
	enum eDisplayOrder
	{
	    DISPLAY_ORDER_NO_DISPLAY,
	    DISPLAY_ORDER_POSTBACKDROP,
	    DISPLAY_ORDER_PREMONSTERS,
	    DISPLAY_ORDER_POSTMONSTERS,
	    DISPLAY_ORDER_PRE_PLAYERS,
	    DISPLAY_ORDER_POST_PLAYERS,
	    DISPLAY_ORDER_PRE_SPRITES,
	    DISPLAY_ORDER_POST_SPRITES,
	    DISPLAY_ORDER_PRE_DISPLAYS,
	    DISPLAY_ORDER_POST_DISPLAYS,
	    DISPLAY_ORDER_PRE_MENUS,
	    DISPLAY_ORDER_POST_MENUS,
	    DISPLAY_ORDER_FINAL
	};

        class Command
        {
        public:
            Command(IBattleAction *pAction, bool groupTarget);
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

        class BattleManager
        {
        public:
             BattleManager(BattleState& parent);
            ~BattleManager();
            void StartPlayerTurn(Character* pC);
            void FinishedTargeting(Character* pC);
        private:
            BattleState & m_parent;

        };

        class Display
        {
            public:
            enum eDisplayType{
                DISPLAY_DAMAGE,
                DISPLAY_MP,
                DISPLAY_MISS
            };

            Display(BattleState& parent,eDisplayType type,int damage,ICharacter* pICharacter);
            ~Display();

            void start();
            void update();
            void draw(CL_GraphicContext& GC);
            bool expired() const;

            private:
            BattleState& m_parent;
            ICharacter * m_pTarget;
            eDisplayType m_eDisplayType;
            int m_amount;
            uint m_start_time;
            float m_complete;
        };
	
	class Sprite
	{
	public:
	    Sprite(CL_Sprite sprite);
	    ~Sprite();
	    
	    int ZOrder() const;
	    void SetPosition(const CL_Pointf& pos);
	    CL_Pointf Position()const;
	    void SetZOrder(int z);
	    void Draw(CL_GraphicContext& gc);
	    bool Enabled() const;
	    void SetEnabled(bool enabled);
	    CL_Sprite GetSprite() const;
	private:
	    CL_Sprite m_sprite;
	    CL_Pointf m_pos;
	    int m_zorder;
	    bool m_enabled;
	};


        typedef void (BattleState::* DrawMethod)(const CL_Rectf &, CL_GraphicContext&);
	//bool SortByBattlePos(const ICharacter* d1, const ICharacter* d2)const;

        void draw_transition_in(const CL_Rectf &screenRect, CL_GraphicContext& GC);
        void draw_start(const CL_Rectf &screenRect, CL_GraphicContext& GC);
        void draw_battle(const CL_Rectf &screenRect, CL_GraphicContext& GC);
        void draw_monsters(const CL_Rectf &monsterRect, CL_GraphicContext& GC);
        void draw_players(const CL_Rectf &playerRect, CL_GraphicContext& GC);
        void draw_status(const CL_Rectf &screenRect, CL_GraphicContext& GC);
        void draw_menus(const CL_Rectf &screenrect, CL_GraphicContext& GC);
        void draw_targets(const CL_Rectf &screenrect, CL_GraphicContext& GC);
        void draw_displays(CL_GraphicContext& GC);
	void draw_sprites(int z, CL_GraphicContext& GC);
	void draw_darkness(const CL_Rectf &screenRect, CL_GraphicContext& GC);
        void draw_status_effects(CL_GraphicContext& GC);
	
	// int is a handle

	SpriteTicket add_sprite(CL_Sprite sprite);
	void set_sprite_pos(SpriteTicket nSprite, CL_Pointf pos);
	CL_Sprite get_sprite(SpriteTicket nSprite) const;
	void remove_sprite(SpriteTicket nSprite);
        
        void init_or_release_players(bool bRelease=false);
        void set_positions_to_loci();
        void roll_initiative();
        void next_turn();
        void pick_next_character();
        void check_for_death();
	bool end_conditions();
	void win();
	void lose();
        void death_animation(Monster* pMonster);
	void move_character (ICharacter* character, CL_Pointf point);
	ICharacterGroup* group_for_character(ICharacter*);
        CL_Rectf  get_group_rect(ICharacterGroup* group)const;
        CL_Rectf  get_character_rect (const ICharacter* pCharacter)const;
        CL_Rectf get_character_locus_rect (const ICharacter* pCharacter)const;
        CL_Sizef get_character_size(const ICharacter*)const;
        CL_Pointf get_character_locus(const ICharacter* pCharacter)const;
        CL_Pointf get_monster_locus(const Monster* pMonster)const;
        CL_Pointf get_player_locus(uint n)const;
        ICharacter* get_next_character(const ICharacterGroup* pGroup, const ICharacter* pCharacter)const;
        ICharacter* get_prev_character(const ICharacterGroup* pGroup, const ICharacter* pCharacter)const;


        /* Battle BIFs */
        SteelType selectTargets(bool single, bool group, bool defaultMonsters);
        SteelType finishTurn();
        // if they back out and want to go back to the battle menu
        SteelType cancelOption();
        SteelType doTargetedAnimation(SteelType::Handle pIActor,  SteelType::Handle pITarget, SteelType::Handle hAnim);
	SteelType doCharacterAnimation(SteelType::Handle pIActor, SteelType::Handle hAnim);
        SteelType createDisplay(int damage,SteelType::Handle pICharacter,int display_type);
        // returns monster group or party as array of character handles
        SteelType getCharacterGroup(bool monsters);
        SteelType getAllCharacters();

        SteelType getMonsterDamageCategory(SteelType::Handle hMonster);

        SteelType doSkill(SteelType::Handle pICharacter,const std::string& whichskill);

	SteelType flee();	
	SteelType isBossBattle();
	SteelType darkMode(int order);
	SteelType darkColor(double r, double g, double b, double a);



        eState m_eState;
        DrawMethod m_draw_method;
        MonsterParty* m_monsters;
        CL_Image m_backdrop;
        CL_Image m_statusBar;
        CL_Image m_battleMenu;
        CL_Image m_battlePopup;
        CL_Rectf m_status_rect;
        CL_Rectf m_popup_rect;
        CL_Rectf m_monster_rect;
        CL_Rectf m_player_rect;
        CL_Pointf m_status_effect_spacing;
	CL_Colorf m_darkColor;
        CL_Colorf m_status_effect_shadow_color;
	uint m_ndarkMode;

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
        uint m_nStatusBarX;
        uint m_nStatusBarY;
        uint m_nPopupX;
        uint m_nPopupY;
        uint m_nRows;
        uint m_nColumns;
        std::deque<ICharacter*> m_initiative;
        uint m_nRound;
        uint m_cur_char;
	bool m_bBossBattle;
        std::list<Display> m_displays;
	std::vector<Sprite> m_sprites;
	BattleConfig * m_config;
	

        friend class BattleState::Display;
        friend class TargetingState;
        friend class BattleManager;
        friend class AnimationState;
    };

}
#endif
