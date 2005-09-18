#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "IApplication.h"
#include "Level.h"
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <sstream>
#include "GraphicsManager.h"
#include "LevelFactory.h"
#include "ItemFactory.h"
#include "ItemManager.h"


typedef unsigned int uint;


using namespace StoneRing;






using std::string;




#ifndef _MSC_VER
using std::max;
using std::min;
using std::abs;
#endif



std::string IntToString(const int &i)
{
    std::ostringstream os;

    os << i;

    return os.str();
    
}

std::string BoolToString( const bool &b)
{
    if(b) return "true";
    else return "false";
}


Option::Option():mpCondition(NULL)
{
}

Option::Option(CL_DomElement * pElement):mpCondition(NULL)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    mText = attributes.get_named_item("text").get_node_value();

    if(attributes.get_length() < 1) throw CL_Error("Error: Option requires text.");

    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if( child.get_node_name() == "condition" )
	{
	    mpCondition = factory->createCondition( &child );
			
	}
	else 
	{
	    mActions.push_back(createAction ( child.get_node_name(), child ));
	}

		
	child = child.get_next_sibling().to_element();


    }


    
}
	
Option::~Option()
{
    delete mpCondition;

    for(std::list<Action*>::const_iterator iter = mActions.begin();
	iter != mActions.end();
	iter++)
    {
	delete *iter;
    }
}

CL_DomElement  Option::createDomElement(CL_DomDocument &document) const
{

    CL_DomElement  element(document,"option");

    element.set_attribute("text",mText );


    for(std::list<Action*>::const_iterator i = mActions.begin();
	i != mActions.end();
	i++)
    {
	CL_DomElement  e = (*i)->createDomElement(document);
	element.append_child(e );

    }

    return element;
    
}

std::string Option::getText() const
{
    return mText;
}
	
bool Option::evaluateCondition() const
{
    if(!mpCondition) return true;

    return mpCondition->evaluate();
}

void Option::choose()
{
#ifndef _MSC_VER
    if(evaluateCondition())
		std::for_each( mActions.begin(), mActions.end(), std::mem_fun(&Action::invoke) );
#else
	if(evaluateCondition())
	{
		for(std::list<Action*>::iterator iter = mActions.begin();
			iter != mActions.end();
			iter++)
			{
				(*iter)->invoke();
			}
	}
#endif
}



Choice::Choice()
{
}

Choice::Choice(CL_DomElement * pElement)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    mText = attributes.get_named_item("text").get_node_value();

    if(attributes.get_length() < 1) throw CL_Error("Error: Choice requires text.");

    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if(child.get_node_name() == "option")
	{
	    mOptions.push_back ( factory->createOption ( &child ) );
	}

	child = child.get_next_sibling().to_element();
    }

    if(!mOptions.size())
	throw CL_Error("Choice missing options!!!");

}

CL_DomElement Choice::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"choice");

    element.set_attribute("text",mText );


    for(std::vector<Option*>::const_iterator i = mOptions.begin();
	i != mOptions.end();
	i++)
    {
	CL_DomElement  e = (*i)->createDomElement(doc);
	element.append_child(e );

    }

    return element;

}

void Choice::invoke()
{
    std::vector<std::string> options;


    // fill the "options" vector with the text of each Option object
#ifndef _MSC_VER
    std::transform(mOptions.begin(), mOptions.end(), std::back_inserter(options),
		   std::mem_fun(&Option::getText));
#else
	for(std::vector<Option*>::iterator iter = mOptions.begin();
		iter != mOptions.end();
		iter++)
		{	
			options.push_back ( (*iter)->getText() );
		}
#endif

    IApplication::getInstance()->choice( mText, options, this );

}

std::string Choice::getText() const
{
    return mText;
}

uint Choice::getOptionCount() const
{
    return mOptions.size();
}

Option * Choice::getOption(uint index )
{
    return mOptions[index];
}

void Choice::chooseOption( uint index)
{
    mOptions[index]->choose();
}



// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2)
{
    uint p1value = p1.y  *  IApplication::getInstance()->getScreenHeight() + p1.x;
    uint p2value = p2.y  * IApplication::getInstance()->getScreenHeight() + p2.x;
	
    return p1value < p2value;
}

bool operator < (const MappableObject::eDirection dir1, const MappableObject::eDirection dir2)
{
    return (int)dir1 < (int)dir2;
}



ItemRef::ItemRef()
{
}


ItemRef::ItemRef(CL_DomElement *pElement ):
    mpNamedItemRef(NULL),mpWeaponRef(NULL),mpArmorRef(NULL)
{
 
    CL_DomElement child = pElement->get_first_child().to_element();
    LevelFactory * levelFactory = IApplication::getInstance()->getLevelFactory();
    ItemFactory * itemFactory = IApplication::getInstance()->getItemFactory();
    const ItemManager * itemManager = IApplication::getInstance()->getItemManager();

    if(child.is_null()) throw CL_Error("Item ref with no child!");


    if(child.get_node_name() == "namedItemRef")
    {
	meType = NAMED_ITEM;
	mpNamedItemRef = levelFactory->createNamedItemRef( &child );
    }
    else if (child.get_node_name() == "weaponRef")
    {
	meType = WEAPON_REF;
	mpWeaponRef = itemFactory->createWeaponRef ( &child );
    }
    else if ( child.get_node_name() == "armorRef")
    {
	meType = ARMOR_REF;
	mpArmorRef = itemFactory->createArmorRef ( &child );
    }
    else throw CL_Error("Item ref had weirdo child " + child.get_node_name());
    

    mpItem = itemManager->getItem ( *this ); 
	
}


CL_DomElement ItemRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc, std::string("itemRef"));


    switch(meType)
    {
    case NAMED_ITEM:
	element.append_child ( mpNamedItemRef->createDomElement(doc) );
	break;
    case WEAPON_REF:
	element.append_child ( mpWeaponRef->createDomElement(doc) );
	break;
    case ARMOR_REF:
	element.append_child ( mpArmorRef->createDomElement(doc) );
	break;
    }

    return element;

}

ItemRef::~ItemRef()
{
    delete mpNamedItemRef;
    delete mpWeaponRef;
    delete mpArmorRef;

}

 #if 0
std::string ItemRef::getItemName() const
{
    switch ( meType )
    {
    case NAMED_ITEM:
	return mpNamedItemRef->getItemName();
	break;
    case WEAPON_REF:
	// Need a standard way to generate names from these suckers
	break;
    case ARMOR_REF:
	break;
    }
}
#endif

ItemRef::eRefType ItemRef::getType() const
{
    return meType;
}

NamedItemRef * ItemRef::getNamedItemRef() const
{
    return mpNamedItemRef;
}

WeaponRef * ItemRef::getWeaponRef() const
{
    return mpWeaponRef;
}

ArmorRef * ItemRef::getArmorRef() const
{
    return mpArmorRef;
}



NamedItemRef::NamedItemRef()
{
}

NamedItemRef::NamedItemRef(CL_DomElement *pElement )
{
    mName = pElement->get_text();
}

NamedItemRef:: ~NamedItemRef()
{
}


std::string NamedItemRef::getItemName()
{
    return mName;
}



CL_DomElement  NamedItemRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"namedItemRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}






 
Tilemap::Tilemap()
{
}

CL_DomElement  Tilemap::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"tilemap");

    element.set_attribute( "mapname" , GraphicsManager::getInstance()->lookUpMapWithSurface ( mpSurface ) );
    element.set_attribute( "mapx", IntToString ( mX ) );
    element.set_attribute( "mapy", IntToString ( mY ) );

    return element;
}


Tilemap::Tilemap(CL_DomElement *pElement):mpSurface(NULL)
{
	
	
    CL_DomNamedNodeMap attributes = pElement->get_attributes();
	
    if(attributes.get_length() < 3) throw CL_Error("Error reading attributes in tilemap");
	
    
    std::string name = attributes.get_named_item("mapname").get_node_value();
    
    mpSurface = GraphicsManager::getInstance()->getTileMap(name);

    mX = atoi ( attributes.get_named_item("mapx").get_node_value().c_str());
    mY = atoi ( attributes.get_named_item("mapy").get_node_value().c_str());


}
 
Tilemap::~Tilemap()
{
}
      
ushort Tilemap::getMapX() const
{
    return mX;
}

ushort Tilemap::getMapY() const
{
    return mY;
}

CL_Surface* Tilemap::getTileMap() const
{
    return mpSurface;
}


SpriteRef::SpriteRef()
{
}

CL_DomElement  SpriteRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"spriteRef");

    std::string dir;

    switch(meType)
    {
    case SPR_STILL:
	dir = "still";
	break;
    case SPR_TWO_WAY:
	dir = "twoway";
	break;
    case SPR_FOUR_WAY:
	dir = "fourway";
	break;
    case SPR_NONE:
	break;
    }

    if(dir.length())
    {
	element.set_attribute("type", dir);
    }

    CL_DomText text(doc,mRef);
    text.set_node_value( mRef );

    element.append_child ( text );

    return element;

}


SpriteRef::SpriteRef( CL_DomElement *pElement)
{
    meType = SPR_NONE;


    CL_DomNamedNodeMap attributes = pElement->get_attributes();


	
    if(!attributes.get_named_item("type").is_null())
    {
	std::string direction = attributes.get_named_item("type").get_node_value();
		
	if(direction == "still") meType = SPR_STILL;
	if(direction == "twoway") meType = SPR_TWO_WAY;
	if(direction == "fourway")  meType = SPR_FOUR_WAY;

    }

    mRef = pElement->get_text();
}

SpriteRef::~SpriteRef()
{
}

std::string SpriteRef::getRef() const
{
    return mRef;
}

SpriteRef::eType SpriteRef::getType() const
{
    return meType;
}


AttributeModifier::AttributeModifier()
{
}

CL_DomElement  AttributeModifier::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"attributeModifier");

    element.set_attribute("attribute",mAttribute );

    if(mAdd)
	element.set_attribute("add", IntToString ( mAdd )  );

    switch ( meTarget )
    {
    case CURRENT:
	element.set_attribute("target","current");
	break;
    case ALL:
	element.set_attribute("target","all");
	break;
    case CASTER:
	element.set_attribute("target","caster");
	break;
    }

    switch ( meChangeTo )
    {
    case ADD:
	element.set_attribute("changeTo","add");
	break;
    case TO_MIN:
	element.set_attribute("changeTo", "min");
	break;
    case TO_MAX:
	element.set_attribute("changeTo", "max");
	break;
    }


    for(std::list<Condition*>::const_iterator i = mConditions.begin();
	i != mConditions.end();
	i++)
    {
	CL_DomElement  e = (*i)->createDomElement(doc);
	element.append_child(e );

    }

    return element;

}

 
AttributeModifier::AttributeModifier (CL_DomElement *pElement):mAdd(0),meTarget(CURRENT)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    std::cout << "READING AN A.M." << std::endl;
	
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 2) throw CL_Error("Error reading attributes in A.M.");


    mAttribute = attributes.get_named_item("attribute").get_node_value();
    mAdd = atoi(attributes.get_named_item("add").get_node_value().c_str());

	
    if(!attributes.get_named_item("target").is_null())
    {
	std::string target = attributes.get_named_item("target").get_node_value();

	if(target == "current")
	{
	    meTarget = CURRENT;
	}
	else if (target == "all")
	{
	    meTarget = ALL;
	}
	else if (target == "caster")
	{
	    meTarget = CASTER;
	}
	else throw CL_Error("Unrecognized target type in attribute modifier: " + target);

    }



    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if( child.get_node_name() == "condition" )
	{
	    mConditions.push_back(factory->createCondition( &child ));
			
	}
	else throw CL_Error( "Bad element in an A.M." );

		
	child = child.get_next_sibling().to_element();


    }
	

}

