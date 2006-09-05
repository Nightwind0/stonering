#ifndef CHARACTER_DEFINITION_H
#define CHARACTER_DEFINITION_H

#include "Element.h"
#include "Character.h" // For the enums


namespace StoneRing
{


	eCharacterAttribute CharAttributeFromString(const std::string &str); 
	eCommonAttribute CommonAttributeFromString(const std::string &str);

	// CA encompasses both common and character attributes
	uint CAFromString(const std::string &str);

	std::string CAToString(uint);

	class CharacterDefinition : 	public Element
	{
	public:
		CharacterDefinition(void);
		virtual ~CharacterDefinition(void);

		virtual CL_DomElement createDomElement(CL_DomDocument&)const;
		virtual eElement whichElement() const { return ECHARACTER; }

	private:
		virtual bool handleElement(eElement, Element * );
		virtual void loadAttributes(CL_DomNamedNodeMap *);
		std::string mSpriteRef;
		std::string mName;
		// Should these both be maps? maybe later...
		std::list<AnimationDefinition*> mAnimationDefinitions;
		std::list<WeaponTypeSprite*> mWeaponTypeSprites;
		CharacterClass * mpClass;

	};

	class AnimationDefinition : public Element
	{
	public:
		AnimationDefinition();
		virtual ~AnimationDefinition();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
		virtual eElement whichElement() const { return EANIMATIONDEFINITION; }

		SkillRef * getSkillRef() const;
		Animation * getAnimation() const;

	private:
		virtual bool handleElement(eElement, Element * );
		virtual void loadAttributes(CL_DomNamedNodeMap *);

		SkillRef * mpSkillRef;
		Animation * mpAnimation; 
		std::string mAnimationRef;
	};

	class WeaponTypeSprite : public Element
	{
	public:
		WeaponTypeSprite();
		virtual ~WeaponTypeSprite();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
		virtual eElement whichElement() const { return EWEAPONTYPESPRITE; }
		WeaponTypeRef * getWeaponTypeRef() const;
		std::string getSpriteRef() const;


	private:
		virtual bool handleElement(eElement, Element * );
		virtual void loadAttributes(CL_DomNamedNodeMap *);

		WeaponTypeRef * mpWeaponTypeRef;
		std::string mSpriteRef;
	};




};

#endif


