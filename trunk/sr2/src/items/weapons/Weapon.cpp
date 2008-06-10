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
    Clear_Weapon_Enhancers();
}

std::string StoneRing::Weapon::CreateWeaponName(WeaponType *pType, WeaponClass *pClass, SpellRef *pSpell, RuneType *pRune)
{
    std::ostringstream os;

    if ( pRune )
    {
        os << pRune->GetRuneTypeAsString() << ' ';
    }

    os << pClass->GetName() << ' ' << pType->GetName();

    if(pSpell)
    {
        os << " of " << pSpell->GetName();
    }

    return os.str();
}



int Weapon::ModifyWeaponAttribute( eAttribute attr, int current )
{

    int value = current;

    for(std::list<WeaponEnhancer*>::iterator iter = m_weapon_enhancers.begin();
        iter != m_weapon_enhancers.end();
        iter++)
    {
        WeaponEnhancer * pEnhancer = *iter;

        if( pEnhancer->GetAttribute() == attr )
        {
            value= (int)(pEnhancer->GetMultiplier() * (float)value);
            value += pEnhancer->GetAdd();
        }
    }

    return value;
}

float Weapon::ModifyWeaponAttribute ( eAttribute attr, float current )
{

    float value = current;

    for(std::list<WeaponEnhancer*>::iterator iter = m_weapon_enhancers.begin();
        iter != m_weapon_enhancers.end();
        iter++)
    {
        WeaponEnhancer * pEnhancer = *iter;

        if( pEnhancer->GetAttribute() == attr )
        {
            value *= pEnhancer->GetMultiplier() ;
            value += pEnhancer->GetAdd();
        }
    }

    return value;
}



//todo: Getters for weapon enhancers. need 'em.

void Weapon::Clear_Weapon_Enhancers()
{
    for(std::list<WeaponEnhancer*>::iterator iter = m_weapon_enhancers.begin();
        iter != m_weapon_enhancers.end();
        iter++)
    {
        delete *iter;
    }
    m_weapon_enhancers.clear();
}

void Weapon::Add_Weapon_Enhancer (WeaponEnhancer * pEnhancer)
{
    m_weapon_enhancers.push_back ( pEnhancer );
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
Weapon::AttributeForString(const std::string str)
{
    if(str == "ATK") return ATTACK;
    else if (str == "HIT") return HIT;
    else if (str == "Steal_HP%") return STEAL_HP;
    else if (str == "Steal_MP%") return STEAL_MP;
    else if (str == "Critical%") return CRITICAL;
    else throw CL_Error("Bad Weapon Enhancer Attribute : " + str );


}



