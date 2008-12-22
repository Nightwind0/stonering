#include "StatusEffect.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"
#include "AttributeModifier.h"


void StoneRing::StatusEffect::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name", pAttributes );
    std::string last = get_required_string("last",pAttributes );

    if(last == "battle") m_eLast = BATTLE;
    else if (last == "round_count") m_eLast = ROUND_COUNT;
    else if (last == "permanent") m_eLast = PERMANENT;

    if( m_eLast == BATTLE )
    {
        m_fLengthMultiplier = get_implied_float("lengthMultiplier",pAttributes,1);
    }
    else if (m_eLast == ROUND_COUNT )
    {
        m_nRoundCount = get_required_int("roundCount",pAttributes);
    }

}

bool StoneRing::StatusEffect::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EONINVOKE:
        m_pOnInvoke = dynamic_cast<OnInvoke*>(pElement);
        break;
    case EONROUND:
        m_pOnRound = dynamic_cast<OnRound*>(pElement);
        break;
    case EONCOUNTDOWN:
        m_pOnCountdown = dynamic_cast<OnCountdown*>(pElement);
        break;
    case EONREMOVE:
        m_pOnRemove = dynamic_cast<OnRemove*>(pElement);
        break;
    case EATTRIBUTEMODIFIER:{
        AttributeModifier * pAM = dynamic_cast<AttributeModifier*>(pElement);
        m_attribute_modifiers.insert(std::pair<ICharacter::eCharacterAttribute,AttributeModifier*>
            (static_cast<ICharacter::eCharacterAttribute>(pAM->GetAttribute()),pAM));
        break;
                            }
    default:
        return false;
    }
    return true;
}

StoneRing::StatusEffect::StatusEffect():m_pOnInvoke(NULL),m_pOnRound(NULL),
                                        m_pOnCountdown(NULL), m_pOnRemove(NULL)
{
}

StoneRing::StatusEffect::~StatusEffect()
{
    delete m_pOnInvoke;
    delete m_pOnRound;
    delete m_pOnCountdown;
    delete m_pOnRemove;
}

StoneRing::OnInvoke * StoneRing::StatusEffect::GetOnInvoke() const
{
    return m_pOnInvoke;
}

StoneRing::OnRound * StoneRing::StatusEffect::GetOnRound() const
{
    return m_pOnRound;
}

StoneRing::OnCountdown * StoneRing::StatusEffect::GetOnCountdown() const
{
    return m_pOnCountdown;
}

StoneRing::OnRemove * StoneRing::StatusEffect::GetOnRemove() const
{
    return m_pOnRemove;
}

std::string StoneRing::StatusEffect::GetName() const
{
    return m_name;
}

StoneRing::StatusEffect::eLast 
StoneRing::StatusEffect::GetLast() const
{
    return m_eLast;
}

uint StoneRing::StatusEffect::GetRoundCount() const
{
    return m_nRoundCount;
}


// Multiply the magic power of the user by this using an algorithm to get length..
float StoneRing::StatusEffect::GetLengthMultiplier() const
{
    return m_fLengthMultiplier;
}
                
double StoneRing::StatusEffect::GetAttributeMultiplier(ICharacter::eCharacterAttribute attr) const
{
    double multiplier = 0.0;
    for(AttributeModifierSet::const_iterator iter = m_attribute_modifiers.lower_bound(attr);
        iter != m_attribute_modifiers.upper_bound(attr);iter++)
    {
        if(iter->second->GetType() == AttributeModifier::EMULTIPLY)
            multiplier += iter->second->GetMultiplier();
    }

    return multiplier;
}

double StoneRing::StatusEffect::GetAttributeAdd(ICharacter::eCharacterAttribute attr)const
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

