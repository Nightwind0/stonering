#include "SpecialItem.h"
#include "Description.h"

using namespace StoneRing;

SpecialItem::SpecialItem()
{
}

SpecialItem::~SpecialItem()
{
}


bool SpecialItem::operator==(const ItemRef& ref)
{
     return (ref.GetType() == ItemRef::NAMED_ITEM && 
     ref.GetItemName() == GetName());
}

bool SpecialItem::handle_element(eElement element, Element * pElement )
{
    if(NamedItemElement::handle_element(element,pElement))
        return true;
    if(element == Element::EDESCRIPTION){
        m_description = dynamic_cast<Description*>(pElement)->GetText();
        delete pElement;
        return true;
    }
    
    return false;
}


