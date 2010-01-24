#include "DamageCategory.h"
#include "Magic.h"

using namespace StoneRing;

bool StoneRing::DamageCategoryIsPhysical(eDamageCategory category)
{
    return category < _END_OF_PHYSICAL_DAMAGE;
}

bool StoneRing::DamageCategoryIsMagic(eDamageCategory category)
{
    return category > _END_OF_PHYSICAL_DAMAGE;
}

bool StoneRing::DamageCategoryIsElemental(eDamageCategory category)
{
    switch(category)
    {
        case EARTH:
        case FIRE:
        case WIND:
        case WATER:
        // case HEART:
        case LIGHTNING:
        case ICE:
            return true;
        default:
            return false;
    }
}

eDamageCategory StoneRing::DamageCategoryFromString(const std::string& string)
{
    if(string == "bash") return BASH;
    else if(string == "jab") return JAB;
    else if(string == "slash") return SLASH;
    else if(string == "holy") return HOLY;
    else if(string == "dark") return DARK;
    else if(string == "fire") return FIRE;
    else if(string == "wind") return WIND;
    else if(string == "water") return WATER;
    else if(string == "ice") return ICE;
    else if(string == "earth") return EARTH;
    else if(string == "lightning") return LIGHTNING;
    else if(string == "poison") return POISON;

    throw CL_Exception("Damage Category unknown: " + string);
    return BASH;
}






