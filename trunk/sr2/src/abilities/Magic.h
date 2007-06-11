#ifndef SR_MAGIC_H
#define SR_MAGIC_H
#include <string>

namespace StoneRing{

class Magic
{
public:
    Magic();
    virtual ~Magic();
    enum eMagicType
    {
        UNKNOWN = 0,
        FIRE=1, 
        WATER=2,
        WIND=4, 
        EARTH=8,
        HOLY=16,
        DARK=32, 
        OTHER=64,
        STATUS=128,
        ELEMENTAL = (FIRE | WATER | WIND | EARTH),
        DIVINE = (DARK | HOLY),
        ALL = (ELEMENTAL | DIVINE | STATUS | OTHER)
    };

    static eMagicType typeOf(const std::string &);

private:
};

};
#endif