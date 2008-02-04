#include "Stat.h"
#include "SpriteDefinition.h"

using namespace StoneRing;

Stat::Stat()
{
}

Stat::~Stat()
{
}

double Stat::getStat() const
{
    return mfValue;
}

bool Stat::getToggle() const
{
    return mbToggle;
}

ICharacter::eCharacterAttribute Stat::getAttribute() const
{
    return meAttr;
}

void Stat::loadAttributes(CL_DomNamedNodeMap *attr)
{
    meAttr = ICharacter::CharAttributeFromString( getRequiredString("id",attr) );
    if(meAttr = ICharacter::CA_INVALID)
        throw CL_Error("Unknown stat type in monster stat");

    if(meAttr > ICharacter::_START_OF_TOGGLES && meAttr < ICharacter::_END_OF_TOGGLES)
        mbToggle = getRequiredBool("toggle",attr);
    else  mfValue = getRequiredFloat("value",attr);

}
