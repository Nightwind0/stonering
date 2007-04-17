#include "AttributeEnhancer.h"
#include "IApplication.h"
#include "CharacterDefinition.h"

using namespace StoneRing;

AttributeEnhancer::AttributeEnhancer():mnAdd(0),mfMultiplier(1)
{
}

void AttributeEnhancer::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mnAttribute = CAFromString(getRequiredString("attribute", pAttributes));

    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);

    mnAdd = getImpliedInt("add",pAttributes,0);
}


AttributeEnhancer::~AttributeEnhancer()
{
}


uint AttributeEnhancer::getAttribute() const
{
    return mnAttribute;
}

int AttributeEnhancer::getAdd() const
{
    return mnAdd;
}

float AttributeEnhancer::getMultiplier() const
{
    return mfMultiplier;
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when invoking. (By calling equip on the armor/weapon...)
void AttributeEnhancer::invoke()
{
    ICharacter * pCharacter = IApplication::getInstance()
        ->getSelectedCharacterGroup()
        ->getSelectedCharacter();

    int original = pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute));

    pCharacter->modifyAttribute( static_cast<eCharacterAttribute>(mnAttribute), mnAdd, mfMultiplier);

    int now = pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute));

    // So we can get back to how we were.
    mnDelta =  original - now;
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when revoking. (By calling unequip on the armor/weapon...)
void AttributeEnhancer::revoke()
{

    ICharacter * pCharacter = IApplication::getInstance()
        ->getSelectedCharacterGroup()
        ->getSelectedCharacter();


    int add =  mnDelta;

    pCharacter->modifyAttribute( static_cast<eCharacterAttribute>(mnAttribute), add, 1 );

}




