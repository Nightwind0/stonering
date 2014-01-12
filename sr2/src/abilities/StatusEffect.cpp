#include "StatusEffect.h"
#include "IApplication.h"
#include "Animation.h"
#include "AttributeModifier.h"
#include "StatusEffectModifier.h"

using namespace StoneRing;


void StoneRing::StatusEffect::load_attributes(clan::DomNamedNodeMap attributes)
{
    m_name = get_required_string("name", attributes );
	std::cout << "Loading status effect " << m_name << std::endl;
    std::string last = get_required_string("last",attributes );

    if(last == "battle") m_eLast = BATTLE;
    else if (last == "round_count") m_eLast = ROUND_COUNT;
    else if (last == "permanent") m_eLast = PERMANENT;

    if( m_eLast == BATTLE )
    {
        m_fLengthMultiplier = get_implied_float("lengthMultiplier",attributes,1);
    }
    else if (m_eLast == ROUND_COUNT )
    {
        m_nRoundCount = get_required_int("roundCount",attributes);
    }

}

bool StoneRing::StatusEffect::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESPRITEREF:{
	SpriteRef * pRef = dynamic_cast<SpriteRef*>(pElement);
	m_icon = pRef->CreateSprite();
	break;
    }
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
    case ESTATUSEFFECTMODIFIER:{
        StatusEffectModifier* pModifier = dynamic_cast<StatusEffectModifier*>(pElement);
        m_statuseffect_modifiers[pModifier->GetStatusEffect()->GetName()] = pModifier;
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


void StatusEffect::Countdown(const ParameterList& params)
{
    if(m_pOnCountdown)
        m_pOnCountdown->ExecuteScript(params);
}

void StatusEffect::Invoke ( const ParameterList& params )
{
    if(m_pOnInvoke)
        m_pOnInvoke->ExecuteScript(params);
}

void StatusEffect::Remove ( const ParameterList& params )
{
    if(m_pOnRemove)
        m_pOnRemove->ExecuteScript(params);
}

void StatusEffect::Round ( const ParameterList& params )
{
    if(m_pOnRound)
        m_pOnRound->ExecuteScript(params);
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

clan::Sprite StatusEffect::GetIcon() const
{
    return m_icon;
}


// Multiply the magic power of the user by this using an algorithm to get length..
float StoneRing::StatusEffect::GetLengthMultiplier() const
{
    return m_fLengthMultiplier;
}

double StoneRing::StatusEffect::GetAttributeMultiplier(ICharacter::eCharacterAttribute attr) const
{
    double multiplier = 1.0;
    for(AttributeModifierSet::const_iterator iter = m_attribute_modifiers.lower_bound(attr);
        iter != m_attribute_modifiers.upper_bound(attr);iter++)
    {
        if(iter->second->GetType() == AttributeModifier::EMULTIPLY)
            multiplier *= iter->second->GetMultiplier();
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

bool StatusEffect::GetAttributeToggle ( ICharacter::eCharacterAttribute attr, bool current ) const
{
    double base = current;
    std::pair<AttributeModifierSet::const_iterator,AttributeModifierSet::const_iterator> range = m_attribute_modifiers.equal_range(attr);
    
    
    
    for(AttributeModifierSet::const_iterator iter = range.first;
        iter != range.second; iter++)
        {
            if(iter->second->GetType() == AttributeModifier::ETOGGLE)
                if(ICharacter::ToggleDefaultTrue(attr))
                    base = base && iter->second->GetToggle(); 
                else 
                    base = base || iter->second->GetToggle();
        }
        
    return base;
}


double StoneRing::StatusEffect::GetStatusEffectModifier(const std::string &statuseffect) const
{
    std::map<std::string,StatusEffectModifier*>::const_iterator iter = m_statuseffect_modifiers.find(statuseffect);
    if(iter == m_statuseffect_modifiers.end()) return 0.0;
    else return iter->second->GetModifier();   
}


