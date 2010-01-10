#include "BattleMenuOption.h"
#include "BattleMenu.h"
#include "GraphicsManager.h"

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
    default:
        break;
    }
}

std::string BattleMenuOption::GetName() const
{
    return m_name;
}
bool BattleMenuOption::Enabled(const ParameterList &params) const
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition(params);
    else return true;
    // TODO: Check for BP/MP requirements on skills here??
}

void BattleMenuOption::Select(StoneRing::BattleMenuStack& stack, const ParameterList& params){
    switch(m_action_type){
        case SUBMENU:
            stack.push(m_action.m_pSubMenu);
            break;
        case SKILLREF:
            {
                Skill * pSkill =  m_action.m_pSkillRef->GetSkill();
                pSkill->Select(params);
                break;
            }
        case SCRIPT:
            m_action.m_pScript->ExecuteScript(params);
            break;
        default:
            break;
    }
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
    m_pIcon = GraphicsManager::GetInstance()->GetIcon( get_implied_string("icon",pAttributes,"no_icon") );
}

