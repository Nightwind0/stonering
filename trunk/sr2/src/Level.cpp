#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "Application.h"
#include "Level.h"
#include <algorithm>
#include <stdlib.h>
#include "GraphicsManager.h"


using std::string;

#ifdef _MSC_VER

#define min _cpp_min
#define max _cpp_max

template <class T>
T abs( const T& a)
{
    if( a < 0) return -a;
    else return a;
}

#else
using std::max;
using std::min;
using std::abs;
#endif


using namespace StoneRing;

typedef unsigned int uint;


// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2)
{
    uint p1value = p1.y  *  Application::WINDOW_HEIGHT + p1.x;
    uint p2value = p2.y  * Application::WINDOW_HEIGHT + p2.x;
	
    return p1value < p2value;
}

bool operator < (const SpriteRef::eDirection dir1, const SpriteRef::eDirection dir2)
{
    return (int)dir1 < (int)dir2;
}


#ifdef _MSC_VER
template <>
struct std::greater<MappableObject*>
{
    bool operator()(const MappableObject *n1, const MappableObject *n2) const
	{
	    return moSortCriterion(n1,n2);
	}
};
#endif


ItemRef::ItemRef(CL_DomElement *pElement )
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 1) throw CL_Error("Error reading attributes in itemRef.");


    std::string itemtype;
    itemtype = attributes.get_named_item("itemType").get_node_value();

    if(itemtype == "item")
    {
	meType = Item::ITEM;
    }
    else if(itemtype == "weapon")
    {
	meType = Item::WEAPON;
    }
    else if(itemtype == "armor")
    {
	meType = Item::ARMOR;
    }
    else if(itemtype == "rune")
    {
	meType = Item::RUNE;
    }
    else if(itemtype == "special")
    {
	meType = Item::SPECIAL;
    }
    else throw CL_Error("Bad item type in itemref");

    mItem = pElement->get_text();

	
}

ItemRef::~ItemRef()
{
}

std::string ItemRef::getItemName()
{
    return mItem;
}
 
