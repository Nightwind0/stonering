#include "Weapon.h"
#include "WeaponEnhancer.h"
#include "RuneType.h"
#include "SpellRef.h"
#include "WeaponClass.h"
#include "WeaponType.h"


using namespace StoneRing;

Weapon::Weapon()
{
}

Weapon::~Weapon()
{
    clearWeaponEnhancers();
}

std::string StoneRing::Weapon::CreateWeaponName(WeaponType *pType, WeaponClass *pClass, SpellRef *pSpell, RuneType *pRune)
{
    std::ostringstream os;

    if ( pRune )
    {
        os << pRune->getRuneTypeAsString() << ' ';
    }

    os << pClass->getName() << ' ' << pType->getName();

    if(pSpell)
    {
        os << " of " << pSpell->getName();
    }

    return os.str();
}



int Weapon::modifyWeaponAttribute( eAttribute attr, int current )
{

    int value = current;

    for(std::list<WeaponEnhancer*>::iterator iter = mWeaponEnhancers.begin();
        iter != mWeaponEnhancers.end();
        iter++)
    {
        WeaponEnhancer * pEnhancer = *iter;

        if( pEnhancer->getAttribute() == attr )
        {
            value= (int)(pEnhancer->getMultiplier() * (float)value);
            value += pEnhancer->getAdd();
        }
    }

    return value;
}

float Weapon::modifyWeaponAttribute ( eAttribute attr, float current )
{

    float value = current;

    for(std::list<WeaponEnhancer*>::iterator iter = mWeaponEnhancers.begin();
        iter != mWeaponEnhancers.end();
        iter++)
    {
        WeaponEnhancer * pEnhancer = *iter;

        if( pEnhancer->getAttribute() == attr )
        {
            value *= pEnhancer->getMultiplier() ;
            value += pEnhancer->getAdd();
        }
    }

    return value;
}



//todo: Getters for weapon enhancers. need 'em.

void Weapon::clearWeaponEnhancers()
{
    for(std::list<WeaponEnhancer*>::iterator iter = mWeaponEnhancers.begin();
        iter != mWeaponEnhancers.end();
        iter++)
    {
        delete *iter;
    }
    mWeaponEnhancers.clear();
}

void Weapon::addWeaponEnhancer (WeaponEnhancer * pEnhancer)
{
    mWeaponEnhancers.push_back ( pEnhancer );
}



/* enum eAttribute
   {
   ATTACK,
   HIT,
   POISON,
   STONE,
   DEATH,
   CONFUSE,
   BERSERK,
   SLOW,
   WEAK,
   BREAK, 
   SILENCE,
   SLEEP,
   BLIND,
   STEAL_HP,
   STEAL_MP,
   DROPSTR,
   DROPDEX,
   DROPMAG
   } */



Weapon::eAttribute 
Weapon::attributeForString(const std::string str)
{
    if(str == "ATK") return ATTACK;
    else if (str == "HIT") return HIT;
    else if (str == "Steal_HP%") return STEAL_HP;
    else if (str == "Steal_MP%") return STEAL_MP;
    else if (str == "Critical%") return CRITICAL;
    else throw CL_Error("Bad Weapon Enhancer Attribute : " + str );


}



