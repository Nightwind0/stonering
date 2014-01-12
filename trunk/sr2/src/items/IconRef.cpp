#include "IconRef.h"

using namespace StoneRing;

IconRef::IconRef()
{
}

IconRef::~IconRef()
{
}

std::string IconRef::GetIcon() const
{
    return m_icon;
}

bool IconRef::handle_element(eElement element, Element * pElement )
{
    return false;
}

void IconRef::load_attributes(clan::DomNamedNodeMap attributes)
{

}

void IconRef::handle_text(const std::string &text)
{
    m_icon = text;
}