Item::eItemType ItemRef::getItemType()
{
    return meType;
}

 
Tilemap::Tilemap(CL_DomElement *pElement)
{
	
	
    CL_DomNamedNodeMap attributes = pElement->get_attributes();
	
    if(attributes.get_length() < 3) throw CL_Error("Error reading attributes in tilemap");
	

    mMapName = attributes.get_named_item("mapname").get_node_value();
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

std::string Tilemap::getMapName() const
{
    return mMapName;
}


SpriteRef::SpriteRef( CL_DomElement *pElement)
{
    meDirection = SPR_NONE;


    CL_DomNamedNodeMap attributes = pElement->get_attributes();


	
    if(!attributes.get_named_item("direction").is_null())
    {
	std::string direction = attributes.get_named_item("direction").get_node_value();
		
	if(direction == "still") meDirection = SPR_STILL;
	if(direction == "north") meDirection = SPR_NORTH;
	if(direction == "west")  meDirection = SPR_WEST;
	if(direction == "south") meDirection = SPR_SOUTH;
	if(direction == "east")  meDirection = SPR_EAST;
		
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

SpriteRef::eDirection SpriteRef::getDirection() const
{
    return meDirection;
}


 
AttributeModifier::AttributeModifier (CL_DomElement *pElement)
{
    std::cout << "READING AN A.M." << std::endl;
	
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(attributes.get_length() < 2) throw CL_Error("Error reading attributes in A.M.");


    mAttribute = attributes.get_named_item("attribute").get_node_value();
    mAdd = atoi(attributes.get_named_item("add").get_node_value().c_str());

	
    if(!attributes.get_named_item("target").is_null())
    {
	mTarget = attributes.get_named_item("target").get_node_value();
    }



    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if( child.get_node_name() == "condition" )
	{
	    mConditions.push_back(new Condition( &child ));
			
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



void AttributeModifier::invoke()
{
    for( std::list<Condition*>::iterator i = mConditions.begin();
	 i != mConditions.end();
	 i++)
    {
	Condition * condition = *i;
	if( ! condition->evaluate() ) return;
    }
    // Party->modifyAttribute( blah blah blah ) ;
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
    Party * party = Party::getInstance();
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

 
 
HasItem::HasItem(CL_DomElement *pElement ):mpItemRef(NULL)
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

    CL_DomElement itemRefElement = pElement->get_first_child().to_element();

    if(itemRefElement.is_null()) throw CL_Error("hasItem missing itemRef");


    mpItemRef = new ItemRef ( &itemRefElement );

    if(mbNot) std::cout << "(NOT)";

    std::cout << "HAS ITEM: " << mpItemRef->getItemName() <<  std::endl;

}

HasItem::~HasItem()
{
    delete mpItemRef;
}

bool HasItem::evaluate()
{
    if(mbNot) return ! (Party::getInstance()->hasItem(mpItemRef )) ;
    else	return Party::getInstance()->hasItem ( mpItemRef );
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
    if(mbNot) return ! Party::getInstance()->didEvent ( mEvent );
    else  return Party::getInstance()->didEvent ( mEvent );
}


And::And(CL_DomElement * pElement)
{
//	<!ELEMENT and ((hasGold|hasItem|didEvent|operator)*)>


    CL_DomElement child = pElement->get_first_child().to_element();

    if(child.is_null()) throw CL_Error("\'And\' with no operands.");


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "operator")
	{
	    mOperands.push_back( new Operator ( &child ) );
	}
	else if(name == "hasItem")
	{
	    mOperands.push_back( new HasItem ( &child ) );
	}
	else if(name == "hasGold")
	{
	    mOperands.push_back(new HasGold ( &child ) );
	}
	else if(name == "didEvent")
	{
	    mOperands.push_back(new DidEvent ( &child ) );
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

 
Or::Or(CL_DomElement * pElement)
{

    CL_DomElement child = pElement->get_first_child().to_element();

    if(child.is_null()) throw CL_Error("\'or\' with no operands.");


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "operator")
	{
	    mOperands.push_back( new Operator ( &child ) );
	}
	else if(name == "hasItem")
	{
	    mOperands.push_back( new HasItem ( &child ) );
	}
	else if(name == "hasGold")
	{
	    mOperands.push_back(new HasGold ( &child ) );
	}
	else if(name == "didEvent")
	{
	    mOperands.push_back(new DidEvent ( &child ) );
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

 
Operator::Operator(CL_DomElement *pElement)
{
	

    CL_DomElement child = pElement->get_first_child().to_element();

    if(child.is_null()) throw CL_Error("\'operator\' with no operands.");


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "or")
	{
	    mOperands.push_back( new Or( &child ));
	}
	else if(name == "and")
	{
	    mOperands.push_back(new And ( &child ) );
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
      
 

Condition::Condition(CL_DomElement *pElement)
{
	
    std::cout << "READING CONDITION" << std::endl;
    CL_DomElement child = pElement->get_first_child().to_element();

    while(!child.is_null())
    {

	std::string name = child.get_node_name();

	if(name == "operator")
	{
	    mChecks.push_back ( new Operator( &child ));
	}
	else if ( name == "hasItem")
	{
	    mChecks.push_back ( new HasItem ( &child ));
	}
	else if ( name == "hasGold")
	{
	    mChecks.push_back ( new HasGold ( &child ));
	}
	else if (name == "didEvent")
	{
	    mChecks.push_back ( new DidEvent ( &child ));
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



Event::Event(CL_DomElement *pElement):mbRepeatable(true)
{


	
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


	
    if(!attributes.get_named_item("repeatable").is_null())
    {
	mbRepeatable = ( attributes.get_named_item("repeatable").get_node_value() == "true")?true:false;
    }



    CL_DomElement child = pElement->get_first_child().to_element();

    while( !child.is_null() )
    {
	if( child.get_node_name() == "condition" )
	{
	    mpCondition = new Condition ( &child );
			
	}
	else if (child.get_node_name() == "attributeModifier" )
	{
	    mActions.push_back ( new AttributeModifier (&child ));

	}
	else if (child.get_node_name() == "say" )
	{
	    mActions.push_back ( new Say (&child ));

	}
	else if (child.get_node_name() == "give" )
	{
	    mActions.push_back ( new Give (&child ));

	}
	else if (child.get_node_name() == "take" )
	{
	    mActions.push_back ( new Take (&child ));

	}
	else if (child.get_node_name() == "giveGold" )
	{
	    mActions.push_back ( new GiveGold (&child ));

	}
	else if (child.get_node_name() == "playAnimation" )
	{
	    mActions.push_back ( new PlayAnimation (&child ));

	}
	else if (child.get_node_name() == "playSound" )
	{
	    mActions.push_back ( new PlaySound (&child ));

	}
	else if (child.get_node_name() == "loadLevel" )
	{
	    mActions.push_back ( new LoadLevel (&child ));

	}
	else if (child.get_node_name() == "startBattle" )
	{
	    mActions.push_back ( new StartBattle (&child ));

	}
	else if (child.get_node_name() == "pause" )
	{
	    mActions.push_back ( new Pause (&child ));

	}
	else if (child.get_node_name() == "invokeShop" )
	{
	    mActions.push_back ( new InvokeShop (&child ));

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
 
bool Event::invoke()
{

    if(! mpCondition->evaluate() ) return false;

    // Party->doEvent ( mName );

    for(std::list<Action*>::iterator i = mActions.begin();
	i != mActions.end();
	i++)
    {
	Action *action = *i;

	action->invoke();
    }

    return true;
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
    // Application->playAnimation ( mAnimation );
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
    // Application->playSound ( mSound );
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
	meType - MOVEMENT_NONE;
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
    // Application->loadLevel ( mName, startX, startY );
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
    // Application->startBattle ( mMonster, mCount, mbIsBoss ) ;
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
    // Application->invokeShop ( mShopType );
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
    // Application->pause ( mMs ) ;
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
    // Application -> say ( mText, mSpeaker );
}
 
Give::Give(CL_DomElement *pElement ):mpItemRef(NULL)
{


    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    CL_DomNode countNode = attributes.get_named_item("count");

    mCount=1;


    if(! countNode.is_null() )
    {
	mCount = atoi(countNode.get_node_value().c_str());

    }

    CL_DomElement itemRefElement = pElement->get_first_child().to_element();

    if(itemRefElement.is_null()) throw CL_Error("\'give\' missing itemRef");


    mpItemRef = new ItemRef ( &itemRefElement );


}
Give::~Give()
{
    delete mpItemRef;
}

void Give::invoke()
{
    // Party->giveItem ( mpItemRef );
}
 
Take::Take(CL_DomElement *pElement ):mpItemRef(NULL)
{

    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    CL_DomNode count = attributes.get_named_item("count");

    mCount=1;


    if(! count.is_null() )
    {
	mCount = atoi(count.get_node_value().c_str());

    }

    CL_DomElement itemRefElement = pElement->get_first_child().to_element();

    if(itemRefElement.is_null()) throw CL_Error("\'take\' missing itemRef");


    mpItemRef = new ItemRef ( &itemRefElement );


}


Take::~Take()
{
    delete mpItemRef;
}

void Take::invoke()
{
    // Party->takeItem ( mpItemRef );
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
    // Party->giveGold ( mCount );
}





Graphic::Graphic()
{
}

Graphic::~Graphic()
{
}


DirectionBlock::DirectionBlock(CL_DomElement *pElement )
{
}

DirectionBlock::~DirectionBlock()
{
}

eDirectionBlock DirectionBlock::getDirectionBlock() const
{
    return meDirectionBlock;
}

bool Tile::isHot() const
{

    return cFlags & HOT;
}

Tile::Tile(CL_DomElement *pElement):mpSprite(NULL),mpCondition(NULL), mpAM(NULL), mZOrder(0), cFlags(0)
{

    std::cout << "READING A TILE." << std::endl;

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
			
	    mGraphic.asTilemap = new Tilemap( &child );
			
	}
	else if (child.get_node_name() == "spriteRef" )
	{
	    GraphicsManager * GM = GraphicsManager::getInstance();
	    mGraphic.asSpriteRef = new SpriteRef ( &child );
	    cFlags |= SPRITE;

	    // Actually create the ref'd sprite here.
	    // And assign to mpSprite
	    mpSprite = GM->createSprite( mGraphic.asSpriteRef->getRef() );

	}
	else if (child.get_node_name() == "condition" )
	{
	    mpCondition = new Condition ( &child );
	}
	else if (child.get_node_name() == "attributeModifier" )
	{
	    mpAM = new AttributeModifier ( &child );
	}
	else if ( child.get_node_name() == "directionBlock" )
	{
	    DirectionBlock block(&child);

	    eDirectionBlock db = block.getDirectionBlock();

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

    GraphicsManager * GM = GraphicsManager::getInstance();

    if( !isSprite() )
    {
	CL_Surface * tilemap = GM->getTileMap ( mGraphic.asTilemap->getMapName() );

	//		void draw(	const CL_Rect& src, const CL_Rect& dest, CL_GraphicContext* context = 0);

	CL_Rect srcRect(mGraphic.asTilemap->getMapX() * 32 + src.left, mGraphic.asTilemap->getMapY() * 32 + src.top,
			(mGraphic.asTilemap->getMapX() * 32) + src.right, (mGraphic.asTilemap->getMapY() * 32) + src.bottom);

		
	tilemap->draw(srcRect, dst, pGC);
		
    }
    else
    {
	update();
	mpSprite->draw( dst, pGC );
    }

}

void Tile::update()
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


MappableObject::eSize MappableObject::getSize() const
{
    return meSize;
} 
 
MappableObject::MappableObject(CL_DomElement *pElement):mpMovement(0),mTimeOfLastUpdate(0),mCountInCurDirection(0)
{

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

    meDirection = SpriteRef::SPR_NONE;

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
	    mGraphic.asTilemap = new Tilemap( &child );
			
	}
	else if (child.get_node_name() == "spriteRef" )
	{
	    GraphicsManager *GM = GraphicsManager::getInstance();

	    SpriteRef * pRef = new SpriteRef ( &child );
	    cFlags |= SPRITE;

	    mSprites[ pRef->getDirection() ]  = GM->createSprite ( pRef->getRef() );


	    int swidth = mSprites [ pRef->getDirection() ]->get_width();
	    int sheight = mSprites [ pRef->getDirection()]->get_height();

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
	    }


	    // Actually create the ref'd sprite here.
	    // And assign to mpSprite
	    meDirection = pRef->getDirection();

	}
	else if (child.get_node_name() == "event" )
	{
	    mEvents.push_back ( new Event ( &child ) );
	}
	else if( child.get_node_name() == "movement")
	{
	    mpMovement = new Movement ( &child );

	    // Make sure the proper sprites are around for the movement type
	    switch(mpMovement->getMovementType())
	    {
	    case Movement::MOVEMENT_WANDER:
		if(!mSprites.count( SpriteRef::SPR_NORTH )) throw CL_Error("Wandering MO with no north sprite.");
		if(!mSprites.count( SpriteRef::SPR_SOUTH )) throw CL_Error("Wandering MO with no south sprite.");
		if(!mSprites.count( SpriteRef::SPR_EAST )) throw CL_Error("Wandering MO with no east sprite.");
		if(!mSprites.count( SpriteRef::SPR_WEST )) throw CL_Error("Wandering MO with no west sprite.");
		break;
	    case Movement::MOVEMENT_PACE_NS:
		if(!mSprites.count( SpriteRef::SPR_NORTH )) throw CL_Error("Pacing MO with no north sprite.");
		if(!mSprites.count( SpriteRef::SPR_SOUTH )) throw CL_Error("Pacing MO with no south sprite.");
		break;
	    case Movement::MOVEMENT_PACE_EW:
		if(!mSprites.count( SpriteRef::SPR_EAST )) throw CL_Error("Pacing MO with no east sprite.");
		if(!mSprites.count( SpriteRef::SPR_WEST )) throw CL_Error("Pacing MO with no west sprite.");
		break;
	    default:
		break;
	    }
	}
	// TODO Movement

	child = child.get_next_sibling().to_element();
    }

	
}

MappableObject::~MappableObject()
{
    for( std::list<Event*>::iterator i = mEvents.begin();
	 i != mEvents.end();
	 i++)
    {
	delete *i;
    }
	     
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
    // HACKAROONIE! Needs to be width minus 1, height minus 1
    return CL_Rect(mX, mY, mX+ 32, mY + 32);
}

bool MappableObject::isSprite() const
{
    return cFlags & SPRITE;
}

void MappableObject::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    if(isSprite())
    {
	update();
	mSprites[meDirection]->draw( dst, pGC );
    }
    else if( cFlags & TILEMAP )
    {
	GraphicsManager * GM = GraphicsManager::getInstance();

	CL_Surface * tilemap = GM->getTileMap ( mGraphic.asTilemap->getMapName() );


	CL_Rect srcRect(mGraphic.asTilemap->getMapX() * 32 + src.left, mGraphic.asTilemap->getMapY() * 32 + src.top,
			(mGraphic.asTilemap->getMapX() * 32) + src.right, (mGraphic.asTilemap->getMapY() * 32) + src.bottom);

		
	tilemap->draw(srcRect, dst, pGC);
		
    }
	

}

void MappableObject::moveInCurrentDirection()
{


    int delay = 0;

    CL_Rect newRect = getRect();

    int nX = mX;
    int nY = mY;
	
    switch( mpMovement->getMovementSpeed() )
    {
    case Movement::SLOW:
	delay = 500;
	break;
    case Movement::FAST:
	delay = 250;
	break;
    }


    if(CL_System::get_time() > mTimeOfLastUpdate + delay)
    {

	mTimeOfLastUpdate = CL_System::get_time();

	switch ( meDirection )
	{
	case SpriteRef::SPR_SOUTH:
	    newRect.top++;
	    newRect.bottom++;
	    nY++;
	    break;
	case SpriteRef::SPR_NORTH:
	    newRect.top--;
	    newRect.bottom--;
	    nY--;
	    break;
	case SpriteRef::SPR_EAST:
	    newRect.left++;
	    newRect.right++;
	    nX++;
	    break;
	case SpriteRef::SPR_WEST:
	    newRect.right--;
	    newRect.left--;
	    nX--;
	    break;
	default:
	    break;
		
	}

	if(!Application::getApplication()->canMove ( getRect(), newRect, true))
	{
	    randomNewDirection();
	}
	else
	{
	    mX = nX;
	    mY = nY;
	    mCountInCurDirection++;

	    if(mCountInCurDirection > rand() %128 + 32)
		randomNewDirection();
	}

    }


	

}

void MappableObject::randomNewDirection()
{
    mCountInCurDirection = 0;

    int r= rand() % 20;
	
    switch( r)
    {
    case 0:
	if(mSprites.count(SpriteRef::SPR_NORTH))
	    meDirection = SpriteRef::SPR_NORTH;
	break;
    case 1:
	if(mSprites.count(SpriteRef::SPR_SOUTH))
	    meDirection = SpriteRef::SPR_SOUTH;
	break;
    case 2:
	if(mSprites.count(SpriteRef::SPR_EAST))
	    meDirection = SpriteRef::SPR_EAST;
	break;
    case 3:
	if(mSprites.count(SpriteRef::SPR_WEST))
	    meDirection = SpriteRef::SPR_WEST;
	break;
    case 4:
	if(mSprites.count(SpriteRef::SPR_STILL))
	    meDirection = SpriteRef::SPR_STILL;
	break;
    default:
	break;
			
    }
	
	
	
	
}

void MappableObject::update()
{
    if(isSprite())
	mSprites[meDirection]->update();

    if(!mpMovement) return;

    switch(mpMovement->getMovementType())
    {
    case Movement::MOVEMENT_PACE_NS:
    case Movement::MOVEMENT_PACE_EW:
    case Movement::MOVEMENT_WANDER:
	moveInCurrentDirection();
	break;
    case Movement::MOVEMENT_NONE:
	break;
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




void MappableObject::provokeEvents ( Event::eTriggerType trigger )
{
    Party *party = Party::getInstance();

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


Level::Level(const std::string &name,CL_ResourceManager * pResources): mpDocument(NULL)
{
    srand(time(0));
    // Get the level file name from resources

    std::string path = CL_String::load("Game/LevelPath", pResources);
    std::string filename = CL_String::load("Levels/" + name, pResources);

    // Load the level
    LoadLevel ( path + filename );
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

	delete mTileMap[x];
    
    }
}
    

void Level::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC, bool floaters)
{
//	int maxSrcX = max( ceil(dst.get_width() / 32.0), mLevelWidth );
//	int maxSrcY = max( ceil(dst.get_height() / 32.0), mLevelHeight);
	
    int widthInTiles = ceil((float)src.get_width() / 32.0 );
    int heightInTiles = ceil((float)src.get_height() / 32.0 );

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
			pTile->draw(tileSrc, tileDst , pGC );
		    
		    
		    
		    
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
 						pTile->draw(tileSrc, tileDst , pGC );
 				
  					
 					
 					
 				}
 			}
			
 			
 			
 			
 			
  		}
  	}
	
    }
    
    


	

}	    


void Level::drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    // This brings the close MOs to the top
    // moSortCriterion queries the party to see where they are at.
    mMappableObjects.sort( moSortCriterion );




    for(std::list<MappableObject*>::iterator i = mMappableObjects.begin();
	i != mMappableObjects.end();
	i++)
    {
	CL_Rect moRect = (*i)->getRect();
	CL_Rect dstRect( moRect.left + dst.left, moRect.top + dst.top,
			 moRect.left + dst.left + moRect.get_width(), moRect.top + dst.top + moRect.get_height());
	if( ! src.is_overlapped ( moRect ) )
	{
	    // This MO was outside our field of vision, so we stop iterating.
	    break;
	}

	(*i)->draw( moRect, dstRect, pGC );
    }
	
}


void Level::drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC)
{
    draw(src,dst,pGC, true);
}


      
// Checks relevant tile and MO direction block information
bool Level::canMove(const CL_Rect &currently, const CL_Rect & destination, bool noHot) const
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
	
	


    //  Check MOs for overlap, return true if any
    for(std::list<MappableObject*>::const_iterator iter = mMappableObjects.begin();
	iter != mMappableObjects.end();
	iter++)
    {


	MappableObject *pMO = *iter;

	CL_Rect moRect = pMO->getRect();

	if(pMO->getRect() == currently) 
	{
	    // HACK: This is us, therefore we dont care
	    break;
	}

	//TODO: Possible optimization. we ought to be able to sort the map here
	// And then, like in the draw, stop iterating once we hit something outside the rect
	// but, we should already be sorted........

	if(! Application::getApplication()->getLevelRect().is_overlapped ( moRect ) )
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
		if( oppositeDir & (*iter)->getDirectionBlock()) return false;
	    }


	    // Check for blocks in our destination
	    if(np.x < mLevelWidth && np.y < mLevelHeight)
	    {
		for(std::list<Tile*>::const_iterator iter = mTileMap[np.x][np.y].begin();
		    iter != mTileMap[np.x][np.y].end();
		    iter++)
		{
		    
		   // Blocked
		   if( movementDir & (*iter)->getDirectionBlock()) return false;

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
		    if( oppositeDir & (*iter)->getDirectionBlock()) return false;
		}
		


	    // Check for blocks in our destination
	    if(np.x < mLevelWidth && np.y < mLevelHeight)
	    {


		for(std::list<Tile*>::const_iterator iter = mTileMap[np.x][np.y].begin();
		    iter != mTileMap[np.x][np.y].end();
		    iter++)
		{
		    

		    // Blocked
		    if( movementDir & (*iter)->getDirectionBlock()) return false;

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
void Level::step(uint levelX, uint levelY)
{
}
      
// Any talk events fire (assuming they meet conditions)
void Level::talk(uint levelX, uint levelY)
{
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

    Application * pApp = Application::getApplication();

    uint pX = pApp->getLevelRect().get_width() / 2;
    uint pY = pApp->getLevelRect().get_height() / 2;

	
    uint p1Distance, p2Distance;

	
    p1Distance = std::max(abs( (long)pX - p1->getX()) , abs((long)pY - p1->getY()));
    p2Distance = std::max(abs( (long)pX - p2->getX()) , abs((long)pY - p2->getY()));

    return p1Distance < p2Distance;


}

// Sort tiles on zOrder
bool Level::tileSortCriterion ( const Tile * p1, const Tile * p2)
{
    return p1->getZOrder() < p2->getZOrder();
}

void Level::LoadLevel( const std::string & filename  )
{
    CL_InputSource_File file(filename);

    CL_DomDocument document;

	
    document.load(&file);

    std::cout << "LOADING LEVEL: " << filename << std::endl;


    CL_DomElement levelNode = document.named_item("level").to_element(); 


    CL_DomNamedNodeMap levelAttributes = levelNode.get_attributes();

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

    // Create tilemap

    mTileMap = new std::list<Tile*> * [mLevelWidth];

    for(int x=0;x< mLevelWidth; x++)
    {
	mTileMap[x] = new std::list<Tile*> [mLevelHeight];
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

void Level::loadMo ( CL_DomElement * moElement )
{
    MappableObject * mo = new MappableObject ( moElement );

    mMappableObjects.push_back( mo );

}


void Level::loadTile ( CL_DomElement * tileElement)
{
    CL_Point point;

    Tile * tile = new Tile ( tileElement );

    point.x = tile->getX();
    point.y = tile->getY();
	
    if( tile->isFloater() )
    {
	std::cout << "Placing floater at: " << point.x << ',' << point.y << std::endl;

	mFloaterMap[ point ].push_back ( tile );
		
	mFloaterMap[ point ].sort( &tileSortCriterion );
    }
    else
    {

	std::cout << "Placing tile at : " << point.x << ',' << point.y << std::endl;
		
	mTileMap[ point.x ][point.y].push_back ( tile );

	// Sort by ZOrder, so that they display correctly
	mTileMap[ point.x ][point.y].sort( &tileSortCriterion );
    }
}
