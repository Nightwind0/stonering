#include "WeaponEnhancer.h"

using namespace StoneRing;

void WeaponEnhancer::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string strAttr = get_required_string("attribute", pAttributes);
    m_eAttribute = Weapon::AttributeForString ( strAttr );
    m_fMultiplier = get_implied_float("multiplier",pAttributes,1);
    m_nAdd = get_implied_int("add",pAttributes,0);
}

WeaponEnhancer::WeaponEnhancer():m_fMultiplier(1),m_nAdd(0)
{
}

WeaponEnhancer::~WeaponEnhancer()
{
}

Weapon::eAttribute WeaponEnhancer::GetAttribute() const
{
    return m_eAttribute;
}

int WeaponEnhancer::GetAdd() const
{
    return m_nAdd;
}

float WeaponEnhancer::GetMultiplier() const
{
    return m_fMultiplier;
}




