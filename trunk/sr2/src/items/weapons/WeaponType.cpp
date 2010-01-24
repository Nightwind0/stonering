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

void WeaponType::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    m_nBasePrice = get_required_int("basePrice",attributes);
    m_nBaseAttack = get_required_int("baseAttack",attributes);
    m_fBaseHit = get_required_float("hitAdd",attributes);
    m_fBaseCritical = get_implied_float("baseCritical",attributes,0.05);
    m_bRanged = get_implied_bool("ranged",attributes,false);
    m_bTwoHanded  = get_implied_bool("twoHanded",attributes,false);
    m_damageCategory = DamageCategoryFromString(get_required_string("damageCategory",attributes));
}

bool WeaponType::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EICONREF:

        m_icon_ref = dynamic_cast<IconRef*>(pElement)->GetIcon();
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




