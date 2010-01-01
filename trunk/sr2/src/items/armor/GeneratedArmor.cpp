#include "GeneratedArmor.h"
#include "ArmorType.h"
#include "ArmorRef.h"
#include "RuneType.h"
#include "ArmorClass.h"
#include "SpellRef.h"
#include "AbilityManager.h"
#include "IApplication.h"

using namespace StoneRing;

GeneratedArmor::GeneratedArmor():m_pType(NULL),m_pClass(NULL)
{
}

GeneratedArmor::~GeneratedArmor()
{
}

std::string GeneratedArmor::GetIconRef() const
{
    return m_pType->GetIconRef();
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

void GeneratedArmor::ExecuteScript()
{
    m_pClass->ExecuteScript();
}

bool GeneratedArmor::EquipCondition()
{
    return m_pClass->EquipCondition();
}

void GeneratedArmor::OnEquipScript()
{
    return m_pClass->OnEquipScript();
}

void GeneratedArmor::OnUnequipScript()
{
    return m_pClass->OnUnequipScript();
}

bool GeneratedArmor::operator== ( const ItemRef &ref )
{
    if( ref.GetType() == ItemRef::ARMOR_REF
        && *ref.GetArmorRef()->GetArmorClass() == *m_pClass &&
        *ref.GetArmorRef()->GetArmorType() == *m_pType)
    {
        if(HasSpell() && ref.GetArmorRef()->GetSpellRef())
        {
            if(*GetSpellRef() == *ref.GetArmorRef()->GetSpellRef())
            {
                return true;
            }
            else return false;
        }
        else if ( HasSpell() || ref.GetArmorRef()->GetSpellRef())
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
    if( HasSpell() || HasRuneType() )
    {
        return RARE;
    }
    else return UNCOMMON;
}


uint GeneratedArmor::GetValue() const
{
    // @todo: add rune value
    const AbilityManager * pManager = IApplication::GetInstance()->GetAbilityManager();

    uint value =  (int)((float)m_pType->GetBasePrice() * m_pClass->GetValueMultiplier())
        + m_pClass->GetValueAdd();

    if(HasSpell())
    {
        SpellRef * pSpellRef = GetSpellRef();

        Spell * pSpell = pManager->GetSpell ( *pSpellRef );

        value += pSpell->getValue();
    }

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

    return ArmorRef ( GetArmorType(), GetArmorClass(), GetSpellRef(), GetRuneType() );
}

void GeneratedArmor::generate( ArmorType * pType, ArmorClass * pClass,
                               SpellRef *pSpell , RuneType *pRune)
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

    m_pType = pType;
    m_pClass = pClass;
    Set_Spell_Ref(pSpell);
    Set_Rune_Type(pRune);

    m_name = CreateArmorName(pType,pClass,pSpell,pRune);
}




