#include "AttributeEnhancer.h"
#include "IApplication.h"
#include "SpriteDefinition.h"

using namespace StoneRing;

AttributeEnhancer::AttributeEnhancer():mnAdd(0),mfMultiplier(1)
{
}

void AttributeEnhancer::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mnAttribute = ICharacter::CAFromString(getRequiredString("attribute", pAttributes));
    meType = static_cast<eType>( getImpliedInt("type",pAttributes, EAUTO) );
    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);
    mnAdd = getImpliedInt("add",pAttributes,0);
    mbToggle = getImpliedBool("toggle",pAttributes,false);

    if(meType == EAUTO)
    {
        // Lets calculate the real type based on what is supplied
        if(hasAttr("multiplier",pAttributes))
            meType = static_cast<eType>(meType | EMULTIPLY);
        if(hasAttr("add",pAttributes))
            meType = static_cast<eType>(meType | EADD);
        if(hasAttr("toggle",pAttributes))
            meType = ETOGGLE; // No or, we can't combine this
    }
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

bool AttributeEnhancer::getToggle() const
{
    return mbToggle;
}

AttributeEnhancer::eType AttributeEnhancer::getType() const
{
    return meType;
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when invoking. (By calling equip on the armor/weapon...)
void AttributeEnhancer::invoke()
{
    ICharacter * pCharacter = IApplication::getInstance()
        ->getSelectedCharacterGroup()
        ->getSelectedCharacter();

    ICharacter::eCharacterAttribute attr = static_cast<ICharacter::eCharacterAttribute>(mnAttribute);
    if(meType & EMULTIPLY)
        pCharacter->attachMultiplication(attr, mfMultiplier);
    if(meType & EADD)
        pCharacter->attachAddition(attr, mnAdd);
    if(meType & ETOGGLE)
        pCharacter->fixAttribute(attr,mbToggle);
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when revoking. (By calling unequip on the armor/weapon...)
void AttributeEnhancer::revoke()
{

    ICharacter * pCharacter = IApplication::getInstance()
        ->getSelectedCharacterGroup()
        ->getSelectedCharacter();

    ICharacter::eCharacterAttribute attr = static_cast<ICharacter::eCharacterAttribute>(mnAttribute);

    if(meType & EADD)
        pCharacter->detachAddition(attr, mnAdd);
    if(meType & EMULTIPLY)
        pCharacter->detachMultiplication(attr, mfMultiplier);
    if(meType & ETOGGLE)
        pCharacter->fixAttribute(attr, ! mbToggle );

}




