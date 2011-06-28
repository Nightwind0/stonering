#include "DamageCategory.h"
#include <ClanLib/core.h>

using StoneRing::DamageCategory;


bool StoneRing::DamageCategory::DamageCategoryIsPhysical(eDamageCategory category)
{
    switch(category)
    {
        case PIERCE:
        case BASH:
        case SLASH:
            return true;
        default:
            return false;
    }
}

bool StoneRing::DamageCategory::DamageCategoryIsMagic(eDamageCategory category)
{
    switch(category)
    {
        case WIND:
        case FIRE:
        case EARTH:
        case WATER:
        case DARK:
        case HOLY:
            return true;
        default:
            return false;
    }
}

bool StoneRing::DamageCategory::DamageCategoryIsElemental(eDamageCategory category)
{
    switch(category)
    {
        case WIND:
        case FIRE:
        case WATER:
        case EARTH:
            return true;
        default:
            return false;
    }
}

DamageCategory::eDamageCategory StoneRing::DamageCategory::DamageCategoryFromString(const std::string& string)
{
    if(string == "bash") return BASH;
    else if(string == "pierce") return PIERCE;
    else if(string == "slash") return SLASH;
    else if(string == "holy") return HOLY;
    else if(string == "dark") return DARK;
    else if(string == "fire") return FIRE;
    else if(string == "wind") return WIND;
    else if(string == "water") return WATER;
    else if(string == "earth") return EARTH;

    throw CL_Exception("Damage Category unknown: " + string);
    return BASH;
}






