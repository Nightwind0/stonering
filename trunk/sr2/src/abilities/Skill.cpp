
#include "Element.h"
#include <ClanLib/display.h>
#include "Skill.h"
#include "IApplication.h"
#include "Animation.h"
#include "Level.h"
#include "SpellRef.h"
#include "ActionQueue.h"
#include "NamedScript.h"
#include "GraphicsManager.h"

using namespace StoneRing;

StoneRing::Skill::eType
StoneRing::Skill::TypeFromString(const std::string type)
{
    if(type == "battle") return BATTLE;
    else if(type == "switch") return SWITCH;
    else throw CL_Exception("Bad type on skill = " + type);
}

void StoneRing::Skill::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    m_nBp = get_required_int("bp",attributes);
    m_eType = TypeFromString(get_implied_string("type",attributes,"battle"));
    m_bAllowsGroupTarget = get_implied_bool("allowsGroupTarget",attributes,false);
    m_bDefaultToEnemyGroup = get_implied_bool("defaultToEnemyGroup", attributes,true);
    m_pIcon = GraphicsManager::GetIcon(get_implied_string("icon",attributes,"no_icon"));
    m_description = get_implied_string("desc",attributes, "");
}

void StoneRing::Skill::Invoke(const ParameterList& params)
{
    if(m_pOnInvoke)
    {
        m_pOnInvoke->ExecuteScript(params);
    }
}

void StoneRing::Skill::Select(const ParameterList& params)
{
    if(m_pOnSelect)
    {
        m_pOnSelect->ExecuteScript(params);
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
void StoneRing::Skill::Deselect()
{

}

bool StoneRing::Skill::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EONSELECT:
        m_pOnSelect = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONDESELECT:
        m_pOnDeselect = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONINVOKE:
        m_pOnInvoke = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONREMOVE:
        m_pOnRemove = dynamic_cast<NamedScript*>(pElement);
        break;
    case ECONDITIONSCRIPT:
        m_pCondition = dynamic_cast<NamedScript*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

StoneRing::Skill::Skill():m_nBp(0), 
                          m_pOnInvoke(NULL),m_pOnRemove(NULL),m_pCondition(NULL),m_pOnSelect(NULL),m_pOnDeselect(NULL),
                          m_bAllowsGroupTarget(false),m_bDefaultToEnemyGroup(true)
{

}

Skill::~Skill()
{
    delete m_pCondition;
    delete m_pOnRemove;
    delete m_pOnInvoke;
    delete m_pOnSelect;
    delete m_pOnDeselect;
}


std::string Skill::GetName() const
{
    return m_name;
}


uint Skill::GetBPCost() const
{
    return m_nBp;
}


Skill * SkillRef::GetSkill() const{
    if(!AbilityManager::SkillExists(m_ref))
    {
	throw CL_Exception("Missing skill: " + m_ref);
	return NULL;
    }
    return AbilityManager::GetSkill(*this);
}




SkillRef::SkillRef()
{
}

SkillRef::~SkillRef()
{
}

std::string SkillRef::GetRef() const
{
    return m_ref;
}



void SkillRef::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_ref = get_required_string("skillName", attributes);
}

bool operator==(const SkillRef& lhs, const SkillRef& rhs) 
{
    return lhs.GetRef() == rhs.GetRef();
}


SkillTreeNode::SkillTreeNode()
{

}

SkillTreeNode::~SkillTreeNode()
{

} 

uint SkillTreeNode::GetSPCost() const
{
    return m_nSp;
}

uint SkillTreeNode::GetMinLevel() const
{
    return m_nMinLevel;
}

bool SkillTreeNode::CanLearn ( Character* pCharacter )
{
    // Shove character into parameters, call condition script
    return true;
}

SkillRef* SkillTreeNode::GetRef() const
{
    return m_ref;
}

void SkillTreeNode::load_attributes ( CL_DomNamedNodeMap attributes )
{
    m_nSp = get_implied_int("sp",attributes,0);
    m_nMinLevel = get_implied_int("min_level",attributes,1);
    m_requirements = get_implied_string("reqs",attributes,"");
}


bool SkillTreeNode::handle_element ( Element::eElement element, Element* pElement )
{
    switch(element)
    {
        case ESKILLREF:
            m_ref = dynamic_cast<SkillRef*>(pElement);
            break;
        case ESKILLTREENODE:
            m_sub_skills.push_back(dynamic_cast<SkillTreeNode*>(pElement));
            break;
        case ECONDITIONSCRIPT:
            m_pCondition = dynamic_cast<ScriptElement*>(pElement);
            break;
        default:
            return false;
    }
    
    return true;
}




