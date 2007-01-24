#include "Pop.h"
#include "IApplication.h"

using namespace StoneRing;


Pop::Pop():mbAll(false)
{
}

Pop::~Pop()
{
}

void Pop::invoke()
{
    IApplication::getInstance()->pop(mbAll);
}

CL_DomElement Pop::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"pop");

    element.set_attribute("all", mbAll?"true":"false");

    return element;
}

void Pop::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mbAll = getImpliedBool("all",pAttributes,false);
}


