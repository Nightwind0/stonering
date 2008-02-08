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
        mRef.mpNamedItemRef = dynamic_cast<NamedItemRef*>(pElement);
        break;
    case EWEAPONREF:
        meType = WEAPON_REF;
        mRef.mpWeaponRef = dynamic_cast<WeaponRef*>(pElement);
        break;
    case EARMORREF:
        meType = ARMOR_REF;
        mRef.mpArmorRef = dynamic_cast<ArmorRef*>(pElement);
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

    if(meType == INVALID)
    {
        throw CL_Error("Item Ref with no child");
    }
    
    mpItem = pItemManager->getItem ( *this ); 

}


StoneRing::ItemRef::ItemRef( ):meType(INVALID)
{
    memset(&mRef,0,sizeof(mRef));
}


StoneRing::ItemRef::~ItemRef()
{

}

std::string StoneRing::ItemRef::getItemName() const
{
    switch ( meType )
    {
    case NAMED_ITEM:
        return mRef.mpNamedItemRef->getItemName();
    case WEAPON_REF:
        return mRef.mpWeaponRef->getName();
    case ARMOR_REF:
        return mRef.mpArmorRef->getName();
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
    assert(meType == NAMED_ITEM);
    return mRef.mpNamedItemRef;
}

StoneRing::WeaponRef * StoneRing::ItemRef::getWeaponRef() const
{
    assert(meType == WEAPON_REF);
    return mRef.mpWeaponRef;
}

StoneRing::ArmorRef * StoneRing::ItemRef::getArmorRef() const
{
    assert(meType == ARMOR_REF);
    return mRef.mpArmorRef;
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


void StoneRing::NamedItemRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
}

