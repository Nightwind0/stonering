#include "WeaponRef.h"
#include "IApplication.h"
#include "WeaponTypeRef.h"
#include "WeaponClassRef.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "ItemManager.h"

using namespace StoneRing;

WeaponRef::WeaponRef():mpWeaponType(NULL), mpWeaponClass(NULL),
                       mpSpellRef(NULL),mpRuneType(NULL)
{
}

std::string WeaponRef::getName() const
{
    return mName;
}

bool WeaponRef::handleElement(eElement element, Element * pElement)
{
    const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    switch(element)
    {
    case EWEAPONTYPEREF:
        mpType =  dynamic_cast<WeaponTypeRef*>(pElement);
        mpWeaponType = pItemManager->getWeaponType(*mpType);
        break;
    case EWEAPONCLASSREF:
        mpClass = dynamic_cast<WeaponClassRef*>(pElement);
        mpWeaponClass = pItemManager->getWeaponClass ( *mpClass );
        break;
    case ESPELLREF:
        mpSpellRef = dynamic_cast<SpellRef*>(pElement);
        break;
    case ERUNETYPE:
        mpRuneType = dynamic_cast<RuneType*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

WeaponRef::~WeaponRef()
{
}


WeaponRef::WeaponRef ( WeaponType *pType, WeaponClass *pClass, 
                       SpellRef * pSpell, RuneType *pRune ):mpWeaponType(pType), mpWeaponClass(pClass),
                                                            mpSpellRef(pSpell), mpRuneType(pRune)

{

}
/*
bool WeaponRef::operator==(const WeaponRef &lhs)
{
    if( *mpWeaponType == *lhs.mpWeaponType &&
        *mpWeaponClass == *lhs.mpWeaponClass)
    {
        if( mpSpellRef && lhs.mpSpellRef )
        {
            if(!(*mpSpellRef == *lhs.mpSpellRef))
            {
                return false;
            }
        }
        else if ( mpSpellRef || lhs.mpSpellRef )
        {
            // One, but not both, had a spell ref
            return false;
        }


        if( mpRuneType && lhs.mpRuneType )
        {
            if(!(*mpRuneType == *lhs.mpRuneType))
            {
                return false;
            }
        }
        else if ( mpRuneType || lhs.mpRuneType)
        {
            return false;
        }
    }

    return true;

}
*/
void WeaponRef::loadFinished()
{
    mName = Weapon::CreateWeaponName(mpWeaponType,mpWeaponClass,mpSpellRef,mpRuneType);
}


WeaponType * 
WeaponRef::getWeaponType() const
{
    return mpWeaponType;
}

WeaponClass * WeaponRef::getWeaponClass() const
{
    return mpWeaponClass;
}

SpellRef * WeaponRef::getSpellRef() const
{
    return mpSpellRef;
}

RuneType * WeaponRef::getRuneType() const
{
    return mpRuneType;
}




