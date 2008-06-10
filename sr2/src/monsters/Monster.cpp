#include "Monster.h"
#include "MonsterElement.h"
#include "ItemRef.h"
#include "StatusEffect.h"

using StoneRing::Monster;
using StoneRing::ICharacter;
using StoneRing::MonsterElement;
using StoneRing::ItemRef;
using StoneRing::StatusEffect;
using StoneRing::SpriteDefinition;



Monster::Monster(MonsterElement *pDefinition)
:m_pMonsterDefinition(pDefinition),m_name(pDefinition->GetName())
{

}

Monster::~Monster()
{
}


std::list<ItemRef*>::const_iterator Monster::GetDropsBegin() const
{
    return m_pMonsterDefinition->GetItemRefsBegin();
}

std::list<ItemRef*>::const_iterator Monster::GetDropsEnd() const
{
    return m_pMonsterDefinition->GetItemRefsEnd();
}

void Monster::Invoke()
{
    m_pMonsterDefinition->Invoke();
}

void Monster::Round()
{
    m_pMonsterDefinition->Round();
}

void Monster::Die()
{
    m_pMonsterDefinition->Die();
}



// For boolean values.
void Monster::FixAttribute(ICharacter::eCharacterAttribute attr, bool state)
{
}


double Monster::GetSpellResistance(StoneRing::Magic::eMagicType type) const
{
    return 0.0;
}

double Monster::GetAttribute(ICharacter::eCharacterAttribute attr) const 
{
    return 1.0;
}


bool Monster::GetToggle(ICharacter::eCharacterAttribute attr) const
{
    return false;
}

void Monster::FixAttribute(ICharacter::eCharacterAttribute attr, double value)
{
    m_attributes.FixAttribute(attr,value);
}

void Monster::AttachMultiplication(ICharacter::eCharacterAttribute attr, double factor)
{
    m_attributes.AttachMultiplication(attr,factor);
}

void Monster::AttachAddition(ICharacter::eCharacterAttribute attr, double value) 
{
    m_attributes.AttachAddition(attr,value);
}

void Monster::DetachMultiplication(ICharacter::eCharacterAttribute attr, double factor)
{
    m_attributes.DetachMultiplication(attr,factor);
}

void Monster::DetachAddition(ICharacter::eCharacterAttribute attr, double value) 
{
    m_attributes.DetachAddition(attr,value);
}


void Monster::AddStatusEffect(StatusEffect *pEffect)
{
    m_status_effects.insert(StatusEffectMap::value_type(pEffect->GetName(),pEffect));
}

void Monster::RemoveEffects(const std::string &name)
{
    StatusEffectMap::iterator start = m_status_effects.lower_bound(name);
    StatusEffectMap::iterator end   = m_status_effects.upper_bound(name);

    m_status_effects.erase(start,end);
}

void Monster::StatusEffectRound()
{
}


ICharacter::eType Monster::GetType() const
{
    return m_pMonsterDefinition->GetType();
}


ICharacter::eGender Monster::GetGender() const
{
    return NEUTER;
}

