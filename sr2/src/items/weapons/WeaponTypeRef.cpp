#include "WeaponTypeRef.h"

using namespace StoneRing;

WeaponTypeRef::WeaponTypeRef()
{
}


WeaponTypeRef::~WeaponTypeRef()
{
}

void WeaponTypeRef::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);
}

bool WeaponTypeRef::operator==(const WeaponTypeRef &lhs)
{
    if( mName == lhs.mName) return true;
    else return false;
}

std::string WeaponTypeRef::getName() const
{
    return mName;
}




