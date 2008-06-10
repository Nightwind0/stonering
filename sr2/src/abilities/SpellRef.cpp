#include "SpellRef.h"

using namespace StoneRing;

void SpellRef::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_eSpellType = Magic::TypeOf(get_required_string("type",pAttributes));

    if(m_eSpellType == Magic::UNKNOWN)
        throw CL_Error("Bad magic type in spell ref");
}

void SpellRef::handle_text(const std::string &text)
{
    m_name = text;
}

SpellRef::SpellRef(  )
{
}

SpellRef::~SpellRef()
{
}


bool SpellRef::operator==(const SpellRef &lhs )
{
    if ( m_eSpellType == lhs.m_eSpellType &&
         m_name == lhs.m_name )
    {
        return true;
    }
    else return false;
}


Magic::eMagicType SpellRef::GetSpellType() const
{
    return m_eSpellType;
}

std::string SpellRef::GetName() const
{
    return m_name;
}

