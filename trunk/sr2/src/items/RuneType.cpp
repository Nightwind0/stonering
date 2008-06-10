#include "RuneType.h"

using namespace StoneRing;

RuneType::RuneType()
{
}

void RuneType::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string runeType = get_required_string("runeType",pAttributes);

    if(runeType == "none")
        m_eRuneType = NONE;
    else if (runeType == "rune")
        m_eRuneType = RUNE;
    else if (runeType == "ultraRune")
        m_eRuneType = ULTRA_RUNE;
    else throw CL_Error("Bogus runetype supplied.");

}

bool RuneType::operator==(const RuneType &lhs )
{
    if ( m_eRuneType == lhs.m_eRuneType )
        return true;
    else return false;
}

RuneType::~RuneType()
{
}

RuneType::eRuneType RuneType::GetRuneType() const
{
    return m_eRuneType;
}

std::string RuneType::GetRuneTypeAsString() const
{

    //@todo : Get this from a setting.
    switch(m_eRuneType)
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




