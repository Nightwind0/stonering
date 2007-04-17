#include "BattleMenuOption.h"

using StoneRing::BattleMenuOption;

BattleMenuOption::BattleMenuOption():mpConditionScript(NULL),mpScript(NULL)
{
}

BattleMenuOption::~BattleMenuOption()
{
    delete mpConditionScript;
    delete mpScript;
}

std::string BattleMenuOption::getName() const
{
    return mName;
}
bool BattleMenuOption::enabled() const
{
    if(mpConditionScript)
        return mpConditionScript->evaluateCondition();
    else return true; 
}
void BattleMenuOption::invoke()
{
    if(mpScript)
        mpScript->executeScript();
}

bool BattleMenuOption::handleElement(Element::eElement element, Element *pElement)
{
    switch(element)
    {
    case ECONDITIONSCRIPT:
        mpConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ESCRIPT:
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    default:
        return false;
    };

    return true;
}
void BattleMenuOption::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);
}

