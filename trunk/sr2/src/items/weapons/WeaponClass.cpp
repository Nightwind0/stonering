#include "WeaponClass.h"
#include "WeaponClass.h"
#include "WeaponEnhancer.h"
#include "StatusEffectModifier.h"
#include "WeaponTypeExclusionList.h"

#include <algorithm>


using namespace StoneRing;

WeaponClass::WeaponClass():m_pScript(NULL),m_pEquipScript(NULL),
m_pUnequipScript(NULL),m_pConditionScript(NULL),m_eScriptMode(Weapon::ATTACK_BEFORE)
{
}

void WeaponClass::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes );
    m_fValueMultiplier = get_implied_float("valueMultiplier",attributes,1);
    m_nValueAdd = get_implied_int("valueAdd",attributes,0);
    std::string script_mode = get_implied_string("scriptMode",attributes,"attackBefore");
    m_eScriptMode = Weapon::ScriptModeForString(script_mode);
}

bool WeaponClass::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONUNEQUIP:
        m_pUnequipScript = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONEQUIP:
        m_pEquipScript = dynamic_cast<NamedScript*>(pElement);
        break;
    case ECONDITIONSCRIPT:
        m_pConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EATTRIBUTEMODIFIER:
        m_attribute_modifiers.push_back( dynamic_cast<AttributeModifier*>(pElement) );
        break;
    case EWEAPONENHANCER:
        m_weapon_enhancers.push_back( dynamic_cast<WeaponEnhancer*>(pElement) );
        break;
    case EWEAPONTYPEEXCLUSIONLIST:
    {
        WeaponTypeExclusionList * pList = dynamic_cast<WeaponTypeExclusionList*>(pElement);
        std::copy(pList->GetWeaponTypeRefsBegin(),pList->GetWeaponTypeRefsEnd(),
                  std::back_inserter(m_excluded_types));

        delete pList;
        break;
    }
    case ESTATUSEFFECTMODIFIER:
        add_status_effect_modifier (dynamic_cast<StatusEffectModifier*>(pElement));
        break;
    default:
        return false;
    }

    return true;
}

void WeaponClass::ExecuteScript()
{
    if(m_pScript) m_pScript->ExecuteScript();
}

bool WeaponClass::EquipCondition()
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition();
    else return true;
}

void WeaponClass::OnEquipScript()
{
    m_pEquipScript->ExecuteScript();
}

void WeaponClass::OnUnequipScript()
{
    m_pUnequipScript->ExecuteScript();
}

WeaponClass::~WeaponClass()
{
    std::for_each(m_attribute_modifiers.begin(),m_attribute_modifiers.end(),del_fun<AttributeModifier>());
    std::for_each(m_weapon_enhancers.begin(),m_weapon_enhancers.end(),del_fun<WeaponEnhancer>());
    std::for_each(m_excluded_types.begin(),m_excluded_types.end(),del_fun<WeaponTypeRef>());

    delete m_pScript;
    delete m_pUnequipScript;
    delete m_pEquipScript;
    delete m_pConditionScript;
}

bool WeaponClass::operator==(const WeaponClass &lhs)
{
    return m_name == lhs.m_name;
}

std::string WeaponClass::GetName() const
{
    return m_name;
}

int WeaponClass::GetValueAdd() const
{
    return m_nValueAdd;
}

float WeaponClass::GetValueMultiplier() const
{
    return m_fValueMultiplier;
}

std::list<AttributeModifier*>::const_iterator
WeaponClass::GetAttributeModifiersBegin()
{
    return m_attribute_modifiers.begin();
}

std::list<AttributeModifier*>::const_iterator
WeaponClass::GetAttributeModifiersEnd()
{
    return m_attribute_modifiers.end();
}

std::list<WeaponEnhancer*>::const_iterator
WeaponClass::GetWeaponEnhancersBegin()
{
    return m_weapon_enhancers.begin();
}

std::list<WeaponEnhancer*>::const_iterator
WeaponClass::GetWeaponEnhancersEnd()
{
    return m_weapon_enhancers.end();
}

bool WeaponClass::IsExcluded ( const WeaponTypeRef &weaponType )
{
    for(std::list<WeaponTypeRef*>::const_iterator iter = m_excluded_types.begin();
        iter != m_excluded_types.end();
        iter++)
    {
        WeaponTypeRef * pRef = *iter;

        if( pRef->GetName() == weaponType.GetName() )
            return true;
    }

    return false;

}



