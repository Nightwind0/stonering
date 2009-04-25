#ifndef SR_ACTION_QUEUE_H
#define SR_ACTION_QUEUE_H

#include <string>
#ifdef _WIN32
#include "SteelType.h"
#include "SteelInterpreter.h"
#else
#include <steel/SteelType.h>
#include <steel/SteelInterpreter.h>
#endif


namespace StoneRing{

class ScriptElement;
class ICharacter;
class ICharacterGroup;
class NamedScript;
class IBattleAction;

  class ActionQueue 
    {
    public:
        ActionQueue();
        ~ActionQueue();

        void ExecuteFront();
        void PopFront();
        void Enqueue(IBattleAction *pAction, ICharacter *pActor, ICharacter *pTarget, bool group,const SteelType &var);
        void RemoveActionForCharacter (ICharacter *pActor);
    private:
        struct Action
        {
            IBattleAction *pAction;
            ICharacter *pActor;
            ICharacter *pTarget;
            bool bGroupTarget;
        };
        std::deque<Action> m_Queue;

    };
}

#endif

