#include "GeneratedWeapon.h"
#include "WeaponRef.h"
#include "WeaponClass.h"
#include "NamedItem.h"
#include "WeaponType.h"
#include "RuneType.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "GraphicsManager.h"

using namespace StoneRing;

GeneratedWeapon::GeneratedWeapon():m_pClass(NULL),m_pImbuement(NULL),m_pType(NULL)
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
        if(m_pImbuement && ref.GetWeaponRef()->GetWeaponImbuement())
        {
            if(*m_pImbuement == *ref.GetWeaponRef()->GetWeaponImbuement())
            {
                return true;
            }
            else return false;
        }
        else if ( m_pImbuement || ref.GetWeaponRef()->GetWeaponImbuement())
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
CL_Image GeneratedWeapon::GetIcon() const
{
    return GraphicsManager::GetIcon(m_pType->GetIconRef());
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

std::string GeneratedWeapon::GetDescription() const
{
    if(m_pImbuement)
    return m_pType->GetName() + " " + m_pClass->GetDescription() + " " + m_pImbuement->GetDescription();
    else return m_pType->GetName() + " " + m_pClass->GetDescription();
}


Item::eDropRarity
GeneratedWeapon::GetDropRarity() const
{
    if( m_pImbuement || HasRuneType() )
    {
        return RARE;
    }
    else return UNCOMMON;
}

uint GeneratedWeapon::GetValue() const
{
    const AbilityManager * pManager = IApplication::GetInstance()->GetAbilityManager();

    uint value= (int)((float)m_pType->GetBasePrice() *
                      m_pClass->GetValueMultiplier());

    if(m_pImbuement)
    {
        value *= m_pImbuement->GetValueMultiplier();
        value += m_pImbuement->GetValueAdd();
    }
    
    value += m_pClass->GetValueAdd();

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
    return WeaponRef( GetWeaponType(), GetWeaponClass(), GetImbuement(), GetRuneType() );
}

void GeneratedWeapon::Invoke(Weapon::eScriptMode invokeTime, const ParameterList& params)
{
    if(m_pClass->GetScriptMode() & invokeTime)
        m_pClass->ExecuteScript(params);
    if(m_pImbuement && m_pImbuement->GetScriptMode() & invokeTime)
        m_pImbuement->ExecuteScript(params);
}

bool GeneratedWeapon::EquipCondition(const ParameterList& params)
{
    return m_pClass->EquipCondition(params)
            && m_pImbuement?m_pImbuement->EquipCondition(params):true;
}

void GeneratedWeapon::OnEquipScript(const ParameterList& params)
{
    m_pClass->OnEquipScript(params);
    if(m_pImbuement)
        m_pImbuement->OnEquipScript(params);
}

void GeneratedWeapon::OnUnequipScript(const ParameterList& params)
{
    m_pClass->OnUnequipScript(params);
    if(m_pImbuement)
        m_pImbuement->OnUnequipScript(params);
}



void GeneratedWeapon::Generate( WeaponType* pType, WeaponClass * pClass,
                                WeaponClass* pImbuement , RuneType *pRune)
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
    for(std::list<StatusEffectModifier*>::const_iterator iter3 = pClass->GetStatusEffectModifiersBegin();
        iter3 != pClass->GetStatusEffectModifiersEnd(); 
        iter3++)
    {
        Add_StatusEffect_Modifier( *iter3 );
    }
    
    if(pImbuement){
        for(std::list<AttributeModifier*>::const_iterator iter = pImbuement->GetAttributeModifiersBegin();
            iter != pImbuement->GetAttributeModifiersEnd();
            iter++)
        {
            Add_Attribute_Modifier  ( *iter );
        }
        for(std::list<WeaponEnhancer*>::const_iterator iter2 = pImbuement->GetWeaponEnhancersBegin();
            iter2 != pImbuement->GetWeaponEnhancersEnd();
            iter2++)
        {
            Add_Weapon_Enhancer ( *iter2 );
        }
        for(std::list<StatusEffectModifier*>::const_iterator iter3 = pImbuement->GetStatusEffectModifiersBegin();
            iter3 != pImbuement->GetStatusEffectModifiersEnd(); 
            iter3++)
        {
            Add_StatusEffect_Modifier( *iter3 );
        }
    }
        
    m_pType = pType;
    m_pClass = pClass;
    m_pImbuement = pImbuement;
    Set_Rune_Type(pRune);
    Add_Script_Mode(m_pClass->GetScriptMode());
    if(m_pImbuement)
        Add_Script_Mode(m_pImbuement->GetScriptMode());

    m_name = CreateWeaponName(pType,pClass,pImbuement,pRune);

}





