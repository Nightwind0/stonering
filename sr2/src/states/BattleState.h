#ifndef SR2_BATTLE_STATE_H
#define SR2_BATTLE_STATE_H

#include "State.h"
#include "sr_defines.h"
#include "MonsterGroup.h"
#include <ClanLib/core.h>

namespace StoneRing{

    class MonsterRef;
    class Monster;

    class BattleState : public State
    {
    public:
        void init(const MonsterGroup &, const std::string &backdrop);
        virtual bool isDone() const;
        virtual void handleKeyDown(const CL_InputEvent &key);
        virtual void handleKeyUp(const CL_InputEvent &key);
        virtual void draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
        virtual bool lastToDraw() const; // Should we continue drawing more states?
        virtual bool disableMappableObjects() const; // Should the app move the MOs? 
        virtual void mappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void start(); 
        virtual void finish(); // Hook to clean up or whatever after being popped
    private:

        enum eState
        {
            TRANSITION_IN,
            BATTLE_START,
            BATTLE,
            BATTLE_REWARDS,
            TRANSITION_OUT
        };

        typedef void (BattleState::* DrawMethod)(const CL_Rect &, CL_GraphicContext *);

        void drawTransitionIn(const CL_Rect &screenRect, CL_GraphicContext *pGC);
        void drawStart(const CL_Rect &screenRect, CL_GraphicContext *pGC);
        void drawBattle(const CL_Rect &screenRect, CL_GraphicContext *pGC);
        void _drawMonsters(const CL_Rect &monsterRect, CL_GraphicContext *pGC);

        eState meState; 
        DrawMethod mDrawMethod;
        std::vector<Monster*> mMonsters;
        CL_Surface *mpBackdrop;
        bool mbDone;
        uint mnRows;
        uint mnColumns;
    };

};
#endif