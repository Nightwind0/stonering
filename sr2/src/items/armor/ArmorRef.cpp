
#include "ArmorRef.h"
#include "ArmorType.h"
#include "ArmorClass.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "ItemManager.h"
#include "IApplication.h"



using namespace StoneRing;

ArmorRef::ArmorRef ( ArmorType *pType, ArmorClass *pClass, 
                     SpellRef * pSpell, RuneType *pRune ):mpArmorType(pType), mpArmorClass(pClass),
                                                          mpSpellRef(pSpell), mpRuneType(pRune)

{

}



bool ArmorRef::operator==(const ArmorRef &lhs)
{
    if( *mpArmorType == *lhs.mpArmorType &&
        *mpArmorClass == *lhs.mpArmorClass)
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

bool ArmorRef::handleElement(eElement element, Element * pElement)
{
    const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    switch(element)
    {
    case EARMORTYPEREF:
        mType = * (dynamic_cast<ArmorTypeRef*>(pElement));
        mpArmorType = pItemManager->getArmorType(mType);
        break;
    case EARMORCLASSREF:
        mClass = * (dynamic_cast<ArmorClassRef*>(pElement));
        mpArmorClass = pItemManager->getArmorClass ( mClass );
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


ArmorRef::ArmorRef():mpArmorType(NULL),
                     mpArmorClass(NULL),
                     mpSpellRef(NULL),
                     mpRuneType(NULL)
{

}

ArmorRef::~ArmorRef()
{
}

CL_DomElement  
ArmorRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"armorRef");

    element.append_child ( mType.createDomElement(doc ) );

    element.append_child ( mClass.createDomElement(doc ) );

    if(mpSpellRef )
    {
        element.append_child ( mpSpellRef->createDomElement(doc ) );
    }
    else if (mpRuneType)
    {
        element.append_child ( mpRuneType->createDomElement(doc ) );
    }

    return element;

}


ArmorType * ArmorRef::getArmorType() const
{
    return mpArmorType;
}

ArmorClass * ArmorRef::getArmorClass() const
{
    return mpArmorClass;
}

SpellRef * ArmorRef::getSpellRef() const
{
    return mpSpellRef;
}

RuneType * ArmorRef::getRuneType() const
{
    return mpRuneType;
}





