#include "WeaponType.h"
#include "IconRef.h"
#include "DamageCategory.h"

using namespace StoneRing;

WeaponType::WeaponType()
{
}


bool WeaponType::operator==(const WeaponType &type )
{
    return m_name == type.m_name;
}

void WeaponType::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_name = get_required_string("name",pAttributes);
    m_nBasePrice = get_required_int("basePrice",pAttributes);
    m_nBaseAttack = get_required_int("baseAttack",pAttributes);
    m_fBaseHit = get_required_float("hitAdd",pAttributes);
    m_fBaseCritical = get_implied_float("baseCritical",pAttributes,0.05);
    m_bRanged = get_implied_bool("ranged",pAttributes,false);
    m_bTwoHanded  = get_implied_bool("twoHanded",pAttributes,false);
}

bool WeaponType::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EICONREF:

        m_icon_ref = dynamic_cast<IconRef*>(pElement)->GetIcon();
        break;
    case EWEAPONDAMAGECATEGORY:
        m_pDamageCategory = dynamic_cast<DamageCategory*>(pElement);
        break;
    case EMAGICDAMAGECATEGORY:
        m_pDamageCategory = dynamic_cast<DamageCategory*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

WeaponType::~WeaponType()
{
    delete m_pDamageCategory;
}

std::string WeaponType::GetName() const
{
    return m_name;
}

std::string WeaponType::GetIconRef() const
{
    return m_icon_ref;
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




