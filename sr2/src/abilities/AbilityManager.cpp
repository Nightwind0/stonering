#include "AbilityManager.h"
#include <ClanLib/core.h>
#include <cassert>
#include "IApplication.h"
#include "StatusEffect.h"
#include "Skill.h"
#include <map>
#include <algorithm>

using namespace StoneRing;
#define     INSTANCE() AbilityManager::m_pInstance


AbilityManager* AbilityManager::m_pInstance = NULL;

AbilityManager::~AbilityManager()
{
    std::for_each(m_status_effects.begin(),m_status_effects.end(),compose_f_gx(del_fun<StatusEffect>(), get_second<StatusEffectMap::value_type>()));
    std::for_each(m_skills.begin(),m_skills.end(),
                  compose_f_gx(del_fun<Skill>(),
                               get_second<SkillMap::value_type>())
                 );

}

void AbilityManager::initialize() {
	if(AbilityManager::m_pInstance == NULL){
		AbilityManager::m_pInstance = new AbilityManager();
	}
}




void AbilityManager::LoadSkillFile ( clan::DomDocument &doc )
{
    AbilityManager * instance = INSTANCE();
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();

    clan::DomElement spellsNode = doc.named_item("skillList").to_element();
    clan::DomElement spellNode = spellsNode.get_first_child().to_element();

    while (!spellNode.is_null())
    {
        Skill * pSkill = dynamic_cast<Skill*>(pAbilityFactory->createElement("skill"));
		try { 
			pSkill->Load(spellNode);
		}catch(XMLException& e){
			e.push_error("skill: " + pSkill->GetDebugId());
			throw e;
		}
		std::cout << "Loaded skill: " << pSkill->GetName() << std::endl;
        instance->m_skills [ pSkill->GetName() ] = pSkill;
        spellNode = spellNode.get_next_sibling().to_element();
    }
}

void AbilityManager::LoadStatusEffectFile ( clan::DomDocument &doc )
{
    AbilityManager * instance = INSTANCE();
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();
    assert ( pAbilityFactory );

    clan::DomElement statusEffectsNode = doc.named_item("statusEffectList").to_element();
    clan::DomElement statusEffectNode = statusEffectsNode.get_first_child().to_element();

    while (!statusEffectNode.is_null())
    {
        StatusEffect * pStatusEffect = dynamic_cast<StatusEffect*>(pAbilityFactory->createElement("statusEffect"));
        try { 
			pStatusEffect->Load(statusEffectNode);
		}catch(XMLException& e){
			e.push_error("statusEffect: " + pStatusEffect->GetDebugId());
			throw e;
		}
        if(instance->m_status_effects.find(pStatusEffect->GetName()) != instance->m_status_effects.end())
            throw XMLException("Duplicate Status Effect named " + pStatusEffect->GetName() + " found");
        instance->m_status_effects[pStatusEffect->GetName()] = pStatusEffect;
        statusEffectNode = statusEffectNode.get_next_sibling().to_element();
    }
}




std::map<std::string,Skill*>::const_iterator AbilityManager::GetSkillsBegin()
{
    AbilityManager * instance = INSTANCE();
    return instance->m_skills.begin();
}


std::map<std::string,Skill*>::const_iterator AbilityManager::GetSkillsEnd()
{
    AbilityManager * instance = INSTANCE();
    return instance->m_skills.end();
}

Skill * AbilityManager::GetSkill ( const SkillRef &ref )
{
    AbilityManager * instance = INSTANCE();
    return instance->m_skills.find( ref.GetRef() )->second;
}

Skill * AbilityManager::GetSkill ( const std::string &skill )
{
    AbilityManager * instance = INSTANCE();
    return instance->m_skills.find ( skill )->second;
}

bool AbilityManager::SkillExists ( const std::string &skill )
{
    AbilityManager * instance = INSTANCE();
    return instance->m_skills.find(skill) != instance->m_skills.end();
}


StatusEffect * AbilityManager::GetStatusEffect ( const std::string &ref )
{
    AbilityManager * instance = INSTANCE();
    StatusEffectMap::const_iterator iter = instance->m_status_effects.find(ref);
    
    if(iter == instance->m_status_effects.end())
        throw clan::Exception("Couldn't find a status ref called: " + ref );
    else return iter->second;
    return NULL;
}







