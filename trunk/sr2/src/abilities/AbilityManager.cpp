#include "AbilityManager.h"
#include "Spell.h"
#include <ClanLib/core.h>
#include <cassert>
#include "AbilityFactory.h"
#include "CharacterFactory.h"
#include "IApplication.h"
#include "StatusEffect.h"
#include "Skill.h"
#include "SpellRef.h"
#include <map>
#include <algorithm>

using namespace StoneRing;


AbilityManager::~AbilityManager()
{
    std::for_each(m_status_effects.begin(),m_status_effects.end(),del_fun<StatusEffect>());
    std::for_each(m_spells.begin(),m_spells.end(),del_fun<Spell>());

    std::for_each(m_skills.begin(),m_skills.end(),
        compose_f_gx(del_fun<Skill>(),
        get_second<SkillMap::value_type>())
        );

}

void AbilityManager::LoadSpellFile ( CL_DomDocument &doc )
{
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement spellsNode = doc.named_item("spellList").to_element();
    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while(!spellNode.is_null())
    {
        Spell * pSpell = dynamic_cast<Spell*>(pAbilityFactory->createElement("spell"));

        pSpell->Load(spellNode);
        m_spells.push_back ( pSpell );
        spellNode = spellNode.get_next_sibling().to_element();
    }
}


void AbilityManager::LoadSkillFile ( CL_DomDocument &doc )
{
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement spellsNode = doc.named_item("skillList").to_element();
    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while(!spellNode.is_null())
    {
        Skill * pSkill = dynamic_cast<Skill*>(pAbilityFactory->createElement("skill"));

        pSkill->Load(spellNode);
        m_skills [ pSkill->GetName() ] = pSkill;
        spellNode = spellNode.get_next_sibling().to_element();
    }
}

void AbilityManager::LoadStatusEffectFile ( CL_DomDocument &doc )
{
    IFactory * pAbilityFactory = IApplication::GetInstance()->GetElementFactory();
    assert ( pAbilityFactory );

    CL_DomElement statusEffectsNode = doc.named_item("statusEffectList").to_element();
    CL_DomElement statusEffectNode = statusEffectsNode.get_first_child().to_element();

    while(!statusEffectNode.is_null())
    {
        StatusEffect * pStatusEffect = dynamic_cast<StatusEffect*>(pAbilityFactory->createElement("statusEffect"));
        pStatusEffect->Load(statusEffectNode);
        m_status_effects.push_back ( pStatusEffect );
        statusEffectNode = statusEffectNode.get_next_sibling().to_element();
    }
}


std::list<Spell*>::const_iterator AbilityManager::GetSpellsBegin() const
{
    return m_spells.begin();
}


std::list<Spell*>::const_iterator AbilityManager::GetSpellsEnd() const
{
    return m_spells.end();
}
std::map<std::string,Skill*>::const_iterator AbilityManager::GetSkillsBegin() const
{
    return m_skills.begin();
}


std::map<std::string,Skill*>::const_iterator AbilityManager::GetSkillsEnd() const
{
    return m_skills.end();
}

Skill * AbilityManager::GetSkill ( const SkillRef &ref ) const
{
    return m_skills.find( ref.GetRef() )->second;
}


Spell * AbilityManager::GetSpell( const SpellRef & ref ) const
{

    for(std::list<Spell*>::const_iterator iter = m_spells.begin();
        iter != m_spells.end();
        iter++)
    {
        SpellRef * pRef = (*iter)->createSpellRef();

        if( *pRef == ref )
        {
            delete pRef;
            return *iter;
        }

        delete pRef;
    }

    throw CL_Exception("Couldn't find spell based on ref.");

    return NULL;
}



StatusEffect * AbilityManager::GetStatusEffect ( const std::string &ref ) const
{
    for(std::list<StatusEffect*>::const_iterator iter = m_status_effects.begin();
        iter != m_status_effects.end();
        iter++)
    {
        if((*iter)->GetName() == ref)
            return *iter;
    }

    throw CL_Exception("Couldn't find a status ref called: " + ref );
    return NULL;
}

#ifndef NDEBUG
void AbilityManager::DumpSpellList()
{

}
#endif






