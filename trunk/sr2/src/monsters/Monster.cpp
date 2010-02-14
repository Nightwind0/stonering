#include "Monster.h"
#include "MonsterElement.h"
#include "ItemRef.h"
#include "StatusEffect.h"
#include "DamageCategory.h"
#include "steel/SteelType.h"

using StoneRing::Monster;
using StoneRing::MonsterParty;
using StoneRing::ICharacter;
using StoneRing::MonsterElement;
using StoneRing::ItemRef;
using StoneRing::StatusEffect;
using StoneRing::SpriteDefinition;
using StoneRing::eDamageCategory;



Monster::Monster(MonsterElement *pDefinition)
        :m_pMonsterDefinition(pDefinition),m_name(pDefinition->GetName()),m_nLevel(0)
{

}

Monster::~Monster()
{
}

void Monster::set_transients()
{
    for(uint i = _START_OF_TRANSIENTS+1;i<_END_OF_TRANSIENTS;i++){
        m_augments[ static_cast<eCharacterAttribute>(i) ] =
            GetAttribute( GetMaximumAttribute(static_cast<eCharacterAttribute>(i)) );
    }


}

void Monster::set_toggle_defaults()
{
    for(uint toggle=_START_OF_TOGGLES;toggle<_END_OF_TOGGLES;toggle++)
    {
        switch(toggle)
        {
            case CA_DRAW_ILL:
            case CA_DRAW_STONE:
            case CA_DRAW_BERSERK:
            case CA_DRAW_WEAK:
            case CA_DRAW_PARALYZED:
            case CA_DRAW_TRANSLUCENT:
            case CA_DRAW_MINI:
                m_toggles[static_cast<eCharacterAttribute>(toggle)] = false;
                break;
            case CA_VISIBLE:
            case CA_CAN_ACT:
            case CA_CAN_FIGHT:
            case CA_CAN_CAST:
            case CA_CAN_SKILL:
            case CA_CAN_ITEM:
            case CA_CAN_RUN:
            case CA_ALIVE:
                m_toggles[static_cast<eCharacterAttribute>(toggle)] = true;
                break;
            default:
                break;
        }
    }
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
    ClearDeathAnimated();
    set_toggle_defaults();
    set_transients();
    m_pMonsterDefinition->Invoke();
}

void Monster::Invoke(const ParameterList& params)
{
    ClearDeathAnimated();
    set_toggle_defaults();
    set_transients();
    m_pMonsterDefinition->Invoke(params);
}


void Monster::Round()
{
    m_pMonsterDefinition->Round();
}

void Monster::Round(const ParameterList& params)
{
    m_pMonsterDefinition->Round(params);
}

void Monster::Die()
{
    m_pMonsterDefinition->Die();
}

void Monster::Die(const ParameterList& params)
{
    m_pMonsterDefinition->Die(params);
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
    m_toggles[attr] = state;
}

void Monster::Kill()
{
    SetToggle(CA_ALIVE,false);
}

void Monster::Attacked()
{
}

double Monster::GetSpellResistance(StoneRing::Magic::eMagicType type) const
{
    return 0.0;
}

double Monster::GetDamageCategoryResistance(eDamageCategory type) const
{
    return (type == HOLY)?-1.0:1.0;
}

double Monster::GetAttribute(ICharacter::eCharacterAttribute attr) const
{
    if(!IsTransient(attr))
    {
        const Stat * pStat = m_pMonsterDefinition->GetStat(attr);
        if (pStat != NULL)
            return pStat->GetStat();
        else
            return 0.0;
    }
    else
    {
        std::map<eCharacterAttribute,double>::const_iterator iter = m_augments.find(attr);
        if( iter != m_augments.end())
            return iter->second;
        else return 0.0;
    }
}


bool Monster::GetToggle(ICharacter::eCharacterAttribute attr) const
{
    std::map<eCharacterAttribute,bool>::const_iterator iter = m_toggles.find(attr);
    if(iter != m_toggles.end())
        return iter->second;
    else return false;
}

void Monster::PermanentAugment(eCharacterAttribute attr, double augment)
{
    std::map<eCharacterAttribute,double>::iterator aug = m_augments.find(attr);
    if (aug == m_augments.end())
        m_augments[attr] = augment;
    else  aug->second += augment;

    if(IsTransient(attr))
    {
        if(m_augments[attr] > GetAttribute ( GetMaximumAttribute(attr) ) )
        {
            m_augments[attr] = GetAttribute ( GetMaximumAttribute(attr) );
        }
    }

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

eDamageCategory Monster::GetDefaultDamageCategory() const
{
      return m_pMonsterDefinition->GetDefaultDamageCategory();
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

