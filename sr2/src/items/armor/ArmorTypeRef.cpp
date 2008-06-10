#include "ArmorTypeRef.h"

using namespace StoneRing;

ArmorTypeRef::ArmorTypeRef()
{
}



ArmorTypeRef::~ArmorTypeRef()
{
}

void ArmorTypeRef::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name",pAttributes);
}

bool ArmorTypeRef::operator==(const ArmorTypeRef &lhs)
{
    if( m_name == lhs.m_name) return true;
    else return false;
}

std::string ArmorTypeRef::GetName() const
{
    return m_name;
}



