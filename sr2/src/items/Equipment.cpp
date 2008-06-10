#include "Equipment.h"
#include "AttributeEnhancer.h"

using namespace StoneRing;

Equipment::Equipment():m_eMagic(NONE)
{
    m_SpellOrRuneRef.mpSpellRef = NULL;
}


Equipment::~Equipment()
{
}

SpellRef * Equipment::GetSpellRef() const
{
    return m_SpellOrRuneRef.mpSpellRef;
}

RuneType * Equipment::GetRuneType() const
{
    return m_SpellOrRuneRef.mpRuneType;
}

bool Equipment::HasSpell() const
{
    if( m_eMagic == SPELL ) return true;
    else return false;
}

bool Equipment::HasRuneType() const
{
    if( m_eMagic == RUNE ) return true;
    else return false;
}

// Apply any attribute enhancements 
void Equipment::Equip(ICharacter *pCharacter)
{

#ifndef _MSC_VER
    std::for_each( m_attribute_enhancers.begin(), m_attribute_enhancers.end(), std::mem_fun(&AttributeEnhancer::Invoke));

#else

    for (std::list<AttributeEnhancer*>::const_iterator iter = m_attribute_enhancers.begin();
         iter != m_attribute_enhancers.end(); iter++)
    {
        (*iter)->Invoke();
    }
#endif
    OnEquipScript();
}

// Remove any attribute enhancements
void Equipment::Unequip(ICharacter *pCharacter)
{
#ifndef _MSC_VER
    std::for_each( m_attribute_enhancers.begin(), m_attribute_enhancers.end(), std::mem_fun(&AttributeEnhancer::Revoke));
#else
    for(std::list<AttributeEnhancer*>::const_iterator iter = m_attribute_enhancers.begin();
        iter != m_attribute_enhancers.end(); iter++)
    {
        (*iter)->Revoke();
    }
#endif
    OnUnequipScript();
}


std::list<AttributeEnhancer*>::const_iterator Equipment::GetAttributeEnhancersBegin() const
{
    return m_attribute_enhancers.begin();
}

std::list<AttributeEnhancer*>::const_iterator Equipment::GetAttributeEnhancersEnd() const
{
    return m_attribute_enhancers.end();
}


void Equipment::Clear_Attribute_Enhancers()
{
    m_attribute_enhancers.clear();
}

void Equipment::Add_Attribute_Enhancer( AttributeEnhancer * pAttr )
{
    m_attribute_enhancers.push_back ( pAttr );
}

void Equipment::Set_Spell_Ref ( SpellRef * pRef )
{
    m_SpellOrRuneRef.mpSpellRef = pRef;
    m_eMagic = SPELL;
}

void Equipment::Set_Rune_Type ( RuneType * pType )
{
    m_SpellOrRuneRef.mpRuneType = pType;

    m_eMagic = RUNE;
}




