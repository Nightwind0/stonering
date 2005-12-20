#include "Skill.h"
#include "AbilityFactory.h"
#include "ItemFactory.h"
#include "IApplication.h"
#include "Animation.h"

using namespace StoneRing;

CharacterClass::CharacterClass(CL_DomElement *pElement)
{
	CL_DomNamedNodeMap attributes = pElement->get_attributes();
	AbilityFactory * pAbilityFactory = IApplication::getInstance()->getAbilityFactory();
	ItemFactory    * pItemFactory = IApplication::getInstance()->getItemFactory();


	mName = getRequiredString("name",&attributes);

	if(hasAttr("gender",&attributes))
	{
		std::string gender = getString("gender",&attributes);

		if(gender == "male") meGender = MALE;
		else if (gender == "female") meGender = FEMALE;
		else meGender = EITHER;
	}
	else meGender = EITHER;
	

	CL_DomElement child = GET_CHILD;

	while(!child.is_null())
	{
		if(child.get_node_name() == "weaponTypeRef")
		{
			mWeaponTypes.push_back ( pItemFactory->createWeaponTypeRef( &child ) );
		}
		else if ( child.get_node_name() == "armorTpeRef")
		{
			mArmorTypes.push_back ( pItemFactory->createArmorTypeRef( &child ) );
		}
		else if ( child.get_node_name() == "startingStat")
		{
			mStartingStats.push_back ( pAbilityFactory->createStartingStat ( &child ) );
		}
		else if ( child.get_node_name() == "statIncrease")
		{
			mStatIncreases.push_back ( pAbilityFactory->createStatIncrease ( &child ) );
		}
		else if ( child.get_node_name() == "skillRef")
		{
			mSkillRefs.push_back ( child.get_node_value() );
		}
			
		child = child.get_next_sibling().to_element();	
	}


}

CharacterClass::~CharacterClass()
{
}

CL_DomElement CharacterClass::createDomElement( CL_DomDocument &doc ) const
{
	return CL_DomElement(doc,"characterClass");
}

std::list<WeaponTypeRef*>::const_iterator CharacterClass::getWeaponTypeRefsBegin() const
{
	return mWeaponTypes.begin();
}

std::list<WeaponTypeRef*>::const_iterator CharacterClass::getWeaponTypeRefsEnd() const
{
	return mWeaponTypes.end();
}

std::list<ArmorTypeRef*>::const_iterator CharacterClass::getArmorTypeRefsBegin() const
{
	return mArmorTypes.begin();
}

std::list<ArmorTypeRef*>::const_iterator CharacterClass::getArmorTypeRefsEnd() const
{
	return mArmorTypes.end();
}

std::list<StartingStat*>::const_iterator CharacterClass::getStartingStatsBegin() const
{
	return mStartingStats.begin();
}

std::list<StartingStat*>::const_iterator CharacterClass::getStartingStatsEnd() const
{
	return mStartingStats.end();
}

std::list<StatIncrease*>::const_iterator CharacterClass::getStatIncreasesBegin() const
{
	return mStatIncreases.begin();
}

std::list<StatIncrease*>::const_iterator CharacterClass::getStatIncreasesEnd() const
{
	return mStatIncreases.end();
}

std::list<std::string>::const_iterator CharacterClass::getSkillRefsBegin() const
{
	return mSkillRefs.begin();
}

std::list<std::string>::const_iterator CharacterClass::getSkillRefsEnd() const
{
	return mSkillRefs.end();
}

std::string CharacterClass::getName() const
{
	return mName;
}


		
CharacterClass::eGender 
CharacterClass::getGender() const
{
	return meGender;
}
		



StatIncrease::StatIncrease(CL_DomElement * pElement )
{
	CL_DomNamedNodeMap attributes = pElement->get_attributes();	

	std::string stat = getRequiredString("stat",&attributes);
	
	meStat = CharStatFromString ( stat );

	mnPeriod = getRequiredInt("period",&attributes);
	mnIncrement = getRequiredInt("increment",&attributes );
}

StatIncrease::~StatIncrease()
{
}

CL_DomElement 
StatIncrease::createDomElement( CL_DomDocument &doc ) const
{
	return CL_DomElement(doc,"statIncrease");
}

eCharacterStat 
StatIncrease::getCharacterStat() const
{
	return meStat;
}


uint StatIncrease::getPeriod() const
{
	return mnPeriod;
}

int StatIncrease::getIncrement() const
{
	return mnIncrement;
}


	
StartingStat::StartingStat(CL_DomElement * pElement )
{
	CL_DomNamedNodeMap attributes = pElement->get_attributes();	

	std::string stat = getRequiredString("stat",&attributes);
	
	meStat = CharStatFromString ( stat );

	mnValue = getRequiredInt("value",&attributes);
}

StartingStat::~StartingStat()
{
}



CL_DomElement 
StartingStat::createDomElement(CL_DomDocument &doc ) const
{
	return CL_DomElement(doc,"startingStat");
}

eCharacterStat StartingStat::getCharacterStat() const
{
	return meStat;
}


int StartingStat::getValue() const
{
	return mnValue;
}
