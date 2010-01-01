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

namespace StoneRing{

    class MonsterRef;
    class Monster;
    class MonsterParty;
    class IBattleAction;
    class ICharacter;

    class BattleState : public State
    {
    public:
        void init(const MonsterGroup& monsters, const std::string &backdrop);
        virtual bool IsDone() const;
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void SteelInit(SteelInterpreter*);
        virtual void SteelCleanup   (SteelInterpreter *);
        virtual void Finish(); // Hook to clean up or whatever after being popped


    private:
	friend class TargetingState;
	void StartTargeting();
	void FinishTargeting();
	void SelectNextTarget();
	void SelectPreviousTarget();
	void SelectFromLeftGroup();
	void SelectFromRightGroup();
	void SelectLeftGroup();
	void SelectRightGroup();
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

        typedef void (BattleState::* DrawMethod)(const CL_Rect &, CL_GraphicContext *);

        void draw_transition_in(const CL_Rect &screenRect, CL_GraphicContext *pGC);
        void draw_start(const CL_Rect &screenRect, CL_GraphicContext *pGC);
        void draw_battle(const CL_Rect &screenRect, CL_GraphicContext *pGC);
        void draw_monsters(const CL_Rect &monsterRect, CL_GraphicContext *pGC);
        void draw_players(const CL_Rect &playerRect, CL_GraphicContext *pGC);
        void draw_status(const CL_Rect &screenRect, CL_GraphicContext *pGC);
        void draw_menus(const CL_Rect &screenrect, CL_GraphicContext *pGC);
        void draw_targets(const CL_Rect &screenrect, CL_GraphicContext *pGC);

        void init_or_release_players(bool bRelease=false);
        void roll_initiative();
        void next_turn();
        ICharacter* get_next_character(const ICharacterGroup* pGroup, const ICharacter* pCharacter)const;
        ICharacter* get_prev_character(const ICharacterGroup* pGroup, const ICharacter* pCharacter)const;


        /* Battle BIFs */
        SteelType selectTargets(bool single, bool group, bool defaultMonsters);





        eState m_eState;
        DrawMethod m_draw_method;
        MonsterParty* m_monsters;
        CL_Surface *m_pBackdrop;
        CL_Surface *m_pStatusBar;
        CL_Surface *m_pBattleMenu;
        CL_Surface *m_pBattlePopup;
        CL_Sprite  *m_target_sprite;
        CL_Rect m_status_rect;

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
        uint m_nStatusBarX;
        uint m_nStatusBarY;
        uint m_nRows;
        uint m_nColumns;
        std::deque<ICharacter*> m_initiative;
        uint m_nRound;
        uint m_cur_char;
    };

}
#endif
