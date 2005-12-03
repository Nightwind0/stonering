#ifndef SR_SKILL_H
#define SR_SKILL_H

#include "Element.h"
#include "Effect.h"
#include <ClanLib/core.h>
//#include "Item.h"

namespace StoneRing
{

	enum eCharacterStat { HP_MAX, MP_MAX, STR, DEX, EVD, MAG, RST, SPR };

	eCharacterStat CharStatFromString(const std::string &str); 

	class StatIncrease;
	class StartingStat;
	class ArmorTypeRef;
	class WeaponTypeRef;

	class Skill : public Element
	{
	public:
		Skill(CL_DomElement * pElement);
		~Skill();

		std::list<Effect*>::const_iterator getEffectsBegin() const;
		std::list<Effect*>::const_iterator getEffectsEnd() const;

		std::string getName() const;
		uint getSPCost() const;
		uint getBPCost() const;
		
		std::list<std::string>::const_iterator getPreReqsBegin() const;
		std::list<std::string>::const_iterator getPreReqsEnd() const;

		CL_DomElement createDomElement ( CL_DomDocument &doc );
	private:
		std::string mName;
		std::list<Effect*> mEffects;
		std::list<std::string> mPreReqs;
		uint mnSp;
		uint mnBp;
	};

	class CharacterClass : public Element
	{
	public:
		CharacterClass(CL_DomElement *pElement);
		~CharacterClass();

		CL_DomElement createDomElement( CL_DomDocument &doc ) const;

		std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsBegin() const;
		std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsEnd() const;

		std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsBegin() const;
		std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsEnd() const;

		std::list<StartingStat*>::const_iterator getStartingStatsBegin() const;
		std::list<StartingStat*>::const_iterator getStartingStatsEnd() const;

		std::list<StatIncrease*>::const_iterator getStatIncreasesBegin() const;
		std::list<StatIncrease*>::const_iterator getStatIncreasesEnd() const;

		std::list<std::string>::const_iterator getSkillRefsBegin() const;
		std::list<std::string>::const_iterator getSkillRefsEnd() const;

		std::string getName() const;


		enum eGender { MALE, FEMALE, EITHER };

		eGender getGender() const;


	private:
		std::string mName;
		eGender meGender;
		std::list<WeaponTypeRef*> mWeaponTypes;
		std::list<ArmorTypeRef*> mArmorTypes;
		std::list<StartingStat*> mStartingStats;
		std::list<StatIncrease*> mStatIncreases;
		std::list<std::string> mSkillRefs;
	};

	class StatIncrease : public Element
	{
	public:
		StatIncrease(CL_DomElement * pElement );
		~StatIncrease();

		CL_DomElement createDomElement( CL_DomDocument &doc ) const;

		eCharacterStat getCharacterStat() const;

		uint getPeriod() const;	
		int getIncrement() const;
	private:
		uint mnPeriod;
		int mnIncrement;
		eCharacterStat meStat;
	};

	class StartingStat : public Element
	{
	public:
		StartingStat(CL_DomElement * pElement );
		~StartingStat();

		CL_DomElement createDomElement(CL_DomDocument &doc )const;

		eCharacterStat getCharacterStat() const;

		int getValue() const;
	private:
		int mnValue;
		eCharacterStat meStat;
	};
}

#endif
