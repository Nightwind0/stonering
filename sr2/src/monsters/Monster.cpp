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
:mpMonsterDefinition(pDefinition),mName(pDefinition->getName())
{

}

Monster::~Monster()
{
}


std::list<ItemRef*>::const_iterator Monster::getDropsBegin() const
{
    return mpMonsterDefinition->getItemRefsBegin();
}

std::list<ItemRef*>::const_iterator Monster::getDropsEnd() const
{
    return mpMonsterDefinition->getItemRefsEnd();
}

void Monster::invoke()
{
    mpMonsterDefinition->invoke();
}

void Monster::round()
{
    mpMonsterDefinition->round();
}

void Monster::die()
{
    mpMonsterDefinition->die();
}



// For boolean values.
void Monster::fixAttribute(ICharacter::eCharacterAttribute attr, bool state)
{
}


double Monster::getSpellResistance(StoneRing::Magic::eMagicType type) const
{
    return 0.0;
}

double Monster::getAttribute(ICharacter::eCharacterAttribute attr) const 
{
    return 1.0;
}


bool Monster::getToggle(ICharacter::eCharacterAttribute attr) const
{
    return false;
}

void Monster::fixAttribute(ICharacter::eCharacterAttribute attr, double value)
{
    mAttributes.fixAttribute(attr,value);
}

void Monster::attachMultiplication(ICharacter::eCharacterAttribute attr, double factor)
{
    mAttributes.attachMultiplication(attr,factor);
}

void Monster::attachAddition(ICharacter::eCharacterAttribute attr, double value) 
{
    mAttributes.attachAddition(attr,value);
}

void Monster::detachMultiplication(ICharacter::eCharacterAttribute attr, double factor)
{
    mAttributes.detachMultiplication(attr,factor);
}

void Monster::detachAddition(ICharacter::eCharacterAttribute attr, double value) 
{
    mAttributes.detachAddition(attr,value);
}


void Monster::addStatusEffect(StatusEffect *pEffect)
{
    mStatusEffects.insert(StatusEffectMap::value_type(pEffect->getName(),pEffect));
}

void Monster::removeEffects(const std::string &name)
{
    StatusEffectMap::iterator start = mStatusEffects.lower_bound(name);
    StatusEffectMap::iterator end   = mStatusEffects.upper_bound(name);

    mStatusEffects.erase(start,end);
}

void Monster::statusEffectRound()
{
}


ICharacter::eType Monster::getType() const
{
    return mpMonsterDefinition->getType();
}


ICharacter::eGender Monster::getGender() const
{
    return NEUTER;
}

