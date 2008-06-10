#include "StatusEffectModifier.h"
#include "AbilityManager.h"
#include "IApplication.h"

using namespace StoneRing;

StatusEffectModifier::StatusEffectModifier():m_pStatusEffect(NULL)
{
}


void StatusEffectModifier::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    std::string statusRef = get_required_string("statusRef", pAttributes);
    const AbilityManager * pManager = IApplication::GetInstance()->GetAbilityManager();
    m_pStatusEffect = pManager->GetStatusEffect( statusRef );
    m_fModifier = get_required_float("modifier", pAttributes );
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







