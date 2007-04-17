#include "IconRef.h"

using namespace StoneRing;

IconRef::IconRef()
{
}

IconRef::~IconRef()
{
}

std::string IconRef::getIcon() const
{
    return mIcon;
}

CL_DomElement  IconRef::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"iconRef");
}


bool IconRef::handleElement(eElement element, Element * pElement )
{
    return false;
}

void IconRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{

}

void IconRef::handleText(const std::string &text)
{
    mIcon = text;
}



