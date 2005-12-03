
#include "Element.h"
#include "Effect.h"
#include <ClanLib/core.h>
#include "Skill.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"

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

StoneRing::Skill::Skill(CL_DomElement * pElement):mnBp(0), mnSp(0)
{

    CL_DomNamedNodeMap attributes = pElement->get_attributes();
	AbilityFactory * pFactory = IApplication::getInstance()->getAbilityFactory();

	mName = getRequiredString("name",&attributes);

	mnSp = getRequiredInt("sp",&attributes);
	mnBp = getRequiredInt("bp",&attributes);

	CL_DomElement child = GET_CHILD;


    while(!child.is_null())
    {
		std::string name = child.get_node_name();

		if(name == "prereqSkillRef")
		{
			CL_DomNamedNodeMap prAttr = child.get_attributes();

			mPreReqs.push_back ( getRequiredString("skillName",&prAttr));
		}
		else if (name == "animation")
		{
			mEffects.push_back ( static_cast<Effect*>(pFactory->createAnimation(&child)));
		}
		else if (name == "doWeaponDamage")
		{
			mEffects.push_back ( pFactory->createDoWeaponDamage(&child));
		}
		else if (name == "doMagicDamage")
		{
			mEffects.push_back ( pFactory->createDoMagicDamage(&child));
		}
		else if (name == "doStatusEffect")
		{
			mEffects.push_back ( pFactory->createDoStatusEffect(&child));
		}

		child = child.get_next_sibling().to_element();

	}
}

Skill::~Skill()
{
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

CL_DomElement Skill::createDomElement ( CL_DomDocument &doc )
{
	return CL_DomElement(doc,"skill");
}



