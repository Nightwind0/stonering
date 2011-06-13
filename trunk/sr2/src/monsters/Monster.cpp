#include "Monster.h"
#include "MonsterElement.h"
#include "ItemRef.h"
#include "StatusEffect.h"
#include "DamageCategory.h"
#ifdef _WINDOWS_
#include "SteelType.h"
#else
#include "steel/SteelType.h"
#endif
#include "IApplication.h"


using StoneRing::Monster;
using StoneRing::MonsterParty;
using StoneRing::ICharacter;
using StoneRing::MonsterElement;
using StoneRing::ItemRef;
using StoneRing::StatusEffect;
using StoneRing::SpriteDefinition;



 
 
 
Monster::Monster(MonsterElement *pDefinition)
        :m_pMonsterDefinition(pDefinition),m_name(pDefinition->GetName()),m_sprite(GET_MAIN_GC()),m_nLevel(0)
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
    for(uint toggle=_START_OF_TOGGLES+1;toggle<_END_OF_TOGGLES;toggle++)
    {
        m_toggles[static_cast<eCharacterAttribute>(toggle)] = ToggleDefaultTrue(static_cast<eCharacterAttribute>(toggle));
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
    ParameterList params;
    params.push_back(ParameterListItem("$Character",this));
    SetLevel ( m_pMonsterDefinition->GetLevel() );
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
    ParameterList params;
    params.push_back(ParameterListItem("$Character",this));
    m_pMonsterDefinition->Die(params);
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
    Die();
}

void Monster::Attacked()
{
    // TODO: What do I call here?
}


double Monster::GetDamageCategoryResistance(DamageCategory::eDamageCategory type) const
{
    // TODO: Umm... something better than this, monsters should be able to override this
    return (type == DamageCategory::HOLY)?-1.0:1.0;
}

double Monster::GetAttribute(ICharacter::eCharacterAttribute attr) const
{
    
    
   double base = 0.0;
     
    if(!IsTransient(attr))
    {
        if(ICharacter::IsDamageCategoryAttribute(attr))
        {
            base = 1.0;
            if(attr == ICharacter::CA_HOLY_RST)
                base = -1.0;
        }
        else 
        {
            base = m_pMonsterDefinition->GetStat(attr);       
        }
        for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            base *=  iter->second->GetAttributeMultiplier(attr);
        }
        for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            base += iter->second->GetAttributeAdd(attr);
        }
    }
    double augment  = 0.0;
    std::map<eCharacterAttribute,double>::const_iterator aug = m_augments.find(attr);
    if(aug != m_augments.end())
        augment = aug->second;

    if(IsInteger(attr))
        return static_cast<int>( base + augment );
    else
        return base + augment;
}


bool Monster::GetToggle(ICharacter::eCharacterAttribute attr) const
{
    bool base = ToggleDefaultTrue(attr);
    std::map<eCharacterAttribute,bool>::const_iterator iter = m_toggles.find(attr);
    if(iter != m_toggles.end())
        base =  iter->second;
    
    for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            if(ToggleDefaultTrue(attr))
                 base = base && iter->second->GetAttributeToggle(attr, base);
            else
                base = base || iter->second->GetAttributeToggle(attr, base);
        }   
        
    return base;    

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
    m_status_effect_rounds[pEffect->GetName()] = 0;
    m_status_effects[pEffect->GetName()] = pEffect;
    ParameterList params;
    params.push_back(ParameterListItem("$_Character",this));
    pEffect->Invoke(params);    
}

void Monster::RemoveEffect(StatusEffect *pEffect)
{
    m_status_effects.erase(pEffect->GetName());
    m_status_effect_rounds.erase(pEffect->GetName());
    ParameterList params;
    params.push_back(ParameterListItem("$_Character",this));
    pEffect->Remove(params);    
}

double Monster::StatusEffectChance(StatusEffect* pEffect) const
{
    double chance = 1.0;
        for(StatusEffectMap::const_iterator iter = m_status_effects.begin();
            iter != m_status_effects.end(); iter++)
        {
            chance += iter->second->GetStatusEffectModifier(pEffect->GetName());
        }
    return chance;
}

void Monster::StatusEffectRound()
{
        for(StatusEffectMap::iterator iter = m_status_effects.begin();
        iter != m_status_effects.end(); iter++)
        {
            StatusEffect * pEffect = iter->second;
            uint round = ++m_status_effect_rounds[pEffect->GetName()];
            ParameterList params;
            params.push_back(ParameterListItem("$_Character",this));
            params.push_back(ParameterListItem("$_EffectRound",static_cast<int>(round)));
            if(pEffect->GetLast() == StatusEffect::ROUND_COUNT)
            {
                if(round <= pEffect->GetRoundCount())
                    iter->second->Round(params);
                else
                    RemoveEffect(pEffect); // Times up!
            }
            else
            {
                iter->second->Round(params);
            }
        }
}

void Monster::SetCurrentSprite(CL_Sprite sprite)
{
    m_sprite.clone(sprite);
    m_sprite.set_alignment(origin_center);
}

CL_Sprite Monster::GetCurrentSprite() const
{
    return m_sprite;
}


ICharacter::eType Monster::GetType() const
{
    return m_pMonsterDefinition->GetType();
}


ICharacter::eGender Monster::GetGender() const
{
    return NEUTER;
}

DamageCategory::eDamageCategory Monster::GetDefaultDamageCategory() const
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


void StoneRing::Monster::IterateStatusEffects ( Visitor< StoneRing::StatusEffect* >& visitor)
{
    for(StatusEffectMap::iterator iter = m_status_effects.begin();
        iter != m_status_effects.end();
        iter++)
        {
            visitor.Visit(iter->second);
        }
}

