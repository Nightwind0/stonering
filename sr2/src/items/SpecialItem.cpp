#include "SpecialItem.h"

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



