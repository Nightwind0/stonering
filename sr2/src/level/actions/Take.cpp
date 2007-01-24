#include "Take.h"
#include "ItemRef.h"
#include "IApplication.h"

using namespace StoneRing;

CL_DomElement  Take::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"take");

    if(mCount != 1)
        element.set_attribute("count", IntToString ( mCount ) );

    CL_DomElement  itemRef = mpItemRef->createDomElement(doc);

    element.append_child(itemRef );


    return element;

}

void Take::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{

    mCount = getImpliedInt("count",pAttributes,1);
}

bool Take::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EITEMREF:
        mpItemRef = dynamic_cast<ItemRef*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

Take::Take( ):mpItemRef(NULL)
{

}


Take::~Take()
{
    delete mpItemRef;
}

void Take::invoke()
{
    IApplication::getInstance()->getParty()->takeItem ( mpItemRef, mCount );
}


