#include "WeaponClassRef.h"

using namespace StoneRing;

WeaponClassRef::WeaponClassRef()
{
}

void WeaponClassRef::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);
}

WeaponClassRef::~WeaponClassRef()
{
}

bool WeaponClassRef::operator==(const WeaponClassRef &lhs)
{
    if(mName == lhs.mName) return true;
    else return false;
}

std::string WeaponClassRef::getName() const
{
    return mName;
}






