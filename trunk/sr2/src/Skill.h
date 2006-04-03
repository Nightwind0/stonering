#ifndef SR_SKILL_H
#define SR_SKILL_H

#include "Element.h"
#include "Effect.h"
#include <ClanLib/core.h>
//#include "Item.h"
#include "Character.h"



namespace StoneRing
{
	class AttributeModifier;

	class StatIncrease;
	class StartingStat;
	class ArmorTypeRef;
	class WeaponTypeRef;
	class SkillRef;
	class SpellRef;

	class Skill : public Element
	{
	public:
		Skill();
		~Skill();

		enum eType { BATTLE, SWITCH };

		virtual eElement whichElement() const{ return ESKILL; }

		std::list<Effect*>::const_iterator getEffectsBegin() const;
		std::list<Effect*>::const_iterator getEffectsEnd() const;

		std::list<AttributeModifier*>::const_iterator getAttributeModifiersBegin() const;
		std::list<AttributeModifier*>::const_iterator getAttributeModifiersEnd() const;

		std::string getName() const;
		uint getSPCost() const;
		uint getBPCost() const;
		uint getMinLevel() const;
		
		std::list<SkillRef*>::const_iterator getPreReqsBegin() const;
		std::list<SkillRef*>::const_iterator getPreReqsEnd() const;

		SpellRef * getSpellRef() const;

		CL_DomElement createDomElement ( CL_DomDocument &doc ) const;

		eType getType() const { return meType; }
	private:
		virtual void handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		std::string mName;
		std::list<Effect*> mEffects;
		std::list<AttributeModifier*> mAttributeModifiers;
		std::list<SkillRef*> mPreReqs;
		SpellRef * mpSpellRef;
		uint mnSp;
		uint mnBp;
		uint mnMinLevel;
		eType meType;
	};

	class SkillRef: public Element
	{
	public:
		SkillRef();
		~SkillRef();
		virtual eElement whichElement() const{ return ESKILLREF; }
		std::string getRef() const;
		uint getSPCost() const;
		uint getBPCost() const;
		uint getMinLevel() const;

		CL_DomElement createDomElement ( CL_DomDocument &doc )const;
	private:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		uint mnSp;
		uint mnBp;
		uint mnMinLevel;
		std::string mRef;
	};

	class CharacterClass : public Element
	{
	public:
		CharacterClass();
		~CharacterClass();
		virtual eElement whichElement() const{ return ECHARACTERCLASS; }
		CL_DomElement createDomElement( CL_DomDocument &doc ) const;

		std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsBegin() const;
		std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsEnd() const;

		std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsBegin() const;
		std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsEnd() const;

		std::list<StartingStat*>::const_iterator getStartingStatsBegin() const;
		std::list<StartingStat*>::const_iterator getStartingStatsEnd() const;

		std::list<StatIncrease*>::const_iterator getStatIncreasesBegin() const;
		std::list<StatIncrease*>::const_iterator getStatIncreasesEnd() const;

		std::list<SkillRef*>::const_iterator getSkillRefsBegin() const;
		std::list<SkillRef*>::const_iterator getSkillRefsEnd() const;

		std::string getName() const;


		enum eGender { MALE, FEMALE, EITHER };

		eGender getGender() const;


	private:
		virtual void handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		std::string mName;
		eGender meGender;
		std::list<WeaponTypeRef*> mWeaponTypes;
		std::list<ArmorTypeRef*> mArmorTypes;
		std::list<StartingStat*> mStartingStats;
		std::list<StatIncrease*> mStatIncreases;
		std::list<SkillRef*> mSkillRefs;
	};

	class StatIncrease : public Element
	{
	public:
		StatIncrease( );
		~StatIncrease();
		virtual eElement whichElement() const{ return ESTATINCREASE; }
		CL_DomElement createDomElement( CL_DomDocument &doc ) const;

		eCharacterAttribute getCharacterStat() const;

		uint getPeriod() const;	
		int getIncrement() const;
	private:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		uint mnPeriod;
		int mnIncrement;
		eCharacterAttribute meStat;
	};

	class StartingStat : public Element
	{
	public:
		StartingStat( );
		~StartingStat();
		virtual eElement whichElement() const{ return ESTARTINGSTAT; }
		CL_DomElement createDomElement(CL_DomDocument &doc )const;

		eCharacterAttribute getCharacterStat() const;

		int getValue() const;
	private:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		int mnValue;
		eCharacterAttribute meStat;
	};
}

#endif
