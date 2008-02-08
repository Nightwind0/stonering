#include "Equipment.h"
#include "AttributeEnhancer.h"

using namespace StoneRing;

Equipment::Equipment():meMagic(NONE)
{
    mSpellOrRuneRef.mpSpellRef = NULL;
}


Equipment::~Equipment()
{
}

SpellRef * Equipment::getSpellRef() const
{
    return mSpellOrRuneRef.mpSpellRef;
}

RuneType * Equipment::getRuneType() const
{
    return mSpellOrRuneRef.mpRuneType;
}

bool Equipment::hasSpell() const
{
    if( meMagic == SPELL ) return true;
    else return false;
}

bool Equipment::hasRuneType() const
{
    if( meMagic == RUNE ) return true;
    else return false;
}

// Apply any attribute enhancements 
void Equipment::equip(ICharacter *pCharacter)
{

#ifndef _MSC_VER
    std::for_each( mAttributeEnhancers.begin(), mAttributeEnhancers.end(), std::mem_fun(&AttributeEnhancer::invoke));

#else

    for (std::list<AttributeEnhancer*>::const_iterator iter = mAttributeEnhancers.begin();
         iter != mAttributeEnhancers.end(); iter++)
    {
        (*iter)->invoke();
    }
#endif
    onEquipScript();
}

// Remove any attribute enhancements
void Equipment::unequip(ICharacter *pCharacter)
{
#ifndef _MSC_VER
    std::for_each( mAttributeEnhancers.begin(), mAttributeEnhancers.end(), std::mem_fun(&AttributeEnhancer::revoke));
#else
    for(std::list<AttributeEnhancer*>::const_iterator iter = mAttributeEnhancers.begin();
        iter != mAttributeEnhancers.end(); iter++)
    {
        (*iter)->revoke();
    }
#endif
    onUnequipScript();
}


std::list<AttributeEnhancer*>::const_iterator Equipment::getAttributeEnhancersBegin() const
{
    return mAttributeEnhancers.begin();
}

std::list<AttributeEnhancer*>::const_iterator Equipment::getAttributeEnhancersEnd() const
{
    return mAttributeEnhancers.end();
}


void Equipment::clearAttributeEnhancers()
{
    mAttributeEnhancers.clear();
}

void Equipment::addAttributeEnhancer( AttributeEnhancer * pAttr )
{
    mAttributeEnhancers.push_back ( pAttr );
}

void Equipment::setSpellRef ( SpellRef * pRef )
{
    mSpellOrRuneRef.mpSpellRef = pRef;
    meMagic = SPELL;
}

void Equipment::setRuneType ( RuneType * pType )
{
    mSpellOrRuneRef.mpRuneType = pType;

    meMagic = RUNE;
}




