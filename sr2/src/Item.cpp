#include <string>
#include <sstream>
#include "Item.h"
#include "ItemManager.h"



using namespace StoneRing;



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
    case Item::REGULAR_ITEM:
	return "item";
    case Item::WEAPON:
	return "weapon";
    case Item::ARMOR:
	return "armor";
    case Item::RUNE:
	return "rune";
    case Item::SPECIAL:
	return "special";
    case Item::SYSTEM:
	return "system";
	  
    }

    return "";
}




Item::Item()
{
}

Item::~Item()
{
}



Equipment::Equipment():meMagic(NONE)
{
    mSpellOrRuneRef.mpSpellRef = NULL;
}


Equipment::~Equipment()
{
}

SpellRef * Equipment::getSpellRef() const
{
    return mSpellOrRuneRef.mpSpellRef;
}

RuneType * Equipment::getRuneType() const
{
    return mSpellOrRuneRef.mpRuneType;
}

bool Equipment::hasSpell() const
{
    if( meMagic == SPELL ) return true;
    else return false;
}

bool Equipment::hasRuneType() const
{
    if( meMagic == RUNE ) return true;
    else return false;
}

// Apply any attribute enhancements 
void Equipment::equip()
{

    std::for_each( mAttributeEnhancers.begin(), mAttributeEnhancers.end(), std::mem_fun ( &AttributeEnhancer::invoke ) );

/*    for(std::list<AttributeEnhancer*>::const_iterator iter = mAttributeEnhancers.begin();
	iter != mAttributeEnhancers.end(); iter++)
    {
        (*iter)->invoke();
    }
*/

}

// Remove any attribute enhancements
void Equipment::unequip()
{

    std::for_each( mAttributeEnhancers.begin(), mAttributeEnhancers.end(), std::mem_fun ( &AttributeEnhancer::revoke ) );

}


std::list<AttributeEnhancer*>::const_iterator Equipment::getAttributeEnhancersBegin() const
{
    return mAttributeEnhancers.begin();
}

std::list<AttributeEnhancer*>::const_iterator Equipment::getAttributeEnhancersEnd() const
{
    return mAttributeEnhancers.end();
}


void Equipment::clearAttributeEnhancers()
{
    mAttributeEnhancers.clear();
}

void Equipment::addAttributeEnhancer( AttributeEnhancer * pAttr )
{
    mAttributeEnhancers.push_back ( pAttr );
}

void Equipment::setSpellRef ( SpellRef * pRef )
{
    mSpellOrRuneRef.mpSpellRef = pRef;
    meMagic = SPELL;
}

void Equipment::setRuneType ( RuneType * pType )
{
    mSpellOrRuneRef.mpRuneType = pType;

    meMagic = RUNE;
}


Weapon::Weapon()
{
}

Weapon::~Weapon()
{
}

	
	 
int Weapon::modifyWeaponAttribute( eAttribute attr, int current )
{
}

float Weapon::modifyWeaponAttribute ( eAttribute attr, float current )
{
}



//todo: Getters for weapon enhancers. need 'em.

void Weapon::clearWeaponEnhancers()
{
    mWeaponEnhancers.clear();
}

void Weapon::addWeaponEnhancer (WeaponEnhancer * pEnhancer)
{
    mWeaponEnhancers.push_back ( pEnhancer );
}



/* enum eAttribute
	{
	    ATTACK,
	    HIT,
	    POISON,
	    STONE,
	    DEATH,
	    CONFUSE,
	    BERSERK,
	    SLOW,
	    WEAK,
	    BREAK, 
	    SILENCE,
	    SLEEP,
	    BLIND,
	    STEAL_HP,
	    STEAL_MP,
	    DROPSTR,
	    DROPDEX,
	    DROPMAG
	    } */
	


Weapon::eAttribute 
Weapon::attributeForString(const std::string str)
{
    if(str == "ATK") return ATTACK;
    else if (str == "HIT") return HIT;
    else if (str == "Poison%") return POISON;
    else if (str == "Stone%")  return STONE;
    else if (str == "Death%") return DEATH;
    else if (str == "Confuse%") return CONFUSE;
    else if (str == "Berserk%") return BERSERK;
    else if (str == "Slow%")  return SLOW;
    else if (str == "Weak%") return WEAK;
    else if (str == "Break%") return BREAK;
    else if (str == "Silence%") return SILENCE;
    else if (str == "Sleep%")  return SLEEP;
    else if (str == "Blind%") return BLIND;
    else if (str == "Steal_HP%") return STEAL_HP;
    else if (str == "Steal_MP%") return STEAL_MP;
    else if (str == "DropSTR") return DROPSTR;
    else if (str == "DropDEX") return DROPDEX;
    else if (str == "DropMAG") return DROPMAG;
    else throw CL_Error("Bad Weapon Enhancer Attribute : " + str );


}
	



