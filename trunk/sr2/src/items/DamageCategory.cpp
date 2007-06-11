#include "DamageCategory.h"
#include "Magic.h"

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


MagicDamageCategory::MagicDamageCategory()
{
}

void MagicDamageCategory::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    meType = Magic::typeOf(getRequiredString("type",pAttributes));
}

MagicDamageCategory::~MagicDamageCategory()
{
}


Magic::eMagicType MagicDamageCategory::getType() const
{
    return meType;
}






