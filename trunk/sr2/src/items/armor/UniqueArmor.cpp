#include "UniqueArmor.h"
#include "ArmorType.h"
#include "ItemManager.h"
#include "ArmorTypeRef.h"
#include "ArmorEnhancer.h"
#include "AttributeModifier.h"
#include "RuneType.h"
#include "StatusEffectModifier.h"

using namespace StoneRing;

UniqueArmor::UniqueArmor():m_pArmorType(NULL),m_pScript(NULL),
m_pEquipScript(NULL),m_pUnequipScript(NULL)
{

}
UniqueArmor::~UniqueArmor()
{
    delete m_pScript;
    delete m_pEquipScript;
    delete m_pUnequipScript;
    delete m_pConditionScript;
}

void UniqueArmor::Invoke(const ParameterList& params)
{
    if(m_pScript) m_pScript->ExecuteScript(params);
}

bool UniqueArmor::EquipCondition(const ParameterList& params)
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition(params);
    else return true;
}

void UniqueArmor::OnEquipScript(const ParameterList& params)
{
    m_pEquipScript->ExecuteScript(params);
}
void UniqueArmor::OnUnequipScript(const ParameterList& params)
{
    m_pUnequipScript->ExecuteScript(params);
}

uint UniqueArmor::GetValue() const
{
    return m_nValue;
}

uint UniqueArmor::GetSellValue() const
{
    return m_nValue / 2;
}

ArmorType *UniqueArmor::GetArmorType() const
{
    return m_pArmorType;
}

void UniqueArmor::load_attributes(CL_DomNamedNodeMap attributes)
{
    NamedItemElement::load_attributes(attributes);
    m_value_multiplier = get_implied_float("valueMultiplier",attributes,1);

}

void UniqueArmor::load_finished()
{
    assert ( m_pArmorType );
    NamedItemElement::load_finished();
    m_nValue = (int)(m_pArmorType->GetBasePrice() * m_value_multiplier);
}

bool UniqueArmor::handle_element(eElement element, Element * pElement)
{
    if(NamedItemElement::handle_element(element,pElement))
	return true;
    switch(element)
    {
    case EARMORTYPEREF:
    {
        const ItemManager * pItemManager = IApplication::GetInstance()->GetItemManager();
        ArmorTypeRef * pType = dynamic_cast<ArmorTypeRef*>(pElement);
        m_pArmorType = pItemManager->GetArmorType( *pType );
        break;
    }
    case EARMORENHANCER:
        Add_Armor_Enhancer( dynamic_cast<ArmorEnhancer*>(pElement) );
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
        Add_StatusEffect_Infliction( dynamic_cast<StatusEffectInfliction*>(pElement));
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


bool UniqueArmor::operator==(const ItemRef& ref)
{
    return ref.GetType() == ItemRef::NAMED_ITEM &&
    ref.GetItemName() == GetName();
}



