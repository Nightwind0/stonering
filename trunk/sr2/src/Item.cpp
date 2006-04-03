#include <string>
#include <sstream>
#include "Item.h"
#include "ItemManager.h"
#include "ItemFactory.h"




using namespace StoneRing;

// Defined in Level.cpp. No need to include the entire header.

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


Item::eDropRarity 
Item::DropRarityFromString(const std::string &str)
{
    eDropRarity eRarity = NEVER;

    if(str == "never") eRarity = NEVER;
    else if (str == "common") eRarity = COMMON;
    else if (str == "uncommon") eRarity = UNCOMMON;
    else if (str == "rare") eRarity = RARE;

    return eRarity;
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

#ifndef _MSC_VER
    std::for_each( mAttributeEnhancers.begin(), mAttributeEnhancers.end(), std::mem_fun ( &AttributeEnhancer::invoke ) );
#else

    for	(std::list<AttributeEnhancer*>::const_iterator iter = mAttributeEnhancers.begin();
	 iter != mAttributeEnhancers.end(); iter++)
    {
        (*iter)->invoke();
    }
#endif
}

// Remove any attribute enhancements
void Equipment::unequip()
{
#ifndef _MSC_VER
    std::for_each( mAttributeEnhancers.begin(), mAttributeEnhancers.end(), std::mem_fun ( &AttributeEnhancer::revoke ) );
#else
    for(std::list<AttributeEnhancer*>::const_iterator iter = mAttributeEnhancers.begin();
	iter != mAttributeEnhancers.end(); iter++)
    {
        (*iter)->revoke();
    }
#endif

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
    clearWeaponEnhancers();
}

	
	 
int Weapon::modifyWeaponAttribute( eAttribute attr, int current )
{

    int value = current;

    for(std::list<WeaponEnhancer*>::iterator iter = mWeaponEnhancers.begin();
	iter != mWeaponEnhancers.end();
	iter++)
    {
	WeaponEnhancer * pEnhancer = *iter;

	if( pEnhancer->getAttribute() == attr )
	{
	    value= (int)(pEnhancer->getMultiplier() * (float)value);
	    value += pEnhancer->getAdd();
	}
    }

    return value;
}

float Weapon::modifyWeaponAttribute ( eAttribute attr, float current )
{

    float value = current;

    for(std::list<WeaponEnhancer*>::iterator iter = mWeaponEnhancers.begin();
	iter != mWeaponEnhancers.end();
	iter++)
    {
	WeaponEnhancer * pEnhancer = *iter;

	if( pEnhancer->getAttribute() == attr )
	{
	    value *= pEnhancer->getMultiplier() ;
	    value += pEnhancer->getAdd();
	}
    }

    return value;
}



//todo: Getters for weapon enhancers. need 'em.

void Weapon::clearWeaponEnhancers()
{
    for(std::list<WeaponEnhancer*>::iterator iter = mWeaponEnhancers.begin();
	iter != mWeaponEnhancers.end();
	iter++)
    {
	delete *iter;
    }
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
    else if (str == "Steal_HP%") return STEAL_HP;
    else if (str == "Steal_MP%") return STEAL_MP;
    else if (str == "Critical%") return CRITICAL;
    else throw CL_Error("Bad Weapon Enhancer Attribute : " + str );


}
	
/*
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
  ELEMENTAL_RESIST,
  RESIST, // All magic
  STATUS // Resistance against ANY status affect
*/


Armor::eAttribute 
Armor::attributeForString ( const std::string str )
{
    if(str == "AC") return AC;
   
    else if (str == "Steal_MP%") return STEAL_MP;
    else if (str == "Steal_HP%") return STEAL_HP;
   
    else if (str == "ElementalRST") return ELEMENTAL_RESIST;
    else if (str == "RST") return RESIST; // Resist is basically Magic AC
    else if (str == "Status%") return STATUS;
    else if (str == "SlashAC") return SLASH_AC; // Extra AC against slash attacks
    else if (str == "JabAC") return JAB_AC;
    else if (str == "BashAC") return BASH_AC;
    else if (str == "WhiteRST") return WHITE_RESIST;
    else throw CL_Error("Bad Armor enhancer attribute.");
}


Armor::Armor()
{
}


Armor::~Armor()
{
    clearArmorEnhancers();
}
	


int Armor::modifyArmorAttribute( eAttribute attr, int current )
{
    int value = current;

    for(std::list<ArmorEnhancer*>::iterator iter = mArmorEnhancers.begin();
	iter != mArmorEnhancers.end();
	iter++)
    {
	ArmorEnhancer * pEnhancer = *iter;

	if( pEnhancer->getAttribute() == attr )
	{
	    value= (int)(pEnhancer->getMultiplier() * (float)value);
	    value += pEnhancer->getAdd();
	}
    }

    return value;
}

