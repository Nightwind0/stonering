#include "StartBattle.h"
#include "IApplication.h"


using namespace StoneRing;

CL_DomElement  StartBattle::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"startBattle");

	element.set_attribute("isBoss", mbIsBoss? "true":"false");
	element.set_attribute("count", IntToString (mCount ) );
	element.set_attribute("monster", mMonster );

	return element;
}

bool StartBattle::handleElement(eElement element, Element *pElement)
{
	return false;
}

void StartBattle::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mbIsBoss = getImpliedBool("isBoss",pAttributes,false);

	mCount = getImpliedInt("count",pAttributes,1);

	mMonster = getRequiredString("monster",pAttributes);
}

StartBattle::StartBattle():mbIsBoss(false),mCount(1)
{

}

StartBattle::~StartBattle()
{
}

void StartBattle::invoke()
{
	IApplication::getInstance()->startBattle ( mMonster, mCount, mbIsBoss ) ;
}


