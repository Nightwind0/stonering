#include "AbilityFactory.h"
#include <ClanLib/core.h>
#include "Spell.h"
#include "Animation.h"
#include "Item.h"
#include "StatusEffect.h"

using namespace StoneRing;




DoWeaponDamage * AbilityFactory::createDoWeaponDamage(CL_DomElement * pElement) const
{
    return new DoWeaponDamage( pElement );
}

DoMagicDamage  * AbilityFactory::createDoMagicDamage(CL_DomElement * pElement) const
{
    return new DoMagicDamage ( pElement );
}

DoStatusEffect * AbilityFactory::createDoStatusEffect(CL_DomElement * pElement)const
{
    return new DoStatusEffect ( pElement );
}

Spell          * AbilityFactory::createSpell(CL_DomElement * pElement) const
{
    return new Spell ( pElement );
}

WeaponDamageCategory * AbilityFactory::createWeaponDamageCategory(CL_DomElement * pElement) const
{
    return new WeaponDamageCategory ( pElement );
}

MagicDamageCategory  * AbilityFactory::createMagicDamageCategory(CL_DomElement * pElement) const
{
    return new MagicDamageCategory ( pElement );
}

Animation            * AbilityFactory::createAnimation(CL_DomElement * pElement) const
{
    return new Animation( pElement );
}

MagicResistance      * AbilityFactory::createMagicResistance ( CL_DomElement * pElement) const
{
    return new MagicResistance ( pElement );
}

AttributeEffect* AbilityFactory::createAttributeEffect(CL_DomElement *pElement) const
{
	return new AttributeEffect ( pElement );
}

StatusEffectActions  *AbilityFactory::createStatusEffectActions(CL_DomElement * pElement) const
{
	return new StatusEffectActions( pElement );
}

StatusEffect * AbilityFactory::createStatusEffect(CL_DomElement * pElement) const
{
	return new StatusEffect( pElement );
}