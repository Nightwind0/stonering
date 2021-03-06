#include "UniqueWeapon.h"
#include "WeaponType.h"
#include "ItemManager.h"
#include "WeaponTypeRef.h"
#include "WeaponEnhancer.h"
#include "AttributeModifier.h"
#include "RuneType.h"
#include "StatusEffectModifier.h"
#include "Description.h"
#include "GraphicsManager.h"

using namespace StoneRing;

UniqueWeapon::UniqueWeapon():m_pWeaponType(NULL),m_pScript(NULL),m_pEquipScript(NULL),m_pUnequipScript(NULL),m_pConditionScript(NULL)
{
}

UniqueWeapon::~UniqueWeapon()
{
    delete m_pScript;
    delete m_pEquipScript;
    delete m_pUnequipScript;
    delete m_pConditionScript;
}

void UniqueWeapon::Invoke(eScriptMode invokeTime, const ParameterList& params)
{
    if(m_pScript && ScriptModeApplies(invokeTime)) 
        m_pScript->ExecuteScript(params);
}

bool UniqueWeapon::EquipCondition(const ParameterList& params)
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition(params);
    else return true;
}

void UniqueWeapon::OnEquipScript(const ParameterList& params)
{
	if(m_pEquipScript)
		m_pEquipScript->ExecuteScript(params);
}

void UniqueWeapon::OnUnequipScript(const ParameterList& params)
{
	if(m_pUnequipScript)
		m_pUnequipScript->ExecuteScript(params);
}

clan::Sprite UniqueWeapon::GetSprite() const 
{
	if(m_sprite.is_null()){
		return GetWeaponType()->GetSprite();
	}
	return m_sprite;
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

void UniqueWeapon::load_attributes(clan::DomNamedNodeMap attributes)
{
    NamedItemElement::load_attributes(attributes);
    m_value_multiplier = get_implied_float("valueMultiplier",attributes,1);
    Add_Script_Mode( ScriptModeForString( get_implied_string("scriptMode",attributes,"attackBefore") ) );

    if(has_attribute("damageCategory",attributes)){
		SetDamageCategory(DamageCategory::DamageCategoryFromString(get_required_string("damageCategory",attributes)));
	}
}


void UniqueWeapon::load_finished()
{
    assert ( m_pWeaponType );
    NamedItemElement::load_finished();
	m_sprite = GraphicsManager::GetSpriteWithImage(GetIcon());
    m_nValue = (int)(m_pWeaponType->GetBasePrice() * m_value_multiplier);
}
bool UniqueWeapon::handle_element(eElement element, Element * pElement)
{
    if(NamedItemElement::handle_element(element,pElement))
	return true;
    switch(element)
    {
    case EWEAPONTYPEREF:
    {
        WeaponTypeRef * pType = dynamic_cast<WeaponTypeRef*>(pElement);
        m_pWeaponType = ItemManager::GetWeaponType( *pType );
        break;
    }
    case EWEAPONENHANCER:
        Add_Weapon_Enhancer( dynamic_cast<WeaponEnhancer*>(pElement) );
        break;
    case EATTRIBUTEMODIFIER:
        Add_Attribute_Modifier( dynamic_cast<AttributeModifier*>(pElement) );
        break;
    case ERUNETYPE:
        Set_Rune_Type( dynamic_cast<RuneType*>(pElement) );
        break;
    case ESTATUSEFFECTMODIFIER:
        Add_StatusEffect_Modifier( dynamic_cast<StatusEffectModifier*>(pElement) );
        break;
    case ESTATUSEFFECTINFLICTION:
        Add_StatusEffect_Infliction( dynamic_cast<StatusEffectInfliction*>(pElement) );
        break;
	case EANIMATIONSCRIPT:
		SetAnimationScript(dynamic_cast<ScriptElement*>(pElement));
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
    case EDESCRIPTION:
        m_description = dynamic_cast<Description*>(pElement)->GetText();
        delete pElement;
        break;
    default:
        return false;

    }

    return true;
}


bool UniqueWeapon::operator == ( const ItemRef &ref )
{
    return ref.GetType() == ItemRef::NAMED_ITEM && 
    ref.GetItemName() == GetName();
}