AttributeModifier::~AttributeModifier()
{
    for(std::list<Condition*>::iterator i = mConditions.begin();
	i != mConditions.end();
	i++)
    {
	delete *i;
    }
}


bool AttributeModifier::applicable() const
{
    for( std::list<Condition*>::const_iterator i = mConditions.begin();
	 i != mConditions.end();
	 i++)
    {
	Condition * condition = *i;
	if( ! condition->evaluate() ) return false;
    }


    ICharacterGroup * pParty = IApplication::getInstance()->getSelectedCharacterGroup();
    ICharacter * pCharacter = NULL;


    switch(meTarget)
    {
    case CURRENT:
	pCharacter = pParty->getSelectedCharacter();
	break;
    case CASTER:
	pCharacter = pParty->getCasterCharacter();
	break;
    }


    //@todo: Act on ALL or CURRENT

    switch(meChangeTo)
    {
    case TO_MAX:
	if( pCharacter->getAttribute(mAttribute) < pCharacter->getMaxAttribute(mAttribute))
	{
	    return true;
	}
	else return false;
    case TO_MIN:
	if( pCharacter->getAttribute(mAttribute) > pCharacter->getMinAttribute(mAttribute))
	{
	    return true;
	}
	else return false;
    case ADD:
	if(mAdd > 0 )
	{
	    if ( pCharacter->getAttribute(mAttribute) < pCharacter->getMaxAttribute(mAttribute))
	    {
		return true;
	    }
	    else return false;
	}
	else // counts 0, but... that does nothing anyway
	{
	    if ( pCharacter->getAttribute(mAttribute) > pCharacter->getMinAttribute(mAttribute))
	    {
		return true;
	    }
	    else return false;
	}
    }

    

}


void AttributeModifier::invoke()
{
    for( std::list<Condition*>::iterator i = mConditions.begin();
	 i != mConditions.end();
	 i++)
    {
	Condition * condition = *i;
	if( ! condition->evaluate() ) return;
    }


    ICharacterGroup * pParty = IApplication::getInstance()->getSelectedCharacterGroup();
    ICharacter * pCharacter = NULL;


    switch(meTarget)
    {
    case CURRENT:
	pCharacter = pParty->getSelectedCharacter();
	break;
    case CASTER:
	pCharacter = pParty->getCasterCharacter();
	break;
    }

    int add = 0;

    //@todo: ALL or CURRENT


    switch(meChangeTo)
    {
    case TO_MAX:
	add = pCharacter->getMaxAttribute(mAttribute) - 
	    pCharacter->getAttribute(mAttribute);
	break;
    case TO_MIN:
	add = 0 - (pCharacter->getAttribute(mAttribute ) -
		   pCharacter->getMinAttribute(mAttribute ));
	break;
    case ADD:
	add = mAdd;
	break;
    }


//    pCharacter->modifyAttribute(mAttribute, add, 1) ;
}


HasGold::HasGold()
{
}

CL_DomElement  HasGold::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc, "hasGold");

    std::string oper;

    switch(meOperator)
    {
    case LT:
	oper = "lt";
	break;
    case GT:
	oper = "gt";
	break;
    case GTE:
	oper = "gte";
	break;
    case LTE:
	oper = "lte";
	break;
    case EQ:
	oper = "eq";
	break;
    }

    element.set_attribute("operator",oper);

    if(mbNot) element.set_attribute("not","true");

    CL_DomText text (doc, IntToString(mAmount ) );

    text.set_node_value( IntToString(mAmount ) );

    element.append_child( text );

    return element;
}


HasGold::HasGold( CL_DomElement *pElement)
{

    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 1) throw CL_Error("Error reading attributes in hasGold");


    std::string op = attributes.get_named_item("operator").get_node_value();

	
    if(op == "lt") meOperator = LT;
    else if(op == "gt") meOperator = GT;
    else if(op == "gte") meOperator = GTE;
    else if(op == "lte") meOperator = LTE;
    else if(op == "eq") meOperator = EQ;
    else throw CL_Error("Bad operator type in hasGold");

    CL_DomNode isnot = attributes.get_named_item("not");

    std::string notstring;


    if(! isnot.is_null() )
    {
	notstring = isnot.get_node_value();

	if(notstring == "true")
	{
	    mbNot = true;
	}
    }
    else
    {
	mbNot = false;
    }


    mAmount = atoi( pElement->get_text().c_str() );

    if(mbNot) std::cout << "(NOT) ";

    std::cout << "HAS GOLD: " << op << ' ' << mAmount << std::endl;


}

HasGold::~HasGold()
{

}

bool HasGold::evaluate()
{
    IParty * party = IApplication::getInstance()->getParty();
    int gold = party->getGold();

    if(!mbNot)
    {
	switch ( meOperator )
	{
	case LT:
	    return gold < mAmount;
	case GT:
	    return gold > mAmount;
	case GTE:
	    return gold >= mAmount;
	case LTE:
	    return gold <= mAmount;
	case EQ:
	    return gold == mAmount;
	}
    }
    else
    {
	// Opposites (Why not just specify these in the level xml? cause whoever made it felt like using a not)
	switch ( meOperator )
	{
	case LT:
	    return gold >= mAmount;
	case GT:
	    return gold <= mAmount;
	case GTE:
	    return gold < mAmount;
	case LTE:
	    return gold > mAmount;
	case EQ:
	    return gold != mAmount;
	}
    }
}

HasItem::HasItem():mpItemRef(NULL)
{
    
}


CL_DomElement  HasItem::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"hasItem");

    if(mbNot) element.set_attribute("not","true");

    if(mCount != 1)
	element.set_attribute("count", IntToString(mCount));

    CL_DomElement e = mpItemRef->createDomElement(doc);
    element.append_child (e );


    return element;
}
 
HasItem::HasItem(CL_DomElement *pElement ):mpItemRef(NULL)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();


    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    CL_DomNode isnot = attributes.get_named_item("not");

    std::string notstring;

    CL_DomNode countNode = attributes.get_named_item("count");

    mCount=1;


    if(! countNode.is_null() )
    {
	mCount = atoi(countNode.get_node_value().c_str());

    }

    if(! isnot.is_null() )
    {
	notstring = isnot.get_node_value();

	if(notstring == "true")
	{
	    mbNot = true;
	}
    }
    else
    {
	mbNot = false;
    }

    CL_DomElement itemRefElement = pElement->get_first_child().to_element();

    if(itemRefElement.is_null()) throw CL_Error("hasItem missing itemRef");


    mpItemRef = factory->createItemRef ( &itemRefElement );

    if(mbNot) std::cout << "(NOT)";

    std::cout << "HAS ITEM " << std::endl;

}

HasItem::~HasItem()
{
    delete mpItemRef;
}

bool HasItem::evaluate()
{
    if(mbNot) return ! (IApplication::getInstance()->getParty()->hasItem(mpItemRef, mCount )) ;
    else	return (IApplication::getInstance()->getParty()->hasItem(mpItemRef, mCount )) ;
}



DidEvent::DidEvent()
{
}

CL_DomElement  DidEvent::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"didEvent");

    if(mbNot) element.set_attribute("not","true");

    CL_DomText text ( doc, mEvent );
    text.set_node_value ( mEvent );

    element.append_child ( text);

    return element;
}

 
DidEvent::DidEvent(CL_DomElement *pElement)
{

    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    CL_DomNode isnot = attributes.get_named_item("not");

    std::string notstring;


    if(! isnot.is_null() )
    {
	notstring = isnot.get_node_value();

	if(notstring == "true")
	{
	    mbNot = true;
	}
    }
    else
    {
	mbNot = false;
    }

    mEvent = pElement->get_text();


    std::cout << "DID EVENT: " << mEvent <<  std::endl;


}

DidEvent::~DidEvent()
{
}

bool DidEvent::evaluate()
{
    if(mbNot) return ! IApplication::getInstance()->getParty()->didEvent ( mEvent );
    else  return  IApplication::getInstance()->getParty()->didEvent ( mEvent );
}


And::And()
{
}

CL_DomElement  And::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"and");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	CL_DomElement e = (*i)->createDomElement(doc);
	element.append_child ( e );

    }

    return element;
}

And::And(CL_DomElement * pElement)
{
//	<!ELEMENT and ((hasGold|hasItem|didEvent|operator)*)>
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    CL_DomElement child = pElement->get_first_child().to_element();

    if(child.is_null()) throw CL_Error("\'And\' with no operands.");


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "operator")
	{
	    mOperands.push_back( factory->createOperator ( &child ) );
	}
	else if(name == "hasItem")
	{
	    mOperands.push_back( factory->createHasItem ( &child ) );
	}
	else if(name == "hasGold")
	{
	    mOperands.push_back(factory->createHasGold ( &child ) );
	}
	else if(name == "didEvent")
	{
	    mOperands.push_back(factory->createDidEvent ( &child ) );
	}
	else throw CL_Error("Bad operand in \'and\'.");

	child = child.get_next_sibling().to_element();
    }

}
And::~And()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	delete *i;
    }
}

bool And::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	Check * check = *i;
	if(! check->evaluate() )
	    return false;
    }

    return true;
}

ushort And::order()
{
    return 0;
}


Or::Or()
{
}

CL_DomElement  Or::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"or");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	CL_DomElement e = (*i)->createDomElement(doc);
	element.append_child ( e );


    }

    return element;
}

 
Or::Or(CL_DomElement * pElement)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    CL_DomElement child = pElement->get_first_child().to_element();

    if(child.is_null()) throw CL_Error("\'or\' with no operands.");


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "operator")
	{
	    mOperands.push_back( factory->createOperator ( &child ) );
	}
	else if(name == "hasItem")
	{
	    mOperands.push_back( factory->createHasItem ( &child ) );
	}
	else if(name == "hasGold")
	{
	    mOperands.push_back(factory->createHasGold ( &child ) );
	}
	else if(name == "didEvent")
	{
	    mOperands.push_back(factory->createDidEvent ( &child ) );
	}
	else throw CL_Error("Bad operand in \'or\'.");

	child = child.get_next_sibling().to_element();
    }


}
Or::~Or()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	delete *i;
    }
}

bool Or::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	Check * check = *i;
	if( check->evaluate() )
	    return true;
    }

    return false;
}

ushort Or::order()
{
    return 0;
}

Operator::Operator()
{
}

