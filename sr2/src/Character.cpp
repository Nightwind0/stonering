#include "ChoiceState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"
#include "Character.h"

using namespace StoneRing;

const char *statXMLLookup[] = 
{
	"hp",
	"hp_max",
	"mp",
	"mp_max",
	"str",
	"dex",
	"evd",
	"mag",
	"rst",
	"spr",
	"null",
	"encounterRate",
	"goldDropRate",
	"itemDropRate",
	"priceMultiplier"

};



StoneRing::eCharacterAttribute StoneRing::CharAttributeFromString(const std::string &str)
{

	uint numberStats = _LAST_CHARACTER_ATTR_;
	
	for(int i =0; i < numberStats; i++)
	{
		if( str == statXMLLookup[i] )
		{
			return (eCharacterAttribute)i;
		}
	}

}

StoneRing::eCommonAttribute StoneRing::CommonAttributeFromString(const std::string &str)
{
	for(int i = _LAST_CHARACTER_ATTR_; i < _LAST_COMMON_ATTR_; i++)
	{
		if(str == statXMLLookup[i])
		{
			return (eCommonAttribute)i;
		}
	}
}


uint StoneRing::CAFromString(const std::string &str)
{

	for(int i =0; i < _LAST_COMMON_ATTR_; i++)
	{
		if(str == statXMLLookup[i])
		{
			return i;
		}
	}
}

std::string StoneRing::CAToString(uint i)
{

	return statXMLLookup[i];
}



CL_DomElement  StoneRing::Character::createDomElement(CL_DomDocument &doc) const
{
	return CL_DomElement(doc,"character");
}

void StoneRing::Character::handleElement(eElement element, StoneRing::Element * )
{
	switch(element)
	{

	}
}

void StoneRing::Character::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mName = getRequiredString("name",pAttributes);
	mSpriteRef = getRequiredString("spriteResource",pAttributes);
	std::string className = getRequiredString("class",pAttributes);

	// Get the class pointer

}

void StoneRing::Character::handleText(const std::string &txt)
{
}


void StoneRing::Character::modifyAttribute(eCharacterAttribute attr, int add, float multiplier){}
int StoneRing::Character::getMaxAttribute(eCharacterAttribute attr) const { return 1; }
int StoneRing::Character::getMinAttribute(eCharacterAttribute attr) const { return 1; }
int StoneRing::Character::getAttribute(eCharacterAttribute attr) const{ return 1;}

