#include "BattleMenuOption.h"
#include "BattleMenu.h"
#include "GraphicsManager.h"
#include "Character.h"

using StoneRing::BattleMenuOption;
using StoneRing::Character;

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
bool BattleMenuOption::Enabled(const ParameterList &params, Character * pCharacter) const
{
    if(m_action_type == SKILLREF)
    {
        Skill * pSkill = m_action.m_pSkillRef->GetSkill();
        if(pSkill->GetBPCost() * pCharacter->GetAttribute(ICharacter::CA_BP_COST) > pCharacter->GetAttribute(ICharacter::CA_BP))
        {
            return false;
        }
        
        if(pSkill->GetMPCost() * pCharacter->GetAttribute(ICharacter::CA_MP_COST) > pCharacter->GetAttribute(ICharacter::CA_MP))
        {
            return false;
        }
    }
    else if(m_action_type == SUBMENU)
	{
		// Be enabled iff submenu options are available
		m_action.m_pSubMenu->SetEnableConditionParams(params,pCharacter);
		return m_action.m_pSubMenu->HasEnabledOptions();
	}
    
    
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition(params);
    else return true;
}

void BattleMenuOption::Select(StoneRing::BattleMenuStack& stack, const ParameterList& params, Character * pCharacter){
    switch(m_action_type){
        case SUBMENU:
            m_action.m_pSubMenu->SetEnableConditionParams(params,pCharacter);
            m_action.m_pSubMenu->Init();
            stack.push(m_action.m_pSubMenu);
            break;
        case SKILLREF:
            {
                Skill * pSkill =  m_action.m_pSkillRef->GetSkill();
                if(pSkill->Invoke(pCharacter,params)){
		  
		}
                break;
            }
        case SCRIPT:
            m_action.m_pScript->ExecuteScript(params);
            break;
        default:
            break;
    }
}


bool BattleMenuOption::Visible ( StoneRing::Character* pCharacter )
{
    if(m_action_type == SKILLREF)
    {
        assert(m_action.m_pSkillRef);
        if(!pCharacter->HasSkill(m_action.m_pSkillRef->GetRef()))
            return false;
    }
    
    return true;
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
        if(m_icon.is_null())
            m_icon = m_action.m_pSkillRef->GetSkill()->GetIcon();
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
    if(m_icon.is_null()) 
        throw XMLException("Battle menu option needs either an icon or a skill ref");
    if(m_action_type == INVALID)
        throw XMLException("Battle menu needs a skill ref or a script or submenu");
}

void BattleMenuOption::load_attributes(clan::DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    if(has_attribute("icon",attributes))
        m_icon = GraphicsManager::GetIcon( get_required_string("icon",attributes) );
}

int BattleMenuOption::GetBPCost() const
{
    if(m_action_type == SKILLREF)
    {
        return m_action.m_pSkillRef->GetSkill()->GetBPCost();
    }
    
    return 0;
}

int BattleMenuOption::GetMPCost() const
{
    if(m_action_type == SKILLREF)
    {
        return m_action.m_pSkillRef->GetSkill()->GetMPCost();
    }
    
    return 0;
}

