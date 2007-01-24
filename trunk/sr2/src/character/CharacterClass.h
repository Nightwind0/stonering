#ifndef SR_CHARACTER_CLASS
#define SR_CHARACTER_CLASS

#include "Element.h"
#include "Effect.h"
#include <ClanLib/core.h>
#include "Character.h"

namespace StoneRing
{

	class WeaponTypeRef;
	class ArmorTypeRef;
	class StatIncrease;
	class SkillRef;

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

		std::list<StatIncrease*>::const_iterator getStatIncreasesBegin() const;
		std::list<StatIncrease*>::const_iterator getStatIncreasesEnd() const;

		std::list<SkillRef*>::const_iterator getSkillRefsBegin() const;
		std::list<SkillRef*>::const_iterator getSkillRefsEnd() const;

		std::string getName() const;


		enum eGender { MALE, FEMALE, EITHER };

		eGender getGender() const;


	private:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		std::string mName;
		eGender meGender;
		std::list<WeaponTypeRef*> mWeaponTypes;
		std::list<ArmorTypeRef*> mArmorTypes;
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

		float getMultiplier() const;	
		float getBase() const;
	private:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		float mfMultiplier;
		float mfBase;
		eCharacterAttribute meStat;
	};



};

#endif