float Armor::modifyArmorAttribute ( eAttribute attr, float current )
{

    float  value = current;

    for(std::list<ArmorEnhancer*>::iterator iter = mArmorEnhancers.begin();
	iter != mArmorEnhancers.end();
	iter++)
    {
	ArmorEnhancer * pEnhancer = *iter;

	if( pEnhancer->getAttribute() == attr )
	{
	    value *= pEnhancer->getMultiplier();
	    value += pEnhancer->getAdd();
	}
    }

    return value;

}

	

void Armor::clearArmorEnhancers()
{
    for(std::list<ArmorEnhancer*>::iterator iter = mArmorEnhancers.begin();
	iter != mArmorEnhancers.end();
	iter++)
    {
	delete *iter;
    }
    mArmorEnhancers.clear();
}

void Armor::addArmorEnhancer (ArmorEnhancer * pEnhancer)
{
    mArmorEnhancers.push_back ( pEnhancer );
}



void NamedItemElement::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    std::string dropRarity = getRequiredString("dropRarity",pAttributes);

    meDropRarity = Item::DropRarityFromString ( dropRarity );

    mnMaxInventory = getImpliedInt("maxInventory",pAttributes,99);
}

void NamedItemElement::handleElement(eElement element, Element * pElement)
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
    }
}

void NamedItemElement::loadFinished()
{
    if(mpNamedItem == NULL) throw CL_Error("No named item within a named item element.");	
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



RegularItem::RegularItem()
{
}

RegularItem::~RegularItem()
{
    for( std::list<Action*>::iterator iter = mActions.begin();
	 iter != mActions.end();
	 iter++)
    {
	delete *iter;
    }
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

RegularItem::eUseType 
RegularItem::UseTypeFromString ( const std::string &str )
{

    eUseType type = WORLD;

    if(str == "battle") type = BATTLE;
    else if (str == "world") type = WORLD;
    else if (str == "both") type = BOTH;
    else throw CL_Error("Bad targetable on regular item. " + str);

    return type;

}



RegularItem::eTargetable 
RegularItem::TargetableFromString ( const std::string &str )
{

    eTargetable targetable = SINGLE;

    if(str == "all") targetable = ALL;
    else if (str == "single") targetable = SINGLE;
    else if (str == "either") targetable = EITHER;
    else if (str == "self_only") targetable = SELF_ONLY;
    else throw CL_Error("Bad targetable on regular item. " + str);


    return targetable;
}

void RegularItem::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mnValue = getRequiredInt("value",pAttributes);

    mnSellValue = mnValue / 2;
      

    std::string useType = getRequiredString("use",pAttributes);
    meUseType = UseTypeFromString ( useType );    

    std::string targetable = getRequiredString("targetable",pAttributes); 
    meTargetable = TargetableFromString ( targetable );    

    if(hasAttr("sellValueMultiplier", pAttributes))
    {
	float multiplier = getFloat("sellValueMultiplier",pAttributes);
	mnSellValue = (int)(mnValue * multiplier);
    }

    mbReusable = getRequiredBool("reusable",pAttributes);
    
    if(hasAttr("defaultTarget",pAttributes))
    {
	std::string str = getString("defaultTarget",pAttributes);

	if( str == "party" )
	    meDefaultTarget = PARTY;
	else if (str == "monsters")
	    meDefaultTarget = MONSTERS;
	else throw CL_Error("Bogus default target on regular item.");

    }
    else
    {
	meDefaultTarget = PARTY;
    }
	
}

void RegularItem::handleElement(eElement element, Element * pElement)
{
    if(isAction(element))
    {
	mActions.push_back( dynamic_cast<Action*>(pElement) );
    }
}

void RegularItem::loadItem ( CL_DomElement * pElement )
{

}

CL_DomElement  RegularItem::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement (doc,"regularItem");
}



SpecialItem::SpecialItem()
{
}

SpecialItem::~SpecialItem()
{
}
	
CL_DomElement  
SpecialItem::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"specialItem");
}




SystemItem::SystemItem()
{
}

SystemItem::~SystemItem()
{
}
	
	
CL_DomElement  
SystemItem::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"systemItem");
}


Rune::Rune():mpSpellRef(NULL)
{
}

Rune::~Rune()
{
    delete mpSpellRef;
}


uint Rune::getValue() const 
{
    //@todo: When we have spells implemented, we will have to look up their value here.
    return 0;
}

uint Rune::getSellValue() const 
{
    //@todo: When we have spells implemented, we will have to look up their value here.
    return 0;
}

SpellRef * Rune::getSpellRef() const
{
    return mpSpellRef;
}

void Rune::loadAttributes(CL_DomNamedNodeMap*)
{
}

