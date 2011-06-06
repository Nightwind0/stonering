#ifndef SR_MAGIC_H
#define SR_MAGIC_H
#include <string>
#include "DamageCategory.h"


using StoneRing::DamageCategory;

namespace StoneRing{

class Magic
{
public:
    Magic();
    virtual ~Magic();
    enum eMagicType
    {
        UNKNOWN = 0,
        HOLY=DamageCategory::HOLY,
        DARK=DamageCategory::DARK,
        FIRE=DamageCategory::FIRE,
        WATER=DamageCategory::WATER,
        WIND=DamageCategory::WIND,
        EARTH=DamageCategory::EARTH,

        OTHER=(EARTH <<1),
        STATUS=(EARTH <<2),
        ELEMENTAL = (FIRE | WATER | WIND | EARTH),
        DIVINE = (DARK | HOLY),
        ALL = (ELEMENTAL | DIVINE | STATUS | OTHER)
    };

    static eMagicType TypeOf(const std::string &);
    static std::string ToString(eMagicType type);

private:
};

}
#endif

