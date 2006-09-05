#ifndef SR_ABILITY_FACTORY_H
#define SR_ABILITY_FACTORY_H

#include "IFactory.h"

#include <ClanLib/core.h>

namespace StoneRing
{

	class CharacterClass;
    class DoWeaponDamage;
    class DoMagicDamage;
    class DoStatusEffect;
    class Spell;
    class WeaponDamageCategory;
    class MagicDamageCategory;
    class Animation;
	class AnimationSpriteRef;
    class MagicResistance;
    class AttributeEffect;
    class StatusEffectActions;
    class StatusEffect;
    class StatIncrease;
	class AnimationSpriteRef;
	class Skill;
	class SkillRef;


    class AbilityFactory : public IFactory
	{
	public:
	    AbilityFactory(){}
	    virtual ~AbilityFactory(){}

	    virtual bool canCreate( Element::eElement element );
	    virtual Element * createElement( Element::eElement element );
	protected:


	    Element * createDoWeaponDamage() const;
	    Element * createDoMagicDamage() const;
	    Element * createDoStatusEffect()const;
	    Element * createSpell() const;
	    Element * createWeaponDamageCategory() const;
	    Element * createMagicDamageCategory() const;
	    Element * createAnimation() const;
	    Element * createMagicResistance ( ) const;
	    Element * createAttributeEffect() const;
	    Element * createStatusEffectActions() const;
	    Element * createStatusEffect() const;
	    Element * createStatIncrease() const;
		Element * createAnimationSpriteRef() const;
		Element * createPar() const;
		Element * createSkillRef() const;
		Element * createSkill() const;


	private:
	    typedef Element * (AbilityFactory::*factoryMethod)() const;

	    factoryMethod getMethod(Element::eElement element) const;

	};


};

#endif


