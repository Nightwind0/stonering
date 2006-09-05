#include "Give.h"
#include "ItemRef.h"
#include "IApplication.h"

using namespace StoneRing;

CL_DomElement  Give::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"give");

	if(mCount != 1)
		element.set_attribute("count", IntToString ( mCount ) );

	CL_DomElement  itemRef = mpItemRef->createDomElement(doc);

	element.append_child(itemRef );

	return element;

}

void Give::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{

	mCount = getImpliedInt("count",pAttributes,1);
}

bool Give::handleElement(eElement element, Element * pElement)
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

Give::Give( ):mpItemRef(NULL),mCount(1)
{

}
Give::~Give()
{
	delete mpItemRef;
}

void Give::invoke()
{
	IApplication::getInstance()->getParty()->giveItem ( mpItemRef, mCount );
}


