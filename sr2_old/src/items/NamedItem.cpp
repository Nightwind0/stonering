#include "NamedItem.h"
#include "IconRef.h"
#include "GraphicsManager.h"

using namespace StoneRing;



NamedItemElement::NamedItemElement()
{
}

NamedItemElement::~NamedItemElement()
{
}

void NamedItemElement::load_attributes(clan::DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    std::string dropRarity = get_required_string("dropRarity",attributes);
    m_eDropRarity = Item::DropRarityFromString ( dropRarity );
    m_nMaxInventory = get_implied_int("maxInventory",attributes,99);
}

bool NamedItemElement::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EICONREF:{
	std::string icon_ref = dynamic_cast<IconRef*>(pElement)->GetIcon();
	m_icon = GraphicsManager::GetIcon(icon_ref);
        break;
    }

    default:
        return false;
    }
    return true;
}

void NamedItemElement::load_finished()
{

}


std::string NamedItemElement::GetName() const
{
    return m_name;
}

uint NamedItemElement::GetMaxInventory() const
{
    return m_nMaxInventory;
}

Item::eDropRarity NamedItemElement::GetDropRarity() const
{
    return m_eDropRarity;
}

void NamedItemElement::SetName ( const std::string &name )
{
    m_name = name;
}

void NamedItemElement::SetMaxInventory ( uint max )
{
    m_nMaxInventory = max;
}

void NamedItemElement::SetDropRarity( Item::eDropRarity rarity )
{
    m_eDropRarity = rarity;
}

clan::Image NamedItemElement::GetIcon() const
{
    return m_icon;
}


