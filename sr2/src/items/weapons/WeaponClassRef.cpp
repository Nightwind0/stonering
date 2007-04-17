#include "WeaponClassRef.h"

using namespace StoneRing;

WeaponClassRef::WeaponClassRef()
{
}

void WeaponClassRef::handleText(const std::string &text)
{
    mName = text;
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






