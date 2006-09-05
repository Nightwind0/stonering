#include "AbilityFactory.h"
#include <ClanLib/core.h>
#include "Spell.h"
#include "Animation.h"
#include "Item.h"
#include "StatusEffect.h"
#include "Skill.h"
#include "CharacterClass.h"
#include "DamageCategory.h"

using namespace StoneRing;

 
Element * AbilityFactory::createDoWeaponDamage() const
{
    return new DoWeaponDamage();
}

Element  * AbilityFactory::createDoMagicDamage() const
{
    return new DoMagicDamage ();
}

Element * AbilityFactory::createDoStatusEffect()const
{
    return new DoStatusEffect ();
}

Element          * AbilityFactory::createSpell() const
{
    return new Spell ();
}

Element * AbilityFactory::createWeaponDamageCategory() const
{
    return new WeaponDamageCategory ();
}

Element  * AbilityFactory::createMagicDamageCategory() const
{
    return new MagicDamageCategory ();
}

Element            * AbilityFactory::createAnimation() const
{
    return new Animation();
}

Element      * AbilityFactory::createMagicResistance ( ) const
{
    return new MagicResistance ();
}

Element* AbilityFactory::createAttributeEffect() const
{
    return new AttributeEffect ();
}

Element  *AbilityFactory::createStatusEffectActions() const
{
    return new StatusEffectActions();
}

Element * AbilityFactory::createStatusEffect() const
{
    return new StatusEffect();
}


Element * AbilityFactory::createStatIncrease() const
{
    return new StatIncrease();
}


Element * AbilityFactory::createAnimationSpriteRef() const
{
    return new AnimationSpriteRef();
}

Element * AbilityFactory::createPar() const
{
    return new Par;
}

Element * AbilityFactory::createSkillRef() const
{
    return new SkillRef();
}

Element * AbilityFactory::createSkill() const
{
	return new Skill();
}



bool AbilityFactory::canCreate( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    if(method == NULL) return false;
    else return true;
                
}

Element * AbilityFactory::createElement( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    Element * pElement = (this->*method)();

    return pElement;
}



AbilityFactory::factoryMethod 
AbilityFactory::getMethod(Element::eElement element) const
{
    switch(element)
    {
    case Element::EDOWEAPONDAMAGE:
        return &AbilityFactory::createDoWeaponDamage;
    case Element::EDOMAGICDAMAGE:
        return &AbilityFactory::createDoMagicDamage;
    case Element::EDOSTATUSEFFECT:
        return &AbilityFactory::createDoStatusEffect;
    case Element::ESPELL:
        return &AbilityFactory::createSpell;
    case Element::EWEAPONDAMAGECATEGORY:
        return &AbilityFactory::createWeaponDamageCategory;
    case Element::EMAGICDAMAGECATEGORY:
        return &AbilityFactory::createMagicDamageCategory;
    case Element::EANIMATION:
        return &AbilityFactory::createAnimation;
    case Element::EMAGICRESISTANCE:
        return &AbilityFactory::createMagicResistance;
    case Element::EATTRIBUTEEFFECT:
        return &AbilityFactory::createAttributeEffect;
    case Element::ESTATUSEFFECTACTIONS:
        return &AbilityFactory::createStatusEffectActions;
    case Element::ESTATUSEFFECT:
        return &AbilityFactory::createStatusEffect;
    case Element::ESTATINCREASE:
        return &AbilityFactory::createStatIncrease;
    case Element::EANIMATIONSPRITEREF:
        return &AbilityFactory::createAnimationSpriteRef;
	case Element::EPAR:
		return &AbilityFactory::createPar;
	case Element::EPREREQSKILLREF:
	case Element::ESKILL:
		return &AbilityFactory::createSkill;
    case Element::EONROUND:
    case Element::EONREMOVE:
    case Element::EONCOUNTDOWN:
    case Element::EONINVOKE:
        return &AbilityFactory::createStatusEffectActions;
    default:
        return NULL;
    }
}