void Rune::handleElement(eElement element, Element * pElement)
{
    if(element == ESPELLREF)
    {
	mpSpellRef = dynamic_cast<SpellRef*>(pElement);
    }
}

void Rune::loadFinished()
{
    if(!mpSpellRef) throw CL_Error("Rune without spellref.");
}

CL_DomElement  
Rune::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"rune");
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
UniqueWeapon::createDomElement(CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"uniqueWeapon");
}



WeaponType * UniqueWeapon::getWeaponType() const 
{
    return mpWeaponType;
}

bool UniqueWeapon::isRanged() const 
{
    return mpWeaponType->isRanged();
}

bool UniqueWeapon::isTwoHanded() const
{
    return mpWeaponType->isTwoHanded();
}

void UniqueWeapon::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);
    
}

void UniqueWeapon::loadFinished()
{
	cl_assert ( mpWeaponType );
	mnValue = (int)(mpWeaponType->getBasePrice() * mValueMultiplier);
}
void UniqueWeapon::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EWEAPONTYPEREF:
    {
		const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

		WeaponTypeRef * pType = dynamic_cast<WeaponTypeRef*>(pElement);
		mpWeaponType = pItemManager->getWeaponType( *pType );
	break;
    }
    case EWEAPONENHANCER:
	addWeaponEnhancer( dynamic_cast<WeaponEnhancer*>(pElement) );
	break;
    case EATTRIBUTEENHANCER:
	addAttributeEnhancer( dynamic_cast<AttributeEnhancer*>(pElement) );
	break;
    case ESPELLREF:
	setSpellRef ( dynamic_cast<SpellRef*>(pElement) );
	break;
    case ERUNETYPE:
	setRuneType( dynamic_cast<RuneType*>(pElement) );
	break;
    case ESTATUSEFFECTMODIFIER:
	addStatusEffectModifier( dynamic_cast<StatusEffectModifier*>(pElement) );
	break;

    default:
		throw CL_Error("Found bogus element in unique weapon." );

    }
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
UniqueArmor::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"uniqueArmor");
}

void UniqueArmor::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);

}

void UniqueArmor::loadFinished()
{
	cl_assert ( mpArmorType );
	mnValue = (int)(mpArmorType->getBasePrice() * mValueMultiplier);
}

void UniqueArmor::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EARMORTYPEREF:
    {
		const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();
	ArmorTypeRef * pType = dynamic_cast<ArmorTypeRef*>(pElement);
	mpArmorType = pItemManager->getArmorType( *pType );
	break;
    }
    case EARMORENHANCER:
	addArmorEnhancer( dynamic_cast<ArmorEnhancer*>(pElement) );
	break;
    case EATTRIBUTEENHANCER:
	addAttributeEnhancer( dynamic_cast<AttributeEnhancer*>(pElement) );
	break;
    case ESPELLREF:
	setSpellRef ( dynamic_cast<SpellRef*>(pElement) );
	break;
    case ERUNETYPE:
	setRuneType( dynamic_cast<RuneType*>(pElement) );
	break;
    case ESTATUSEFFECTMODIFIER:
	addStatusEffectModifier( dynamic_cast<StatusEffectModifier*>(pElement) );
	break;
    default:
	throw CL_Error("Found bogus element in unique armor.");

    }
}


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


bool GeneratedArmor::operator== ( const ItemRef &ref )
{
    if( ref.getType() == ItemRef::ARMOR_REF
	&& *ref.getArmorRef()->getArmorClass() == *mpClass &&
	*ref.getArmorRef()->getArmorType() == *mpType)
    {
	if(hasSpell() && ref.getArmorRef()->getSpellRef())
	{
	    if(*getSpellRef() == *ref.getArmorRef()->getSpellRef())
	    {
		return true;
	    } 
	    else return false;
	}
	else if ( hasSpell() || ref.getArmorRef()->getSpellRef())
	{
	    // One had a spell ref and one didn't.
	    return false;
	}
	
	if(hasRuneType() && ref.getArmorRef()->getRuneType())
	{
	    if(*getRuneType() == *ref.getArmorRef()->getRuneType())
	    {
		return true;
	    }
	    else return false;
	}
	else if ( hasRuneType() || ref.getArmorRef()->getRuneType())
	{
	    return false;
	}

	return true;
    }

    return false;
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
    // @todo: add rune value
    const AbilityManager * pManager = IApplication::getInstance()->getAbilityManager();

    uint value =  (int)((float)mpType->getBasePrice() * mpClass->getValueMultiplier()) 
	+ mpClass->getValueAdd();

    if(hasSpell())
    {
	SpellRef * pSpellRef = getSpellRef();

	Spell * pSpell = pManager->getSpell ( *pSpellRef );

	value += pSpell->getValue();
    }

    if(hasRuneType())
    {
	switch( getRuneType()->getRuneType() )
	{
	case RuneType::RUNE:
	    value *= 2; //@todo : get value from game settings
	    break;
	case RuneType::ULTRA_RUNE:
	{
	    double dValue = value;
	    dValue *= 2.75; //@todo: get value from game settings
	    value = (int)dValue;
	    break;
	}
	}
    }

    return value;
}

