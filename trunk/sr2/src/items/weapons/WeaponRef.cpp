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


bool WeaponRef::handleElement(eElement element, Element * pElement)
{
    const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    switch(element)
    {
    case EWEAPONTYPEREF:
        mType = * (dynamic_cast<WeaponTypeRef*>(pElement));
        mpWeaponType = pItemManager->getWeaponType(mType);
        break;
    case EWEAPONCLASSREF:
        mClass = * (dynamic_cast<WeaponClassRef*>(pElement));
        mpWeaponClass = pItemManager->getWeaponClass ( mClass );
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


CL_DomElement  
WeaponRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"weaponRef");



    element.append_child ( mType.createDomElement(doc ) );

    element.append_child ( mClass.createDomElement(doc ) );

    if(mpSpellRef)
    {
        element.append_child ( mpSpellRef->createDomElement(doc ) );
    }
    else if (mpRuneType)
    {
        element.append_child ( mpRuneType->createDomElement(doc ) );
    }

    return element;

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


