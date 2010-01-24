#include "WeaponTypeRef.h"

using namespace StoneRing;

WeaponTypeRef::WeaponTypeRef()
{
}


WeaponTypeRef::~WeaponTypeRef()
{
}

void WeaponTypeRef::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
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




