#include <string>

#include "Item.h"




StoneRing::Item::Item()
{
}
StoneRing::Item::~Item()
{
}

 



std::string StoneRing::Item::getName() const
{
    return mItemName;
}

void StoneRing::Item::setName(const std::string &name )
{
    mItemName = name;
}


void StoneRing::Item::setItemType ( eItemType type )
{
    meItemType = type;
}

StoneRing::Item::eItemType 
StoneRing::Item::getItemType() const
{
    return meItemType;
}


bool   StoneRing::operator < ( const StoneRing::Item &lhs, const StoneRing::Item &rhs )
{
    return std::string(Item::ItemTypeAsString(lhs.getItemType()) + lhs.getName())
					<
	std::string(Item::ItemTypeAsString(rhs.getItemType()) + rhs.getName());
}


std::string StoneRing::Item::ItemTypeAsString ( StoneRing::Item::eItemType type )
{

    switch(type )
    {
	//enum eItemType { ITEM, WEAPON, ARMOR, RUNE, SPECIAL, SYSTEM };
    case ITEM:
	return "item";
    case WEAPON:
	return "weapon";
    case ARMOR:
	return "armor";
    case RUNE:
	return "rune";
    case SPECIAL:
	return "special";
    case SYSTEM:
	return "system";
	  
    }

    return "";
}



