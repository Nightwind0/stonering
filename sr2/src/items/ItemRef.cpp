#include "ItemRef.h"
#include "WeaponRef.h"
#include "ArmorRef.h"
#include "ItemManager.h"




bool StoneRing::operator<(const StoneRing::ItemRef &lhs,const StoneRing::ItemRef &rhs)
{
    return lhs.getItemName() < rhs.getItemName();
}

bool StoneRing::ItemRef::handleElement(eElement element, StoneRing::Element * pElement )
{
    switch(element)
    {
    case ENAMEDITEMREF:
        meType = NAMED_ITEM;
        mpNamedItemRef = dynamic_cast<NamedItemRef*>(pElement);
        break;
    case EWEAPONREF:
        meType = WEAPON_REF;
        mpWeaponRef = dynamic_cast<WeaponRef*>(pElement);
        break;
    case EARMORREF:
        meType = ARMOR_REF;
        mpArmorRef = dynamic_cast<ArmorRef*>(pElement);
        break;
    default:
        
        return false;
    }

    return true;
}

void StoneRing::ItemRef::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    
}

void StoneRing::ItemRef::loadFinished()
{
    ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    if(!mpNamedItemRef && !mpWeaponRef && !mpArmorRef)
    {
        throw CL_Error("Item Ref with no child");
    }
    
    mpItem = pItemManager->getItem ( *this ); 

}


StoneRing::ItemRef::ItemRef( ):
    mpNamedItemRef(NULL),mpWeaponRef(NULL),mpArmorRef(NULL)
{
 
  
}


StoneRing::ItemRef::~ItemRef()
{
    delete mpNamedItemRef;
    delete mpWeaponRef;
    delete mpArmorRef;

}

std::string StoneRing::ItemRef::getItemName() const
{
    switch ( meType )
    {
    case NAMED_ITEM:
        return mpNamedItemRef->getItemName();
    case WEAPON_REF:
        return mpWeaponRef->getName();
    case ARMOR_REF:
        return mpArmorRef->getName();
    default:
        assert(0);
        return "";
    }
}

StoneRing::ItemRef::eRefType StoneRing::ItemRef::getType() const
{
    return meType;
}

StoneRing::NamedItemRef * StoneRing::ItemRef::getNamedItemRef() const
{
    return mpNamedItemRef;
}

StoneRing::WeaponRef * StoneRing::ItemRef::getWeaponRef() const
{
    return mpWeaponRef;
}

StoneRing::ArmorRef * StoneRing::ItemRef::getArmorRef() const
{
    return mpArmorRef;
}



StoneRing::NamedItemRef::NamedItemRef()
{
}

StoneRing::NamedItemRef::~NamedItemRef()
{
}


std::string StoneRing::NamedItemRef::getItemName()
{
    return mName;
}

void StoneRing::NamedItemRef::handleText(const std::string &text)
{
    mName = text;
}