uint GeneratedArmor::getSellValue() const 
{
    return getValue() / 2;
}



ArmorType * GeneratedArmor::getArmorType() const 
{
    return mpType;
}

ArmorRef GeneratedArmor::generateArmorRef() const
{
    
    return ArmorRef ( getArmorType(), getArmorClass(), getSpellRef(), getRuneType() );
}

void GeneratedArmor::generate( ArmorType * pType, ArmorClass * pClass, 
			       SpellRef *pSpell , RuneType *pRune)
{
    
    for(std::list<AttributeEnhancer*>::const_iterator iter = pClass->getAttributeEnhancersBegin();
	iter != pClass->getAttributeEnhancersEnd();
	iter++)
    {
	addAttributeEnhancer  ( *iter );
    }
    for(std::list<ArmorEnhancer*>::const_iterator iter2 = pClass->getArmorEnhancersBegin();
	iter2 != pClass->getArmorEnhancersEnd();
	iter2++)
    {
	addArmorEnhancer ( *iter2 );
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
	os << " of " <<  pSpell->getName();
    }




    mName = os.str();
}
	


IconRef::IconRef()
{
}

IconRef::~IconRef()
{
}

std::string IconRef::getIcon() const
{
    return mIcon;
}

CL_DomElement  IconRef::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"iconRef");
}


void IconRef::handleElement(eElement element, Element * pElement )
{
}

void IconRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{

}

void IconRef::handleText(const std::string &text)
{
    mIcon = text;
}




WeaponTypeRef::WeaponTypeRef()
{
}

    
WeaponTypeRef::~WeaponTypeRef()
{
}

void WeaponTypeRef::handleText(const std::string &text)
{
	mName = text;
}

bool WeaponTypeRef::operator==(const WeaponTypeRef &lhs)
{
    if( mName == lhs.mName) return true;
    else return false;
}

CL_DomElement  
WeaponTypeRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"weaponTypeRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}

std::string WeaponTypeRef::getName() const
{
    return mName;
}


    
WeaponClassRef::WeaponClassRef()
{
}

void WeaponClassRef::handleText(const std::string &text)
{
    mName = text;
}

WeaponClassRef::~WeaponClassRef()
{
}

bool WeaponClassRef::operator==(const WeaponClassRef &lhs)
{
    if(mName == lhs.mName) return true;
    else return false;
}

CL_DomElement  WeaponClassRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"weaponClassRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}


std::string WeaponClassRef::getName() const
{
    return mName;
}




ArmorTypeRef::ArmorTypeRef()
{
}



ArmorTypeRef::~ArmorTypeRef()
{
}

void ArmorTypeRef::handleText(const std::string &text)
{
    mName = text;
}

bool ArmorTypeRef::operator==(const ArmorTypeRef &lhs)
{
    if( mName == lhs.mName) return true;
    else return false;
}


CL_DomElement  
ArmorTypeRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"armorTypeRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}



std::string ArmorTypeRef::getName() const
{
    return mName;
}


ArmorClassRef::ArmorClassRef()
{
}
	

ArmorClassRef::~ArmorClassRef()
{
}

void ArmorClassRef::handleText(const std::string &text)
{
    mName = text;
}

bool ArmorClassRef::operator==(const ArmorClassRef &lhs)
{
    if( mName == lhs.mName) return true;
    else return false;
}

CL_DomElement  
ArmorClassRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"armorClassRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}


std::string 
ArmorClassRef::getName() const
{
    return mName;
}



WeaponRef::WeaponRef():mpWeaponType(NULL), mpWeaponClass(NULL),
		       mpSpellRef(NULL),mpRuneType(NULL)
{
}


void WeaponRef::handleElement(eElement element, Element * pElement)
{
	const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    switch(element)
    {
    case EWEAPONTYPEREF:
	mType = * (dynamic_cast<WeaponTypeRef*>(pElement));
	mpWeaponType = pItemManager->getWeaponType(mType);
	break;
    case EWEAPONCLASSREF:
	mClass = * (dynamic_cast<WeaponClassRef*>(pElement));
	mpWeaponClass = pItemManager->getWeaponClass ( mClass );
	break;
    case ESPELLREF:
	mpSpellRef = dynamic_cast<SpellRef*>(pElement);
	break;
    case ERUNETYPE:
	mpRuneType = dynamic_cast<RuneType*>(pElement);
	break;
    default:
	throw CL_Error("Bad element found in WeaponRef");
    }
}

