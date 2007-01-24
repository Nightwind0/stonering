#include "ArmorEnhancer.h"

using namespace StoneRing;

ArmorEnhancer::ArmorEnhancer()
{
}

void ArmorEnhancer::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string strAttr = getRequiredString("attribute", pAttributes);

    meAttribute = Armor::attributeForString ( strAttr );

    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);

    mnAdd = getImpliedInt("add",pAttributes,0);
}


ArmorEnhancer::~ArmorEnhancer()
{
}

CL_DomElement 
ArmorEnhancer::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "armorEnhancer");
}

Armor::eAttribute 
ArmorEnhancer::getAttribute() const
{
    return meAttribute;
}

int ArmorEnhancer::getAdd() const
{
    return mnAdd;
}

float ArmorEnhancer::getMultiplier() const
{
    return mfMultiplier;
}



