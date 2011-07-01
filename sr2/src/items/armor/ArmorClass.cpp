#include "ArmorClass.h"
#include "AttributeModifier.h"
#include "ArmorEnhancer.h"
#include "ArmorTypeExclusionList.h"
#include "StatusEffectModifier.h"
#include <algorithm>
#include <iterator>


using namespace StoneRing;

ArmorClass::ArmorClass():m_pScript(NULL),m_pEquipScript(NULL)
,m_pUnequipScript(NULL),m_pConditionScript(NULL)
{
}


bool ArmorClass::operator==( const ArmorClass &lhs )
{
    return m_name == lhs.m_name;
}

void ArmorClass::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes );
    m_fValueMultiplier = get_implied_float("valueMultiplier",attributes,1);
    m_nValueAdd = get_implied_int("valueAdd",attributes,0);
    m_desc = get_implied_string("desc",attributes,"Description Missing");
    m_bImbuement = get_implied_bool("imbuement",attributes,false);
}

std::string ArmorClass::GetDescription() const
{
    return m_desc;
}


bool ArmorClass::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EONEQUIP:
        m_pEquipScript = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONUNEQUIP:
        m_pUnequipScript = dynamic_cast<NamedScript*>(pElement);
        break;
    case EATTRIBUTEMODIFIER:
        m_attribute_modifiers.push_back( dynamic_cast<AttributeModifier*>(pElement) );
        break;
    case ECONDITIONSCRIPT:
        m_pConditionScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EARMORENHANCER:
        m_armor_enhancers.push_back( dynamic_cast<ArmorEnhancer*>(pElement) );
        break;
    case EARMORTYPEEXCLUSIONLIST:
    {
        ArmorTypeExclusionList * pList = dynamic_cast<ArmorTypeExclusionList*>(pElement);
        std::copy(pList->GetArmorTypeRefsBegin(),pList->GetArmorTypeRefsEnd(),
                  std::back_inserter(m_excluded_types));

        delete pList;
        break;
    }
    case ESTATUSEFFECTMODIFIER:
        AddStatusEffectModifier (dynamic_cast<StatusEffectModifier*>(pElement));
        break;
    case ESTATUSEFFECTINFLICTION:
        AddStatusEffectInfliction (dynamic_cast<StatusEffectInfliction*>(pElement));
        break;
    default:
        return false;
    }

    return true;
}


ArmorClass::~ArmorClass()
{
    std::for_each(m_attribute_modifiers.begin(),m_attribute_modifiers.end(),del_fun<AttributeModifier>());
    std::for_each(m_armor_enhancers.begin(),m_armor_enhancers.end(),del_fun<ArmorEnhancer>());
    std::for_each(m_excluded_types.begin(),m_excluded_types.end(),del_fun<ArmorTypeRef>());

    delete m_pScript;
    delete m_pEquipScript;
    delete m_pUnequipScript;
    delete m_pConditionScript;
}

void ArmorClass::ExecuteScript(const ParameterList& params)
{
    if(m_pScript) m_pScript->ExecuteScript(params);
}

bool ArmorClass::EquipCondition(const ParameterList& params)
{
    if(m_pConditionScript)
        return m_pConditionScript->EvaluateCondition(params);
    else return true;
}

void ArmorClass::OnEquipScript(const ParameterList& params)
{
    m_pEquipScript->ExecuteScript(params);
}

void ArmorClass::OnUnequipScript(const ParameterList& params)
{
    m_pUnequipScript->ExecuteScript(params);
}

std::string ArmorClass::GetName() const
{
    return m_name;
}

int ArmorClass::GetValueAdd() const
{
    return m_nValueAdd;
}

float ArmorClass::GetValueMultiplier() const
{
    return m_fValueMultiplier;
}

std::list<AttributeModifier*>::const_iterator
ArmorClass::GetAttributeModifiersBegin()
{
    return m_attribute_modifiers.begin();
}

std::list<AttributeModifier*>::const_iterator
ArmorClass::GetAttributeModifiersEnd()
{
    return m_attribute_modifiers.end();
}

std::list<ArmorEnhancer*>::const_iterator
ArmorClass::GetArmorEnhancersBegin()
{
    return m_armor_enhancers.begin();
}

std::list<ArmorEnhancer*>::const_iterator
ArmorClass::GetArmorEnhancersEnd()
{
    return m_armor_enhancers.end();
}

bool ArmorClass::IsExcluded ( const ArmorTypeRef &armorType )
{
    for(std::list<ArmorTypeRef*>::const_iterator iter = m_excluded_types.begin();
        iter != m_excluded_types.end();
        iter++)
    {
        ArmorTypeRef * pRef = *iter;

        if( pRef->GetName() == armorType.GetName() )
            return true;
    }

    return false;
}




