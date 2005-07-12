#include <ClanLib/core.h>
#include "ItemManager.h"
#include "IApplication.h"
#include "ItemFactory.h"

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
    ItemFactory * pItemFactory = IApplication::getInstance()->getItemFactory();

    CL_DomElement itemsNode = doc.named_item("items").to_element();
    CL_DomElement weaponClassesNode = itemsNode.named_item("weaponClasses").to_element(); 
    

    CL_DomElement weaponClassNode = weaponClassesNode.get_first_child().to_element();

    while(!weaponClassNode.is_null())
    {
	
	mWeaponClasses.push_back ( pItemFactory->createWeaponClass ( &weaponClassNode ) );
	weaponClassNode = weaponClassNode.get_next_sibling().to_element();
    }
    
    CL_DomElement weaponTypesNode = itemsNode.named_item("weaponTypes").to_element();
    
    CL_DomElement weaponTypeNode = weaponTypesNode.get_first_child().to_element();
    
    while(!weaponTypeNode.is_null())
    {
	
	mWeaponTypes.push_back ( pItemFactory->createWeaponType ( &weaponTypeNode ) );

	weaponTypeNode = weaponTypeNode.get_next_sibling().to_element();
    }

    CL_DomElement armorClassesNode = itemsNode.named_item("armorClasses").to_element();

    CL_DomElement armorClassNode = armorClassesNode.get_first_child().to_element();

    while(!armorClassNode.is_null())
    {

	mArmorClasses.push_back ( pItemFactory->createArmorClass ( &armorClassNode  ));

	armorClassNode = armorClassNode.get_next_sibling().to_element();
    }


    CL_DomElement armorTypesNode = itemsNode.named_item("armorTypes").to_element();

    CL_DomElement armorTypeNode = weaponTypesNode.get_first_child().to_element();

    while(!armorTypeNode.is_null())
    {
	
	mArmorTypes.push_back ( pItemFactory->createArmorType ( &armorTypeNode ) );

	armorTypeNode = armorTypeNode.get_next_sibling().to_element();
    }
    

	    
    CL_DomElement itemListNode = itemsNode.named_item("itemList").to_element();

    CL_DomElement namedItemNode = itemListNode.get_first_child().to_element();
    
    while(!namedItemNode.is_null())
    {
	
	NamedItemElement * pElement = pItemFactory->createNamedItemElement ( &namedItemNode );
	
	NamedItem * pItem = pElement->getNamedItem();
	
	pItem->setIconRef ( pElement->getIconRef() );
	pItem->setMaxInventory ( pElement->getMaxInventory() );
	pItem->setName ( pElement->getName() );
	pItem->setDropRarity ( pElement->getDropRarity() );
	
	mItems.push_back ( pItem );
	
	delete pElement;
	
	namedItemNode = namedItemNode.get_next_sibling().to_element();
    }
    
    
    generateWeapons();
    generateArmor();
    
    
}
    
void ItemManager::generateWeapons()
{
}

void ItemManager::generateArmor()
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

