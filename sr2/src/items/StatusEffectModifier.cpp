#include "StatusEffectModifier.h"
#include "AbilityManager.h"
#include "IApplication.h"

using namespace StoneRing;

StatusEffectModifier::StatusEffectModifier():mpStatusEffect(NULL)
{
}


void StatusEffectModifier::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{

    std::string statusRef = getRequiredString("statusRef", pAttributes);

    const AbilityManager * pManager = IApplication::getInstance()->getAbilityManager();

    mpStatusEffect = pManager->getStatusEffect( statusRef );

    mfModifier = getRequiredFloat("modifier", pAttributes );
}

StatusEffectModifier::~StatusEffectModifier()
{
}


StatusEffect * StatusEffectModifier::getStatusEffect() const
{
    return mpStatusEffect;
}

float StatusEffectModifier::getModifier() const
{
    return mfModifier;
}



CL_DomElement StatusEffectModifier::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"statusEffectModifier");
}