WeaponRef::~WeaponRef()
{
}


WeaponRef::WeaponRef ( WeaponType *pType, WeaponClass *pClass, 
		       SpellRef * pSpell, RuneType *pRune ):mpWeaponType(pType), mpWeaponClass(pClass),
							    mpSpellRef(pSpell), mpRuneType(pRune)

{

}

bool WeaponRef::operator==(const WeaponRef &lhs)
{
    if( *mpWeaponType == *lhs.mpWeaponType &&
	*mpWeaponClass == *lhs.mpWeaponClass)
    {
	if( mpSpellRef && lhs.mpSpellRef )
	{
	    if(!(*mpSpellRef == *lhs.mpSpellRef))
	    {
		return false;
	    }
	}
	else if ( mpSpellRef || lhs.mpSpellRef )
	{
	    // One, but not both, had a spell ref
	    return false;
	}


	if( mpRuneType && lhs.mpRuneType )
	{
	    if(!(*mpRuneType == *lhs.mpRuneType))
	    {
		return false;
	    }
	}
	else if ( mpRuneType || lhs.mpRuneType)
	{
	    return false;
	}
    }

    return true;
	
}


CL_DomElement  
WeaponRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"weaponRef");



    element.append_child ( mType.createDomElement(doc ) );

    element.append_child ( mClass.createDomElement(doc ) );

    if(mpSpellRef )
    {
	element.append_child ( mpSpellRef->createDomElement(doc ) );
    }
    else if (mpRuneType)
    {
	element.append_child ( mpRuneType->createDomElement(doc ) );
    }

    return element;
    
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



ArmorRef::ArmorRef ( ArmorType *pType, ArmorClass *pClass, 
		     SpellRef * pSpell, RuneType *pRune ):mpArmorType(pType), mpArmorClass(pClass),
							  mpSpellRef(pSpell), mpRuneType(pRune)

{

}



bool ArmorRef::operator==(const ArmorRef &lhs)
{
    if( *mpArmorType == *lhs.mpArmorType &&
	*mpArmorClass == *lhs.mpArmorClass)
    {
	if( mpSpellRef && lhs.mpSpellRef )
	{
	    if(!(*mpSpellRef == *lhs.mpSpellRef))
	    {
		return false;
	    }
	}
	else if ( mpSpellRef || lhs.mpSpellRef )
	{
	    // One, but not both, had a spell ref
	    return false;
	}


	if( mpRuneType && lhs.mpRuneType )
	{
	    if(!(*mpRuneType == *lhs.mpRuneType))
	    {
		return false;
	    }
	}
	else if ( mpRuneType || lhs.mpRuneType)
	{
	    return false;
	}
    }

    return true;
	
}

void ArmorRef::handleElement(eElement element, Element * pElement)
{
	const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    switch(element)
    {
    case EARMORTYPEREF:
	mType = * (dynamic_cast<ArmorTypeRef*>(pElement));
	mpArmorType = pItemManager->getArmorType(mType);
	break;
    case EARMORCLASSREF:
	mClass = * (dynamic_cast<ArmorClassRef*>(pElement));
	mpArmorClass = pItemManager->getArmorClass ( mClass );
	break;
    case ESPELLREF:
	mpSpellRef = dynamic_cast<SpellRef*>(pElement);
	break;
    case ERUNETYPE:
	mpRuneType = dynamic_cast<RuneType*>(pElement);
	break;
    default:
	throw CL_Error("Bad element found in WeaponRef");
    }
}


ArmorRef::ArmorRef():mpArmorType(NULL),
		     mpArmorClass(NULL),
		     mpSpellRef(NULL),
		     mpRuneType(NULL)
{

}

ArmorRef::~ArmorRef()
{
}

CL_DomElement  
ArmorRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"armorRef");
    
    element.append_child ( mType.createDomElement(doc ) );

    element.append_child ( mClass.createDomElement(doc ) );

    if(mpSpellRef )
    {
	element.append_child ( mpSpellRef->createDomElement(doc ) );
    }
    else if (mpRuneType)
    {
	element.append_child ( mpRuneType->createDomElement(doc ) );
    }

    return element;
    
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

void RuneType::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string runeType = getRequiredString("runeType",pAttributes);

    if(runeType == "none")
	meRuneType = NONE;
    else if (runeType == "rune")
	meRuneType = RUNE;
    else if (runeType == "ultraRune")
	meRuneType = ULTRA_RUNE;
    else throw CL_Error("Bogus runetype supplied.");

}

bool RuneType::operator==(const RuneType &lhs )
{
    if ( meRuneType == lhs.meRuneType )
	return true;
    else return false;
}

RuneType::~RuneType()
{
}

