#include "AbilityManager.h"
#include "Spell.h"
#include <ClanLib/core.h>
#include "AbilityFactory.h"
#include "IApplication.h"
#include "StatusEffect.h"

using namespace StoneRing;


void AbilityManager::loadSpellFile ( CL_DomDocument &doc )
{
    AbilityFactory * pAbilityFactory = IApplication::getInstance()->getAbilityFactory();

    CL_DomElement spellsNode = doc.named_item("spellList").to_element();


    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while(!spellNode.is_null())
    {
		Spell * pSpell = dynamic_cast<Spell*>(pAbilityFactory->createElement(Element::ESPELL));
	
		pSpell->load(&spellNode);
		mSpells.push_back ( pSpell );
		spellNode = spellNode.get_next_sibling().to_element();
    }
    

}

void AbilityManager::loadStatusEffectFile ( CL_DomDocument &doc )
{
    AbilityFactory * pAbilityFactory = IApplication::getInstance()->getAbilityFactory();

    CL_DomElement statusEffectsNode = doc.named_item("statusEffectList").to_element();


    CL_DomElement statusEffectNode = statusEffectsNode.get_first_child().to_element();

    while(!statusEffectNode.is_null())
    {
		StatusEffect * pStatusEffect = dynamic_cast<StatusEffect*>(pAbilityFactory->createElement(Element::ESTATUSEFFECT));
		pStatusEffect->load(&statusEffectNode);
		mStatusEffects.push_back ( pStatusEffect );
		statusEffectNode = statusEffectNode.get_next_sibling().to_element();
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
    

