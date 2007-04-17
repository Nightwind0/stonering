#include "NamedItem.h"
#include "IconRef.h"

using namespace StoneRing;

void NamedItemElement::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    std::string dropRarity = getRequiredString("dropRarity",pAttributes);

    meDropRarity = Item::DropRarityFromString ( dropRarity );

    mnMaxInventory = getImpliedInt("maxInventory",pAttributes,99);
}

bool NamedItemElement::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EICONREF:
        mIconRef = dynamic_cast<IconRef*>(pElement)->getIcon();
        break;
    case EREGULARITEM:
    case EUNIQUEWEAPON:
    case EUNIQUEARMOR:
    case ERUNE:
    case ESPECIALITEM:
    case ESYSTEMITEM:
        mpNamedItem = dynamic_cast<NamedItem*>(pElement);
        break;
    default:
        return false;
    }
    return true;
}

void NamedItemElement::loadFinished()
{
    if(mpNamedItem == NULL) throw CL_Error("No named item within a named item element :" + mName);   
    mpNamedItem->setIconRef( mIconRef );
    mpNamedItem->setName ( mName );
    mpNamedItem->setMaxInventory ( mnMaxInventory );
    mpNamedItem->setDropRarity( meDropRarity );
}

NamedItemElement::NamedItemElement ():mpNamedItem(NULL),meDropRarity(Item::NEVER),mnMaxInventory(0)
{
}

NamedItemElement::~NamedItemElement()
{

}



CL_DomElement  NamedItemElement::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"namedItem");
}

NamedItem * 
NamedItemElement::getNamedItem() const
{
    return mpNamedItem;
}

std::string NamedItemElement::getIconRef() const
{
    return mIconRef;
}

uint NamedItemElement::getMaxInventory() const
{
    return mnMaxInventory;
}

Item::eDropRarity 
NamedItemElement::getDropRarity() const
{
    return meDropRarity;
}


std::string 
NamedItemElement::getName() const
{
    return mName;
}




NamedItem::NamedItem()
{
}

NamedItem::~NamedItem()
{
}

bool NamedItem::operator== ( const ItemRef &ref )
{
    if( ref.getType() == ItemRef::NAMED_ITEM
        && ref.getNamedItemRef()->getItemName() == mName)
        return true;
    else return false;
}




std::string NamedItem::getIconRef() const
{
    return mIconRef;
}

std::string NamedItem::getName() const
{
    return mName;
}

uint NamedItem::getMaxInventory() const 
{
    return mnMaxInventory;
}

NamedItem::eDropRarity NamedItem::getDropRarity() const
{
    return meDropRarity;
}




void NamedItem::setIconRef(const std::string &ref)
{
    mIconRef = ref;
}

void NamedItem::setName ( const std::string &name )
{
    mName = name;
}

void NamedItem::setMaxInventory ( uint max )
{
    mnMaxInventory = max;
}

void NamedItem::setDropRarity( Item::eDropRarity rarity )
{
    meDropRarity = rarity;
}




