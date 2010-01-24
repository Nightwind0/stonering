#include "NamedItem.h"
#include "IconRef.h"

using namespace StoneRing;

void NamedItemElement::load_attributes(CL_DomNamedNodeMap attributes)
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
    case EICONREF:
        m_icon_ref = dynamic_cast<IconRef*>(pElement)->GetIcon();
        break;
    case EREGULARITEM:
    case EUNIQUEWEAPON:
    case EUNIQUEARMOR:
    case ERUNE:
    case ESPECIALITEM:
    case ESYSTEMITEM:
        m_pNamedItem = dynamic_cast<NamedItem*>(pElement);
        break;
    default:
        return false;
    }
    return true;
}

void NamedItemElement::load_finished()
{
    if(m_pNamedItem == NULL) throw CL_Exception("No named item within a named item element :" + m_name);
    m_pNamedItem->SetIconRef( m_icon_ref );
    m_pNamedItem->SetName ( m_name );
    m_pNamedItem->SetMaxInventory ( m_nMaxInventory );
    m_pNamedItem->SetDropRarity( m_eDropRarity );
}

NamedItemElement::NamedItemElement ():m_pNamedItem(NULL),m_eDropRarity(Item::NEVER),m_nMaxInventory(0)
{
}

NamedItemElement::~NamedItemElement()
{

}


NamedItem *
NamedItemElement::GetNamedItem() const
{
    return m_pNamedItem;
}

std::string NamedItemElement::GetIconRef() const
{
    return m_icon_ref;
}

uint NamedItemElement::GetMaxInventory() const
{
    return m_nMaxInventory;
}

Item::eDropRarity
NamedItemElement::GetDropRarity() const
{
    return m_eDropRarity;
}


std::string
NamedItemElement::GetName() const
{
    return m_name;
}




NamedItem::NamedItem()
{
}

NamedItem::~NamedItem()
{
}

bool NamedItem::operator== ( const ItemRef &ref )
{
    if( ref.GetType() == ItemRef::NAMED_ITEM
        && ref.GetNamedItemRef()->GetItemName() == m_name)
        return true;
    else return false;
}




std::string NamedItem::GetIconRef() const
{
    return m_icon_ref;
}

std::string NamedItem::GetName() const
{
    return m_name;
}

uint NamedItem::GetMaxInventory() const
{
    return m_nMaxInventory;
}

NamedItem::eDropRarity NamedItem::GetDropRarity() const
{
    return m_eDropRarity;
}

void NamedItem::SetIconRef(const std::string &ref)
{
    m_icon_ref = ref;
}

void NamedItem::SetName ( const std::string &name )
{
    m_name = name;
}

void NamedItem::SetMaxInventory ( uint max )
{
    m_nMaxInventory = max;
}

void NamedItem::SetDropRarity( Item::eDropRarity rarity )
{
    m_eDropRarity = rarity;
}




