
#include "Element.h"
#include <ClanLib/core.h>
#include "Skill.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"
#include "Level.h"
#include "SpellRef.h"
#include "ActionQueue.h"

using namespace StoneRing;

StoneRing::Skill::eType 
StoneRing::Skill::typeFromString(const std::string type)
{
    if(type == "battle") return BATTLE;
    else if(type == "switch") return SWITCH;
    else throw CL_Error("Bad type on skill = " + type);
}

void StoneRing::Skill::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
    mnSp = getRequiredInt("sp",pAttributes);
    mnBp = getRequiredInt("bp",pAttributes);
    mnMinLevel = getImpliedInt("minLevel",pAttributes,0);
    meType = typeFromString(getImpliedString("type",pAttributes,"battle"));
    mbAllowsGroupTarget = getImpliedBool("allowsGroupTarget",pAttributes,false);
    mbDefaultToEnemyGroup = getImpliedBool("defaultToEnemyGroup", pAttributes,true);
}

void StoneRing::Skill::select(StoneRing::ActionQueue *pQueue)
{
    if(mpOnSelect)
    {
        // Put pQueue in scope
        ParameterList params;
        params.push_back ( ParameterListItem("$_ActionQueue",pQueue) );
        params.push_back ( ParameterListItem("$_ActionScript", mpOnInvoke ) );

        mpOnSelect->executeScript(params);
    }
    else
    {
        // Default implementation
        /*
        //selectTarget(true,true,true); 
        if(mbGroup && groupSelection(mbDefaultToEnemies))
        {
           ICharacterGroup *pGroup = selectGroup();
           pQueue->enqueueGroup(mpScript,pChar,pGroup,0);
        }
        else
        {
            ICharacter *pTarget = selectTarget(true,true,true);
            pQueue->enqueueAction(mpScript,pChar,pTarget,0);
        }
        */
    }
}

// If you cancel an option, it should be able to clean itself up
// (especially removing entries from the queue)
void StoneRing::Skill::deselect(StoneRing::ActionQueue *pQueue)
{
    ICharacter *pCharacter = 
        IApplication::getInstance()->getActorCharacterGroup()->getActorCharacter();
    pQueue->remove(pCharacter, mpOnDeselect);
}

bool StoneRing::Skill::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EONSELECT:
        mpOnSelect = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONDESELECT:
        mpOnDeselect = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONINVOKE:
        mpOnInvoke = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONREMOVE:
        mpOnRemove = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ECONDITIONSCRIPT:
        mpCondition = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EPREREQSKILLREF:
        mPreReqs.push_back(dynamic_cast<SkillRef*>(pElement));
        break;
    default:
        return false;
    }

    return true;
}

StoneRing::Skill::Skill():mnBp(0), mnSp(0),mnMinLevel(0),
mpOnInvoke(NULL),mpOnRemove(NULL),mpCondition(NULL),mpOnSelect(NULL),mpOnDeselect(NULL),
mbAllowsGroupTarget(FALSE),mbDefaultToEnemyGroup(TRUE)
{

}

Skill::~Skill()
{
    delete mpCondition;
    delete mpOnRemove;
    delete mpOnInvoke;
    delete mpOnSelect;
    delete mpOnDeselect;
}


std::string Skill::getName() const
{
    return mName;
}

uint Skill::getSPCost() const
{
    return mnSp;
}

uint Skill::getBPCost() const
{
    return mnBp;
}

uint Skill::getMinLevel() const
{
    return mnMinLevel;
}


        
std::list<SkillRef*>::const_iterator 
Skill::getPreReqsBegin() const
{
    return mPreReqs.begin();
}

std::list<SkillRef*>::const_iterator 
Skill::getPreReqsEnd() const
{
    return mPreReqs.end();
}








