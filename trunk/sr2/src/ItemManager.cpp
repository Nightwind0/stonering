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

    CL_DomElement armorTypeNode = armorTypesNode.get_first_child().to_element();

    while(!armorTypeNode.is_null())
    {
	
	mArmorTypes.push_back ( pItemFactory->createArmorType ( &armorTypeNode ) );

	armorTypeNode = armorTypeNode.get_next_sibling().to_element();
    }
    

	    
    CL_DomElement itemListNode = itemsNode.named_item("itemList").to_element();

    CL_DomElement namedItemNode = itemListNode.get_first_child().to_element();
    
#ifndef NDEBUG
    uint namedItemCount = 0;
#endif

    while(!namedItemNode.is_null())
    {
#ifndef NDEBUG
	namedItemCount++;
#endif
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
    
#ifndef NDEBUG
    std::cout << "Found " << mWeaponTypes.size() << " weapon types." << std::endl;
    std::cout << "Found " << mWeaponClasses.size() << " weapon classes." << std::endl;
    std::cout << "Found " << mArmorTypes.size() << " armor types." << std::endl;
    std::cout << "Found " << mArmorClasses.size() << " armor classes." << std::endl;
    std::cout << "Found " << namedItemCount << " named items." << std::endl;
    std::cout << "Found " << mItems.size() << " items total." << std::endl;
#endif

    
}
    
void ItemManager::generateWeapons()
{
    for(std::list<WeaponType*>::iterator iter = mWeaponTypes.begin();
	iter != mWeaponTypes.end();
	iter++)
    {
	WeaponType *pType = *iter;
#ifndef NDEBUG
	std::cout << "Generating weapons for type: " << pType->getName() << std::endl;
#endif

	for(std::list<WeaponClass*>::iterator classIter = mWeaponClasses.begin();
	    classIter != mWeaponClasses.end();
	    classIter++)
	{
	    WeaponClass * pClass = *classIter;

	    WeaponTypeRef typeRef;
	    typeRef.setName ( pType->getName() );

	    if(!pClass->isExcluded( typeRef ) )
	    {
		//@todo: Create all the spell combinations. 
		
		// Create the no spell, no rune weapon

		GeneratedWeapon * pPlainWeapon = new GeneratedWeapon();
		pPlainWeapon->generate( pType, pClass, NULL, NULL );

		mItems.push_back ( pPlainWeapon );
	       

		// Create the rune version.
		GeneratedWeapon * pRuneWeapon = new GeneratedWeapon();
		RuneType * runeType = new RuneType();
		runeType->setRuneType( RuneType::RUNE );
		pRuneWeapon->generate( pType, pClass, NULL, runeType );
		
		mItems.push_back ( pRuneWeapon );

	    }
	}
    }
}

void ItemManager::generateArmor()
{

    for(std::list<ArmorType*>::const_iterator iter = mArmorTypes.begin();
	iter != mArmorTypes.end();
	iter++)
    {
	ArmorType *pType = *iter;

#ifndef NDEBUG
	std::cout << "Generating armor for type: " << pType->getName() << std::endl;
#endif
	
	for(std::list<ArmorClass*>::const_iterator classIter = mArmorClasses.begin();
	    classIter != mArmorClasses.end();
	    classIter++)
	{
	    ArmorClass * pClass = *classIter;

	    ArmorTypeRef typeRef;
	    typeRef.setName ( pType->getName() );

	    if(!pClass->isExcluded( typeRef ) )
	    {
		//@todo: Create all the spell combinations. 
		
		// Create the no spell, no rune Armor

		GeneratedArmor * pPlainArmor = new GeneratedArmor();
		pPlainArmor->generate( pType, pClass );

		mItems.push_back ( pPlainArmor );
	       

		// Create the rune version.
		GeneratedArmor * pRuneArmor = new GeneratedArmor();
		RuneType * runeType = new RuneType();
		runeType->setRuneType( RuneType::RUNE );
		pRuneArmor->generate( pType, pClass, NULL, runeType );
		
		mItems.push_back ( pRuneArmor );

		// Create the ultra rune version

		GeneratedArmor * pUltraRuneArmor = new GeneratedArmor();
		RuneType * ultraRuneType = new RuneType();
		ultraRuneType->setRuneType( RuneType::ULTRA_RUNE );

		pUltraRuneArmor->generate( pType, pClass, NULL, ultraRuneType );

		mItems.push_back ( pUltraRuneArmor );


	    }
	}
    }
}

    
WeaponType * ItemManager::getWeaponType(const WeaponTypeRef &ref) const
{
    for(std::list<WeaponType*>::const_iterator iter = mWeaponTypes.begin();
	iter != mWeaponTypes.end();
	iter++)
    {
	if( ref.getName() == (*iter)->getName())
	    return *iter;
    }
}

ArmorType  * ItemManager::getArmorType ( const ArmorTypeRef &ref) const
{
    for(std::list<ArmorType*>::const_iterator iter = mArmorTypes.begin();
	iter != mArmorTypes.end();
	iter++)
    {
	if( ref.getName() == (*iter)->getName())
	    return *iter;
    }
}

WeaponClass * ItemManager::getWeaponClass ( const WeaponClassRef & ref ) const
{
    for(std::list<WeaponClass*>::const_iterator iter = mWeaponClasses.begin();
	iter != mWeaponClasses.end();
	iter++)
    {
	if( ref.getName() == (*iter)->getName())
	    return *iter;
    }
}

ArmorClass  * ItemManager::getArmorClass ( const ArmorClassRef & ref ) const
{


    for(std::list<ArmorClass*>::const_iterator iter = mArmorClasses.begin();
	iter != mArmorClasses.end();
	iter++)
    {
	if( ref.getName() == (*iter)->getName())
	    return *iter;
    }
    

}

Item * ItemManager:: getItem( const ItemRef & ref ) const
{

    if(ref.getType() == ItemRef::NAMED_ITEM)
    {
	for(std::list<Item*>::const_iterator iter = mItems.begin();
	    iter != mItems.end();
	    iter++)
	{
	    
	}
    }
}


#ifndef NDEBUG
void ItemManager::dumpItemList()
{
    for(std::list<Item*>::iterator iter = mItems.begin();
	iter != mItems.end();
	iter++)
    {


	std::cout << '[' << Item::ItemTypeAsString((*iter)->getItemType()) << ']' << ' ';
	std::cout << (*iter)->getName();
	std::cout << " (" << (*iter)->getDropRarity() << ')';
	std::cout << '$' << (*iter)->getValue();
	std::cout << std::endl;
	
    }
}
#endif
