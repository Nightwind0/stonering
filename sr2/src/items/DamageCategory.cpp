#include "DamageCategory.h"
#include "Magic.h"

using namespace StoneRing;

WeaponDamageCategory::WeaponDamageCategory()
{
}

void WeaponDamageCategory::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_eType = TypeFromString(get_required_string("type",pAttributes));
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


WeaponDamageCategory::eType WeaponDamageCategory::GetType() const
{
    return m_eType;
}


MagicDamageCategory::MagicDamageCategory()
{
}

void MagicDamageCategory::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_eType = Magic::TypeOf(get_required_string("type",pAttributes));
}

MagicDamageCategory::~MagicDamageCategory()
{
}


Magic::eMagicType MagicDamageCategory::GetType() const
{
    return m_eType;
}






