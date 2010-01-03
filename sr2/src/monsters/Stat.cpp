#include "Stat.h"
#include "SpriteDefinition.h"

using namespace StoneRing;

Stat::Stat()
{
}

Stat::~Stat()
{
}

double Stat::GetStat() const
{
    return m_fValue;
}

bool Stat::GetToggle() const
{
    return m_bToggle;
}

ICharacter::eCharacterAttribute Stat::GetAttribute() const
{
    return m_eAttr;
}

void Stat::load_attributes(CL_DomNamedNodeMap *attr)
{
    m_eAttr = ICharacter::CharAttributeFromString( get_required_string("id",attr) );
    if(m_eAttr == ICharacter::CA_INVALID)
        throw CL_Error("Unknown stat type in monster stat");

    if(m_eAttr > ICharacter::_START_OF_TOGGLES && m_eAttr < ICharacter::_MAXIMA_BASE)
        m_bToggle = get_required_bool("toggle",attr);
    else  m_fValue = get_required_float("value",attr);

}
