#ifndef SR_CHARACTER_H
#define SR_CHARACTER_H

#include "sr_defines.h"

namespace StoneRing{

	class CharacterClass; //fwd
	class SkillRef;
	class Animation;
	class WeaponTypeRef;
	class AnimationDefinition;
	class WeaponTypeSprite;

	//str|dex|evd|mag|rst|spr
	enum eCharacterAttribute
	{
		CA_HP,
		CA_HPMAX,
		CA_MP,
		CA_MPMAX,
		CA_STR,
		CA_DEX,
		CA_EVD,
		CA_MAG,
		CA_RST,
		CA_SPR,
		_LAST_CHARACTER_ATTR_
	};

	enum eCommonAttribute
	{
		CA_ENCOUNTER_RATE = _LAST_CHARACTER_ATTR_,
		CA_GOLD_DROP_RATE,
		CA_ITEM_DROP_RATE,
		CA_PRICE_MULTIPLIER,
		_LAST_COMMON_ATTR_
	};


	class ICharacter
	{
	public:
		virtual void modifyAttribute(eCharacterAttribute attr, int add, float multiplier)=0;
		virtual int getMaxAttribute(eCharacterAttribute attr) const = 0;
		virtual int getMinAttribute(eCharacterAttribute attr) const = 0;
		virtual int getAttribute(eCharacterAttribute attr) const = 0;
	private:
	};


	class ICharacterGroup
	{
	public:
		virtual uint getCharacterCount() const = 0;
		virtual uint getSelectedCharacterIndex() const = 0;
		virtual uint getCasterCharacterIndex() const = 0;
		virtual ICharacter * getCharacter(uint index) const = 0;
		virtual ICharacter * getSelectedCharacter() const = 0;
		virtual ICharacter * getCasterCharacter() const = 0;
	private:
	};

	class Character : public ICharacter
	{
	public:
		Character();
		virtual void modifyAttribute(eCharacterAttribute attr, int add, float multiplier);
		virtual int getMaxAttribute(eCharacterAttribute attr) const ;
		virtual int getMinAttribute(eCharacterAttribute attr) const ;
		virtual int getAttribute(eCharacterAttribute attr) const;


	private:
		std::string mName;
	};

};
#endif
