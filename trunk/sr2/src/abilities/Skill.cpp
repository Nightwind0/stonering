
#include "Element.h"
#include <ClanLib/core.h>
#include "Skill.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"
#include "Level.h"
#include "SpellRef.h"

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
}

bool StoneRing::Skill::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESPELLREF:
        mpSpellRef = dynamic_cast<SpellRef*>(pElement);
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

StoneRing::Skill::Skill():mnBp(0), mnSp(0),mnMinLevel(0),mpSpellRef(NULL),
mpOnInvoke(NULL),mpOnRemove(NULL),mpCondition(NULL)
{

}

Skill::~Skill()
{
    delete mpCondition;
    delete mpOnRemove;
    delete mpOnInvoke;
    delete mpSpellRef;
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








