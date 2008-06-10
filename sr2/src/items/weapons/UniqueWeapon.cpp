#include "UniqueWeapon.h"
#include "WeaponType.h"
#include "ItemManager.h"
#include "WeaponTypeRef.h"
#include "WeaponEnhancer.h"
#include "AttributeEnhancer.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "StatusEffectModifier.h"

using namespace StoneRing;

UniqueWeapon::UniqueWeapon():m_pWeaponType(NULL),m_pScript(NULL)
{
}

UniqueWeapon::~UniqueWeapon()
{ 
    delete m_pScript;
    delete m_pEquipScript;
    delete m_pUnequipScript;
    delete m_pConditionScript;
}

void UniqueWeapon::ExecuteScript()
{
    if(m_pScript) m_pScript->ExecuteScript();
}

bool UniqueWeapon::EquipCondition()
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition();
    else return true;
}

void UniqueWeapon::OnEquipScript()
{
    m_pEquipScript->ExecuteScript();
}

void UniqueWeapon::OnUnequipScript()
{
    m_pUnequipScript->ExecuteScript();
}

uint UniqueWeapon::GetValue() const 
{
    return m_nValue;
}

uint UniqueWeapon::GetSellValue() const 
{
    return m_nValue / 2;
}

WeaponType * UniqueWeapon::GetWeaponType() const 
{
    return m_pWeaponType;
}

bool UniqueWeapon::IsRanged() const 
{
    return m_pWeaponType->IsRanged();
}

bool UniqueWeapon::IsTwoHanded() const
{
    return m_pWeaponType->IsTwoHanded();
}

void UniqueWeapon::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_value_multiplier = get_implied_float("valueMultiplier",pAttributes,1);

}

void UniqueWeapon::load_finished()
{
    cl_assert ( m_pWeaponType );
    m_nValue = (int)(m_pWeaponType->GetBasePrice() * m_value_multiplier);
}
bool UniqueWeapon::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EWEAPONTYPEREF:
    {
        const ItemManager * pItemManager = IApplication::GetInstance()->GetItemManager();

        WeaponTypeRef * pType = dynamic_cast<WeaponTypeRef*>(pElement);
        m_pWeaponType = pItemManager->GetWeaponType( *pType );
        break;
    }
    case EWEAPONENHANCER:
        Add_Weapon_Enhancer( dynamic_cast<WeaponEnhancer*>(pElement) );
        break;
    case EATTRIBUTEENHANCER:
        Add_Attribute_Enhancer( dynamic_cast<AttributeEnhancer*>(pElement) );
        break;
    case ESPELLREF:
        Set_Spell_Ref ( dynamic_cast<SpellRef*>(pElement) );
        break;
    case ERUNETYPE:
        Set_Rune_Type( dynamic_cast<RuneType*>(pElement) );
        break;
    case ESTATUSEFFECTMODIFIER:
        Add_Status_Effect_Modifier( dynamic_cast<StatusEffectModifier*>(pElement) );
        break;
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONEQUIP:
        m_pEquipScript = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONUNEQUIP:
        m_pUnequipScript = dynamic_cast<NamedScript*>(pElement);
        break;
    case ECONDITIONSCRIPT:
        m_pConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    default:
        return false;

    }

    return true;
}