CL_DomElement  Operator::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"operator");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	element.append_child (  (*i)->createDomElement(doc) );
    }

    return element;    
}

 
Operator::Operator(CL_DomElement *pElement)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    CL_DomElement child = pElement->get_first_child().to_element();

    if(child.is_null()) throw CL_Error("\'operator\' with no operands.");


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "or")
	{
	    mOperands.push_back( factory->createOr( &child ));
	}
	else if(name == "and")
	{
	    mOperands.push_back(factory->createAnd ( &child ) );
	}
	else throw CL_Error("Bad operand in \'operator\'.");

	child = child.get_next_sibling().to_element();
    }


}
Operator::~Operator()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	delete *i;
    }
	
}

bool Operator::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
	i != mOperands.end();
	i++)
    {
	Check * check = *i;
	if(! check->evaluate() )
	    return false;
    }

    return true;
}

ushort Operator::order()
{
    return 0;
}
      

Condition::Condition()
{
}

CL_DomElement  Condition::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"condition");

    for(std::list<Check*>::const_iterator i = mChecks.begin();
	i != mChecks.end();
	i++)
    {
	CL_DomElement e= (*i)->createDomElement(doc);
	element.append_child ( e );

    }

    return element;
}

 

Condition::Condition(CL_DomElement *pElement)
{
	
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    std::cout << "READING CONDITION" << std::endl;
    CL_DomElement child = pElement->get_first_child().to_element();

    while(!child.is_null())
    {

	std::string name = child.get_node_name();

	if(name == "operator")
	{
	    mChecks.push_back ( factory->createOperator( &child ));
	}
	else if ( name == "hasItem")
	{
	    mChecks.push_back ( factory->createHasItem ( &child ));
	}
	else if ( name == "hasGold")
	{
	    mChecks.push_back ( factory->createHasGold ( &child ));
	}
	else if (name == "didEvent")
	{
	    mChecks.push_back ( factory->createDidEvent ( &child ));
	}
	else throw CL_Error("Bad child in conditon: " + name );

	child = child.get_next_sibling().to_element();
    }
}

Condition::~Condition()
{
    for(std::list<Check*>::iterator i = mChecks.begin();
	i != mChecks.end(); i++)
    {
	delete *i;
    }
}

bool Condition::evaluate() const
{
    for(std::list<Check*>::const_iterator i = mChecks.begin();
	i != mChecks.end(); i++)
    {
	Check * check = *i;
	// If anybody returns false, the whole condition is false
	if(! check->evaluate() ) return false;
    }

    return true;
}

Event::Event():mbRepeatable(true),mbRemember(false),mpCondition(NULL)
{
}

CL_DomElement  Event::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"event");

    element.set_attribute("name", mName );

    std::string triggertype;

    switch( meTriggerType )
    {
    case STEP:
	triggertype = "step";
	break;
    case TALK:
	triggertype = "talk";
	break;
    case ACT:
	triggertype = "act";
	break;
    }

    element.set_attribute("triggerType", triggertype);

    if(!mbRepeatable) element.set_attribute("repeatable","false");

    if(mbRemember) element.set_attribute("remember","true");

    if(mpCondition )
    {
	CL_DomElement e = mpCondition->createDomElement(doc);

	element.append_child ( e );

    }

    for(std::list<Action*>::const_iterator i = mActions.begin();
	i != mActions.end();
	i++)
    {
	CL_DomElement e = (*i)->createDomElement(doc);
	
	element.append_child ( e );

    }


    return element;

}

Action * 
StoneRing::createAction ( const std::string & action,  CL_DomElement & child )
{

    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    if (child.get_node_name() == "attributeModifier" )
    {
	return factory->createAttributeModifier (&child );
	
    }
    else if (child.get_node_name() == "say" )
    {
	return factory->createSay (&child );
	
    }
    else if (child.get_node_name() == "give" )
    {
       return factory->createGive (&child );
	
    }
    else if (child.get_node_name() == "take" )
    {
       return factory->createTake (&child );
	
    }
    else if (child.get_node_name() == "giveGold" )
    {
       return factory->createGiveGold (&child );

    }
    else if (child.get_node_name() == "playAnimation" )
    {
       return factory->createPlayAnimation (&child );
	
    }
    else if (child.get_node_name() == "playSound" )
    {
       return factory->createPlaySound (&child );
	
    }
    else if (child.get_node_name() == "loadLevel" )
    {
       return factory->createLoadLevel (&child );
	
    }
    else if (child.get_node_name() == "startBattle" )
    {
       return factory->createStartBattle (&child );
	
    }
    else if (child.get_node_name() == "pause" )
    {
       return factory->createPause (&child );
	
    }
    else if (child.get_node_name() == "invokeShop" )
    {
       return factory->createInvokeShop (&child );
       
    }
    else if (child.get_node_name() == "choice")
    {
	return factory->createChoice ( &child );
    }
    else
    {
	throw CL_Error("Action was not recognized: " + child.get_node_name());
    }



}

Event::Event(CL_DomElement *pElement):mbRepeatable(true),mpCondition(NULL)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

	
    std::cout << "READING AN EVENT." << std::endl;

    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 2) throw CL_Error("Error reading attributes in event");


    mName = attributes.get_named_item("name").get_node_value();
    std::string triggertype = attributes.get_named_item("triggerType").get_node_value();


    if(triggertype == "step")
	meTriggerType = STEP;
    else if (triggertype == "talk")
	meTriggerType = TALK;
    else if (triggertype == "act")
	meTriggerType = ACT;
    else throw CL_Error(" Bad trigger type on event " + mName );


	
    if(!attributes.get_named_item("repeatable").is_null())
    {
	mbRepeatable = ( attributes.get_named_item("repeatable").get_node_value() == "true")?true:false;
    }

    if(!attributes.get_named_item("remember").is_null())
    {
	mbRemember = ( attributes.get_named_item("remember").get_node_value() == "true")?true:false;
    }



    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if( child.get_node_name() == "condition" )
	{
	    mpCondition = factory->createCondition ( &child );
			
	}
	else
	{
	    mActions.push_back ( StoneRing::createAction ( child.get_node_name(), child ) );
	}
	
	child = child.get_next_sibling().to_element();
    }







}

Event::~Event()
{
    for(std::list<Action*>::iterator i = mActions.begin();
	i != mActions.end();
	i++)
    {
	delete *i;
    }
}

std::string Event::getName() const
{
    return mName;
}

Event::eTriggerType Event::getTriggerType()
{
    return meTriggerType;
}

bool Event::repeatable()
{
    return mbRepeatable;
}

bool Event::remember()
{
    return mbRemember;
}
 
bool Event::invoke()
{
    
    if(mpCondition && !mpCondition->evaluate() ) return false;

    IApplication::getInstance()->getParty()->doEvent ( mName, mbRemember );

    for(std::list<Action*>::iterator i = mActions.begin();
	i != mActions.end();
	i++)
    {
	Action *action = *i;

	action->invoke();
    }

    return true;
}
      

PlayAnimation::PlayAnimation()
{

}


CL_DomElement  PlayAnimation::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"playAnimation");

    CL_DomText text(doc,mAnimation);

    text.set_node_value( mAnimation );

    element.append_child ( text );

    return element;
}


PlayAnimation::PlayAnimation(CL_DomElement * pElement )
{

    mAnimation = pElement->get_text();
}
 
PlayAnimation::~PlayAnimation()
{
}

void PlayAnimation::invoke()
{
    IApplication::getInstance()->playAnimation ( mAnimation );
}
 

PlaySound::PlaySound()
{
}

CL_DomElement  PlaySound::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"playSound");

    CL_DomText text(doc, mSound );

    text.set_node_value ( mSound );

    element.append_child ( text );

    return element;
}

PlaySound::PlaySound(CL_DomElement *pElement )
{
    mSound = pElement->get_text();
}

PlaySound::~PlaySound()
{
}

void PlaySound::invoke()
{
    IApplication::getInstance()->playSound ( mSound );

}

LoadLevel::LoadLevel()
{
}


CL_DomElement  LoadLevel::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"loadLevel");

    element.set_attribute("startx", IntToString (mStartX ) ) ;
    element.set_attribute("starty", IntToString (mStartY ) );
    element.set_attribute("name", mName );
    
    return element;
}
 
LoadLevel::LoadLevel(CL_DomElement *pElement)
{

    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 3) throw CL_Error("Error reading loadLevel attributes");


    mStartX = atoi(attributes.get_named_item("startx").get_node_value().c_str());
    mStartY = atoi(attributes.get_named_item("starty").get_node_value().c_str());

    mName = attributes.get_named_item("name").get_node_value();
	

}
LoadLevel::~LoadLevel()
{
}




Movement::Movement()
{
}


CL_DomElement Movement::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"movement") ;

    
    std::string movementType;

    switch( meType )
    {
    case MOVEMENT_WANDER:
	movementType = "wander";
	break;
    case MOVEMENT_PACE_NS:
	movementType = "paceNS";
	break;
    case MOVEMENT_PACE_EW:
	movementType = "paceEW";
	break;
    case MOVEMENT_NONE:
	movementType = "none";
	break;
    }


    element.set_attribute("movementType", movementType );

    std::string speed;

    switch(meSpeed)
    {
    case SLOW:
	speed = "slow";
	break;
    case FAST:
	speed = "fast";
	break;
    }

    element.set_attribute("speed", speed );

    return element;

}

Movement::Movement ( CL_DomElement * pElement ):meType(MOVEMENT_NONE),meSpeed(SLOW)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 1) throw CL_Error("Error reading movement attributes");


    std::string type  = attributes.get_named_item("movementType").get_node_value();

    if(!attributes.get_named_item("speed").is_null())
    {
	std::string speed = attributes.get_named_item("speed").get_node_value();
	if(speed == "slow")
	{
	    meSpeed = SLOW;
	}
	else if(speed == "fast")
	{
	    meSpeed = FAST;
	}
	else throw CL_Error("Error, movement speed must be fast or slow.");
    }
	
    if(type == "wander")
    {
	meType = MOVEMENT_WANDER;

    }
    else if(type == "paceNS")
    {
	meType = MOVEMENT_PACE_NS;
    }
    else if(type == "paceEW")
    {
	meType = MOVEMENT_PACE_EW;
    }
    else if(type == "none")
    {
	// Why would they ever....
	meType = MOVEMENT_NONE;
    }


  
}

Movement::~Movement()
{
}


Movement::eMovementType Movement::getMovementType() const
{
    return meType;
}

Movement::eMovementSpeed Movement::getMovementSpeed() const
{
    return meSpeed;
}





void LoadLevel::invoke()
{
    IApplication::getInstance()->loadLevel ( mName, mStartX, mStartY );
}

StartBattle::StartBattle()
{
}

CL_DomElement  StartBattle::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"startBattle");

    element.set_attribute("isBoss", mbIsBoss? "true":"false");
    element.set_attribute("count", IntToString (mCount ) );
    element.set_attribute("monster", mMonster );

    return element;
}


