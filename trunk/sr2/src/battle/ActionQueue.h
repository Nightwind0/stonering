#ifndef SR_ACTION_QUEUE_H
#define SR_ACTION_QUEUE_H

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


  class ActionQueue 
    {
    public:
        ActionQueue();
        ~ActionQueue();

        void executeFront();
        void popFront();
        void enqueue(ScriptElement *pScript, ICharacter *pActor, 
            ICharacterGroup *pActorGroup, const SteelType &var);
        void remove (ICharacter *pActor, ScriptElement *pDeselect);
    private:
        class ActionEntry
        {
        public:
            ActionEntry(ScriptElement *pScript, ICharacter *pChar, 
                ICharacterGroup *pParty, const SteelType &var);
            ~ActionEntry();
            void execute();
            void deselect(ScriptElement *pDeselect);
            bool matches(ICharacter *pChar) const;
        private:
            SteelType mVar;
            ScriptElement *mpScript;
            ICharacter *mpActor;
            ICharacterGroup *mpGroup;
        };

        std::list<ActionEntry> mQueue;

    };
}

#endif