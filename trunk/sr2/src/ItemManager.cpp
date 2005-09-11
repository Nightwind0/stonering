#include <ClanLib/core.h>
#include "ItemManager.h"
#include "IApplication.h"
#include "ItemFactory.h"

#ifndef NDEBUG
#include <algorithm>
#include <iomanip>
#endif



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
		// Create all the spell combinations. 
		const AbilityManager * pAbilityManager = IApplication::getInstance()->getAbilityManager();
		for(std::list<Spell*>::const_iterator spellIter = pAbilityManager->getSpellsBegin();
		    spellIter != pAbilityManager->getSpellsEnd();
		    spellIter++)
		{
		    Spell * pSpell = *spellIter;

		    if(pSpell->appliesToWeapons())
		    {
			GeneratedWeapon * pWeapon = new GeneratedWeapon();
			pWeapon->generate(pType, pClass, pSpell->createSpellRef(), NULL);
			mItems.push_back ( pWeapon );
		    }
		}
		
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
		// Create all the spell combinations. 
		const AbilityManager * pAbilityManager = IApplication::getInstance()->getAbilityManager();
		for(std::list<Spell*>::const_iterator spellIter = pAbilityManager->getSpellsBegin();
		    spellIter != pAbilityManager->getSpellsEnd();
		    spellIter++)
		{
		    Spell * pSpell = *spellIter;

		    if(pSpell->appliesToArmor())
		    {
			GeneratedArmor * pArmor = new GeneratedArmor();
			pArmor->generate(pType, pClass, pSpell->createSpellRef(), NULL);
			mItems.push_back ( pArmor );
		    }
		}
		


		
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


	for(std::list<Item*>::const_iterator iter = mItems.begin();
	    iter != mItems.end();
	    iter++)
	{
	    if( *(*iter) == ref ) 
		return *iter;
	}

	throw CL_Error("Couldn't find item based on ref.");

	return NULL;

}


#ifndef NDEBUG
void ItemManager::dumpItemList()
{
    for(std::list<Item*>::iterator iter = mItems.begin();
	iter != mItems.end();
	iter++)
    {
	Item * pItem = *iter;
	
	std::cout << '[' << Item::ItemTypeAsString(pItem->getItemType()) << ']' << ' ';
	std::cout << pItem->getName();
	std::cout << " (" << pItem->getDropRarity() << ") ";
	std::cout << '$' << pItem->getValue();
	std::cout << std::endl;


	switch(pItem->getItemType())
	{
	case Item::WEAPON:
	{
	    Weapon * pWeapon = dynamic_cast<Weapon*>(pItem);
	    WeaponType * pType = pWeapon->getWeaponType();
	    std::cout << '\t' << "ATK: " << std::setw(5) <<  pWeapon->modifyWeaponAttribute(Weapon::ATTACK, (int)pType->getBaseAttack());
	    std::cout << ' ' << "Hit% " << std::setw(4) << pWeapon->modifyWeaponAttribute(Weapon::HIT, pType->getBaseHit()) * 100;
	    std::cout << ' ' << "Critical% " << std::setw(4) << pWeapon->modifyWeaponAttribute(Weapon::CRITICAL, pType->getBaseCritical()) * 100 << std::endl;
	    DamageCategory * pDamageCategory = pType->getDamageCategory();

	    if(pDamageCategory->getClass() != DamageCategory::WEAPON) throw CL_Error("What? This weapon has a magic damage category.");

	    WeaponDamageCategory * pWDC = dynamic_cast<WeaponDamageCategory*>(pDamageCategory);
	    

	    switch ( pWDC->getType() )
	    {
	    case WeaponDamageCategory::JAB:
		std::cout << "\t[JAB]";
		break;
	    case WeaponDamageCategory::BASH:
		std::cout << "\t[BASH]";
		break;
	    case WeaponDamageCategory::SLASH:
		std::cout << "\t[SLASH]";
		break;
		
	    }
	    

	    if(pWeapon->isTwoHanded()) std::cout << "(Two Handed)";

	    if(pWeapon->isRanged()) std::cout << "(Ranged)" ;
	       
	    std::cout << std::endl;

	    // If there are attribute enhancers, lets list them.
	    printAttributeEnhancers(pWeapon);

	    break;
	}
	case Item::ARMOR:
	{
	    Armor * pArmor = dynamic_cast<Armor*>(pItem);

	    ArmorType * pType = pArmor->getArmorType();
	    std::cout << '\t' << "AC: " << std::setw(5) <<  pArmor->modifyArmorAttribute(Armor::AC, (int)pType->getBaseAC());
	    std::cout << ' ' << "RST " << std::setw(4) << pArmor->modifyArmorAttribute(Armor::RESIST, pType->getBaseRST()) << std::endl;
	    
	    switch( pType->getSlot())
	    {
	    case ArmorType::HEAD:
		std::cout << "\t[HEAD]";
		break;
	    case ArmorType::BODY:
		std::cout << "\t[BODY]";
		break;
	    case ArmorType::SHIELD:
		std::cout << "\t[SHIELD]";
		break;
	    case ArmorType::FEET:
		std::cout << "\t[FEET]";
		break;
	    case ArmorType::HANDS:
		std::cout << "\t[HANDS]";
		break;
	    }

	    std::cout << std::endl;

	    printAttributeEnhancers(pArmor);

	    break;
	}
	    
	}
	
    }
}
void ItemManager::printAttributeEnhancers(Equipment * pItem )
{
    if( pItem->getAttributeEnhancersBegin() != pItem->getAttributeEnhancersEnd() )
    {
	std::cout << "\tAttribute Enhancers:"  << std::endl;
	
	for(std::list<AttributeEnhancer*>::const_iterator iter = pItem->getAttributeEnhancersBegin();
	    iter != pItem->getAttributeEnhancersEnd();
	    iter++)
	{
	    AttributeEnhancer * pEnhancer = *iter;
	    std::cout << "\t\t" << pEnhancer->getAttribute() << ' ';
	    if(pEnhancer->getMultiplier() != 1)
	    {
		if ( pEnhancer->getMultiplier() > 1)
		    std::cout << (pEnhancer->getMultiplier() - 1) * 100 << "% BONUS";
		else std::cout << abs(pEnhancer->getMultiplier() - 1) * 100 << "% PENALTY";
	    }
	    if(pEnhancer->getAdd() != 0)
		std::cout << " +" << pEnhancer->getAdd();
	    std::cout << std::endl;
	}
    }
    
}



#endif
