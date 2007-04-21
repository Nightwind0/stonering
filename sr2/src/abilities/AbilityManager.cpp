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

using namespace StoneRing;


void AbilityManager::loadSpellFile ( CL_DomDocument &doc )
{
    IFactory * pAbilityFactory = IApplication::getInstance()->getElementFactory();

    CL_DomElement spellsNode = doc.named_item("spellList").to_element();
    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while(!spellNode.is_null())
    {
        Spell * pSpell = dynamic_cast<Spell*>(pAbilityFactory->createElement("spell"));

        pSpell->load(&spellNode);
        mSpells.push_back ( pSpell );
        spellNode = spellNode.get_next_sibling().to_element();
    }
}


void AbilityManager::loadSkillFile ( CL_DomDocument &doc )
{
    IFactory * pAbilityFactory = IApplication::getInstance()->getElementFactory();

    CL_DomElement spellsNode = doc.named_item("skillList").to_element();
    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while(!spellNode.is_null())
    {
        Skill * pSkill = dynamic_cast<Skill*>(pAbilityFactory->createElement("skill"));

        pSkill->load(&spellNode);
        mSkills [ pSkill->getName() ] = pSkill;
        spellNode = spellNode.get_next_sibling().to_element();
    }
}

void AbilityManager::loadStatusEffectFile ( CL_DomDocument &doc )
{
    IFactory * pAbilityFactory = IApplication::getInstance()->getElementFactory();
    assert ( pAbilityFactory );

    CL_DomElement statusEffectsNode = doc.named_item("statusEffectList").to_element();
    CL_DomElement statusEffectNode = statusEffectsNode.get_first_child().to_element();

    while(!statusEffectNode.is_null())
    {
        StatusEffect * pStatusEffect = dynamic_cast<StatusEffect*>(pAbilityFactory->createElement("statusEffect"));
        pStatusEffect->load(&statusEffectNode);
        mStatusEffects.push_back ( pStatusEffect );
        statusEffectNode = statusEffectNode.get_next_sibling().to_element();
    }
}

void AbilityManager::loadCharacterClassFile ( CL_DomDocument &doc )
{
    IFactory * pFactory = IApplication::getInstance()->getElementFactory();

    CL_DomElement classesNode = doc.named_item("characterClasses").to_element();
    CL_DomElement classNode = classesNode.get_first_child().to_element();

    while(!classNode.is_null())
    {
        CharacterClass * pCharacterClass = dynamic_cast<CharacterClass*>
            (pFactory->createElement("characterClass"));

        pCharacterClass->load(&classNode);
        mCharacterClasses [ pCharacterClass->getName() ] = pCharacterClass;
        classNode = classNode.get_next_sibling().to_element();

#ifndef NDEBUG
        std::cout << "Class: " << pCharacterClass->getName() << std::endl;
#endif
    }


}


std::list<Spell*>::const_iterator AbilityManager::getSpellsBegin() const
{
    return mSpells.begin();
}


std::list<Spell*>::const_iterator AbilityManager::getSpellsEnd() const
{
    return mSpells.end();
}
std::map<std::string,Skill*>::const_iterator AbilityManager::getSkillsBegin() const
{
    return mSkills.begin();
}


std::map<std::string,Skill*>::const_iterator AbilityManager::getSkillsEnd() const
{
    return mSkills.end();
}

Skill * AbilityManager::getSkill ( const SkillRef &ref ) const
{
    return mSkills.find( ref.getRef() )->second;
}


CharacterClass * AbilityManager::getClass ( const std::string &cls ) const
{
    return mCharacterClasses.find(cls)->second;
}


Spell * AbilityManager::getSpell( const SpellRef & ref ) const
{

    for(std::list<Spell*>::const_iterator iter = mSpells.begin();
        iter != mSpells.end();
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

    throw CL_Error("Couldn't find spell based on ref.");

    return NULL;
}



StatusEffect * AbilityManager::getStatusEffect ( const std::string &ref ) const
{
    for(std::list<StatusEffect*>::const_iterator iter = mStatusEffects.begin();
        iter != mStatusEffects.end();
        iter++)
    {
        if((*iter)->getName() == ref)
            return *iter;
    }

    throw CL_Error("Couldn't find a status ref called: " + ref );
    return NULL;
}

#ifndef NDEBUG
void AbilityManager::dumpSpellList()
{

}
#endif






