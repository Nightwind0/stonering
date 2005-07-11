#include <ClanLib/core.h>
#include "ItemManager.h"

using namespace StoneRing;

ItemManager::ItemManager()
{
}

ItemManager::~ItemManager()
{
    // Clean up items..
}


void ItemManager::loadItemFile ( CL_DomDocument &doc )
{
}
    

WeaponType * ItemManager::getWeaponType(const WeaponTypeRef &ref) const
{
    return NULL;
}

ArmorType  * ItemManager::getArmorType ( const ArmorTypeRef &ref) const
{
    return NULL;
}

WeaponClass * ItemManager::getWeaponClass ( const WeaponClassRef & ref ) const
{
    return NULL;
}

ArmorClass  * ItemManager::getArmorClass ( const ArmorClassRef & ref ) const
{
    return NULL;
}

Item * ItemManager:: getItem( const ItemRef & ref ) const
{
    return NULL;
}

