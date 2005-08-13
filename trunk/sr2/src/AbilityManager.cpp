#include "AbilityManager.h"
#include "Spell.h"
#include <ClanLib/core.h>
#include "AbilityFactory.h"
#include "IApplication.h"

using namespace StoneRing;


void AbilityManager::loadSpellFile ( CL_DomDocument &doc )
{
    AbilityFactory * pAbilityFactory = IApplication::getInstance()->getAbilityFactory();

    CL_DomElement spellsNode = doc.named_item("spellList").to_element();


    CL_DomElement spellNode = spellsNode.get_first_child().to_element();

    while(!spellNode.is_null())
    {
	
	mSpells.push_back ( pAbilityFactory->createSpell ( &spellNode ) );
	spellNode = spellNode.get_next_sibling().to_element();
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


#ifndef NDEBUG
void AbilityManager::dumpSpellList()
{

}
#endif
    

