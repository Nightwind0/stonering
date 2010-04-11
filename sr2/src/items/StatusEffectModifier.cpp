#include "StatusEffectModifier.h"
#include "AbilityManager.h"
#include "IApplication.h"

using namespace StoneRing;

StatusEffectModifier::StatusEffectModifier():m_pStatusEffect(NULL)
{
}


void StatusEffectModifier::load_attributes(CL_DomNamedNodeMap attributes)
{
    std::string statusRef = get_required_string("statusRef", attributes);

    m_pStatusEffect = AbilityManager::GetStatusEffect( statusRef );
    m_fModifier = get_required_float("modifier", attributes );
}

StatusEffectModifier::~StatusEffectModifier()
{
}


StatusEffect * StatusEffectModifier::GetStatusEffect() const
{
    return m_pStatusEffect;
}

float StatusEffectModifier::GetModifier() const
{
    return m_fModifier;
}







