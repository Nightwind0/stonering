#ifndef SR2_TARGETING_STATE_H
#define SR2_TARGETING_STATE_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "State.h"

class SteelInterpreter;

namespace StoneRing
{
  class BattleState;

    class TargetingState : public State
    {
    public:
      enum Targetable{
        SINGLE=1,
        GROUP=2,
        EITHER=(SINGLE|GROUP)
      };
      virtual void Init(BattleState *pParent, Targetable targetable, bool bDefaultMonsters=true);
      virtual bool IsDone() const;
      virtual void HandleKeyDown(const CL_InputEvent &key);
      virtual void HandleKeyUp(const CL_InputEvent &key);
      virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
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
        SELECT_SINGLE_LEFT,
        SELECT_SINGLE_RIGHT,
        SELECT_LEFT_GROUP,
        SELECT_RIGHT_GROUP
      };
      void ChangeState(State newState);
      BattleState *m_pParent;
      State m_state;
      Targetable m_targetable;
      bool m_bDefaultMonsters;
      bool m_bDone;

    };

}
#endif
