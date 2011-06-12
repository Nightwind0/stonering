#include "GeneratedArmor.h"
#include "ArmorType.h"
#include "ArmorRef.h"
#include "RuneType.h"
#include "ArmorClass.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "GraphicsManager.h"

using namespace StoneRing;

GeneratedArmor::GeneratedArmor():m_pType(NULL),m_pClass(NULL), m_pImbuement(NULL)
{
}

GeneratedArmor::~GeneratedArmor()
{
}

CL_Image GeneratedArmor::GetIcon() const
{
    return GraphicsManager::GetIcon(m_pType->GetIconRef());
}

std::string GeneratedArmor::GetName() const
{
    return m_name;
}

uint GeneratedArmor::GetMaxInventory() const
{
    // todo: lookup in settings
    return 99;
}

void GeneratedArmor::Invoke(const ParameterList& params)
{
    m_pClass->ExecuteScript(params);
}

bool GeneratedArmor::EquipCondition(const ParameterList& params)
{
    return m_pClass->EquipCondition(params)
    && m_pImbuement?m_pImbuement->EquipCondition(params):true;
}

void GeneratedArmor::OnEquipScript(const ParameterList& params)
{
    m_pClass->OnEquipScript(params);
    if(m_pImbuement) 
        m_pImbuement->OnEquipScript(params);
}

void GeneratedArmor::OnUnequipScript(const ParameterList& params)
{
    m_pClass->OnUnequipScript(params);
    if(m_pImbuement)
        m_pImbuement->OnUnequipScript(params);
}

bool GeneratedArmor::operator== ( const ItemRef &ref )
{
    if( ref.GetType() == ItemRef::ARMOR_REF
        && *ref.GetArmorRef()->GetArmorClass() == *m_pClass &&
        *ref.GetArmorRef()->GetArmorType() == *m_pType)
    {
        if(m_pImbuement && ref.GetArmorRef()->GetArmorImbuement())
        {
            if(*GetImbuement() == *ref.GetArmorRef()->GetArmorImbuement())
            {
                return true;
            }
            else return false;
        }
        else if ( m_pImbuement || ref.GetArmorRef()->GetArmorImbuement())
        {
            // One had a spell ref and one didn't.
            return false;
        }

        if(HasRuneType() && ref.GetArmorRef()->GetRuneType())
        {
            if(*GetRuneType() == *ref.GetArmorRef()->GetRuneType())
            {
                return true;
            }
            else return false;
        }
        else if ( HasRuneType() || ref.GetArmorRef()->GetRuneType())
        {
            return false;
        }

        return true;
    }

    return false;
}


Item::eDropRarity GeneratedArmor::GetDropRarity() const
{
    if( m_pImbuement || HasRuneType() )
    {
        return RARE;
    }
    else return UNCOMMON;
}


uint GeneratedArmor::GetValue() const
{
    // @todo: add rune value

    uint value =  (int)((float)m_pType->GetBasePrice() * m_pClass->GetValueMultiplier());

    if(m_pImbuement)
    {
        value *= m_pImbuement->GetValueMultiplier();
        value += m_pImbuement->GetValueAdd();
    }
    
    value += m_pClass->GetValueAdd();

    if(HasRuneType())
    {
        switch( GetRuneType()->GetRuneType() )
        {
        case RuneType::RUNE:
            value *= 2; //@todo : get value from game settings
            break;
        case RuneType::ULTRA_RUNE:
        {
            double dValue = value;
            dValue *= 2.75; //@todo: get value from game settings
            value = (int)dValue;
            break;
        }
        default:
            assert(0);
        }
    }

    return value;
}

uint GeneratedArmor::GetSellValue() const
{
    return GetValue() / 2;
}



ArmorType * GeneratedArmor::GetArmorType() const
{
    return m_pType;
}

ArmorRef GeneratedArmor::GenerateArmorRef() const
{

    return ArmorRef ( GetArmorType(), GetArmorClass(), GetImbuement(), GetRuneType() );
}

void GeneratedArmor::generate( ArmorType * pType, ArmorClass * pClass,
                               ArmorClass* pImbuement , RuneType *pRune)
{

    for(std::list<AttributeModifier*>::const_iterator iter = pClass->GetAttributeModifiersBegin();
        iter != pClass->GetAttributeModifiersEnd();
        iter++)
    {
        Add_Attribute_Modifier  ( *iter );
    }
    for(std::list<ArmorEnhancer*>::const_iterator iter2 = pClass->GetArmorEnhancersBegin();
        iter2 != pClass->GetArmorEnhancersEnd();
        iter2++)
    {
        Add_Armor_Enhancer ( *iter2 );
    }
    for(std::list<StatusEffectModifier*>::const_iterator iter3 = pClass->GetStatusEffectModifiersBegin();
        iter3 != pClass->GetStatusEffectModifiersEnd();
        iter3++)
    {
            Add_StatusEffect_Modifier( *iter3 );
    }
        
    if(pImbuement)
    {
        for(std::list<AttributeModifier*>::const_iterator iter = pImbuement->GetAttributeModifiersBegin();
            iter != pImbuement->GetAttributeModifiersEnd();
            iter++)
        {
            Add_Attribute_Modifier  ( *iter );
        }
        
        for(std::list<ArmorEnhancer*>::const_iterator iter2 = pImbuement->GetArmorEnhancersBegin();
            iter2 != pImbuement->GetArmorEnhancersEnd();
            iter2++)
        {
            Add_Armor_Enhancer ( *iter2 );
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

    m_name = CreateArmorName(pType,pClass,pImbuement,pRune);
}




