#ifndef SR2_TARGETING_STATE_H
#define SR2_TARGETING_STATE_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>

class SteelInterpreter;

namespace StoneRing
{
	class BattleState;

    class TargetingState
    {
    public:
		enum Targetable{
			SINGLE,
			GROUP,
			EITHER
		};
		virtual void Init(BattleState *pParent, Targetable targetable, bool bDefaultMonsters=true);
        virtual bool IsDone() const;
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const; // Should the app move the MOs? 
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start(); 
        virtual void SteelInit      (SteelInterpreter *);
        virtual void SteelCleanup   (SteelInterpreter *);
        virtual void Finish(); // Hook to clean up or whatever after being popped

private:
	enum State
	{
		SElECT_MONSTER,
		SELECT_PLAYER,
		SELECT_ALL_MONSTERS,
		SELECT_ALL_PLAYERS
	};
	BattleState *m_pParent;
	State m_state;
	Targetable m_targetable;	
	bool m_bDefaultMonsters;
	bool m_bDone;

};

};
#endif