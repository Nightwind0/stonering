#include "SystemItem.h"

using namespace StoneRing;

SystemItem::SystemItem()
{
}

SystemItem::~SystemItem()
{
}


bool SystemItem::operator == ( const ItemRef &ref )
{
    return ref.GetType() == ItemRef::NAMED_ITEM &&
    ref.GetItemName() == GetName();
}



