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
        GRAVITY=DamageCategory::GRAVITY,
        ELECTRIC=DamageCategory::ELECTRIC,
        STATUS=(ELECTRIC <<1),
        OTHER=(ELECTRIC <<2),
        ELEMENTAL = (FIRE | WATER | WIND | EARTH | ELECTRIC),
        DIVINE = (DARK | HOLY),
        ALL = (ELEMENTAL | DIVINE | STATUS | GRAVITY | OTHER)
    };

    static eMagicType TypeOf(const std::string &);
    static std::string ToString(eMagicType type);

private:
};

}
#endif

