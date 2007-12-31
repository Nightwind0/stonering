
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

std::string ArmorRef::getName() const
{
    return  mName;
}

/*
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
*/
bool ArmorRef::handleElement(eElement element, Element * pElement)
{
    const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    switch(element)
    {
    case EARMORTYPEREF:
        mpType = (dynamic_cast<ArmorTypeRef*>(pElement));
        mpArmorType = pItemManager->getArmorType(*mpType);
        break;
    case EARMORCLASSREF:
        mpClass =  (dynamic_cast<ArmorClassRef*>(pElement));
        mpArmorClass = pItemManager->getArmorClass (*mpClass);
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

void ArmorRef::loadFinished()
{
    mName = Armor::CreateArmorName(mpArmorType,mpArmorClass,mpSpellRef,mpRuneType);
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





