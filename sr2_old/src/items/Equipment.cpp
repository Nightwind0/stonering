#include "Equipment.h"
#include "Character.h"
#include "AttributeModifier.h"
#include "StatusEffectModifier.h"

using namespace StoneRing;
using namespace Steel;

Equipment::Equipment():m_eMagic(NONE)
{
}


Equipment::~Equipment()
{
}

std::string Equipment::GetSlotName ( Equipment::eSlot slot )
{
    switch(slot)
    {
        case EBODY:
            return "Body";
        case EFEET:
            return "Feet";
        case EFINGER1:
            return "Ring";
        case EFINGER2:
            return "Ring";
        case EHAND:
            return "Hand";
        case EOFFHAND:
            return "Off Hand";
        case EHANDS:
            return "Gloves";
        case EHEAD:
            return "Head";
        case EANY:
            return "Any";
        case EANYARMOR:
            return "Any Armor";
        case EANYHAND:
            return "Any Hand";
        default:
            assert(0);
            return "Unknown";
    }
}



RuneType * Equipment::GetRuneType() const
{
    return m_SpellOrRuneRef.mpRuneType;
}


bool Equipment::HasRuneType() const
{
    return false; // No runes for now
}

// Apply any attribute enhancements 
void Equipment::Equip(ICharacter *pCharacter)
{
    ParameterList params;
    ParameterListItem item("$_Character",pCharacter);
    params.push_back(item);
    OnEquipScript(params);
}

// Remove any attribute enhancements
void Equipment::Unequip(ICharacter *pCharacter)
{
    ParameterList params;
    ParameterListItem item("$_Character",pCharacter);
    params.push_back(item);
    OnUnequipScript(params);
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
    double multiplier = 1.0; // TODO: Should this be 0??
    for(AttributeModifierSet::const_iterator iter = m_attribute_modifiers.lower_bound(attr);
        iter != m_attribute_modifiers.upper_bound(attr);iter++)
    {
        if(iter->second->GetType() == AttributeModifier::EMULTIPLY)
            multiplier *= iter->second->GetMultiplier();
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

bool Equipment::GetAttributeToggle ( uint i_attr, bool current ) const
{
    ICharacter::eCharacterAttribute attr = static_cast<ICharacter::eCharacterAttribute>(i_attr);
    double base = current;
    for(AttributeModifierSet::const_iterator iter = m_attribute_modifiers.lower_bound(attr);
        iter != m_attribute_modifiers.upper_bound(attr); iter++)
        {
            if(iter->second->GetType() == AttributeModifier::ETOGGLE)
                if(ICharacter::ToggleDefaultTrue(attr))
                    base = base && iter->second->GetToggle(); 
                else 
                    base = base || iter->second->GetToggle();
        }
        
    return base;
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

void Equipment::Clear_StatusEffect_Modifiers()
{
    m_statuseffect_modifiers.clear();
}

void Equipment::Clear_StatusEffect_Inflictions()
{
    m_statuseffect_inflictions.clear();
}


void Equipment::Add_StatusEffect_Modifier( StatusEffectModifier * pModifier )
{
    m_statuseffect_modifiers.insert ( StatusEffectModifierSet::value_type(pModifier->GetStatusEffect()->GetName(),pModifier) );
}

void Equipment::Add_StatusEffect_Infliction( StatusEffectInfliction *pInfliction )
{
    m_statuseffect_inflictions.insert ( StatusEffectInflictionSet::value_type(pInfliction->GetStatusEffect()->GetName(),pInfliction) );
}

double Equipment::GetStatusEffectModifier(const std::string &statuseffect)const
{
    std::pair<StatusEffectModifierSet::const_iterator, 
                StatusEffectModifierSet::const_iterator> bounds = m_statuseffect_modifiers.equal_range(statuseffect);
    double modifier = 0.0;
    for(StatusEffectModifierSet::const_iterator iter = bounds.first; 
        iter != bounds.second; iter++)
    {
        modifier += iter->second->GetModifier();
    }

    return modifier;
}


void Equipment::Set_Rune_Type ( RuneType * pType )
{
    m_SpellOrRuneRef.mpRuneType = pType;
    m_eMagic = RUNE;
}




