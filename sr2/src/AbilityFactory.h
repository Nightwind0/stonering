#ifndef SR_ABILITY_FACTORY_H
#define SR_ABILITY_FACTORY_H



#include <ClanLib/core.h>

namespace StoneRing
{

    class DoWeaponDamage;
    class DoMagicDamage;
    class DoStatusEffect;
    class Spell;
    class WeaponDamageCategory;
    class MagicDamageCategory;
    class Animation;
    class MagicResistance;
 
class AbilityFactory
{
 public:
    AbilityFactory(){}
    ~AbilityFactory(){}


    DoWeaponDamage * createDoWeaponDamage(CL_DomElement * pElement) const;
    DoMagicDamage  * createDoMagicDamage(CL_DomElement * pElement) const;
    DoStatusEffect * createDoStatusEffect(CL_DomElement * pElement)const;
    Spell          * createSpell(CL_DomElement * pElement) const;
    WeaponDamageCategory * createWeaponDamageCategory(CL_DomElement * pElement) const;
    MagicDamageCategory  * createMagicDamageCategory(CL_DomElement * pElement) const;
    Animation            * createAnimation(CL_DomElement * pElement) const;
    MagicResistance      * createMagicResistance ( CL_DomElement * pElement) const;

 private:
};


};

#endif
