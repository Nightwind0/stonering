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

CL_DomElement  
WeaponTypeRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"weaponTypeRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}

std::string WeaponTypeRef::getName() const
{
    return mName;
}