StartBattle::StartBattle(CL_DomElement *pElement):mbIsBoss(false),mCount(1)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 1) throw CL_Error("Error reading loadLevel attributes");



    CL_DomNode isnot = attributes.get_named_item("isBoss");

    std::string notstring;


    if(! isnot.is_null() )
    {
	notstring = isnot.get_node_value();

	if(notstring == "true")
	{
	    mbIsBoss = true;
	}
    }
    else
    {
	mbIsBoss = false;
    }



    CL_DomNode countNode = attributes.get_named_item("count");

    if(!countNode.is_null())
	mCount = atoi(countNode.get_node_value().c_str());


    mMonster = attributes.get_named_item("name").get_node_value();
	



}
 
StartBattle::~StartBattle()
{
}

void StartBattle::invoke()
{
    IApplication::getInstance()->startBattle ( mMonster, mCount, mbIsBoss ) ;
}


InvokeShop::InvokeShop()
{
}


CL_DomElement  InvokeShop::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"invokeShop");

    element.set_attribute("shopType", mShopType );

    return element;
}
 
InvokeShop::InvokeShop(CL_DomElement *pElement)
{


    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 1) throw CL_Error("Error reading invokeshop attributes");


    mShopType = attributes.get_named_item("shopType").get_node_value();

	
}
 
InvokeShop::~InvokeShop()
{
}

void InvokeShop::invoke()
{
    IApplication::getInstance()->invokeShop ( mShopType );
}


Pause::Pause()
{
}

CL_DomElement  Pause::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"pause");

    CL_DomText text(doc,IntToString(mMs));

//    text.set_node_value( IntToString (mMs ) );

    element.append_child ( text );

    return element;
}
 
Pause::Pause(CL_DomElement *pElement )
{
    mMs = atoi ( pElement->get_text().c_str() );
}
Pause::~Pause()
{
}

void Pause::invoke()
{
    IApplication::getInstance()->pause ( mMs ) ;
}
 
Say::Say()
{
}

CL_DomElement  Say::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"say");

    element.set_attribute("speaker", mSpeaker );

    CL_DomText text(doc, mText );

//    text.set_node_value( mText );

    element.append_child( text );

    return element;

}

Say::Say (CL_DomElement *pElement )
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 1) throw CL_Error("Error reading say attributes");


    mSpeaker = attributes.get_named_item("speaker").get_node_value();	


    mText = pElement->get_text();
}

Say::~Say()
{
}

void Say::invoke()
{
    IApplication::getInstance()->say ( mSpeaker, mText );
}

Give::Give()
{
}
 
CL_DomElement  Give::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"give");

    if(mCount != 1)
    element.set_attribute("count", IntToString ( mCount ) );

    CL_DomElement  itemRef = mpItemRef->createDomElement(doc);

    element.append_child(itemRef );

    return element;

}

Give::Give(CL_DomElement *pElement ):mpItemRef(NULL)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    CL_DomNode countNode = attributes.get_named_item("count");

    mCount=1;


    if(! countNode.is_null() )
    {
	mCount = atoi(countNode.get_node_value().c_str());

    }

    CL_DomElement itemRefElement = pElement->get_first_child().to_element();

    if(itemRefElement.is_null()) throw CL_Error("\'give\' missing itemRef");


    mpItemRef = factory->createItemRef ( &itemRefElement );


}
Give::~Give()
{
    delete mpItemRef;
}

void Give::invoke()
{
    IApplication::getInstance()->getParty()->giveItem ( mpItemRef, mCount );
}

Take::Take()
{
}

CL_DomElement  Take::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"take");

    if(mCount != 1)
	element.set_attribute("count", IntToString ( mCount ) );

    CL_DomElement  itemRef = mpItemRef->createDomElement(doc);

    element.append_child(itemRef );


    return element;

}
 
Take::Take(CL_DomElement *pElement ):mpItemRef(NULL)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    CL_DomNode count = attributes.get_named_item("count");

    mCount=1;


    if(! count.is_null() )
    {
	mCount = atoi(count.get_node_value().c_str());

    }

    CL_DomElement itemRefElement = pElement->get_first_child().to_element();

    if(itemRefElement.is_null()) throw CL_Error("\'take\' missing itemRef");


    mpItemRef = factory->createItemRef ( &itemRefElement );


}


Take::~Take()
{
    delete mpItemRef;
}

void Take::invoke()
{
    IApplication::getInstance()->getParty()->takeItem ( mpItemRef, mCount );
}


GiveGold::GiveGold()
{
}

CL_DomElement  GiveGold::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"giveGold");

    element.set_attribute("count", IntToString ( mCount ) );


    return element;
}
 

 
GiveGold::GiveGold( CL_DomElement *pElement )
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    CL_DomNode count = attributes.get_named_item("count");

    mCount=1;


    if(count.is_null() )
    {
	throw CL_Error("Give gold missing count");

    }

    mCount = atoi(count.get_node_value().c_str());


}
GiveGold::~GiveGold()
{
}

void GiveGold::invoke()
{
    IApplication::getInstance()->getParty()->giveGold ( mCount );
}





Graphic::Graphic()
{
}

Graphic::~Graphic()
{
}


DirectionBlock::DirectionBlock():meDirectionBlock(0)
{
}

DirectionBlock::DirectionBlock(int i )
{
    meDirectionBlock = i;
}

CL_DomElement  DirectionBlock::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"directionBlock");

    element.set_attribute("north", BoolToString (meDirectionBlock & DIR_NORTH ));
    element.set_attribute("south", BoolToString (meDirectionBlock & DIR_SOUTH));
    element.set_attribute("east", BoolToString ( meDirectionBlock & DIR_EAST ) );
    element.set_attribute("west", BoolToString ( meDirectionBlock & DIR_WEST ) );

    return element;
}

DirectionBlock::DirectionBlock(CL_DomElement *pElement ):meDirectionBlock(0)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();
    
    if(attributes.get_length() < 4) throw CL_Error("Error reading attributes in directionBlock");
    
    
    bool north =  attributes.get_named_item("north").get_node_value() == "true";
    bool south =  attributes.get_named_item("south").get_node_value() == "true";
    bool east =   attributes.get_named_item("east").get_node_value() == "true";
    bool west =   attributes.get_named_item("west").get_node_value() == "true";


    if(north) meDirectionBlock |= DIR_NORTH;
    if(south) meDirectionBlock |= DIR_SOUTH;
    if(east) meDirectionBlock |= DIR_EAST;
    if(west) meDirectionBlock |= DIR_WEST;


}

DirectionBlock::~DirectionBlock()
{
}

int DirectionBlock::getDirectionBlock() const
{
    return meDirectionBlock;
}

bool Tile::isHot() const
{

    return cFlags & HOT;
}

Tile::Tile():mpSprite(NULL),mpCondition(NULL),mpAM(NULL),mZOrder(0),cFlags(0)
{
}

CL_DomElement  Tile::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement element(doc,"tile");


    element.set_attribute("xpos", IntToString ( mX ) );
    element.set_attribute("ypos", IntToString ( mY ) );
    if(mZOrder >0 ) element.set_attribute("zorder", IntToString (mZOrder ) );
    if(isFloater()) element.set_attribute("floater", "true");
    if(isHot())     element.set_attribute("hot", "true");

    if(isSprite())
    {
	CL_DomElement  spriteEl = mGraphic.asSpriteRef->createDomElement(doc);

	element.append_child ( spriteEl );

    }
    else
    {
	CL_DomElement  tilemapEl = mGraphic.asTilemap->createDomElement(doc);

	element.append_child (  tilemapEl );

    }

    if(mpCondition)
    {
	CL_DomElement  condEl = mpCondition->createDomElement(doc);

	element.append_child ( condEl );

    }

    if(mpAM)
    {
	CL_DomElement  amEl = mpAM->createDomElement(doc);

	element.append_child ( amEl );

    }

    if( getDirectionBlock() > 0)
    {
	DirectionBlock block( getDirectionBlock() );

	CL_DomElement  dirEl = block.createDomElement(doc);

	element.append_child( dirEl );

    }
    

    return element;

    
}


Tile::Tile(CL_DomElement *pElement):mpSprite(NULL),mpCondition(NULL), mpAM(NULL), mZOrder(0), cFlags(0)
{

    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 2) throw CL_Error("Error reading attributes in tile");


    mX = atoi(attributes.get_named_item("xpos").get_node_value().c_str());
    mY = atoi(attributes.get_named_item("ypos").get_node_value().c_str());

	
    if(!attributes.get_named_item("zorder").is_null())
    {
	mZOrder = atoi ( attributes.get_named_item("zorder").get_node_value().c_str());
    }

    // Mark this as a floater
    if(!attributes.get_named_item("floater").is_null())
    {
	std::string floater = attributes.get_named_item("floater").get_node_value();

	if(floater == "true")
	{
	    cFlags |= FLOATER;
	}
    }

    if(!attributes.get_named_item("hot").is_null())
    {
	std::string hot = attributes.get_named_item("hot").get_node_value();

	if(hot == "true")
	{
	    cFlags |= HOT;

	}
    }



    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if( child.get_node_name() == "tilemap" )
	{
			
	    mGraphic.asTilemap = factory->createTilemap( &child );
			
	}
	else if (child.get_node_name() == "spriteRef" )
	{
	    GraphicsManager * GM = GraphicsManager::getInstance();
	    mGraphic.asSpriteRef = factory->createSpriteRef ( &child );
	    cFlags |= SPRITE;

	    // Actually create the ref'd sprite here.
	    // And assign to mpSprite
	    mpSprite = GM->createSprite( mGraphic.asSpriteRef->getRef() );

	}
	else if (child.get_node_name() == "condition" )
	{
	    mpCondition = factory->createCondition ( &child );
	}
	else if (child.get_node_name() == "attributeModifier" )
	{
	    mpAM = factory->createAttributeModifier ( &child );
	}
	else if ( child.get_node_name() == "directionBlock" )
	{
	    DirectionBlock block(&child);

	    int db = block.getDirectionBlock();

	    // This is all done to make tile's take up less space in memory
			
	    if(db & DIR_NORTH)
		cFlags |= BLK_NORTH;
	    if(db & DIR_SOUTH)
		cFlags |= BLK_SOUTH;
	    if(db & DIR_EAST)
		cFlags |= BLK_EAST;
	    if(db & DIR_WEST)
		cFlags |= BLK_WEST;
			
	}

	child = child.get_next_sibling().to_element();
    }



}


bool Tile::hasAM() const
{
    return mpAM != NULL;
}

void Tile::activate() // Call any attributemodifier
{
    mpAM->invoke();	
}

Tile::~Tile()
{
    delete mpAM;
    delete mpCondition;
    delete mpSprite;

    if( isSprite() )
	delete mGraphic.asSpriteRef;
    else delete mGraphic.asTilemap;
}

ushort Tile::getZOrder() const
{
    return mZOrder;
}

bool Tile::evaluateCondition() const
{
    if( mpCondition ) return mpCondition->evaluate();

    else return true;
}


uint Tile::getX() const
{
    return mX;
}

uint Tile::getY() const
{
    return mY;
}