RuneType::eRuneType RuneType::getRuneType() const
{
    return meRuneType;
}

CL_DomElement 
RuneType::createDomElement ( CL_DomDocument &doc) const
{

    CL_DomElement element(doc,"runeType");

    switch(meRuneType)
    {
    case NONE:
	element.set_attribute("type", "none");
	break;
    case RUNE:
	element.set_attribute("type","rune");
	break;
    case ULTRA_RUNE:
	element.set_attribute("type","ultraRune");
	break;
    }

    return element;
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

    return "";
}



void SpellRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string spellType = getRequiredString("type",pAttributes);

    if(spellType == "elemental")
	meSpellType = ELEMENTAL;
    else if (spellType == "white")
	meSpellType = WHITE;
    else if (spellType == "status")
	meSpellType = STATUS;
    else if (spellType == "other")
	meSpellType = OTHER;
    else throw CL_Error("Bad spell type in spell ref.");

}

void SpellRef::handleText(const std::string &text)
{
    mName = text;
}

SpellRef::SpellRef(  )
{
}

SpellRef::~SpellRef()
{
}


bool SpellRef::operator==(const SpellRef &lhs )
{
    if ( meSpellType == lhs.meSpellType &&
	 mName == lhs.mName )
    {
	return true;
    }
    else return false;
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
SpellRef::createDomElement ( CL_DomDocument &doc) const
{

    CL_DomElement element(doc,"spellRef");

    std::string spellType;

    switch(meSpellType)
    {
    case ELEMENTAL:
	spellType = "elemental";
	break;
    case WHITE:
	spellType = "white";
	break;
    case STATUS:
	spellType = "status";
	break;
    case OTHER:
	spellType = "other";
	break;
    }

    element.set_attribute("type", spellType );

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;

}



void WeaponEnhancer::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string strAttr = getRequiredString("attribute", pAttributes);

    meAttribute = Weapon::attributeForString ( strAttr );

    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);

    mnAdd = getImpliedInt("add",pAttributes,0);
}

WeaponEnhancer::WeaponEnhancer():mfMultiplier(1),mnAdd(0)
{
}

WeaponEnhancer::~WeaponEnhancer()
{
}
	
CL_DomElement WeaponEnhancer::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "weaponEnhancer");
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

void ArmorEnhancer::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string strAttr = getRequiredString("attribute", pAttributes);

    meAttribute = Armor::attributeForString ( strAttr );

    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);

    mnAdd = getImpliedInt("add",pAttributes,0);
}


ArmorEnhancer::~ArmorEnhancer()
{
}
	
CL_DomElement 
ArmorEnhancer::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "armorEnhancer");
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

void AttributeEnhancer::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mnAttribute = CAFromString(getRequiredString("attribute", pAttributes));

    mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);

    mnAdd = getImpliedInt("add",pAttributes,0);
}


AttributeEnhancer::~AttributeEnhancer()
{
}


uint AttributeEnhancer::getAttribute() const
{
    return mnAttribute;
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
    ICharacter * pCharacter = IApplication::getInstance()
	->getSelectedCharacterGroup()
	->getSelectedCharacter();

    int original = pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute));

    pCharacter->modifyAttribute( static_cast<eCharacterAttribute>(mnAttribute), mnAdd, mfMultiplier);

    int now = pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute));

    // So we can get back to how we were.
    mnDelta =  original - now;
}

// Uses IParty::modifyAttribute to modify the CURRENT player,
// Meaning that the system must select the proper current player
// when revoking. (By calling unequip on the armor/weapon...)
void AttributeEnhancer::revoke()
{

    ICharacter * pCharacter = IApplication::getInstance()
	->getSelectedCharacterGroup()
	->getSelectedCharacter();


    int add =  mnDelta;

    pCharacter->modifyAttribute( static_cast<eCharacterAttribute>(mnAttribute), add, 1 );

}

CL_DomElement 
AttributeEnhancer::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "attributeEnhancer");
}

WeaponTypeExclusionList::WeaponTypeExclusionList()
{
}

WeaponTypeExclusionList::~WeaponTypeExclusionList()
{
    // std::for_each(mWeaponTypes.begin(),mWeaponTypes.end(),del_fun<WeaponTypeRef>());
}

CL_DomElement WeaponTypeExclusionList::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"weaponTypeExclusionList");
}


std::list<WeaponTypeRef*>::const_iterator WeaponTypeExclusionList::getWeaponTypeRefsBegin()
{
    return mWeaponTypes.begin();
}
std::list<WeaponTypeRef*>::const_iterator WeaponTypeExclusionList::getWeaponTypeRefsEnd()
{
    return mWeaponTypes.end();
}
void WeaponTypeExclusionList::handleElement(eElement element, Element * pElement)
{
    if(element == EWEAPONTYPEREF)
    {
	mWeaponTypes.push_back ( dynamic_cast<WeaponTypeRef*>(pElement) );
    }
}

