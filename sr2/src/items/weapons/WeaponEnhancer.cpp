#include "WeaponEnhancer.h"

using namespace StoneRing;

void WeaponEnhancer::load_attributes(CL_DomNamedNodeMap attributes)
{
    std::string strAttr = get_required_string("attribute", attributes);
    m_eAttribute = Weapon::AttributeForString ( strAttr );
    m_fMultiplier = get_implied_float("multiplier",attributes,1);
    m_fAdd = get_implied_float("add",attributes,0);
}

WeaponEnhancer::WeaponEnhancer():m_fMultiplier(1),m_fAdd(0)
{
}

WeaponEnhancer::~WeaponEnhancer()
{
}

Weapon::eAttribute WeaponEnhancer::GetAttribute() const
{
    return m_eAttribute;
}

float WeaponEnhancer::GetAdd() const
{
    return m_fAdd;
}

float WeaponEnhancer::GetMultiplier() const
{
    return m_fMultiplier;
}




