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

    OnEquipScript();
}

// Remove any attribute enhancements
void Equipment::Unequip(ICharacter *pCharacter)
{
    OnUnequipScript();
}


std::list<AttributeModifier*>::const_iterator Equipment::GetAttributeModifiersBegin() const
{
    return m_attribute_modifiers.begin();
}

std::list<AttributeModifier*>::const_iterator Equipment::GetAttributeModifiersEnd() const
{
    return m_attribute_modifiers.end();
}


void Equipment::Clear_Attribute_Modifiers()
{
    m_attribute_modifiers.clear();
}

void Equipment::Add_Attribute_Modifier( AttributeModifier * pAttr )
{
    m_attribute_modifiers.push_back ( pAttr );
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




