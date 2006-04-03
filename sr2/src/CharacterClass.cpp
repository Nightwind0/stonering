#include "Skill.h"
#include "AbilityFactory.h"
#include "ItemFactory.h"
#include "IApplication.h"
#include "Animation.h"

using namespace StoneRing;


SkillRef::SkillRef()
{
}

SkillRef::~SkillRef()
{
}

std::string SkillRef::getRef() const
{
	return mRef;
}

uint SkillRef::getSPCost() const
{
	return mnSp;
}

uint SkillRef::getBPCost() const
{
	return mnBp;
}

uint SkillRef::getMinLevel() const
{
	return mnMinLevel;
}

CL_DomElement SkillRef::createDomElement ( CL_DomDocument &doc )const
{
	return CL_DomElement(doc,"skillRef");
}


void SkillRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mRef = getRequiredString("skillName", pAttributes);
	mnSp = getImpliedInt("overrideSp", pAttributes,0);
	mnBp = getImpliedInt("overrideBp", pAttributes,0);
	mnMinLevel = getImpliedInt("overrideMinLevel",pAttributes,0);
}

void CharacterClass::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mName = getRequiredString("name",pAttributes);


	std::string gender = getImpliedString("gender",pAttributes, "either");

	if(gender == "male") meGender = MALE;		
	else if (gender == "female") meGender = FEMALE;	
	else if (gender == "either") meGender = EITHER;
}

void CharacterClass::handleElement(eElement element, Element * pElement)
{
	switch(element)
	{
	case EWEAPONTYPEREF:
		mWeaponTypes.push_back ( dynamic_cast<WeaponTypeRef*>(pElement) );
		break;
	case EARMORTYPEREF:
		mArmorTypes.push_back ( dynamic_cast<ArmorTypeRef*>(pElement) );
		break;
	case ESTARTINGSTAT:
		mStartingStats.push_back ( dynamic_cast<StartingStat*>(pElement) );
		break;
	case ESTATINCREASE:
		mStatIncreases.push_back ( dynamic_cast<StatIncrease*>(pElement) );
		break;
	case ESKILLREF:
		mSkillRefs.push_back( dynamic_cast<SkillRef*>(pElement));
		break;
	default:
		break;
	}
}

CharacterClass::CharacterClass()
{
	std::for_each(mWeaponTypes.begin(),mWeaponTypes.end(),del_fun<WeaponTypeRef>());
	std::for_each(mArmorTypes.begin(),mArmorTypes.end(),del_fun<ArmorTypeRef>());
	std::for_each(mStartingStats.begin(),mStartingStats.end(),del_fun<StartingStat>());
	std::for_each(mStatIncreases.begin(),mStatIncreases.end(),del_fun<StatIncrease>());
	std::for_each(mSkillRefs.begin(),mSkillRefs.end(),del_fun<SkillRef>());
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

std::list<SkillRef*>::const_iterator CharacterClass::getSkillRefsBegin() const
{
	return mSkillRefs.begin();
}

std::list<SkillRef*>::const_iterator CharacterClass::getSkillRefsEnd() const
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
		


void StatIncrease::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	std::string stat = getRequiredString("stat",pAttributes);
	
	meStat = CharAttributeFromString ( stat );

	mnPeriod = getRequiredInt("period",pAttributes);
	mnIncrement = getRequiredInt("increment",pAttributes );
}


StatIncrease::StatIncrease( )
{

}

StatIncrease::~StatIncrease()
{
}

CL_DomElement 
StatIncrease::createDomElement( CL_DomDocument &doc ) const
{
	return CL_DomElement(doc,"statIncrease");
}

eCharacterAttribute 
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


void StartingStat::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	std::string stat = getRequiredString("stat",pAttributes);
	meStat = CharAttributeFromString ( stat );
	mnValue = getRequiredInt("value",pAttributes);
}
	
StartingStat::StartingStat()
{

}

StartingStat::~StartingStat()
{
}



CL_DomElement 
StartingStat::createDomElement(CL_DomDocument &doc ) const
{
	return CL_DomElement(doc,"startingStat");
}

eCharacterAttribute StartingStat::getCharacterStat() const
{
	return meStat;
}


int StartingStat::getValue() const
{
	return mnValue;
}