Armor::Armor()
{
}


Armor::~Armor()
{
}
	


/*	enum eAttribute
	{
	    AC,
	    POISON,
	    STONE,
	    DEATH,
	    CONFUSE,
	    BERSERK,
	    SLOW,
	    WEAK,
	    BREAK, 
	    SILENCE,
	    SLEEP,
	    BLIND,
	    STEAL_MP,
	    STEAL_HP,
	    DROPSTR,
	    DROPDEX,
	    DROPMAG,
	    ELEMENTAL_MAGIC,
	    ALL_MAGIC
	}
*/
	
	 
int Armor::modifyArmorAttribute( eAttribute attr, int current )
{
}

float Armor::modifyArmorAttribute ( eAttribute attr, float current )
{
}

	

void Armor::clearArmorEnhancers()
{
    mArmorEnhancers.clear();
}

void Armor::addArmorEnhancer (ArmorEnhancer * pEnhancer)
{
    mArmorEnhancers.push_back ( pEnhancer );
}
 


NamedItem::NamedItem()
{
}

NamedItem::~NamedItem()
{
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



RegularItem::RegularItem()
{
}

RegularItem::~RegularItem()
{
}

// Execute all actions.
void RegularItem::invoke()
{
}




RegularItem::eUseType 
RegularItem::getUseType() const
{
    return meUseType;
}

RegularItem::eTargetable
RegularItem::getTargetable() const
{
    return meTargetable;
}

RegularItem::eDefaultTarget 
RegularItem::getDefaultTarget() const
{
    return meDefaultTarget;
}

bool RegularItem::isReusable() const
{
    return mbReusable;
}


uint RegularItem::getValue() const
{
    return mnValue;
}

uint RegularItem::getSellValue() const
{
    return mnSellValue;
}

void RegularItem::loadItem ( CL_DomElement * pElement )
{
    
}

CL_DomElement  RegularItem::createDomElement(CL_DomDocument&) const
{
}



SpecialItem::SpecialItem()
{
}

SpecialItem::~SpecialItem()
{
}
	

void SpecialItem::loadItem ( CL_DomElement * pElement )
{
}

CL_DomElement  
SpecialItem::createDomElement(CL_DomDocument&) const
{
}




SystemItem::SystemItem()
{
}

SystemItem::~SystemItem()
{
}
	
void SystemItem::loadItem ( CL_DomElement * pElement )
{
}
	
CL_DomElement  
SystemItem::createDomElement(CL_DomDocument&) const
{
}


Rune::Rune()
{
}

Rune::~Rune()
{
}


uint Rune::getValue() const 
{
    // When we have spells implemented, we will have to look up their value here.
    return 0;
}

uint Rune::getSellValue() const 
{
    // When we have spells implemented, we will have to look up their value here.
    return 0;
}

SpellRef * Rune::getSpellRef() const
{
    return mpSpellRef;
}

void Rune::loadItem ( CL_DomElement * pElement )
{
}

CL_DomElement  
Rune::createDomElement(CL_DomDocument&) const
{
}

 

UniqueWeapon::UniqueWeapon():mpWeaponType(NULL)
{
}

UniqueWeapon::~UniqueWeapon()
{
}



uint UniqueWeapon::getValue() const 
{
    return mnValue;
}

uint UniqueWeapon::getSellValue() const 
{
    return mnValue / 2;
}

CL_DomElement  
UniqueWeapon::createDomElement(CL_DomDocument&) const
{

}



WeaponType * UniqueWeapon::getWeaponType() const 
{
    return mpWeaponType;
}

bool UniqueWeapon::isRanged() const 
{
    return mbRanged;
}

void UniqueWeapon::loadItem(CL_DomElement * pElement) 
{
}


UniqueArmor::UniqueArmor():mpArmorType(NULL)
{

}
UniqueArmor::~UniqueArmor()
{
}


uint UniqueArmor::getValue() const
{
    return mnValue;
}

uint UniqueArmor::getSellValue() const 
{
    return mnValue / 2;
}

ArmorType *UniqueArmor::getArmorType() const
{
    return mpArmorType;
}
	
CL_DomElement  
UniqueArmor::createDomElement(CL_DomDocument&) const
{
}

void UniqueArmor::loadItem(CL_DomElement * pElement) 
{
}


GeneratedWeapon::GeneratedWeapon():mpClass(NULL),mpType(NULL)
{
}

GeneratedWeapon::~GeneratedWeapon()
{
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

    return (int)((float)mpType->getBasePrice() * 
		 mpClass->getValueMultiplier()) 
	+ mpClass->getValueAdd();
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


void GeneratedWeapon::generate( WeaponTypeRef * pType, WeaponClassRef * pClass, 
		       SpellRef *pSpell , RuneType *pRune)
{

    const ItemManager * pManager = IApplication::getInstance()->getItemManager();

    std::ostringstream os;

    mpType = pManager->getWeaponType(*pType);
    mpClass = pManager->getWeaponClass(*pClass);

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

    if(pSpell)
    {
	os << pSpell->getName() << ' ';
    }

    os << pType->getName();

    mName = os.str();


}

    


GeneratedArmor::GeneratedArmor():mpType(NULL),mpClass(NULL)
{
}

GeneratedArmor::~GeneratedArmor()
{
}

std::string GeneratedArmor::getIconRef() const
{
    return mpType->getIconRef();
}

std::string GeneratedArmor::getName() const
{
    return mName;
}

uint GeneratedArmor::getMaxInventory() const 
{
    // todo: lookup in settings
    return 99;
}

Item::eDropRarity GeneratedArmor::getDropRarity() const
{
    if( hasSpell() || hasRuneType() )
    {
	return RARE; 
    }
    else return UNCOMMON;
}


uint GeneratedArmor::getValue() const 
{
    return (int)((float)mpType->getBasePrice() * mpClass->getValueMultiplier()) 
	+ mpClass->getValueAdd();
}

uint GeneratedArmor::getSellValue() const 
{
    return getValue() / 2;
}



ArmorType * GeneratedArmor::getArmorType() const 
{
    return mpType;
}



void GeneratedArmor::generate( ArmorTypeRef * pType, ArmorClassRef * pClass, 
		       SpellRef *pSpell , RuneType *pRune)
{
    std::ostringstream os;

    const ItemManager * pManager = IApplication::getInstance()->getItemManager();

    mpType = pManager->getArmorType(*pType);
    mpClass = pManager->getArmorClass(*pClass);

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

    if(pSpell)
    {
	os << pSpell->getName() << ' ';
    }

    os << pType->getName();

    mName = os.str();
}
	






WeaponTypeRef::WeaponTypeRef()
{
}

    
WeaponTypeRef::WeaponTypeRef(CL_DomElement * pElement )
{
}

WeaponTypeRef::~WeaponTypeRef()
{
}

CL_DomElement  
WeaponTypeRef::createDomElement(CL_DomDocument&) const
{
}

std::string WeaponTypeRef::getName() const
{
    return mName;
}


    
WeaponClassRef::WeaponClassRef()
{
}

WeaponClassRef::WeaponClassRef(CL_DomElement *pElement)
{
}

WeaponClassRef::~WeaponClassRef()
{
}

CL_DomElement  WeaponClassRef::createDomElement(CL_DomDocument&) const
{
}


std::string WeaponClassRef::getName() const
{
    return mName;
}




ArmorTypeRef::ArmorTypeRef()
{
}

ArmorTypeRef::ArmorTypeRef(CL_DomElement * pElement )
{
}

ArmorTypeRef::~ArmorTypeRef()
{
}


CL_DomElement  
ArmorTypeRef::createDomElement(CL_DomDocument&) const
{
}



std::string ArmorTypeRef::getName() const
{
    return mName;
}


ArmorClassRef::ArmorClassRef()
{
}
	
ArmorClassRef::ArmorClassRef(CL_DomElement *pElement)
{
}

ArmorClassRef::~ArmorClassRef()
{
}

CL_DomElement  
ArmorClassRef::createDomElement(CL_DomDocument&) const
{
}


std::string 
ArmorClassRef::getName() const
{
}



WeaponRef::WeaponRef():mpWeaponType(NULL), mpWeaponClass(NULL),
				   mpSpellRef(NULL),mpRuneType(NULL)
{
}

WeaponRef::WeaponRef(CL_DomElement * pElement )
{
}

WeaponRef::~WeaponRef()
{
}


CL_DomElement  
WeaponRef::createDomElement(CL_DomDocument&) const
{
}


WeaponType * 
WeaponRef::getWeaponType() const
{
    return mpWeaponType;
}

WeaponClass * WeaponRef::getWeaponClass() const
{
    return mpWeaponClass;
}

SpellRef * WeaponRef::getSpellRef() const
{
    return mpSpellRef;
}

RuneType * WeaponRef::getRuneType() const
{
    return mpRuneType;
}



ArmorRef::ArmorRef()
{
}


ArmorRef::ArmorRef(CL_DomElement * pElement ):mpArmorType(NULL),
					      mpArmorClass(NULL),
					      mpSpellRef(NULL),
					      mpRuneType(NULL)
{
}

ArmorRef::~ArmorRef()
{
}

CL_DomElement  
ArmorRef::createDomElement(CL_DomDocument&) const
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






RuneType::RuneType()
{
}

RuneType::RuneType(CL_DomElement * pElement )
{
}

RuneType::~RuneType()
{
}

RuneType::eRuneType RuneType::getRuneType() const
{
    return meRuneType;
}

CL_DomElement 
RuneType::createDomElement ( CL_DomDocument &) const
{
}

std::string RuneType::getRuneTypeAsString() const
{

    //@todo : Get this from a setting.
    switch(meRuneType)
    {
    case NONE:
	return "";
    case RUNE:
	return "Rune";
    case ULTRA_RUNE:
	return "Ultra Rune";
	break;
    }
}



SpellRef::SpellRef()
{
}

SpellRef::SpellRef( CL_DomElement * pElement )
{
}

SpellRef::~SpellRef()
{
}



SpellRef::eSpellType SpellRef::getSpellType() const
{
    return meSpellType;
}

std::string SpellRef::getName() const
{
    return mName;
}

CL_DomElement 
SpellRef::createDomElement ( CL_DomDocument &) const
{
}



WeaponEnhancer::WeaponEnhancer()
{
}

WeaponEnhancer::WeaponEnhancer(CL_DomElement * pElement )
{
}

WeaponEnhancer::~WeaponEnhancer()
{
}
	
CL_DomElement WeaponEnhancer::createDomElement ( CL_DomDocument &) const
{
    
}

Weapon::eAttribute WeaponEnhancer::getAttribute() const
{
    return meAttribute;
}

int WeaponEnhancer::getAdd() const
{
    return mnAdd;
}

float WeaponEnhancer::getMultiplier() const
{
    return mfMultiplier;
}
	




ArmorEnhancer::ArmorEnhancer()
{
}

ArmorEnhancer::ArmorEnhancer(CL_DomElement * pElement )
{
}

ArmorEnhancer::~ArmorEnhancer()
{
}
	
CL_DomElement 
ArmorEnhancer::createDomElement ( CL_DomDocument &) const
{
}

Armor::eAttribute 
ArmorEnhancer::getAttribute() const
{
    return meAttribute;
}

int ArmorEnhancer::getAdd() const
{
    return mnAdd;
}

float ArmorEnhancer::getMultiplier() const
{
    return mfMultiplier;
}

	


AttributeEnhancer::AttributeEnhancer():mnAdd(0),mfMultiplier(1)
{
}

AttributeEnhancer::AttributeEnhancer(CL_DomElement * pElement ):mnAdd(0),mfMultiplier(1)
{
}

AttributeEnhancer::~AttributeEnhancer()
{
}


std::string AttributeEnhancer::getAttribute() const
{
    return mAttribute;
}

int AttributeEnhancer::getAdd() const
{
    return mnAdd;
}

float AttributeEnhancer::getMultiplier() const
{
    return mfMultiplier;
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when invoking. (By calling equip on the armor/weapon...)
void AttributeEnhancer::invoke()
{
    IParty * pParty = IApplication::getInstance()->getParty();

    mnOriginalAttribute = pParty->getAttribute(mAttribute, IParty::CURRENT);

    pParty->modifyAttribute( mAttribute, mnAdd, mfMultiplier, IParty::CURRENT );
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when revoking. (By calling unequip on the armor/weapon...)
void AttributeEnhancer::revoke()
{
    IParty * pParty = IApplication::getInstance()->getParty();

    int add =  mnOriginalAttribute - pParty->getAttribute(mAttribute, IParty::CURRENT ) ;

    pParty->modifyAttribute( mAttribute, add, 1 , IParty::CURRENT );

}

CL_DomElement 
AttributeEnhancer::createDomElement ( CL_DomDocument &) const
{
}




WeaponClass::WeaponClass()
{
}

WeaponClass::WeaponClass(CL_DomElement * pElement)
{
}

WeaponClass::~WeaponClass()
{
}


CL_DomElement WeaponClass::createDomElement ( CL_DomDocument &) const
{
}

std::string WeaponClass::getName() const
{
    return mName;
}

int WeaponClass::getValueAdd() const
{
    return mnValueAdd;
}

float WeaponClass::getValueMultiplier() const
{
    return mfValueMultiplier;
}

std::list<AttributeEnhancer*>::const_iterator 
WeaponClass::getAttributeEnhancersBegin()
{
    return mAttributeEnhancers.begin();
}

std::list<AttributeEnhancer*>::const_iterator 
WeaponClass::getAttributeEnhancersEnd()
{
    return mAttributeEnhancers.end();
}

std::list<WeaponEnhancer*>::const_iterator 
WeaponClass::getWeaponEnhancersBegin()
{
    return mWeaponEnhancers.begin();
}

std::list<WeaponEnhancer*>::const_iterator 
WeaponClass::getWeaponEnhancersEnd()
{
    return mWeaponEnhancers.end();
}

bool WeaponClass::isExcluded ( const WeaponTypeRef &weaponType )
{
    for(std::list<WeaponTypeRef*>::const_iterator iter = mExcludedTypes.begin();
	iter != mExcludedTypes.end();
	iter++)
    {
	WeaponTypeRef * pRef = *iter;

	if( pRef->getName() == weaponType.getName() ) 
	    return true;
    }

    return false;

}


ArmorClass::ArmorClass()
{
}

ArmorClass::ArmorClass(CL_DomElement * pElement)
{
}

ArmorClass::~ArmorClass()
{
}


CL_DomElement ArmorClass::createDomElement ( CL_DomDocument &) const
{
}

std::string ArmorClass::getName() const
{
    return mName;
}

int ArmorClass::getValueAdd() const
{
    return mnValueAdd;
}

float ArmorClass::getValueMultiplier() const
{
    return mfValueMultiplier;
}

std::list<AttributeEnhancer*>::const_iterator 
ArmorClass::getAttributeEnhancersBegin()
{
    return mAttributeEnhancers.begin();
}

std::list<AttributeEnhancer*>::const_iterator 
ArmorClass::getAttributeEnhancersEnd()
{
    return mAttributeEnhancers.end();
}

std::list<ArmorEnhancer*>::const_iterator 
ArmorClass::getArmorEnhancersBegin()
{
    return mArmorEnhancers.begin();
}

std::list<ArmorEnhancer*>::const_iterator 
ArmorClass::getArmorEnhancersEnd()
{
    return mArmorEnhancers.end();
}

bool ArmorClass::isExcluded ( const ArmorTypeRef &armorType )
{
    for(std::list<ArmorTypeRef*>::const_iterator iter = mExcludedTypes.begin();
	iter != mExcludedTypes.end();
	iter++)
    {
	ArmorTypeRef * pRef = *iter;

	if( pRef->getName() == armorType.getName() ) 
	    return true;
    }

    return false;
}


WeaponType::WeaponType()
{
}


WeaponType::WeaponType(CL_DomElement * pElement )
{
}

WeaponType::~WeaponType()
{
}

CL_DomElement WeaponType::createDomElement ( CL_DomDocument &) const
{
}

std::string WeaponType::getName() const
{
    return mName;
}

std::string WeaponType::getIconRef() const
{
    return mIconRef;
}


uint WeaponType::getBaseAttack() const
{
    return mnBaseAttack;
}

float WeaponType::getBaseHit() const
{
    return mfBaseHit;
}

uint WeaponType::getBasePrice() const
{
    return mnBasePrice;
}

bool WeaponType::isRanged() const
{
    return mbRanged;
}
	



ArmorType::ArmorType()
{
}

ArmorType::ArmorType(CL_DomElement * pElement )
{
}

ArmorType::~ArmorType()
{
}


CL_DomElement 
ArmorType::createDomElement ( CL_DomDocument &) const
{
}


std::string 
ArmorType::getName() const
{
    return mName;
}

std::string ArmorType::getIconRef() const
{
    return mIconRef;
}

uint ArmorType::getBasePrice() const
{
    return mnBasePrice;
}

int ArmorType::getBaseAC() const
{
    return mnBaseAC;
}


ArmorType::eSlot ArmorType::getSlot() const
{
    return meSlot;
}


