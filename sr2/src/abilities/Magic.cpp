#include "Magic.h"
#include <ClanLib/core.h>
//UNKNOWN = 0,
//FIRE=1,
//WATER=2,
//WIND=4,
//EARTH=8,
//HOLY=16,
//DARK=32,
//OTHER=64,
//STATUS=128,
//ELEMENTAL = (FIRE | WATER | WIND | EARTH),
//DIVINE = (DARK | HOLY),
//ALL = (ELEMENTAL | DIVINE | STATUS | OTHER)

using namespace StoneRing;

Magic::eMagicType Magic::TypeOf(const std::string &str)
{
    if(str == "fire")
        return FIRE;
    else if(str == "water")
        return WATER;
    else if(str == "wind")
        return WIND;
    else if(str == "earth")
        return EARTH;
    else if(str == "holy")
        return HOLY;
    else if(str == "dark")
        return DARK;
    else if(str == "other")
        return OTHER;
    else if(str == "status")
        return STATUS;
    else if(str == "elemental")
        return ELEMENTAL;
    else if(str == "divine")
        return DIVINE;
    else if(str == "gravity")
        return GRAVITY;
    else if(str == "electric")
        return ELECTRIC;
    else if(str == "all")
        return ALL;
    else return UNKNOWN;
}

std::string Magic::ToString(eMagicType type)
{
    switch(type)
    {
    case FIRE:
        return "fire";
    case EARTH:
        return "earth";
    case WATER:
        return "water";
    case WIND:
        return "wind";
    case DARK:
        return "dark";
    case OTHER:
        return "other";
    case HOLY:
        return "holy";
    case STATUS:
        return "status";
    case DIVINE:
        return "devine";
    case ALL:
        return "all";
    default:
        throw CL_Exception("Bad magic");
    }
}