CL_Rect Tile::getRect()
{
    return CL_Rect(mX , mY , mX + 1, mY +1);
}

bool Tile::isSprite() const
{
    return cFlags & SPRITE;
}

bool Tile::isFloater() const
{
    return cFlags & FLOATER;
}


void Tile::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    // Get the graphic guy
    // Get our tilemap or sprite
    // Blit it

    static GraphicsManager * GM = GraphicsManager::getInstance();

    if( !isSprite() )
    {
	CL_Surface * tilemap = mGraphic.asTilemap->getTileMap();

	//		void draw(	const CL_Rect& src, const CL_Rect& dest, CL_GraphicContext* context = 0);
	int mapx, mapy;
	mapx = mGraphic.asTilemap->getMapX();
	mapy = mGraphic.asTilemap->getMapY();
	CL_Rect srcRect(mapx * 32 + src.left, mapy * 32 + src.top,
			(mapx * 32) + src.right, (mapy * 32) + src.bottom);

		
	tilemap->draw(srcRect, dst, pGC);
		
    }
    else
    {
	update(false);
	mpSprite->draw( dst, pGC );
    }

}

void Tile::update(bool)
{
    if(isSprite()) mpSprite->update();
}

int Tile::getDirectionBlock() const
{

    int block = 0;

    if( cFlags & BLK_NORTH)
	block |= DIR_NORTH;
    if( cFlags & BLK_SOUTH)
	block |= DIR_SOUTH;
    if( cFlags & BLK_EAST)
	block |= DIR_EAST;
    if( cFlags & BLK_WEST)
	block |= DIR_WEST;

    return block;
}

bool Tile::isTile() const
{
    return true;
}


MappableObject::MappableObject():mpMovement(0),mTimeOfLastUpdate(0),mCountInCurDirection(0)
{
}

CL_DomElement  MappableObject::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"mo");

    element.set_attribute( "name", mName );
    
    std::string motype;
    std::string size;

    switch( meSize )
    {
    case MO_SMALL:
	size = "small";
	break;
    case MO_MEDIUM:
	size = "medium";
	break;
    case MO_LARGE:
	size = "large";
	break;
    case MO_TALL:
	size = "tall";
	break;
    case MO_WIDE:
	size = "wide";
	break;
	
    }


    switch ( meType )
    {
    case NPC:
	motype = "npc";
	break;
    case SQUARE:
	motype = "square";
	break;
    case CONTAINER:
	motype = "container";
	break;
    case DOOR:
	motype = "door";
	break;
    case WARP:
	motype = "warp";
	break;
    }

    element.set_attribute("size", size);
    element.set_attribute("type", motype );
    element.set_attribute("xpos", IntToString(mStartX) );
    element.set_attribute("ypos", IntToString(mStartY) );


    if(isSolid()) element.set_attribute("solid", "true" );

    if(cFlags & TILEMAP)
    {
	CL_DomElement  tilemapEl = mGraphic.asTilemap->createDomElement(doc);

	element.append_child (  tilemapEl );

    }


    if(isSprite())
    {
	
	CL_DomElement  spriteRefEl = mGraphic.asSpriteRef->createDomElement(doc);


	element.append_child ( spriteRefEl );

    }

    if(mpCondition)
    {
	CL_DomElement condition = mpCondition->createDomElement(doc);

	element.append_child( condition );
    }

    for(std::list<Event*>::const_iterator h = mEvents.begin();
	h != mEvents.end(); h++)
    {
	CL_DomElement  evEl= (*h)->createDomElement(doc);

	element.append_child( evEl );


    }

    if(mpMovement)
    {
	CL_DomElement  moveEl = mpMovement->createDomElement(doc);

	element.append_child ( moveEl );


    }


    return element;
    
}

bool MappableObject::evaluateCondition() const
{
    if(mpCondition)
    {
	return mpCondition->evaluate();
    }

    return true;
}

MappableObject::eSize MappableObject::getSize() const
{
    return meSize;
} 
 
MappableObject::MappableObject(CL_DomElement *pElement):mpMovement(0),mpCondition(0),mTimeOfLastUpdate(0),mCountInCurDirection(0)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    cFlags = 0;

    std::cout << "READING A MO." << std::endl;

    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 5) throw CL_Error("Error reading attributes in MO");

    mName = attributes.get_named_item("name").get_node_value();
	
    std::string motype = attributes.get_named_item("type").get_node_value();

    std::string size = attributes.get_named_item("size").get_node_value();

    if(size == "small") meSize = MO_SMALL;
    else if(size == "medium") meSize = MO_MEDIUM;
    else if(size == "large") meSize = MO_LARGE;
    else if(size == "wide") meSize = MO_WIDE;
    else if(size == "tall") meSize = MO_TALL;
    else throw CL_Error("MO size wasnt small, medium, or large.");

    if(motype == "npc") meType = NPC;
    else if (motype == "square") meType = SQUARE;
    else if (motype == "container") meType = CONTAINER;
    else if (motype == "door") meType = DOOR;
    else if (motype == "warp") meType = WARP;


    if(!attributes.get_named_item("solid").is_null())
    {
	if(attributes.get_named_item("solid").get_node_value() == "true")
	{
	    cFlags |= SOLID;
	}
    }

    meDirection = NONE;

    mpSprite = NULL;

    mStartX = atoi(attributes.get_named_item("xpos").get_node_value().c_str());
    mStartY = atoi(attributes.get_named_item("ypos").get_node_value().c_str());

    mX = mStartX * 32;
    mY = mStartY * 32;
	
    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if( child.get_node_name() == "tilemap" )
	{
	    if( meSize != MO_SMALL) throw CL_Error("Mappable objects using tilemaps MUST be size small.");
	    cFlags |= TILEMAP;
	    mGraphic.asTilemap = factory->createTilemap( &child );
			
	}
	else if (child.get_node_name() == "spriteRef" )
	{
	    GraphicsManager *GM = GraphicsManager::getInstance();

	    SpriteRef * pRef = factory->createSpriteRef ( &child );

	    mGraphic.asSpriteRef = pRef;
	    cFlags |= SPRITE;

	    mpSprite = GM->createSprite ( pRef->getRef() );
	    

	    int swidth = mpSprite->get_width();
	    int sheight = mpSprite->get_height();

	    switch( meSize )
	    {
	    case MO_SMALL:
		if( swidth != 32 && sheight !=32) throw CL_Error("Sprite size doesn't match MO size (SMALL)");
		break;
	    case MO_MEDIUM:
		if( swidth != 64 && sheight != 64) throw CL_Error("Sprite size doesn't match MO size (MEDIUM).");
		break;
	    case MO_LARGE:
		if( swidth != 128 && sheight != 128) throw CL_Error("Sprite size doesnt match MO size (LARGE)");
		break;
	    case MO_TALL:
		if( swidth != 32 && sheight != 64) throw CL_Error("Sprite size does not match MO size(TALL)");
		break;
	    case MO_WIDE:
		
		if( swidth != 64 && sheight != 32) throw CL_Error("Sprite size does not match MO size(TALL)");
		break;
	    }


	}
	else if ( child.get_node_name() == "condition")
	{
	    mpCondition = factory->createCondition ( &child ) ;
	}
	else if (child.get_node_name() == "event" )
	{
	    mEvents.push_back ( factory->createEvent ( &child ) );
	}
	else if( child.get_node_name() == "movement")
	{
	    mpMovement = factory->createMovement ( &child );

	    // Make sure the proper sprites are around for the movement type
	    switch(mpMovement->getMovementType())
	    {
	    case Movement::MOVEMENT_WANDER:
		if(!mGraphic.asSpriteRef->getType() == SpriteRef::SPR_FOUR_WAY)
		    throw CL_Error("Wandering MO needs a four way sprite ref.");

		meDirection = SOUTH;
		break;
	    case Movement::MOVEMENT_PACE_NS:
		meDirection = SOUTH;
		if(!mGraphic.asSpriteRef->getType() == SpriteRef::SPR_TWO_WAY)
		    throw CL_Error("Pacing MO needs a two way sprite ref.");
		break;
	    case Movement::MOVEMENT_PACE_EW:
		if(!mGraphic.asSpriteRef->getType() == SpriteRef::SPR_TWO_WAY)
		    throw CL_Error("Pacing MO needs a two way sprite ref.");

		meDirection = EAST;
		break;
	    default:
		break;
	    }
	}


	child = child.get_next_sibling().to_element();
    }

    setFrameForDirection();
	
}

MappableObject::~MappableObject()
{
    for( std::list<Event*>::iterator i = mEvents.begin();
	 i != mEvents.end();
	 i++)
    {
	delete *i;
    }

    delete mpSprite;

    delete mpMovement;
	     
}

ushort MappableObject::getStartX() const
{
    return mStartX;
}

ushort MappableObject::getStartY() const
{
    return mStartY;
}



Movement* MappableObject::getMovement() const
{
    return mpMovement;
}




std::string MappableObject::getName() const
{
    return mName;
}

uint MappableObject::getX() const
{
    return mX;
}

uint MappableObject::getY() const
{
    return mY;
}

CL_Rect MappableObject::getRect()
{

    int width;
    int height;

    switch ( meSize )
    {
    case MO_SMALL:
	width = height = 32;
	break;
    case MO_MEDIUM:
	width = height = 64;
	break;
    case MO_LARGE:
	width = height = 128;
	break;
    case MO_TALL:
	width = 32;
	height = 64;
	break;
    case MO_WIDE:
	width = 64;
	height = 32;
	break;
    }


    return CL_Rect(mX, mY, mX+ width, mY + height);
}

bool MappableObject::isSprite() const
{
    return cFlags & SPRITE;
}

void MappableObject::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{


    if(isSprite())
    {
	setFrameForDirection();
	mpSprite->draw(dst, pGC );
    }
    else if( cFlags & TILEMAP )
    {
#ifndef NDEBUG
	std::cout << "Mappable Object is tilemap?" << std::endl;
#endif

	CL_Rect srcRect(mGraphic.asTilemap->getMapX() * 32 + src.left, mGraphic.asTilemap->getMapY() * 32 + src.top,
			(mGraphic.asTilemap->getMapX() * 32) + src.right, (mGraphic.asTilemap->getMapY() * 32) + src.bottom);

		
	mGraphic.asTilemap->getTileMap()->draw(srcRect, dst, pGC);
		
    }
	

}

void MappableObject::pickOppositeDirection()
{
    switch(meDirection)
    {
    case WEST:
	meDirection = EAST;
	break;
    case EAST:
	meDirection = WEST;
	break;
    case NORTH:
	meDirection = SOUTH;
	break;
    case SOUTH:
	meDirection = NORTH;
	break;
    default:
	break;
    }

}

