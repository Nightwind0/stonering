#include "SystemItem.h"

using namespace StoneRing;

SystemItem::SystemItem()
{
}

SystemItem::~SystemItem()
{
}


CL_DomElement  
SystemItem::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"systemItem");
}

