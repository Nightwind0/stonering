#include "WeaponType.h"
#include "IconRef.h"
#include "DamageCategory.h"
#include "GraphicsManager.h"
#include "ClanLib/core.h"
#include "Animation.h"

using namespace StoneRing;

WeaponType::WeaponType():m_pAnimation(NULL)
{
}


bool WeaponType::operator==(const WeaponType &type )
{
    return m_name == type.m_name;
}

void WeaponType::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    m_nBasePrice = get_required_int("basePrice",attributes);
    m_nBaseAttack = get_required_int("baseAttack",attributes);
    m_fBaseHit = get_required_float("hitAdd",attributes);
    m_fBaseCritical = get_implied_float("baseCritical",attributes,0.05);
    m_bRanged = get_implied_bool("ranged",attributes,false);
    m_bTwoHanded  = get_implied_bool("twoHanded",attributes,false);
    m_damageCategory = DamageCategory::DamageCategoryFromString(get_required_string("damageCategory",attributes));
}

void WeaponType::load_finished()
{
    try {
	m_sprite = GraphicsManager::CreateEquipmentSprite(GraphicsManager::EQUIPMENT_SPRITE_WEAPON,m_name);
    }catch(CL_Exception err)
    {
	std::cerr << "Warning: Missing graphic for weapon type : " << m_name << std::endl;
    }
}

bool WeaponType::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EICONREF:
        m_icon_ref = dynamic_cast<IconRef*>(pElement)->GetIcon();
        break;
    case EANIMATION:
	m_pAnimation = dynamic_cast<Animation*>(pElement);
	break;
	
    default:
        return false;
    }

    return true;
}

WeaponType::~WeaponType()
{

}

std::string WeaponType::GetName() const
{
    return m_name;
}

std::string WeaponType::GetIconRef() const
{
    return m_icon_ref;
}

CL_Sprite WeaponType::GetSprite() const
{
    return m_sprite;
}


float WeaponType::GetBaseCritical() const
{
    return m_fBaseCritical;
}

uint WeaponType::GetBaseAttack() const
{
    return m_nBaseAttack;
}

float WeaponType::GetBaseHit() const
{
    return m_fBaseHit;
}

uint WeaponType::GetBasePrice() const
{
    return m_nBasePrice;
}

bool WeaponType::IsRanged() const
{
    return m_bRanged;
}

bool WeaponType::IsTwoHanded() const
{
    return m_bTwoHanded;
}

Animation* WeaponType::GetAnimation() const 
{
    return m_pAnimation;
}




