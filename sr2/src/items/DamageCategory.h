#ifndef SR_DAMAGE_CATEGORY_H
#define SR_DAMAGE_CATEGORY_H

#include <string>

namespace StoneRing{
    
    class DamageCategory { 
    public:
    enum eDamageCategory{
        BASH=1,
        PIERCE=2,
        SLASH=4,
        HOLY=8,
        DARK=16,
        FIRE=32,
        WATER=64,
        WIND=128,
        EARTH=256,
        _LAST_DAMAGE_CATEGORY=512
    };

    static bool DamageCategoryIsPhysical(eDamageCategory category);
    static bool DamageCategoryIsMagic(eDamageCategory category);
    static bool DamageCategoryIsElemental(eDamageCategory category);
    static eDamageCategory DamageCategoryFromString(const std::string& string);
    };
}

#endif



