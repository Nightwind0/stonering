#include "Weapon.h"
#include "WeaponEnhancer.h"
#include "RuneType.h"
#include "WeaponClass.h"
#include "WeaponType.h"
#include "AbilityManager.h"


using namespace StoneRing;

Weapon::Weapon():m_animation_script(nullptr),m_bHasDmgCategory(false)
{
    m_eScriptMode = 0;
}

Weapon::~Weapon()
{
    Clear_Weapon_Enhancers();
}

DamageCategory::eDamageCategory Weapon::GetDamageCategory() const {
	if(m_bHasDmgCategory){
		return m_dmgCategory;
	}else{
		return GetWeaponType()->GetDamageCategory();
	}
}

void Weapon::SetDamageCategory( DamageCategory::eDamageCategory dmgCategory ) {
	m_dmgCategory = dmgCategory;
	m_bHasDmgCategory = true;
}



void Weapon::SetAnimationScript( ScriptElement* anim ){
	m_animation_script = anim;
}


ScriptElement* Weapon::GetAnimationScript() const { 
	if(m_animation_script)
		return m_animation_script;
	else
		return GetWeaponType()->GetAnimationScript();
}


Equipment::eSlot Weapon::GetSlot() const
{
    if(IsTwoHanded()) return EHAND;
    else return EANYHAND;
}


bool Weapon::ForgoAttack() const
{
    return m_eScriptMode & FORGO_ATTACK;
}

std::string Weapon::StringForAttribute ( Weapon::eAttribute attr )
{
    switch(attr)
    {
        case ATTACK:
            return "W.Atk";
        case HIT:
            return "W.Hit%";
        case CRITICAL:
            return "W.Crit%";
        default:
            assert(0);
            return "";
    }
}


std::string StoneRing::Weapon::CreateWeaponName(WeaponType *pType, WeaponClass *pClass, WeaponClass *pImbuement, RuneType *pRune)
{
    std::ostringstream os;

    if ( pRune )
    {
        os << pRune->GetRuneTypeAsString() << ' ';
    }

    os << pClass->GetName() << ' ' << pType->GetName();

    if (pImbuement)
    {
        os << ' ' << pImbuement->GetName();
    }

    return os.str();
}




double Weapon::GetWeaponAttribute ( eAttribute attr )
{
    double current = 0.0;
    switch(attr){
        case HIT:
            current = GetWeaponType()->GetBaseHit();
            break;
        case ATTACK:
            current = GetWeaponType()->GetBaseAttack();
            break;
        case CRITICAL:
            current = GetWeaponType()->GetBaseCritical();
            break;
        default:
            break;
    }

    double  value = current;

    for (std::list<WeaponEnhancer*>::iterator iter = m_weapon_enhancers.begin();
            iter != m_weapon_enhancers.end();
            iter++)
    {
        WeaponEnhancer * pEnhancer = *iter;

        if ( pEnhancer->GetAttribute() == attr )
        {
            value *= pEnhancer->GetMultiplier() ;
        }
    }

    for (std::list<WeaponEnhancer*>::iterator iter = m_weapon_enhancers.begin();
            iter != m_weapon_enhancers.end();
            iter++)
    {
        WeaponEnhancer * pEnhancer = *iter;

        if ( pEnhancer->GetAttribute() == attr )
        {
            value += pEnhancer->GetAdd();
        }
    }


    return value;
}



bool Weapon::AttributeIsInteger( Weapon::eAttribute attr ) {
	return attr == Weapon::ATTACK;
}


//todo: Getters for weapon enhancers. need 'em.

void Weapon::Clear_Weapon_Enhancers()
{
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
Weapon::AttributeForString(const std::string& str)
{

    if (str == "ATK") return ATTACK;
    else if(str == "HIT") return HIT;
    else if (str == "Critical%") return CRITICAL;
    else throw XMLException("Bad Weapon Enhancer Attribute : " + str );


}

Weapon::eScriptMode
Weapon::ScriptModeForString(const std::string& str)
{
    if(str == "attackBefore") return ATTACK_BEFORE;
    else if(str == "attackAfter") return ATTACK_AFTER;
    else if(str == "forgoAttack") return FORGO_ATTACK;
    else if(str == "world") return WORLD_ONLY;
    else throw XMLException("Bad scriptMode : " + str );
}

