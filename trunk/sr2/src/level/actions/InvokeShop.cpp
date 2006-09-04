#include "InvokeShop.h"
#include "IApplication.h"


using namespace StoneRing;

InvokeShop::InvokeShop()
{
}


CL_DomElement  InvokeShop::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"invokeShop");

	element.set_attribute("shopType", mShopType );

	return element;
}

void InvokeShop::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mShopType = getRequiredString("shopType",pAttributes);
}


InvokeShop::~InvokeShop()
{
}

void InvokeShop::invoke()
{
	IApplication::getInstance()->invokeShop ( mShopType );
}
