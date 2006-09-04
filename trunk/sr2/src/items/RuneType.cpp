#include "RuneType.h"

using namespace StoneRing;

RuneType::RuneType()
{
}

void RuneType::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	std::string runeType = getRequiredString("runeType",pAttributes);

	if(runeType == "none")
		meRuneType = NONE;
	else if (runeType == "rune")
		meRuneType = RUNE;
	else if (runeType == "ultraRune")
		meRuneType = ULTRA_RUNE;
	else throw CL_Error("Bogus runetype supplied.");

}

bool RuneType::operator==(const RuneType &lhs )
{
	if ( meRuneType == lhs.meRuneType )
		return true;
	else return false;
}

RuneType::~RuneType()
{
}

RuneType::eRuneType RuneType::getRuneType() const
{
	return meRuneType;
}

CL_DomElement 
RuneType::createDomElement ( CL_DomDocument &doc) const
{

	CL_DomElement element(doc,"runeType");

	switch(meRuneType)
	{
	case NONE:
		element.set_attribute("type", "none");
		break;
	case RUNE:
		element.set_attribute("type","rune");
		break;
	case ULTRA_RUNE:
		element.set_attribute("type","ultraRune");
		break;
	}

	return element;
}

std::string RuneType::getRuneTypeAsString() const
{

	//@todo : Get this from a setting.
	switch(meRuneType)
	{
	case NONE:
		return "";
	case RUNE:
		return "Rune";
	case ULTRA_RUNE:
		return "Ultra Rune";
		break;
	}

	return "";
}
