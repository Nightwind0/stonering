#include "WeaponTypeRef.h"

using namespace StoneRing;

WeaponTypeRef::WeaponTypeRef()
{
}


WeaponTypeRef::~WeaponTypeRef()
{
}

void WeaponTypeRef::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name",pAttributes);
}

bool WeaponTypeRef::operator==(const WeaponTypeRef &lhs)
{
    if( m_name == lhs.m_name) return true;
    else return false;
}

std::string WeaponTypeRef::GetName() const
{
    return m_name;
}




