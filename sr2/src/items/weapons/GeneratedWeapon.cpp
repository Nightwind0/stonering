#include "GeneratedWeapon.h"
#include "WeaponRef.h"
#include "WeaponClass.h"
#include "NamedItem.h"
#include "WeaponType.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "Spell.h"
#include "AbilityManager.h"
#include "IApplication.h"

using namespace StoneRing;

GeneratedWeapon::GeneratedWeapon():mpClass(NULL),mpType(NULL)
{
}

GeneratedWeapon::~GeneratedWeapon()
{
}

bool GeneratedWeapon::operator== ( const ItemRef &ref )
{
    if( ref.getType() == ItemRef::WEAPON_REF
        && *ref.getWeaponRef()->getWeaponClass() == *mpClass &&
        *ref.getWeaponRef()->getWeaponType() == *mpType)
    {
        if(hasSpell() && ref.getWeaponRef()->getSpellRef())
        {
            if(*getSpellRef() == *ref.getWeaponRef()->getSpellRef())
            {
                return true;
            } 
            else return false;
        }
        else if ( hasSpell() || ref.getWeaponRef()->getSpellRef())
        {
            // One had a spell ref and one didn't.
            return false;
        }

        if(hasRuneType() && ref.getWeaponRef()->getRuneType())
        {
            if(*getRuneType() == *ref.getWeaponRef()->getRuneType())
            {
                return true;
            }
            else return false;
        }
        else if ( hasRuneType() || ref.getWeaponRef()->getRuneType())
        {
            return false;

        }
        return true;
    }

    return false;
}




// Item interface 
std::string GeneratedWeapon::getIconRef() const
{
    return mpType->getIconRef();
}

std::string GeneratedWeapon::getName() const
{
    return mName;
}

uint GeneratedWeapon::getMaxInventory() const 
{
    // todo: get the system setting.... 
    return 99;
}

NamedItem::eDropRarity 
GeneratedWeapon::getDropRarity() const
{
    if( hasSpell() || hasRuneType() )
    {
        return RARE; 
    }
    else return UNCOMMON;
}

uint GeneratedWeapon::getValue() const 
{
    const AbilityManager * pManager = IApplication::getInstance()->getAbilityManager();

    uint value= (int)((float)mpType->getBasePrice() * 
                      mpClass->getValueMultiplier()) 
        + mpClass->getValueAdd();

    if(hasSpell())
    {
        SpellRef * pSpellRef = getSpellRef();

        Spell * pSpell = pManager->getSpell ( *pSpellRef );

        value += pSpell->getValue();
    }

    if(hasRuneType())
    {
        value *= 2; //@todo : get multiplier from game settings
    }

    return value;

}

uint GeneratedWeapon::getSellValue() const 
{
    return getValue() / 2;
}

// Weapon interface



WeaponType * GeneratedWeapon::getWeaponType() const
{
    return mpType;
}

bool GeneratedWeapon::isRanged() const 
{
    return mpType->isRanged();
}

bool GeneratedWeapon::isTwoHanded() const
{
    return mpType->isTwoHanded();
}


WeaponRef GeneratedWeapon::generateWeaponRef() const
{
    return WeaponRef( getWeaponType(), getWeaponClass(), getSpellRef(), getRuneType() );
}

void GeneratedWeapon::executeScript()
{
    mpClass->executeScript();
}

bool GeneratedWeapon::equipCondition()
{
    return mpClass->equipCondition();
}

void GeneratedWeapon::onEquipScript()
{
    return mpClass->onEquipScript();
}

void GeneratedWeapon::onUnequipScript()
{
    return mpClass->onUnequipScript();
}

void GeneratedWeapon::generate( WeaponType* pType, WeaponClass * pClass, 
                                SpellRef *pSpell , RuneType *pRune)
{

    for(std::list<AttributeEnhancer*>::const_iterator iter = pClass->getAttributeEnhancersBegin();
        iter != pClass->getAttributeEnhancersEnd();
        iter++)
    {
        addAttributeEnhancer  ( *iter );
    }
    for(std::list<WeaponEnhancer*>::const_iterator iter2 = pClass->getWeaponEnhancersBegin();
        iter2 != pClass->getWeaponEnhancersEnd();
        iter2++)
    {
        addWeaponEnhancer ( *iter2 );
    }



    std::ostringstream os;

    mpType = pType;
    mpClass = pClass;

    if(pSpell)
    {
        setSpellRef ( pSpell );
    }
    else if ( pRune )
    {
        setRuneType ( pRune );

        os << pRune->getRuneTypeAsString() << ' ';
    }

    os << pClass->getName() << ' ';

    os << pType->getName();

    if(pSpell)
    {
        os << " of " << pSpell->getName();
    }




    mName = os.str();


}





