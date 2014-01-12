#include "WeaponClassRef.h"

using namespace StoneRing;

WeaponClassRef::WeaponClassRef()
{
}

void WeaponClassRef::load_attributes(clan::DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
}

WeaponClassRef::~WeaponClassRef()
{
}

bool WeaponClassRef::operator==(const WeaponClassRef &lhs)
{
    if(m_name == lhs.m_name) return true;
    else return false;
}

std::string WeaponClassRef::GetName() const
{
    return m_name;
}






