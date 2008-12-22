#include "GeneratedWeapon.h"
#include "WeaponRef.h"
#include "WeaponClass.h"
#include "NamedItem.h"
#include "WeaponType.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "Spell.h"
#include "AbilityManager.h"
#include "IApplication.h"

using namespace StoneRing;

GeneratedWeapon::GeneratedWeapon():m_pClass(NULL),m_pType(NULL)
{
}

GeneratedWeapon::~GeneratedWeapon()
{
}

bool GeneratedWeapon::operator== ( const ItemRef &ref )
{
    if( ref.GetType() == ItemRef::WEAPON_REF
        && *ref.GetWeaponRef()->GetWeaponClass() == *m_pClass &&
        *ref.GetWeaponRef()->GetWeaponType() == *m_pType)
    {
        if(HasSpell() && ref.GetWeaponRef()->GetSpellRef())
        {
            if(*GetSpellRef() == *ref.GetWeaponRef()->GetSpellRef())
            {
                return true;
            } 
            else return false;
        }
        else if ( HasSpell() || ref.GetWeaponRef()->GetSpellRef())
        {
            // One had a spell ref and one didn't.
            return false;
        }

        if(HasRuneType() && ref.GetWeaponRef()->GetRuneType())
        {
            if(*GetRuneType() == *ref.GetWeaponRef()->GetRuneType())
            {
                return true;
            }
            else return false;
        }
        else if ( HasRuneType() || ref.GetWeaponRef()->GetRuneType())
        {
            return false;

        }
        return true;
    }

    return false;
}




// Item interface 
std::string GeneratedWeapon::GetIconRef() const
{
    return m_pType->GetIconRef();
}

std::string GeneratedWeapon::GetName() const
{
    return m_name;
}

uint GeneratedWeapon::GetMaxInventory() const 
{
    // todo: get the system setting.... 
    return 99;
}

NamedItem::eDropRarity 
GeneratedWeapon::GetDropRarity() const
{
    if( HasSpell() || HasRuneType() )
    {
        return RARE; 
    }
    else return UNCOMMON;
}

uint GeneratedWeapon::GetValue() const 
{
    const AbilityManager * pManager = IApplication::GetInstance()->GetAbilityManager();

    uint value= (int)((float)m_pType->GetBasePrice() * 
                      m_pClass->GetValueMultiplier()) 
        + m_pClass->GetValueAdd();

    if(HasSpell())
    {
        SpellRef * pSpellRef = GetSpellRef();

        Spell * pSpell = pManager->GetSpell ( *pSpellRef );

        value += pSpell->getValue();
    }

    if(HasRuneType())
    {
        value *= 2; //@todo : get multiplier from game settings
    }

    return value;

}

uint GeneratedWeapon::GetSellValue() const 
{
    return GetValue() / 2;
}

// Weapon interface



WeaponType * GeneratedWeapon::GetWeaponType() const
{
    return m_pType;
}

bool GeneratedWeapon::IsRanged() const 
{
    return m_pType->IsRanged();
}

bool GeneratedWeapon::IsTwoHanded() const
{
    return m_pType->IsTwoHanded();
}


WeaponRef GeneratedWeapon::GenerateWeaponRef() const
{
    return WeaponRef( GetWeaponType(), GetWeaponClass(), GetSpellRef(), GetRuneType() );
}

void GeneratedWeapon::ExecuteScript()
{
    m_pClass->ExecuteScript();
}

bool GeneratedWeapon::EquipCondition()
{
    return m_pClass->EquipCondition();
}

void GeneratedWeapon::OnEquipScript()
{
    return m_pClass->OnEquipScript();
}

void GeneratedWeapon::OnUnequipScript()
{
    return m_pClass->OnUnequipScript();
}

void GeneratedWeapon::Generate( WeaponType* pType, WeaponClass * pClass, 
                                SpellRef *pSpell , RuneType *pRune)
{

    for(std::list<AttributeModifier*>::const_iterator iter = pClass->GetAttributeModifiersBegin();
        iter != pClass->GetAttributeModifiersEnd();
        iter++)
    {
        Add_Attribute_Modifier  ( *iter );
    }
    for(std::list<WeaponEnhancer*>::const_iterator iter2 = pClass->GetWeaponEnhancersBegin();
        iter2 != pClass->GetWeaponEnhancersEnd();
        iter2++)
    {
        Add_Weapon_Enhancer ( *iter2 );
    }
    m_pType = pType;
    m_pClass = pClass;
    Set_Spell_Ref(pSpell);
    Set_Rune_Type(pRune);

    m_name = CreateWeaponName(pType,pClass,pSpell,pRune);


}