ArmorTypeExclusionList::ArmorTypeExclusionList()
{
}

ArmorTypeExclusionList::~ArmorTypeExclusionList()
{
	// Dont delete. The armor class does that.
  //  std::for_each(mArmorTypes.begin(),mArmorTypes.end(),del_fun<ArmorTypeRef>());
}

CL_DomElement ArmorTypeExclusionList::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"ArmorTypeExclusionList");
}


std::list<ArmorTypeRef*>::const_iterator ArmorTypeExclusionList::getArmorTypeRefsBegin()
{
    return mArmorTypes.begin();
}
std::list<ArmorTypeRef*>::const_iterator ArmorTypeExclusionList::getArmorTypeRefsEnd()
{
    return mArmorTypes.end();
}
void ArmorTypeExclusionList::handleElement(eElement element, Element * pElement)
{
    if(element == EARMORTYPEREF)
    {
	mArmorTypes.push_back ( dynamic_cast<ArmorTypeRef*>(pElement) );
    }
}


WeaponClass::WeaponClass()
{
}

void WeaponClass::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes );
    mfValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);
    mnValueAdd = getImpliedInt("valueAdd",pAttributes,0);
}

void WeaponClass::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EATTRIBUTEENHANCER:
	mAttributeEnhancers.push_back( dynamic_cast<AttributeEnhancer*>(pElement) );
	break;
    case EWEAPONENHANCER:
	mWeaponEnhancers.push_back( dynamic_cast<WeaponEnhancer*>(pElement) );
	break;
    case EWEAPONTYPEEXCLUSIONLIST:
    {
	WeaponTypeExclusionList * pList = dynamic_cast<WeaponTypeExclusionList*>(pElement);
	std::copy(pList->getWeaponTypeRefsBegin(),pList->getWeaponTypeRefsEnd(), 
		std::back_inserter(mExcludedTypes));

	delete pList;
	break;
    }
    case ESTATUSEFFECTMODIFIER:
	addStatusEffectModifier (dynamic_cast<StatusEffectModifier*>(pElement));
	break;
    default:
	throw CL_Error("Bad element found in weapon class.");
    }
}


WeaponClass::~WeaponClass()
{
    std::for_each(mAttributeEnhancers.begin(),mAttributeEnhancers.end(),del_fun<AttributeEnhancer>());
    std::for_each(mWeaponEnhancers.begin(),mWeaponEnhancers.end(),del_fun<WeaponEnhancer>());
    std::for_each(mExcludedTypes.begin(),mExcludedTypes.end(),del_fun<WeaponTypeRef>());
}


CL_DomElement WeaponClass::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"weaponClass");
}

bool WeaponClass::operator==(const WeaponClass &lhs)
{
    return mName == lhs.mName;
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


bool ArmorClass::operator==( const ArmorClass &lhs )
{
    return mName == lhs.mName;
}

void ArmorClass::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes );
    mfValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);
    mnValueAdd = getImpliedInt("valueAdd",pAttributes,0);
}

void ArmorClass::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EATTRIBUTEENHANCER:
	mAttributeEnhancers.push_back( dynamic_cast<AttributeEnhancer*>(pElement) );
	break;
    case EARMORENHANCER:
	mArmorEnhancers.push_back( dynamic_cast<ArmorEnhancer*>(pElement) );
	break;
    case EARMORTYPEEXCLUSIONLIST:
    {
	ArmorTypeExclusionList * pList = dynamic_cast<ArmorTypeExclusionList*>(pElement);
	std::copy(pList->getArmorTypeRefsBegin(),pList->getArmorTypeRefsEnd(), 
		std::back_inserter(mExcludedTypes));

	delete pList;
	break;
    }
    case ESTATUSEFFECTMODIFIER:
	addStatusEffectModifier (dynamic_cast<StatusEffectModifier*>(pElement));
	break;
    default:
	throw CL_Error("Bad element found in armor class.");
    }
}


ArmorClass::~ArmorClass()
{
    std::for_each(mAttributeEnhancers.begin(),mAttributeEnhancers.end(),del_fun<AttributeEnhancer>());
    std::for_each(mArmorEnhancers.begin(),mArmorEnhancers.end(),del_fun<ArmorEnhancer>());
    std::for_each(mExcludedTypes.begin(),mExcludedTypes.end(),del_fun<ArmorTypeRef>());
}


CL_DomElement ArmorClass::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "armorClass");
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


bool WeaponType::operator==(const WeaponType &type )
{
    return mName == type.mName;
}

