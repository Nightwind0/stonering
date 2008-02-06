#include "ArmorClassRef.h"


using namespace StoneRing;

ArmorClassRef::ArmorClassRef()
{
}


ArmorClassRef::~ArmorClassRef()
{
}

void ArmorClassRef::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);
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



