#include "SpecialItem.h"

using namespace StoneRing;

SpecialItem::SpecialItem()
{
}

SpecialItem::~SpecialItem()
{
}

CL_DomElement  
SpecialItem::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"specialItem");
}

