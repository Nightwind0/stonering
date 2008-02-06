#include "BattleMenuOption.h"
#include "BattleMenu.h"

using StoneRing::BattleMenuOption;

BattleMenuOption::BattleMenuOption(int level):mnLevel(level),
mpConditionScript(NULL),
mActionType(INVALID)
{
    mAction.mpScript = NULL;
}

BattleMenuOption::~BattleMenuOption()
{
    delete mpConditionScript;

    switch(mActionType)
    {
    case SKILLREF:
        delete mAction.mpSkillRef;
        break;
    case SCRIPT:
        delete mAction.mpScript;
        break;
    case SUBMENU:
        delete mAction.mpSubMenu;
        break;
    }
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


bool BattleMenuOption::handleElement(Element::eElement element, Element *pElement)
{
    switch(element)
    {
    case ECONDITIONSCRIPT:
        mpConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONSELECT:
        mActionType = SCRIPT;
        mAction.mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ESKILLREF:
        mActionType = SKILLREF;
        mAction.mpSkillRef = dynamic_cast<SkillRef*>(pElement);
        break;
    case EBATTLEMENU:
        mActionType = SUBMENU;
        mAction.mpSubMenu = dynamic_cast<BattleMenu*>(pElement);
        break;
    default:
        return false;
    };

    return true;
}

void BattleMenuOption::loadFinished()
{
    if(mActionType == INVALID)
        throw CL_Error("Battle menu needs a skill ref or a script or submenu");
}

void BattleMenuOption::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);
}

