#include "ArmorClassRef.h"


using namespace StoneRing;

ArmorClassRef::ArmorClassRef()
{
}


ArmorClassRef::~ArmorClassRef()
{
}

void ArmorClassRef::handleText(const std::string &text)
{
    mName = text;
}

bool ArmorClassRef::operator==(const ArmorClassRef &lhs)
{
    if( mName == lhs.mName) return true;
    else return false;
}

std::string 
ArmorClassRef::getName() const
{
    return mName;
}



