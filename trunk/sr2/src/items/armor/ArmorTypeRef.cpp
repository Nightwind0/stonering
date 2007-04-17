#include "ArmorTypeRef.h"

using namespace StoneRing;

ArmorTypeRef::ArmorTypeRef()
{
}



ArmorTypeRef::~ArmorTypeRef()
{
}

void ArmorTypeRef::handleText(const std::string &text)
{
    mName = text;
}

bool ArmorTypeRef::operator==(const ArmorTypeRef &lhs)
{
    if( mName == lhs.mName) return true;
    else return false;
}

std::string ArmorTypeRef::getName() const
{
    return mName;
}



