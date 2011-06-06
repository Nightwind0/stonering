#include "ArmorEnhancer.h"

using namespace StoneRing;

ArmorEnhancer::ArmorEnhancer()
{
}

void ArmorEnhancer::load_attributes(CL_DomNamedNodeMap attributes)
{
   std::string strAttr = get_implied_string("attribute", attributes,"");
   std::string dmgCat = get_implied_string("damageCategory",attributes,"");
   
    if(strAttr != ""){ 
        m_eType = ARMOR_ATTRIBUTE;
        m_eAttribute = Armor::AttributeForString ( strAttr );
    }else if(dmgCat != ""){
        m_eType = DAMAGE_CATEGORY;
        m_dmgCategory = DamageCategory::DamageCategoryFromString(dmgCat);
    }else {
        throw CL_Exception("Required attribute or damageCategory on armorEnhancer");
    }
    
    m_fMultiplier = get_implied_float("multiplier",attributes,1);
    m_nAdd = get_implied_int("add",attributes,0);
}


ArmorEnhancer::~ArmorEnhancer()
{
}

ArmorEnhancer::eType ArmorEnhancer::GetType() const
{
    return m_eType;
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
        
DamageCategory::eDamageCategory ArmorEnhancer::GetDamageCategory() const
{
    return m_dmgCategory;
}





