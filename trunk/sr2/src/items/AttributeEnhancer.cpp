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
    meType = static_cast<eType>( getRequiredInt("type",pAttributes) );
    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);
    mnAdd = getImpliedInt("add",pAttributes,0);
    mbToggle = getImpliedBool("toggle",pAttributes,false);
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

    eCharacterAttribute attr = static_cast<eCharacterAttribute>(mnAttribute);

    if(meType & EADD)
        pCharacter->attributeAdd(attr,mnAdd);
    if(meType & EMULTIPLY)
        pCharacter->attributeMultiply(attr,mfMultiplier);
    if(meType & ETOGGLE)
        pCharacter->toggleAttribute(attr,mbToggle);
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when revoking. (By calling unequip on the armor/weapon...)
void AttributeEnhancer::revoke()
{

    ICharacter * pCharacter = IApplication::getInstance()
        ->getSelectedCharacterGroup()
        ->getSelectedCharacter();

    eCharacterAttribute attr = static_cast<eCharacterAttribute>(mnAttribute);

    if(meType & EMULTIPLY)
        pCharacter->attributeMultiply(attr,  1 / mfMultiplier);
    if(meType & EADD)
        pCharacter->attributeAdd(attr, 0 - mnAdd);
    if(meType & ETOGGLE)
        pCharacter->toggleAttribute(attr, ! mbToggle );

}




