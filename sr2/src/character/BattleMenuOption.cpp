#include "BattleMenuOption.h"
#include "BattleMenu.h"

using StoneRing::BattleMenuOption;

BattleMenuOption::BattleMenuOption(int level):m_nLevel(level),
m_pConditionScript(NULL),
m_action_type(INVALID)
{
    m_action.m_pScript = NULL;
}

BattleMenuOption::~BattleMenuOption()
{
    delete m_pConditionScript;

    switch(m_action_type)
    {
    case SKILLREF:
        delete m_action.m_pSkillRef;
        break;
    case SCRIPT:
        delete m_action.m_pScript;
        break;
    case SUBMENU:
        delete m_action.m_pSubMenu;
        break;
    }
}

std::string BattleMenuOption::GetName() const
{
    return m_name;
}
bool BattleMenuOption::Enabled() const
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition();
    else return true; 
}


bool BattleMenuOption::handle_element(Element::eElement element, Element *pElement)
{
    switch(element)
    {
    case ECONDITIONSCRIPT:
        m_pConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONSELECT:
        m_action_type = SCRIPT;
        m_action.m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ESKILLREF:
        m_action_type = SKILLREF;
        m_action.m_pSkillRef = dynamic_cast<SkillRef*>(pElement);
        break;
    case EBATTLEMENU:
        m_action_type = SUBMENU;
        m_action.m_pSubMenu = dynamic_cast<BattleMenu*>(pElement);
        break;
    default:
        return false;
    };

    return true;
}

void BattleMenuOption::load_finished()
{
    if(m_action_type == INVALID)
        throw CL_Error("Battle menu needs a skill ref or a script or submenu");
}

void BattleMenuOption::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name",pAttributes);
}

