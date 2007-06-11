#include "SpellRef.h"

using namespace StoneRing;

void SpellRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    meSpellType = Magic::typeOf(getRequiredString("type",pAttributes));

    if(meSpellType == Magic::UNKNOWN)
        throw CL_Error("Bad magic type in spell ref");
}

void SpellRef::handleText(const std::string &text)
{
    mName = text;
}

SpellRef::SpellRef(  )
{
}

SpellRef::~SpellRef()
{
}


bool SpellRef::operator==(const SpellRef &lhs )
{
    if ( meSpellType == lhs.meSpellType &&
         mName == lhs.mName )
    {
        return true;
    }
    else return false;
}


Magic::eMagicType SpellRef::getSpellType() const
{
    return meSpellType;
}

std::string SpellRef::getName() const
{
    return mName;
}

