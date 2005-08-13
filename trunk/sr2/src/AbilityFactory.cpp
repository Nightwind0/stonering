#include "AbilityFactory.h"
#include <ClanLib/core.h>
#include "Spell.h"
#include "Animation.h"
#include "Item.h"

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