void WeaponType::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
    mnBasePrice = getRequiredInt("basePrice",pAttributes);
    mnBaseAttack = getRequiredInt("baseAttack",pAttributes);
    mfBaseHit = getRequiredFloat("baseHit",pAttributes);
    mfBaseCritical = getImpliedFloat("baseCritical",pAttributes,0.05);
    mbRanged = getImpliedBool("ranged",pAttributes,false);
    mbTwoHanded  = getImpliedBool("twoHanded",pAttributes,false);
}

void WeaponType::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EICONREF:

		mIconRef = dynamic_cast<IconRef*>(pElement)->getIcon();
	break;
    case EWEAPONDAMAGECATEGORY:
	mpDamageCategory = dynamic_cast<DamageCategory*>(pElement);
	break;
    case EMAGICDAMAGECATEGORY:
	mpDamageCategory = dynamic_cast<DamageCategory*>(pElement);
	break;
    default:
	throw CL_Error("Strange element on weapon type");
    }
}

WeaponType::~WeaponType()
{
    delete mpDamageCategory;
}

CL_DomElement WeaponType::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"weaponType");
}

std::string WeaponType::getName() const
{
    return mName;
}

std::string WeaponType::getIconRef() const
{
    return mIconRef;
}


float WeaponType::getBaseCritical() const
{
    return mfBaseCritical;
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

bool WeaponType::isTwoHanded() const
{
    return mbTwoHanded;
}
	



ArmorType::ArmorType()
{
}


bool ArmorType::operator==(const ArmorType &lhs )
{
    return mName == lhs.mName;
}

void ArmorType::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
    mnBasePrice = getRequiredInt("basePrice",pAttributes);
    mnBaseAC = getRequiredInt("baseArmorClass",pAttributes);
    mnBaseRST = getRequiredInt("baseResist",pAttributes);

    std::string slot = getRequiredString("slot",pAttributes);

    if(slot == "head")
	meSlot = HEAD;
    else if (slot == "shield")
	meSlot = SHIELD;
    else if (slot == "body")
	meSlot = BODY;
    else if (slot == "feet")
	meSlot = FEET;
    else if (slot == "hands")
	meSlot = HANDS;

}

void ArmorType::handleElement(eElement element, Element * pElement)
{
if(element == EICONREF)
{
    //@todo
}
}

ArmorType::~ArmorType()
{
}


CL_DomElement 
ArmorType::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"armorType");
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

int ArmorType::getBaseRST() const
{
    return mnBaseRST;
}


ArmorType::eSlot ArmorType::getSlot() const
{
    return meSlot;
}




WeaponDamageCategory::WeaponDamageCategory()
{
}

void WeaponDamageCategory::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    meType = TypeFromString(getRequiredString("type",pAttributes));
}

WeaponDamageCategory::eType
WeaponDamageCategory::TypeFromString ( const std::string &str )
{
    if(str == "slash") return SLASH;
    else if (str == "bash") return BASH;
    else if (str == "jab") return JAB;
    else throw CL_Error("Unknown type " + str + " On weapon damage category");
}


WeaponDamageCategory::~WeaponDamageCategory()
{
}

	
WeaponDamageCategory::eType WeaponDamageCategory::getType() const
{
    return meType;
}

CL_DomElement WeaponDamageCategory::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "weaponDamageCategory");
}




MagicDamageCategory::MagicDamageCategory()
{
}

void MagicDamageCategory::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    meType = TypeFromString(getRequiredString("type",pAttributes));
}

MagicDamageCategory::~MagicDamageCategory()
{
}


MagicDamageCategory::eType
MagicDamageCategory::TypeFromString ( const std::string &str )
{
    if(str == "fire") return FIRE;
    else if (str == "wind") return WIND;
    else if (str == "water") return WATER;
    else if (str == "earth") return EARTH;
    else if (str == "holy") return HOLY;
    else if (str == "other") return OTHER;
    else throw CL_Error("Unknown type " + str + " On magic damage category");
}

MagicDamageCategory::eType MagicDamageCategory::getType() const
{
    return meType;
}

CL_DomElement MagicDamageCategory::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "magicDamageCategory");
}

StatusEffectModifier::StatusEffectModifier():mpStatusEffect(NULL)
{
}


void StatusEffectModifier::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{

    std::string statusRef = getRequiredString("statusRef", pAttributes);

    const AbilityManager * pManager = IApplication::getInstance()->getAbilityManager();

    mpStatusEffect = pManager->getStatusEffect( statusRef );

    mfModifier = getRequiredFloat("modifier", pAttributes );
}

StatusEffectModifier::~StatusEffectModifier()
{
}


StatusEffect * StatusEffectModifier::getStatusEffect() const
{
    return mpStatusEffect;
}

float StatusEffectModifier::getModifier() const
{
    return mfModifier;
}



CL_DomElement StatusEffectModifier::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"statusEffectModifier");
}
