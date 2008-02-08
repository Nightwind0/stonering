#include "ActionQueue.h"
#include "NamedScript.h"

using StoneRing::ActionQueue;

ActionQueue::ActionQueue()
{
}

ActionQueue::~ActionQueue()
{
}


void ActionQueue::executeFront()
{
    ActionEntry entry = mQueue.front();
    entry.execute();
}

void ActionQueue::popFront()
{
    mQueue.pop_front();
}

void ActionQueue::enqueue(StoneRing::ScriptElement *pScript, StoneRing::ICharacter *pTarget,
                          StoneRing::ICharacterGroup *pActorGroup, const SteelType &var)
{
    mQueue.push_back( ActionEntry(pScript,pTarget,pActorGroup,var) );
}


void ActionQueue::remove (StoneRing::ICharacter *pActor, StoneRing::NamedScript *pDeselect)
{

    std::list<ActionEntry>::iterator it = std::find_if(mQueue.begin(),mQueue.end(), 
        std::bind2nd(std::mem_fun_ref(&ActionEntry::matches),pActor));

    it->deselect(pDeselect);

    mQueue.erase(it);

}


ActionQueue::ActionEntry::ActionEntry(StoneRing::ScriptElement *pScript, StoneRing::ICharacter *pChar, 
                                      StoneRing::ICharacterGroup *pParty, const SteelType &var)
                                      :mpScript(pScript),mpActor(pChar),mpGroup(pParty),mVar(var)
{
}
ActionQueue::ActionEntry::~ActionEntry()
{
}

void ActionQueue::ActionEntry::deselect(StoneRing::NamedScript *pDeselect)
{
    if(pDeselect)
    {
        ParameterList params;
        params.push_back( ParameterListItem("$_Actor",mpActor) );
        params.push_back( ParameterListItem("$_ActorGroup",mpGroup) );
        params.push_back( ParameterListItem("$_VAR",mVar) );

        pDeselect->executeScript(params);
    }
}

void ActionQueue::ActionEntry::execute()
{
    if(mpScript)
    {
        ParameterList params;
        params.push_back( ParameterListItem("$_Actor",mpActor) );
        params.push_back( ParameterListItem("$_ActorGroup",mpGroup) );
        params.push_back( ParameterListItem("$_VAR",mVar) );

        mpScript->executeScript();
    }
}

bool ActionQueue::ActionEntry::matches(StoneRing::ICharacter *pChar) const
{
    return mpActor == pChar;
}