#include "RuneType.h"

using namespace StoneRing;

RuneType::RuneType()
{
}

void RuneType::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string runeType = getRequiredString("runeType",pAttributes);

    if(runeType == "none")
        meRuneType = NONE;
    else if (runeType == "rune")
        meRuneType = RUNE;
    else if (runeType == "ultraRune")
        meRuneType = ULTRA_RUNE;
    else throw CL_Error("Bogus runetype supplied.");

}

bool RuneType::operator==(const RuneType &lhs )
{
    if ( meRuneType == lhs.meRuneType )
        return true;
    else return false;
}

RuneType::~RuneType()
{
}

RuneType::eRuneType RuneType::getRuneType() const
{
    return meRuneType;
}

std::string RuneType::getRuneTypeAsString() const
{

    //@todo : Get this from a setting.
    switch(meRuneType)
    {
    case NONE:
        return "";
    case RUNE:
        return "Rune";
    case ULTRA_RUNE:
        return "Ultra Rune";
        break;
    }

    return "";
}




