
#include "Element.h"
#include "Effect.h"
#include <ClanLib/core.h>
#include "Skill.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"
#include "Level.h"


using namespace StoneRing;

const unsigned int STAT_COUNT = 8;

const char *statXMLLookup[STAT_COUNT] = 
{
		"hp_max",
		"mp_max",
		"str",
		"dex",
		"evd",
		"mag",
		"rst",
		"spr"

};



StoneRing::eCharacterStat StoneRing::CharStatFromString(const std::string &str)
{
	
		for(int i =0; i < STAT_COUNT; i++)
		{
			if( str == statXMLLookup[i] )
			{
				return (eCharacterStat)i;
			}
		}

}

void StoneRing::Skill::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mName = getRequiredString("name",pAttributes);
	mnSp = getRequiredInt("sp",pAttributes);
	mnBp = getRequiredInt("bp",pAttributes);

}

void StoneRing::Skill::handleElement(eElement element, Element * pElement)
{
	switch(element)
	{
	case EANIMATION:
	case EDOWEAPONDAMAGE:
	case EDOMAGICDAMAGE:
	case EDOSTATUSEFFECT:
	case EDOATTACK:
			mEffects.push_back(dynamic_cast<Effect*>(pElement));
			break;
	case EATTRIBUTEMODIFIER:
		mAttributeModifiers.push_back(dynamic_cast<AttributeModifier*>(pElement));
		break;
	default:
		throw CL_Error("Bad element in skill");
		//@todo EPREREQSKILLREF:
	}
}

StoneRing::Skill::Skill():mnBp(0), mnSp(0)
{

}

Skill::~Skill()
{
	std::for_each(mEffects.begin(),mEffects.end(),del_fun<Effect>());
}

std::list<Effect*>::const_iterator 
Skill::getEffectsBegin() const
{

		return mEffects.begin();
}

std::list<Effect*>::const_iterator 
Skill::getEffectsEnd() const
{
		return mEffects.end();
}

std::list<AttributeModifier*>::const_iterator 
Skill::getAttributeModifiersBegin() const
{
	return mAttributeModifiers.begin();
}

std::list<AttributeModifier*>::const_iterator 
Skill::getAttributeModifiersEnd() const
{
	return mAttributeModifiers.end();
}

std::string Skill::getName() const
{
		return mName;
}

uint Skill::getSPCost() const
{
		return mnSp;
}

uint Skill::getBPCost() const
{
		return mnBp;
}

		
std::list<std::string>::const_iterator 
Skill::getPreReqsBegin() const
{
	return mPreReqs.begin();
}

std::list<std::string>::const_iterator 
Skill::getPreReqsEnd() const
{
	return mPreReqs.end();
}

CL_DomElement Skill::createDomElement ( CL_DomDocument &doc ) const
{
	return CL_DomElement(doc,"skill");
}



