#ifndef SR2_BATTLE_STATE_H
#define SR2_BATTLE_STATE_H

#include "State.h"
#include "sr_defines.h"
#include "MonsterGroup.h"
#include <ClanLib/core.h>

namespace StoneRing{

    class MonsterRef;
    class Monster;
    class IBattleAction;
	class ICharacter;

    class BattleState : public State
    {
    public:
        void init(const MonsterGroup &, const std::string &backdrop);
        virtual bool IsDone() const;
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const; // Should the app move the MOs? 
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start(); 
        virtual void RegisterSteelFunctions(SteelInterpreter*);
        virtual void Finish(); // Hook to clean up or whatever after being popped
    private:

        enum eState
        {
            TRANSITION_IN,
            BATTLE_START,
            BATTLE,
            BATTLE_REWARDS,
            TRANSITION_OUT
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

        void init_or_release_players(bool bRelease=false);


        eState m_eState; 
        DrawMethod m_draw_method;
        std::vector<Monster*> m_monsters;
        CL_Surface *m_pBackdrop;
        CL_Surface *m_pStatusBar;
        CL_Font *m_pStatusGeneralFont;
        CL_Font *m_pStatusHPFont;
        CL_Font *m_pStatusMPFont;
        CL_Font *m_pStatusBPFont;
        CL_Font *m_pStatusBadFont;
        CL_Rect m_status_rect;
        bool m_bDone;
        uint m_nStatusBarX;
        uint m_nStatusBarY;
        uint m_nRows;
        uint m_nColumns;
    };

};
#endif