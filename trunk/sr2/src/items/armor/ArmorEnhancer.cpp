#include "ArmorEnhancer.h"

using namespace StoneRing;

ArmorEnhancer::ArmorEnhancer()
{
}

void ArmorEnhancer::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string strAttr = get_required_string("attribute", pAttributes);
    m_eAttribute = Armor::AttributeForString ( strAttr );
    m_fMultiplier = get_implied_float("multiplier",pAttributes,1);
    m_nAdd = get_implied_int("add",pAttributes,0);
}


ArmorEnhancer::~ArmorEnhancer()
{
}

Armor::eAttribute 
ArmorEnhancer::GetAttribute() const
{
    return m_eAttribute;
}

int ArmorEnhancer::GetAdd() const
{
    return m_nAdd;
}

float ArmorEnhancer::GetMultiplier() const
{
    return m_fMultiplier;
}





