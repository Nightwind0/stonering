#include "Equipment.h"
#include "Character.h"
#include "AttributeModifier.h"

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


Equipment::AttributeModifierSet::const_iterator Equipment::GetAttributeModifiersBegin() const
{
    return m_attribute_modifiers.begin();
}

Equipment::AttributeModifierSet::const_iterator Equipment::GetAttributeModifiersEnd() const
{
    return m_attribute_modifiers.end();
}

double StoneRing::Equipment::GetAttributeMultiplier(uint attr) const
{
    double multiplier = 1.0;
    for(AttributeModifierSet::const_iterator iter = m_attribute_modifiers.lower_bound(attr);
        iter != m_attribute_modifiers.upper_bound(attr);iter++)
    {
        if(iter->second->GetType() == AttributeModifier::EMULTIPLY)
            multiplier += iter->second->GetMultiplier();
    }

    return multiplier;
}

double StoneRing::Equipment::GetAttributeAdd(uint attr)const
{
    double add = 0.0;
    for(AttributeModifierSet::const_iterator iter = m_attribute_modifiers.lower_bound(attr);
        iter != m_attribute_modifiers.upper_bound(attr);iter++)
    {
        if(iter->second->GetType() == AttributeModifier::EADD)
            add += iter->second->GetAdd();
    }

    return add;
}


void Equipment::Clear_Attribute_Modifiers()
{
    m_attribute_modifiers.clear();
}

void Equipment::Add_Attribute_Modifier( AttributeModifier * pAttr )
{
    m_attribute_modifiers.insert(std::pair<ICharacter::eCharacterAttribute,AttributeModifier*>
            (static_cast<ICharacter::eCharacterAttribute>(pAttr->GetAttribute()),pAttr));
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




