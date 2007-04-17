#include "WeaponEnhancer.h"

using namespace StoneRing;

void WeaponEnhancer::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string strAttr = getRequiredString("attribute", pAttributes);

    meAttribute = Weapon::attributeForString ( strAttr );

    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);

    mnAdd = getImpliedInt("add",pAttributes,0);
}

WeaponEnhancer::WeaponEnhancer():mfMultiplier(1),mnAdd(0)
{
}

WeaponEnhancer::~WeaponEnhancer()
{
}

CL_DomElement WeaponEnhancer::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "weaponEnhancer");
}

Weapon::eAttribute WeaponEnhancer::getAttribute() const
{
    return meAttribute;
}

int WeaponEnhancer::getAdd() const
{
    return mnAdd;
}

float WeaponEnhancer::getMultiplier() const
{
    return mfMultiplier;
}




