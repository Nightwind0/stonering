#include "WeaponType.h"
#include "IconRef.h"
#include "DamageCategory.h"

using namespace StoneRing;

WeaponType::WeaponType()
{
}


bool WeaponType::operator==(const WeaponType &type )
{
    return mName == type.mName;
}

void WeaponType::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
    mnBasePrice = getRequiredInt("basePrice",pAttributes);
    mnBaseAttack = getRequiredInt("baseAttack",pAttributes);
    mfBaseHit = getRequiredFloat("baseHit",pAttributes);
    mfBaseCritical = getImpliedFloat("baseCritical",pAttributes,0.05);
    mbRanged = getImpliedBool("ranged",pAttributes,false);
    mbTwoHanded  = getImpliedBool("twoHanded",pAttributes,false);
}

bool WeaponType::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EICONREF:

        mIconRef = dynamic_cast<IconRef*>(pElement)->getIcon();
        break;
    case EWEAPONDAMAGECATEGORY:
        mpDamageCategory = dynamic_cast<DamageCategory*>(pElement);
        break;
    case EMAGICDAMAGECATEGORY:
        mpDamageCategory = dynamic_cast<DamageCategory*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

WeaponType::~WeaponType()
{
    delete mpDamageCategory;
}

std::string WeaponType::getName() const
{
    return mName;
}

std::string WeaponType::getIconRef() const
{
    return mIconRef;
}


float WeaponType::getBaseCritical() const
{
    return mfBaseCritical;
}

uint WeaponType::getBaseAttack() const
{
    return mnBaseAttack;
}

float WeaponType::getBaseHit() const
{
    return mfBaseHit;
}

uint WeaponType::getBasePrice() const
{
    return mnBasePrice;
}

bool WeaponType::isRanged() const
{
    return mbRanged;
}

bool WeaponType::isTwoHanded() const
{
    return mbTwoHanded;
}




