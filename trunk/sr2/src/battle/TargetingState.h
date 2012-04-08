#ifndef SR2_TARGETING_STATE_H
#define SR2_TARGETING_STATE_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "State.h"
#include "ICharacter.h"
#include "Party.h"

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
      bool SelectRightGroup();
      bool SelectLeftGroup();
      bool SelectFromRightGroup();
      bool SelectFromLeftGroup();
      bool SelectTargetOnLeft();
      bool SelectTargetOnRight();
      void SelectDownTarget();
      void SelectUpTarget();
      bool may_target_single()const;
      bool may_target_group()const;
      bool can_target(ICharacter*pTarget)const;

      ICharacterGroup * get_left_group() const;
      ICharacterGroup * get_right_group() const;

      void ChangeState(State newState);
      BattleState *m_pParent;
      Party * m_pParty;
      State m_state;
      Targetable m_targetable;
      bool m_bDefaultMonsters;
      bool m_bDone;
      CL_Sprite  m_target_sprite;
    };

    inline bool TargetingState::may_target_single() const{
        return m_targetable & SINGLE;
    }
    inline bool TargetingState::may_target_group() const{
        return m_targetable & GROUP;
    }

}
#endif
