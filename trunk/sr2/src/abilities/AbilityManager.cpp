#include "AbilityManager.h"
#include "Spell.h"
#include <ClanLib/core.h>
#include <cassert>
#include "IApplication.h"
#include "StatusEffect.h"
#include "Skill.h"
#include "SpellRef.h"
#include <map>
#include <algorithm>
#include "Animation.h"

using namespace StoneRing;
#define     INSTANCE() IApplication::GetInstance()->GetAbilityManager();


AbilityManager::~AbilityManager()
{
    std::for_each(m_status_effects.begin(),m_status_effects.end(),compose_f_gx(del_fun<StatusEffect>(), get_second<StatusEffectMap::value_type>()));
    std::for_each(m_spells.begin(),m_spells.end(),del_fun<Spell>());

    std::for_each(m_skills.begin(),m_skills.end(),
                  compose_f_gx(del_fun<Skill>(),
                               get_second<SkillMap::value_type>())
                 );

}

void AbilityManager::LoadSpellFile ( CL_DomDocument &doc )
{
    AbilityManager* instance = INSTANCE();
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement spellsNode = doc.named_item("spellList").to_element();
    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while (!spellNode.is_null())
    {
        Spell * pSpell = dynamic_cast<Spell*>(pAbilityFactory->createElement("spell"));

        pSpell->Load(spellNode);
        instance->m_spells.push_back ( pSpell );
        spellNode = spellNode.get_next_sibling().to_element();
    }
}


void AbilityManager::LoadSkillFile ( CL_DomDocument &doc )
{
    AbilityManager * instance = INSTANCE();
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement spellsNode = doc.named_item("skillList").to_element();
    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while (!spellNode.is_null())
    {
        Skill * pSkill = dynamic_cast<Skill*>(pAbilityFactory->createElement("skill"));

        pSkill->Load(spellNode);
        instance->m_skills [ pSkill->GetName() ] = pSkill;
        spellNode = spellNode.get_next_sibling().to_element();
    }
}

void AbilityManager::LoadStatusEffectFile ( CL_DomDocument &doc )
{
    AbilityManager * instance = INSTANCE();
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();
    assert ( pAbilityFactory );

    CL_DomElement statusEffectsNode = doc.named_item("statusEffectList").to_element();
    CL_DomElement statusEffectNode = statusEffectsNode.get_first_child().to_element();

    while (!statusEffectNode.is_null())
    {
        StatusEffect * pStatusEffect = dynamic_cast<StatusEffect*>(pAbilityFactory->createElement("statusEffect"));
        pStatusEffect->Load(statusEffectNode);
        if(instance->m_status_effects.find(pStatusEffect->GetName()) != instance->m_status_effects.end())
            throw CL_Exception("Duplicate Status Effect named " + pStatusEffect->GetName() + " found");
        instance->m_status_effects[pStatusEffect->GetName()] = pStatusEffect;
        statusEffectNode = statusEffectNode.get_next_sibling().to_element();
    }
}


void AbilityManager::LoadAnimationFile ( CL_DomDocument &doc )
{
    AbilityManager * instance = INSTANCE();
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();
    assert ( pAbilityFactory );

    CL_DomElement animationsNode = doc.named_item("animations").to_element();
    CL_DomElement animationNode = animationsNode.get_first_child().to_element();

    while (!animationNode.is_null())
    {
        Animation * pAnimation = dynamic_cast<Animation*>(pAbilityFactory->createElement("animation"));
        pAnimation->Load(animationNode);
        instance->m_animations[ pAnimation->GetName() ] =  pAnimation ;
        animationNode = animationNode.get_next_sibling().to_element();
    }
}


std::list<Spell*>::const_iterator AbilityManager::GetSpellsBegin()
{
    AbilityManager * instance = INSTANCE();
    return instance->m_spells.begin();
}


std::list<Spell*>::const_iterator AbilityManager::GetSpellsEnd()
{
    AbilityManager * instance = INSTANCE();
    return instance->m_spells.end();
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

Spell * AbilityManager::GetSpell( const SpellRef & ref )
{
    AbilityManager * instance = INSTANCE();
    for (std::list<Spell*>::const_iterator iter = instance->m_spells.begin();
            iter != instance->m_spells.end();
            iter++)
    {
        SpellRef * pRef = (*iter)->createSpellRef();

        if ( *pRef == ref )
        {
            delete pRef;
            return *iter;
        }

        delete pRef;
    }

    throw CL_Exception("Couldn't find spell based on ref, named " + ref.GetName());

    return NULL;
}



StatusEffect * AbilityManager::GetStatusEffect ( const std::string &ref )
{
    AbilityManager * instance = INSTANCE();
    StatusEffectMap::const_iterator iter = instance->m_status_effects.find(ref);
    
    if(iter == instance->m_status_effects.end())
        throw CL_Exception("Couldn't find a status ref called: " + ref );
    else return iter->second;
    return NULL;
}

Animation* AbilityManager::GetAnimation ( const std::string &animation )
{
	AbilityManager * instance = INSTANCE();

	if(instance->m_animations.find(animation) != instance->m_animations.end())
	{
		return instance->m_animations.find(animation)->second;
	}
	return NULL;
}

#ifndef NDEBUG
void AbilityManager::DumpSpellList()
{

}
#endif






