#ifndef SR_DAMAGE_CATEGORY_H
#define SR_DAMAGE_CATEGORY_H

#include "Element.h"
#include "Magic.h"

namespace StoneRing{
    enum eDamageCategory{
        _START_OF_PHYSICAL_DAMAGE,
        BASH,
        JAB,
        SLICE,
        _END_OF_PHYSICAL_DAMAGE,
        HOLY,
        DARK,
        FIRE,
        WATER,
        WIND,
        EARTH,
        LIGHTNING,
        ICE,
        POISON
    };

    bool DamageCategoryIsPhysical(eDamageCategory category);
    bool DamageCategoryIsMagic(eDamageCategory category);
    eDamageCategory DamageCategoryFromString(const std::string& string);
};

#endif



