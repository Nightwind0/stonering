#include "GiveGold.h"
#include "IApplication.h"

using namespace StoneRing;

GiveGold::GiveGold()
{
}

CL_DomElement  GiveGold::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"giveGold");

	element.set_attribute("count", IntToString ( mCount ) );


	return element;
}


void GiveGold::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mCount = getImpliedInt("count",pAttributes,1);
}


GiveGold::~GiveGold()
{
}

void GiveGold::invoke()
{
	IApplication::getInstance()->getParty()->giveGold ( mCount );
}

