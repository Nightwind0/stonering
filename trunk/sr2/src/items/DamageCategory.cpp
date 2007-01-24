#include "DamageCategory.h"

using namespace StoneRing;

WeaponDamageCategory::WeaponDamageCategory()
{
}

void WeaponDamageCategory::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    meType = TypeFromString(getRequiredString("type",pAttributes));
}

WeaponDamageCategory::eType
WeaponDamageCategory::TypeFromString ( const std::string &str )
{
    if(str == "slash") return SLASH;
    else if (str == "bash") return BASH;
    else if (str == "jab") return JAB;
    else throw CL_Error("Unknown type " + str + " On weapon damage category");
}


WeaponDamageCategory::~WeaponDamageCategory()
{
}


WeaponDamageCategory::eType WeaponDamageCategory::getType() const
{
    return meType;
}

CL_DomElement WeaponDamageCategory::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "weaponDamageCategory");
}


MagicDamageCategory::MagicDamageCategory()
{
}

void MagicDamageCategory::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    meType = TypeFromString(getRequiredString("type",pAttributes));
}

MagicDamageCategory::~MagicDamageCategory()
{
}


eMagicType
MagicDamageCategory::TypeFromString ( const std::string &str )
{
    if(str == "fire") return FIRE;
    else if (str == "wind") return WIND;
    else if (str == "water") return WATER;
    else if (str == "earth") return EARTH;
    else if (str == "holy") return HOLY;
    else if (str == "dark") return DARK;
    else if (str == "other") return OTHER;
    else throw CL_Error("Unknown type " + str + " On magic damage category");
}

eMagicType MagicDamageCategory::getType() const
{
    return meType;
}

CL_DomElement MagicDamageCategory::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "magicDamageCategory");
}

