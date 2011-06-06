#include "DamageCategory.h"
#include <ClanLib/core.h>

using StoneRing::DamageCategory;


bool StoneRing::DamageCategory::DamageCategoryIsPhysical(eDamageCategory category)
{
    return category & PHYSICAL;
}

bool StoneRing::DamageCategory::DamageCategoryIsMagic(eDamageCategory category)
{
    return category & MAGIC;
}

bool StoneRing::DamageCategory::DamageCategoryIsElemental(eDamageCategory category)
{
    return category & ELEMENTAL;
}

DamageCategory::eDamageCategory StoneRing::DamageCategory::DamageCategoryFromString(const std::string& string)
{
    if(string == "bash") return BASH;
    else if(string == "jab") return JAB;
    else if(string == "slash") return SLASH;
    else if(string == "holy") return HOLY;
    else if(string == "dark") return DARK;
    else if(string == "fire") return FIRE;
    else if(string == "wind") return WIND;
    else if(string == "water") return WATER;
    else if(string == "earth") return EARTH;
    else if(string == "poison") return POISON;
    else if(string == "magic") return MAGIC;
    else if(string == "elemental") return ELEMENTAL;
    else if(string == "divine") return DIVINE;
    else if(string == "physical") return PHYSICAL;

    throw CL_Exception("Damage Category unknown: " + string);
    return BASH;
}