bool MappableObject::moveInCurrentDirection()
{

    int delay = 0;

    CL_Rect newRect = getRect();

    int nX = mX;
    int nY = mY;
	
    if(!mpMovement) throw CL_Error("MO mpMovement was null in moveInCurrentDirection");

    switch( mpMovement->getMovementSpeed() )
    {
    case Movement::SLOW:
	delay = 500;
	break;
    case Movement::FAST:
	delay = 150;
	break;
    }


    if(CL_System::get_time() > mTimeOfLastUpdate + delay)
    {

	mTimeOfLastUpdate = CL_System::get_time();

	// Don't step if we're still... most FF
	// games have the characters stepping even if they are chillin...
	// But lets try it this way
	if(meDirection != NONE)
	{
	    if(mbStep) mbStep = false;
	    else mbStep = true;
	}

	switch ( meDirection )
	{
	case SOUTH:
	    newRect.top++;
	    newRect.bottom++;
	    nY++;
	    break;
	case NORTH:
	    newRect.top--;
	    newRect.bottom--;
	    nY--;
	    break;
	case EAST:
	    newRect.left++;
	    newRect.right++;
	    nX++;
	    break;
	case WEST:
	    newRect.right--;
	    newRect.left--;
	    nX--;
	    break;
	default:
	    break;
		
	}

	if(!IApplication::getInstance()->canMove ( getRect(), newRect, true,false))
	{
	    return false;
	}
	else
	{
	    mX = nX;
	    mY = nY;

	

	    mCountInCurDirection++;

	    if(mCountInCurDirection > 128) // @todo: come from movement object?
	    {
		mCountInCurDirection = 0;
		randomNewDirection();
	    }

	}

    }

    return true;
	

}

void MappableObject::randomNewDirection()
{


    if(!mpMovement) return;

    eDirection current = meDirection;
    
    while(meDirection == current)
    {
	int r= rand() % 5;
	
	switch ( mpMovement->getMovementType())
	{
	case Movement::MOVEMENT_NONE:
	    break;
	case Movement::MOVEMENT_WANDER:
	    if(r == 0)
		meDirection = NORTH;
	    else if(r == 1)
		meDirection = SOUTH;
	    else if(r == 2)
		meDirection = EAST;
	    else if(r == 3)
		meDirection = WEST;
	    else if(r == 4)
		meDirection = NONE;
	    break;
	case Movement::MOVEMENT_PACE_NS:
	case Movement::MOVEMENT_PACE_EW:
	    if(r > 2)
		pickOppositeDirection();
	    break;
	default:
	    break;
	    
	}
    }
    
	
}

void MappableObject::setFrameForDirection()
{

    
    if(!mpMovement && mpSprite) mpSprite->set_frame(0);
    else if (!mpMovement) return;

    switch(mpMovement->getMovementType())
    {
    case Movement::MOVEMENT_NONE:
	if(mbStep) mpSprite->set_frame(1);
	else mpSprite->set_frame(0);
	
	break;
    case Movement::MOVEMENT_WANDER:
    {
	switch( meDirection )
	{
	case NORTH:
	    mpSprite->set_frame(mbStep? 6 : 7);
	    break;
	case EAST:
	    mpSprite->set_frame(mbStep? 0 : 1);
	    break;
	case WEST:
	    mpSprite->set_frame(mbStep? 2 : 3);
	    break;
	case SOUTH:
	    mpSprite->set_frame(mbStep? 4 : 5);
	    break;
	case NONE:
	    if(mbStep) mpSprite->set_frame(4);
	    else mpSprite->set_frame(5);
	    break;
	}
	break;
    }
    case Movement::MOVEMENT_PACE_NS:
    {
	switch(meDirection)
	{
	case NORTH:
	    mpSprite->set_frame ( mbStep? 2 : 3);
	    break;
	case SOUTH:
	case NONE:
	    mpSprite->set_frame ( mbStep? 0 : 1);
	    break;
	}
	break;
    }
    case Movement::MOVEMENT_PACE_EW:
    {
	switch(meDirection)
	{
	case EAST:
	case NONE:
	    mpSprite->set_frame ( mbStep? 0 : 1 );
	    break;
	case WEST:
	    mpSprite->set_frame ( mbStep? 2 : 3 );
	    break;
	}
	break;
    }
    }
}


void MappableObject::update(bool bMove)
{

    // Don't bother with disabled MOs
    if(!evaluateCondition()) return;

    if(isSprite())
    {


	// If bMove is false, then moveInCurrentDirection won't even get called
	// and we won't move.
	if(bMove && !moveInCurrentDirection())
	{
	    
	    // pickOppositeDirection();
	    randomNewDirection();
	  
	}
    }
}





bool MappableObject::isSolid() const
{
    return cFlags & MappableObject::SOLID;
}

int MappableObject::getDirectionBlock() const
{
    if (isSolid()) return  (DIR_NORTH | DIR_SOUTH | DIR_EAST | DIR_WEST);
    else return 0;
}

bool MappableObject::isTile() const
{
    return false;
}

void MappableObject::prod()
{
    randomNewDirection();
}


void MappableObject::provokeEvents ( Event::eTriggerType trigger )
{
    IParty *party = IApplication::getInstance()->getParty();

    for(std::list<Event*>::iterator i = mEvents.begin();
	i != mEvents.end();
	i++)
    {
	Event * event = *i;
		
	// If this is the correct trigger,
	// And the event is either repeatable or
	// Hasn't been done yet, invoke it

	if( event->getTriggerType() == trigger 
	    && (event->repeatable() || ! party->didEvent ( event->getName() ))
	    )
	{
	    event->invoke();
	}

    }
}

Level::Level()
{
}

CL_DomElement  Level::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement element(doc, "level");

    element.set_attribute("name",mName);

    CL_DomElement levelHeader(doc, "levelHeader");
    CL_DomElement tiles(doc, "tiles");
    CL_DomElement mappableObjects(doc,"mappableObjects");


    levelHeader.set_attribute("music", mMusic );
    levelHeader.set_attribute("width", IntToString(mLevelWidth) );
    levelHeader.set_attribute("height", IntToString(mLevelHeight) );
    levelHeader.set_attribute("allowsRunning", mbAllowsRunning? "true" : "false");

    element.append_child( levelHeader );

    

    for(int x=0; x< mLevelWidth; x++)
    {
	for(int y =0;y< mLevelHeight; y++)
	{
	    for( std::list<Tile*>::const_iterator i = mTileMap[x][y].begin();
		 i != mTileMap[x][y].end();
		 i++)
	    {
		CL_DomElement  tileEl = (*i)->createDomElement(doc);

		tiles.append_child ( tileEl );

	    }
	}
    }

    for(std::map<CL_Point,std::list<Tile*> >::const_iterator j = mFloaterMap.begin();
	j != mFloaterMap.end();
	j++)
    {
	for(std::list<Tile*>::const_iterator jj = j->second.begin();
	    jj != j->second.end();
	    jj++)
	{
	    CL_DomElement floaterEl = (*jj)->createDomElement(doc);

	    tiles.append_child ( floaterEl );

	}
	    
    }

    element.append_child(tiles);


    for(std::list<MappableObject*>::const_iterator mo = mMappableObjects.begin();
	mo != mMappableObjects.end();
	mo++)
    {
	CL_DomElement  moEl = (*mo)->createDomElement ( doc );
	mappableObjects.append_child( moEl );

    }

    element.append_child(mappableObjects);
    

    return element;

}


Level::Level(const std::string &name,CL_ResourceManager * pResources): mpDocument(NULL)
{
    srand(time(0));
    // Get the level file name from resources

    std::string path = CL_String::load("Game/LevelPath", pResources);
    std::string filename = CL_String::load("Levels/" + name, pResources);

    // Load the level
    LoadLevel ( path + filename );
}

Level::Level(CL_DomDocument &document):mbAllowsRunning(false)
{
    LoadLevel ( document );
}

Level::~Level()
{
    for(int x=0;x<mLevelWidth;x++)
    {
	for(int y=0;y<mLevelHeight;y++)
	{
	    for(std::list<Tile*>::iterator i = mTileMap[x][y].begin();
		i != mTileMap[x][y].end();
		i++)
	    {
		delete *i;
	    }

	    
	}


    
    }
}
    

void Level::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC, bool floaters,
		 bool highlightHot, bool indicateBlocks)
{
//	int maxSrcX = max( ceil(dst.get_width() / 32.0), mLevelWidth );
//	int maxSrcY = max( ceil(dst.get_height() / 32.0), mLevelHeight);
	
    int widthInTiles = (int)ceil((float)src.get_width() / 32.0 );
    int heightInTiles = (int)ceil((float)src.get_height() / 32.0 );

    int widthInPx = widthInTiles * 32;
    int heightInPx = heightInTiles * 32;

    if(src.left % 32)
	widthInTiles = max(widthInTiles, (src.get_width() / 32 + 1));
    if(src.top % 32)
	heightInTiles = max(heightInTiles, (src.get_height() /32 + 1));


    CL_Rect exDst = dst; // expanded Dest
	 
    // Make it as big as the full source tiles
    // (It maintains the top/left position when you do this)
    exDst.set_size(CL_Size( widthInPx, heightInPx ));
	
    // Move the top and left position out

    int newleftDelta = src.left - ((src.left / 32) * 32);
    int newtopDelta = src.top - ((src.top / 32) * 32);

    exDst.left -= newleftDelta;
    exDst.right -= newleftDelta;
    exDst.top -= newtopDelta;
    exDst.bottom -= newtopDelta;
	


    if(floaters)
    {
	
	for(std::map<CL_Point,std::list<Tile*> >::iterator f = mFloaterMap.begin();
	    f != mFloaterMap.end();
	    f++)
	{
	
	    // Possible optimization... instead of using is_overlapped, do a couple quick comparisons
	    CL_Rect floaterRect(f->first.x,f->first.y,f->first.x + 32, f->first.y + 32);
	    

	    // Is this floater even on screen
	    if(src.is_overlapped( floaterRect))
	    
		for(std::list<Tile*>::iterator i = f->second.begin();
		    i != f->second.end();
		    i++)
		{
		    CL_Rect tileSrc(0,0,32,32);
		    CL_Rect tileDst ( exDst.left  + f->first.x * 32,
				      exDst.top + f->first.y * 32,
				      exDst.left + f->first.x * 32 + 32,
				      exDst.top + f->first.y * 32 + 32);
					
		    Tile * pTile = *i;
		    if(pTile->evaluateCondition())
		    {
			pTile->draw(tileSrc, tileDst , pGC );

		    }
		    
		    
		}
	}
	
    }		
    else
    {
	// Regular tiles, not floaters
	
	for(int tileX = 0; tileX < widthInTiles; tileX++)
  	{
	    for(int tileY =0; tileY < heightInTiles; tileY++)
	    {
		
		CL_Point p( src.left / 32 + tileX, src.top /32 + tileY);
		
		
		if(p.x >=0 && p.y >=0 && p.x < mLevelWidth && p.y < mLevelHeight)
		{
		    
		    
		    for(std::list<Tile*>::iterator i = mTileMap[p.x][p.y].begin();
			i != mTileMap[p.x][p.y].end();
			i++)
		    {
			CL_Rect tileSrc(0,0,32,32);
			CL_Rect tileDst ( exDst.left  + tileX * 32,
					  exDst.top + tileY * 32,
					  exDst.left + tileX * 32 + 32,
					  exDst.top + tileY * 32 + 32);
			
			Tile * pTile = *i;
			if(pTile->evaluateCondition())
			{
			    pTile->draw(tileSrc, tileDst , pGC );
			    
			    

			    // Extra code for level editing
			    if(highlightHot && pTile->isHot())
			    {
				pGC->fill_rect(tileDst, CL_Color(255,0,0,160));
			    }
			    if(indicateBlocks)
			    {
				int block = pTile->getDirectionBlock();
						    
				if(block & DIR_WEST)
				{
				    pGC->fill_rect(CL_Rect(tileDst.left,tileDst.top,tileDst.left + 8, tileDst.bottom), CL_Color(0,255,255,120));
				}
				if(block & DIR_EAST)
				{
				    pGC->fill_rect(CL_Rect(tileDst.right - 8, tileDst.top, tileDst.right,tileDst.bottom),CL_Color(0,255,255,120));
				}
				if(block & DIR_NORTH)
				{
				    pGC->fill_rect(CL_Rect(tileDst.left, tileDst.top, tileDst.right, tileDst.top +8), CL_Color(0,255,255,120));
				}
				if(block & DIR_SOUTH)
				{
				    pGC->fill_rect(CL_Rect(tileDst.left,tileDst.bottom -8, tileDst.right, tileDst.bottom), CL_Color(0,255,255,120));
				}
			    }
			}
					

  					

 					
 					
		    }
		}
			
 			
 			
 			
 			
	    }
  	}
	
    }
    
    


	

}	    


