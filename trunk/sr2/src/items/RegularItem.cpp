#include "RegularItem.h"
#include "Action.h"

using namespace StoneRing;


RegularItem::RegularItem()
{
}

RegularItem::~RegularItem()
{
	for( std::list<Action*>::iterator iter = mActions.begin();
         iter != mActions.end();
         iter++)
	{
		delete *iter;
	}
}

// Execute all actions.
void RegularItem::invoke()
{
}




RegularItem::eUseType 
RegularItem::getUseType() const
{
	return meUseType;
}

RegularItem::eTargetable
RegularItem::getTargetable() const
{
	return meTargetable;
}

RegularItem::eDefaultTarget 
RegularItem::getDefaultTarget() const
{
	return meDefaultTarget;
}

bool RegularItem::isReusable() const
{
	return mbReusable;
}


uint RegularItem::getValue() const
{
	return mnValue;
}

uint RegularItem::getSellValue() const
{
	return mnSellValue;
}

RegularItem::eUseType 
RegularItem::UseTypeFromString ( const std::string &str )
{

	eUseType type = WORLD;

	if(str == "battle") type = BATTLE;
	else if (str == "world") type = WORLD;
	else if (str == "both") type = BOTH;
	else throw CL_Error("Bad targetable on regular item. " + str);

	return type;

}



RegularItem::eTargetable 
RegularItem::TargetableFromString ( const std::string &str )
{

	eTargetable targetable = SINGLE;

	if(str == "all") targetable = ALL;
	else if (str == "single") targetable = SINGLE;
	else if (str == "either") targetable = EITHER;
	else if (str == "self_only") targetable = SELF_ONLY;
	else throw CL_Error("Bad targetable on regular item. " + str);


	return targetable;
}

void RegularItem::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mnValue = getRequiredInt("value",pAttributes);

	mnSellValue = mnValue / 2;


	std::string useType = getRequiredString("use",pAttributes);
	meUseType = UseTypeFromString ( useType );    

	std::string targetable = getRequiredString("targetable",pAttributes); 
	meTargetable = TargetableFromString ( targetable );    

	if(hasAttr("sellValueMultiplier", pAttributes))
	{
		float multiplier = getFloat("sellValueMultiplier",pAttributes);
		mnSellValue = (int)(mnValue * multiplier);
	}

	mbReusable = getRequiredBool("reusable",pAttributes);

	if(hasAttr("defaultTarget",pAttributes))
	{
		std::string str = getString("defaultTarget",pAttributes);

		if( str == "party" )
			meDefaultTarget = PARTY;
		else if (str == "monsters")
			meDefaultTarget = MONSTERS;
		else throw CL_Error("Bogus default target on regular item.");

	}
	else
	{
		meDefaultTarget = PARTY;
	}

}

bool RegularItem::handleElement(eElement element, Element * pElement)
{
	if(isAction(element))
	{
		mActions.push_back( dynamic_cast<Action*>(pElement) );
		return true;
	}
	else return false;
}

void RegularItem::loadItem ( CL_DomElement * pElement )
{

}

CL_DomElement  RegularItem::createDomElement(CL_DomDocument &doc) const
{
	return CL_DomElement (doc,"regularItem");
}



