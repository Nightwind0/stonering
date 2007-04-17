#include "WeaponTypeRef.h"

using namespace StoneRing;

WeaponTypeRef::WeaponTypeRef()
{
}


WeaponTypeRef::~WeaponTypeRef()
{
}

void WeaponTypeRef::handleText(const std::string &text)
{
    mName = text;
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




