#include "Stat.h"
#include "CharacterDefinition.h"

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

eCharacterAttribute Stat::getAttribute() const
{
    return meAttr;
}

void Stat::loadAttributes(CL_DomNamedNodeMap *attr)
{
    meAttr = CharAttributeFromString( getRequiredString("id",attr) );
    if(meAttr = CA_INVALID)
        throw CL_Error("Unknown stat type in monster stat");

    if(meAttr > _START_OF_TOGGLES && meAttr < _LAST_TOGGLE)
        mbToggle = getRequiredBool("toggle",attr);
    else  mfValue = getRequiredFloat("value",attr);

}
