
#include "Element.h"
#include <ClanLib/display.h>
#include "Skill.h"
#include "IApplication.h"
#include "Animation.h"
#include "Level.h"
#include "ActionQueue.h"
#include "NamedScript.h"
#include "GraphicsManager.h"
#include "Description.h"

using namespace StoneRing;

StoneRing::Skill::eType
StoneRing::Skill::TypeFromString(const std::string type)
{
    if(type == "battle") return BATTLE;
    else if(type == "world") return WORLD;
    else if(type == "both") return BOTH;
    else throw CL_Exception("Bad type on skill = " + type);
}

void StoneRing::Skill::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    m_nBp = get_implied_int("bp",attributes,0);
    m_nMp = get_implied_int("mp",attributes,0);
    if(m_nBp && m_nMp)
        throw CL_Exception("Skill " + m_name + " has both BP and MP cost (Not allowed)");
    m_eType = TypeFromString(get_implied_string("type",attributes,"battle"));
    m_bAllowsGroupTarget = get_implied_bool("allowsGroupTarget",attributes,false);
    m_bDefaultToEnemyGroup = get_implied_bool("defaultToEnemyGroup", attributes,true);
    m_pIcon = GraphicsManager::GetIcon(get_implied_string("icon",attributes,"no_icon"));
}

uint Skill::GetMPCost() const
{
    return m_nMp;
}


void StoneRing::Skill::Invoke(ICharacter* pCharacter,const ParameterList& params)
{
    // Take the BP and MP cost here
    double mp_mult = pCharacter->GetAttribute(ICharacter::CA_MP_COST);
	double bp_mult = pCharacter->GetAttribute(ICharacter::CA_BP_COST);
    assert(bp_mult * pCharacter->GetAttribute(ICharacter::CA_BP) >= m_nBp &&
        mp_mult * pCharacter->GetAttribute(ICharacter::CA_MP) >= m_nMp  &&
        "Assure BP and MP before calling Invoke on a skill");
    
    bool charge = true;
    
    if(m_pOnInvoke)
    {
        SteelType res =  m_pOnInvoke->ExecuteScript(params);
        charge = (int)res >= 0; // TODO: This cast right here causes an exception when there is no "return" from a skill. why?
								// also note... even if you don't return, but you call a bunch of shit, then it's okay. why??
								// what's on the stack that doesn't want to be cast into an int... something that shouldn't be there?
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
    if(charge){
    	pCharacter->PermanentAugment(ICharacter::CA_BP,-double(m_nBp * pCharacter->GetAttribute(ICharacter::CA_BP_COST)));
    	pCharacter->PermanentAugment(ICharacter::CA_MP,-double(m_nMp * pCharacter->GetAttribute(ICharacter::CA_MP_COST)));
    }else{
    	std::cout << "Cancelled charge";
    }
}


bool StoneRing::Skill::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EONINVOKE:
    case EONSELECT:
        m_pOnInvoke = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONREMOVE:
        m_pOnRemove = dynamic_cast<NamedScript*>(pElement);
        break;
    case ECONDITIONSCRIPT:
        m_pCondition = dynamic_cast<NamedScript*>(pElement);
        break;
    case EDESCRIPTION:
        m_description = (dynamic_cast<Description*>(pElement))->GetText();
        break;
    default:
        return false;
    }

    return true;
}

StoneRing::Skill::Skill():m_nBp(0), 
                          m_pOnInvoke(NULL),m_pOnRemove(NULL),m_pCondition(NULL),
                          m_bAllowsGroupTarget(false),m_bDefaultToEnemyGroup(true)
{

}

Skill::~Skill()
{
    delete m_pCondition;
    delete m_pOnRemove;
    delete m_pOnInvoke;
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

// StoneRing:: is the namespace here (confusing kinda)
bool StoneRing::operator==(const SkillRef& lhs, const SkillRef& rhs) 
{
    return lhs.GetRef() == rhs.GetRef();
}


SkillTreeNode::SkillTreeNode():m_pCondition(NULL),m_parent(NULL)
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

SkillTreeNode* SkillTreeNode::GetParent() const
{
    return m_parent;
}


bool SkillTreeNode::CanLearn ( Character* pCharacter )
{
    if(pCharacter->GetLevel() < m_nMinLevel)
        return false;
    
    if(m_parent)
    {
        // They have to have the parent skill. That's how trees work, you see.
        if(!pCharacter->HasSkill(m_parent->GetRef()->GetRef()))
            return false;
    }
    
    if(m_pCondition)
    {
        ParameterList params;
        params.push_back(Steel::ParameterListItem("$_Character",pCharacter));
        return m_pCondition->EvaluateCondition(params);
    } 
    
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
        case ESKILLTREENODE:{
            SkillTreeNode* pNode = dynamic_cast<SkillTreeNode*>(pElement);
            pNode->m_parent = this;
            m_sub_skills.push_back(pNode);
            break;
        }
        case ECONDITIONSCRIPT:
            m_pCondition = dynamic_cast<ScriptElement*>(pElement);
            break;
        default:
            return false;
    }
    
    return true;
}


std::list< SkillTreeNode* >::const_iterator SkillTreeNode::GetSubSkillsBegin() const
{
    return m_sub_skills.begin();
}

std::list< SkillTreeNode* >::const_iterator SkillTreeNode::GetSubSkillsEnd() const
{
    return m_sub_skills.end();
}