void Level::drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC, bool bMove)
{


    for(std::list<MappableObject*>::iterator i = mMappableObjects.begin();
	i != mMappableObjects.end();
	i++)
    {
	
	if( (*i)->evaluateCondition())
	{


	    CL_Rect moRect = (*i)->getRect();
	    CL_Rect dstRect( moRect.left - src.left + dst.left, moRect.top + dst.top - src.top,
			     moRect.left - src.left + dst.left +moRect.get_width(), moRect.top - src.top + dst.top + moRect.get_height());
	    if( ! src.is_overlapped ( moRect ) )
	    {
		// This MO was outside our field of vision, so we stop iterating.
		break;
	    }
	    
	    (*i)->update(bMove);
	    (*i)->draw( moRect, dstRect, pGC );
	    
	}
    }
	
}


void Level::drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC)
{
    draw(src,dst,pGC, true);
}


      
// Checks relevant tile and MO direction block information
bool Level::canMove(const CL_Rect &currently, const CL_Rect & destination, bool noHot, bool isPlayer) const
{



    if(currently == destination) return true;


    if( destination.left < 0 || destination.top <0) return false;

    // TODO: Enforce rule that all objects must call this for every pixel movement?
    // if they move faster than a pixel at once, then they have to call this for each one.
    // That simplifies life.

    int movementDir=0;
    int oppositeDir=0;
    
	
    if ( currently.left > destination.left) 
    {
	movementDir |= DIR_WEST;
	oppositeDir |= DIR_EAST;
    }
    if ( currently.left < destination.left)
    {
	movementDir |= DIR_EAST;
	oppositeDir |= DIR_WEST;
    }
    if ( currently.top < destination.top)
    {
	movementDir |= DIR_SOUTH;
	oppositeDir |= DIR_NORTH;
    }
    if ( currently.top > destination.top)
    {
	movementDir |= DIR_NORTH;
	oppositeDir |= DIR_SOUTH;
    }
	
	
    // Check if we bump into the player (unless we are the player..)
    
    if(!isPlayer)
    {
	IParty *party = IApplication::getInstance()->getParty();

	if (party->getCollisionRect().is_overlapped( destination)) return false;
    }




    //  Check MOs for overlap, return true if any
    for(std::list<MappableObject*>::const_iterator iter = mMappableObjects.begin();
	iter != mMappableObjects.end();
	iter++)
    {


	MappableObject *pMO = *iter;

	if(pMO->evaluateCondition())
	{

	    CL_Rect moRect = pMO->getRect();
	    
	    if(pMO->getRect() == currently) 
	    {
		// HACK: This is us, therefore we dont care
		break;
	    }
	    
	    //TODO: Possible optimization. we ought to be able to sort the map here
	    // And then, like in the draw, stop iterating once we hit something outside the rect
	    // but, we should already be sorted........
	    
	    if(! IApplication::getInstance()->getLevelRect().is_overlapped ( moRect ) )
	    {
		// This MO is too far away, doesn't need to be tested, nor do any more
		break;
	    }
	    
	    
	    //  Check for overlap, if none, continue
	    //  Check direction block, compare to direction. 
	    //  If they match, you get a non-zero number
	    // That means they moved in the direction in which there is a block
	    if( moRect.is_overlapped(destination) && (pMO->isSolid() ))
	    {
		
		return false;
	    }
	}
	
    }
    

    // Okay, we're not touching any mappable objects. Now for tiles


    CL_Point edgeStart;
    CL_Point edgeEnd;

    // Based on the movement direction, calculate the edgeStart and edgeEnd for the current rectangle.
   

    CL_Point newEdgeStart;
    CL_Point newEdgeEnd;

    // Based on the movement direction, calculate the newEdgeStart and newEdgeEnd for the new rectangle.

    switch(movementDir)
    {
    case DIR_EAST:

	edgeStart.x = (currently.right -1) / 32 ;
	edgeStart.y = currently.top / 32;
	edgeEnd.x = (currently.right -1) / 32 ;
	edgeEnd.y = (currently.bottom -1)/ 32 ;
	newEdgeStart.x = (destination.right-1) / 32 ;
	newEdgeStart.y = destination.top /32;
	newEdgeEnd.x = (destination.right-1) / 32 ;
	newEdgeEnd.y = (destination.bottom-1) / 32 ;
	break;
    case DIR_WEST:
	edgeStart.x = currently.left / 32;
	edgeStart.y = currently.top / 32;
	edgeEnd.x = currently.left / 32;
	edgeEnd.y = (currently.bottom-1) / 32 ;
	newEdgeStart.x = destination.left / 32;
	newEdgeStart.y = destination.top /32;
	newEdgeEnd.x = destination.left / 32;
	newEdgeEnd.y = (destination.bottom-1) / 32 ;
	break;
    case DIR_SOUTH:
	edgeStart.x = currently.left / 32;
	edgeStart.y = (currently.bottom-1) / 32 ;
	edgeEnd.x = (currently.right-1) / 32;
	edgeEnd.y = (currently.bottom-1) / 32 ;
	newEdgeStart.x = destination.left / 32;
	newEdgeStart.y = (destination.bottom-1) /32;
	newEdgeEnd.x = (destination.right-1) / 32;
	newEdgeEnd.y = (destination.bottom-1) / 32;
	break;
    case DIR_NORTH:
	edgeStart.x = currently.left / 32;
	edgeStart.y = currently.top / 32;
	edgeEnd.x = (currently.right -1) / 32;
	edgeEnd.y = currently.top / 32;
	newEdgeStart.x = destination.left / 32;
	newEdgeStart.y = destination.top /32;
	newEdgeEnd.x = (destination.right -1) / 32;
	newEdgeEnd.y = (destination.top-1) / 32;
	break;
    default: 
	break;
    }




    // If the new edge start equals the edge start, then no breach has occured and we can return true.

    // Otherwise, calculate the number of tiles crossed by subtracting the edgeStart.x (or .y for north and south) from
    // the edgeEnd.x (or .y)



    if(movementDir == DIR_NORTH || movementDir == DIR_SOUTH)
    {
	if(edgeStart.y == newEdgeStart.y ) return true;



	// If you are dealing with a horizonal edge,
	// Loop from edgeStart.x to edgeEnd.x; x++
	for(int x = edgeStart.x; x <= edgeEnd.x; x++)
	{
	    CL_Point p(x,edgeStart.y);
	    CL_Point np(x, newEdgeStart.y );
	    




	    // TODO : optimization if we find once rather than each time
	    
	    
	    for(std::list<Tile*>::const_iterator iter = mTileMap[p.x][p.y].begin();
		iter != mTileMap[p.x][p.y].end();
		iter++)
	    {
		// Blocked



		if( movementDir & (*iter)->getDirectionBlock()) return false;
	    }


	    // Check for blocks in our destination
	    if(np.x < mLevelWidth && np.y < mLevelHeight)
	    {
		for(std::list<Tile*>::const_iterator iter = mTileMap[np.x][np.y].begin();
		    iter != mTileMap[np.x][np.y].end();
		    iter++)
		{


		    // Blocked
		    if( oppositeDir & (*iter)->getDirectionBlock()) return false;

		    if( noHot && (*iter)->isHot()) return false;
		}
	    }
	    else
	    {
		return false; // No tiles here
	    }


	}

    }
    else
    {
	if(edgeStart.x == newEdgeStart.x ) return true;

	// If you are dealing with a vertical edge,
	// Loop from edgeStart.y to edgeEnd.y; y++ 

	// If you are dealing with a horizonal edge,
	// Loop from edgeStart.x to edgeEnd.x; x++
	for(int y = edgeStart.y; y <= edgeEnd.y; y++)
	{
	    CL_Point p(edgeStart.x, y);
	    CL_Point np(newEdgeStart.x, y);
	    

	    // Check for blocks in our current location

	    // TODO : optimization if we dont actually copy the list each time
	    for(std::list<Tile*>::const_iterator iter = mTileMap[p.x][p.y].begin();
		iter != mTileMap[p.x][p.y].end();
		iter++)
	    {

		// Blocked
		if( movementDir & (*iter)->getDirectionBlock()) return false;
	    }
		


	    // Check for blocks in our destination
	    if(np.x < mLevelWidth && np.y < mLevelHeight)
	    {


		for(std::list<Tile*>::const_iterator iter = mTileMap[np.x][np.y].begin();
		    iter != mTileMap[np.x][np.y].end();
		    iter++)
		{
		    

		    // Blocked
		    if( oppositeDir & (*iter)->getDirectionBlock()) return false;

		    if( noHot && (*iter)->isHot()) return false;
		}
	    }
	    else
	    {
		return false; // No tiles here
	    }


	}


    }
    


    return true; 

    // In the loop, construct points with the variable dimension and the fixed one
    // (So, in the case of a verticle edge, use the for loop counter for the y, and the .x from either
    // edgeStart or edgeEnd since they are the same
	
    // Use that point to look up a tilestack. Iterate the tilestack and if you find one that has a directionblock
    // That ANDs with your direction, you can return false.  Or, if you care about hot, and any tile is hot, return false.

    // Now, same thing again, only with the newEdgeStart and newEdgeEnd. 

    // If we've come this far, theres nothing in the way. return true.
	
}

