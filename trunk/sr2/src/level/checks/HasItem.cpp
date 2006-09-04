#include "HasItem.h"
#include "IApplication.h"

using namespace StoneRing;


CL_DomElement  HasItem::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"hasItem");

    if(mbNot) element.set_attribute("not","true");

    if(mCount != 1)
        element.set_attribute("count", IntToString(mCount));

    CL_DomElement e = mpItemRef->createDomElement(doc);
    element.append_child (e );


    return element;
}

bool HasItem::handleElement(eElement element, Element *pElement)
{
    if(element == EITEMREF)
    {
        mpItemRef = dynamic_cast<ItemRef*>(pElement);
		return true;
    }
    else return false;
}
 
void HasItem::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mbNot = getImpliedBool("not",pAttributes,false);

    mCount = getImpliedInt("count",pAttributes,1);
}

HasItem::HasItem( ):mpItemRef(NULL)
{
    
}

HasItem::~HasItem()
{
    delete mpItemRef;
}

bool HasItem::evaluate()
{
    if(mbNot) return ! (IApplication::getInstance()->getParty()->hasItem(mpItemRef, mCount )) ;
    else  return (IApplication::getInstance()->getParty()->hasItem(mpItemRef, mCount )) ;
}
