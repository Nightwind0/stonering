#include "ArmorClassRef.h"


using namespace StoneRing;

ArmorClassRef::ArmorClassRef()
{
}


ArmorClassRef::~ArmorClassRef()
{
}

void ArmorClassRef::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name",pAttributes);
}

bool ArmorClassRef::operator==(const ArmorClassRef &lhs)
{
    if( m_name == lhs.m_name) return true;
    else return false;
}

std::string 
ArmorClassRef::GetName() const
{
    return m_name;
}



