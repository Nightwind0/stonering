#include "Monster.h"
#include "MonsterElement.h"
#include "ItemRef.h"
#include "StatusEffect.h"

using StoneRing::Monster;
using StoneRing::MonsterParty;
using StoneRing::ICharacter;
using StoneRing::MonsterElement;
using StoneRing::ItemRef;
using StoneRing::StatusEffect;
using StoneRing::SpriteDefinition;



Monster::Monster(MonsterElement *pDefinition)
        :m_pMonsterDefinition(pDefinition),m_name(pDefinition->GetName()),m_nLevel(0)
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

uint Monster::GetLevel(void)const
{
    return m_nLevel;
}

void Monster::SetLevel(uint level)
{
    m_nLevel = level;
}
// For boolean values.
void Monster::SetToggle(ICharacter::eCharacterAttribute attr, bool state)
{
}

void Monster::Kill()
{
    SetToggle(CA_ALIVE,false);
}

double Monster::GetSpellResistance(StoneRing::Magic::eMagicType type) const
{
    return 0.0;
}

double Monster::GetAttributeReal(ICharacter::eCharacterAttribute attr) const
{
    return 1.0;
}

int Monster::GetAttribute(ICharacter::eCharacterAttribute attr) const
{
    return 0;
}


bool Monster::GetToggle(ICharacter::eCharacterAttribute attr) const
{
    return false;
}

void Monster::PermanentAugment(eCharacterAttribute attr, int augment)
{
    std::map<eCharacterAttribute,int>::iterator aug = m_augments.find(attr);
    if (aug == m_augments.end())
        m_augments[attr] = augment;
    else aug->second += augment;
}

void Monster::PermanentAugment(eCharacterAttribute attr, double augment)
{
    std::map<eCharacterAttribute,double>::iterator aug = m_real_augments.find(attr);
    if (aug == m_real_augments.end())
        m_real_augments[attr] = augment;
    else  aug->second += augment;
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



void MonsterParty::AddMonster(Monster * pMonster)
{
    m_monsters.push_back(pMonster);
}
uint MonsterParty::GetCharacterCount() const
{
    return m_monsters.size();
}

ICharacter * MonsterParty::GetCharacter(uint index) const
{
    return m_monsters[index];
}