// All AM's from tiles fire, as do any step events
void Level::step(const CL_Rect &dest, const CL_Rect & old)
{

    // This brings the close MOs to the top
    // moSortCriterion queries the party to see where they are at.
    // NOTE: This was moved from drawMappableObjects because it really only needs to be called
    // when the player moves. Remember that MOs can't move on screen from offscreen because they don't get run time.
    // Otherwise, this WOULD have to be called from drawMappableObjects
#ifndef _MSC_VER
    mMappableObjects.sort( moSortCriterion );
#else

	mMappableObjects.sort(std::greater<MappableObject*>());
#endif
    

    // First, process any MO step events you may have triggered


    // No need to sort MOs. They will just have been sorted.

    for(std::list<MappableObject*>::iterator i = mMappableObjects.begin();
	i != mMappableObjects.end();
	i++)
    {

	if((*i)->evaluateCondition())
	{
	    CL_Rect moRect = (*i)->getRect();
	    
	    
	    if( ! IApplication::getInstance()->getLevelRect().is_overlapped ( moRect ) )
	    {
		// This MO was outside our field of vision, so we stop iterating.
		break;
	    }
	    else if( dest.is_overlapped(moRect ) && ! old.is_overlapped(moRect)) //only if we aren't already on it...
	    {
		// This MO needs to be stepped on
		(*i)->provokeEvents ( Event::STEP );
		
	    }
	}
    }
    

    
    // Next, process any tile related consequences on applicable (and enabled)
    // tiles. 

    std::multiset<CL_Point> newTiles;
    // First, we calculate the four tiles which are being stepped on.
    // We do NOT trigger all tiles beneath the rectangle. Instead,
    // We trigger only the 4 corners. However, to properly calculate the four corners,
    // We have to adjust slightly, otherwise we would get weird tiles not actually UNDER
    // the rectangle.

    int fourth = dest.get_width() / 4;

    int tile1x = (dest.left + 1 + fourth) / 32;
    int tile1y = (dest.top + 1) / 32;

    int tile2x = (dest.right - 1 - fourth) /32;
    int tile2y = (dest.top + 1) / 32;

    int tile3x = (dest.left + 1 + fourth) / 32;
    int tile3y = (dest.bottom - 1) / 32;
    
    int tile4x = (dest.right - 1 - fourth) / 32;
    int tile4y = (dest.bottom - 1) / 32;

    // Add these to a set. Any that are the same will only be added once.

    newTiles.insert ( CL_Point(tile1x,tile1y ) );
    newTiles.insert ( CL_Point(tile2x,tile2y ) );
    newTiles.insert ( CL_Point(tile3x,tile3y ) );
    newTiles.insert ( CL_Point(tile4x,tile4y ) );
    
    std::list<CL_Point> forStepping;


    // Remove any tiles from newTiles which can be found in oldTiles.

    std::set_difference( newTiles.begin(), newTiles.end(), mLastSteppedTiles.begin(),mLastSteppedTiles.end(),
			 std::inserter( forStepping, forStepping.begin() ) );


    for(std::list<CL_Point>::iterator iter = forStepping.begin();
	iter != forStepping.end();
	iter++)
    {
	activateTilesAt ( iter->x, iter->y );
    }

#if 0 
    std::cout << "NewTile Size = " << newTiles.size();
    std::cout << " OldTile Size = " << mLastSteppedTiles.size();
    std::cout << " stepping size = " << forStepping.size();
#endif

    mLastSteppedTiles = newTiles;

}


void Level::activateTilesAt ( uint x, uint y )
{
    for(std::list<Tile*>::const_iterator iter = mTileMap[x][y].begin();
	iter != mTileMap[x][y].end();
	iter++)
    {
	if ( (*iter)->hasAM() && (*iter)->evaluateCondition() )
	{
	    (*iter)->activate();
	}
    }
    
}
      
// Any talk events fire (assuming they meet conditions)
void Level::talk(const CL_Rect &target, bool prod)
{
 // No need to sort MOs. They will just have been sorted.

    for(std::list<MappableObject*>::iterator i = mMappableObjects.begin();
	i != mMappableObjects.end();
	i++)
    {

	if((*i)->evaluateCondition())
	{
	    CL_Rect moRect = (*i)->getRect();
	    
	    
	    if( ! IApplication::getInstance()->getLevelRect().is_overlapped ( moRect ) )
	    {
		// This MO was outside our field of vision, so we stop iterating.
		break;
	    }
	    else if( target.is_overlapped(moRect ))
	    {
		if(!prod)
		{
		    // This MO needs to be talked to
		    (*i)->provokeEvents ( Event::TALK );
		}
		else
		{
		    // Prod!
		    // (Can't prod things that aren't solid. They aren't in your way anyways)
		    // And if it has no movement, prodding it does nothing.
		    if((*i)->isSolid() && (*i)->getMovement() != NULL)
		    {
			(*i)->prod();
		    }
		}
		
		break; // We only do the first one.
		
	    }
	}
    }
}

// Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
void Level::update(const CL_Rect & updateRect)
{
}
      
 
       
// Sort MO's in order to bring close ones to the top
bool Level::moSortCriterion( const MappableObject *p1, const MappableObject * p2)
{
//	Party * pParty = Party::getInstance();
//	uint pX = pParty->getLevelX();
//	uint pY = pParty->getLevelY();
// Get the center point of the screen instead of the party.

    IApplication * pApp = IApplication::getInstance();

    uint pX = pApp->getLevelRect().get_width() / 2;
    uint pY = pApp->getLevelRect().get_height() / 2;

	
    uint p1Distance, p2Distance;

	
    p1Distance = max(abs( (long)pX - p1->getX()) , abs((long)pY - p1->getY()));
    p2Distance = max(abs( (long)pX - p2->getX()) , abs((long)pY - p2->getY()));

    return p1Distance < p2Distance;


}

#ifndef NDEBUG
void Level::dumpMappableObjects() const
{

	IApplication * pApp = IApplication::getInstance();

	CL_Rect levelRect = pApp->getLevelRect();
	int pX = pApp->getLevelRect().get_width() / 2;
    int pY = pApp->getLevelRect().get_height() / 2;


	std::cout << "=== Mappable Objects ===" << std::endl;

	for(std::list<MappableObject *>::const_iterator i = mMappableObjects.begin();
		i != mMappableObjects.end();
		i++)
		{
			MappableObject * pMO = *i;


			std::cout << pMO->getName();

			int dx1 = pX - pMO->getX();
		    int dy1 = pY - pMO->getY();

			int distance = sqrt( dx1 * dx1 + dy1 * dy1 );

			std::cout << " Dst: " << distance;

			if( levelRect.is_overlapped(pMO->getRect()) )
			{
				std::cout << " is Onscreen ";
			}

			std::cout << pMO->getX() << ',' << pMO->getY();
			std::cout << std::endl;
			
		}


}
#endif

// Sort tiles on zOrder
bool Level::tileSortCriterion ( const Tile * p1, const Tile * p2)
{
    return p1->getZOrder() < p2->getZOrder();
}

void Level::load ( CL_DomDocument &document)
{
    LoadLevel ( document );
}


void Level::LoadLevel (CL_DomDocument &document )
{

    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    CL_DomElement levelNode = document.named_item("level").to_element(); 


    CL_DomNamedNodeMap levelAttributes = levelNode.get_attributes();

    mName = levelAttributes.get_named_item("name").get_node_value();
    std::cout << "LEVEL NAME = " << levelAttributes.get_named_item("name").get_node_value() << std::endl;

    CL_DomElement headerNode = levelNode.named_item("levelHeader").to_element();

    CL_DomElement tilesNode = levelNode.named_item("tiles").to_element();
	
    std::cout << "FOUND TILES TAG. " << tilesNode.get_tag_name() << std::endl;

	
    // Process Header

    CL_DomNamedNodeMap headerAttributes = headerNode.get_attributes();
	
    mMusic = headerAttributes.get_named_item("music").get_node_value();

    std::cout << "MUSIC TYPE = " << mMusic << std::endl;

    mLevelWidth = atoi(headerAttributes.get_named_item("width").get_node_value().c_str());
    mLevelHeight = atoi(headerAttributes.get_named_item("height").get_node_value().c_str());

    std::cout << "DIMENSIONS: " << mLevelWidth << " by " << mLevelHeight << std::endl;


    if(headerAttributes.get_named_item("allowsRunning").get_node_value() == "true")
    {
	mbAllowsRunning = true;
    }
    

    // Create tilemap

    mTileMap.resize( mLevelWidth );

    for(int x=0;x< mLevelWidth; x++)
    {
	mTileMap[x].resize ( mLevelHeight );
    }


    // Process tiles
	
    CL_DomElement moNode = tilesNode.get_next_sibling().to_element();

    std::cout << "Tiles sibling is: " << moNode.get_node_name() << std::endl;

    CL_DomElement currentTile = tilesNode.get_first_child().to_element();
    CL_DomElement *pTile = &currentTile;

    int tilecount = 0;

    while(!pTile->is_null())
    {
	tilecount++;
	loadTile( pTile );

	currentTile = currentTile.get_next_sibling().to_element();
	pTile = &currentTile;
    }




    std::cout << "FOUND " << tilecount << " TILES" << std::endl;

    CL_DomElement currentMo = moNode.get_first_child().to_element();

    int mocount = 0;

    while(!currentMo.is_null())
    {
	mocount++;

	loadMo ( &currentMo );

	currentMo = currentMo.get_next_sibling().to_element();

		
    }

    std:: cout << "FOUND " << mocount << " MAPPABLE OBJECTS" << std::endl;
}

void Level::LoadLevel( const std::string & filename  )
{
    CL_InputSource_File file(filename);

    std::cout << "LOADING LEVEL: " << filename << std::endl;

    CL_DomDocument document;
    CL_DomDocument otherdoc;



	
    document.load(&file);


 
    LoadLevel ( document );


}

void Level::loadMo ( CL_DomElement * moElement )
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    MappableObject * mo = factory->createMappableObject ( moElement );

    mMappableObjects.push_back( mo );

}


void Level::loadTile ( CL_DomElement * tileElement)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    if(factory == NULL) throw CL_Error("Factory was null. My life is a lie!");
    CL_Point point;

    Tile * tile = factory->createTile ( tileElement );

    if(tile == NULL) throw CL_Error("Tile was null, it was all for naught!");

    if(tile->getX() >= mLevelWidth)
    {
	delete tile;
	throw CL_Error("Tile found beyond range of indicated level width.");
    }
    else if (tile->getY() >= mLevelHeight)
    {
	delete tile;
	throw CL_Error("Tile found beyond range of indicated level height.");
    }

    point.x = tile->getX();
    point.y = tile->getY();

   

	
    if( tile->isFloater() )
    {
	std::cout << "Placing floater at: " << point.x << ',' << point.y << std::endl;

	mFloaterMap[ point ].push_back ( tile );

#ifndef _MSC_VER
	mFloaterMap[ point ].sort( &tileSortCriterion );
#else
	mFloaterMap[ point ].sort( std::greater<Tile*>() );
#endif
    }
    else
    {


		
	mTileMap[ point.x ][point.y].push_back ( tile );

	// Sort by ZOrder, so that they display correctly
#ifndef _MSC_VER
	mTileMap[ point.x ][point.y].sort( &tileSortCriterion );
#else
	mTileMap[ point.x ][point.y].sort(std::greater<Tile*>() );
#endif
    }
}
