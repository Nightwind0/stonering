#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "Application.h"
#include "Level.h"
#include <algorithm>
#include <stdlib.h>


using std::string;



using namespace StoneRing;

typedef unsigned int uint;


// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2)
{
	uint p1value = p1.y  *  Application::WINDOW_HEIGHT +
		p1.x;
	uint p2value = p2.y  * Application::WINDOW_HEIGHT + 
		p2.x;
	
	return p1value < p2value;
}



ItemRef::ItemRef(CL_DomElement *pElement )
{
}

ItemRef::~ItemRef()
{
}

std::string ItemRef::getItemName()
{
	return "FUZZBALL";
}
 
Item::eItemType ItemRef::getItemType()
{
}

 
Tilemap::Tilemap(CL_DomElement *pElement)
{
}
 
Tilemap::~Tilemap()
{
}
      
ushort Tilemap::getMapX() const
{
}

ushort Tilemap::getMapY() const
{
}

std::string Tilemap::getMapName() const
{
}


SpriteRef::SpriteRef( CL_DomElement *pElement)
{
}

SpriteRef::~SpriteRef()
{
}

std::string SpriteRef::getRef() const
{
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
	// return Party->hasGold( mAmount ) ;
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
	// return Party->hasItem ( mpItemRef );
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
	// return Party->didEvent ( mEvent );
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



Event::Event(CL_DomElement *pElement)
{
}

Event::~Event()
{
}

std::string Event::getName() const
{
}

Event::eTriggerType Event::getTriggerType()
{
}

bool Event::repeatable()
{
}
 
bool Event::invoke()
{
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



	CL_DomElement child = pElement->get_first_child().to_element();

	while( !child.is_null() )
	{
		if( child.get_node_name() == "tilemap" )
		{
			mGraphic.asTilemap = new Tilemap( &child );
			
		}
		else if (child.get_node_name() == "spriteRef" )
		{
			mGraphic.asSpriteRef = new SpriteRef ( &child );
			cFlags |= SPRITE;

		        // Actually create the ref'd sprite here.
			// And assign to mpSprite

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
	return mX * 32;
}

uint Tile::getY() const
{
	return mY * 32;
}


CL_Rect Tile::getRect()
{
	return CL_Rect(mX * 32, mY * 32, mX * 32 + 32, mY * 32 + 32 );
}

bool Tile::isSprite() const
{
	return cFlags & SPRITE;
}

void Tile::draw(uint targetX, uint targetY, CL_GraphicContext *pGC)
{
	// Get the graphic guy
	// Get our tilemap or sprite
	// Blit it
}

void Tile::update()
{
//	if(mpSprite) mpSprite->update();
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


 
 
MappableObject::MappableObject(CL_DomElement *pElement)
{
}

MappableObject::~MappableObject()
{
}

ushort MappableObject::getStartX() const
{
	return 0;
}

ushort MappableObject::getStartY() const
{
	return 0;
}



MappableObject::eMovementType MappableObject::getMovementType() const
{
	return MOVEMENT_NONE;
}




std::string MappableObject::getName() const
{
}

uint MappableObject::getX() const
{
}

uint MappableObject::getY() const
{
}

CL_Rect MappableObject::getRect()
{
}

bool MappableObject::isSprite() const
{
}

void MappableObject::draw(uint targetX, uint targetY, CL_GraphicContext *pGC)
{
}

void MappableObject::update()
{
}

int MappableObject::getDirectionBlock() const
{
}

bool MappableObject::isTile() const
{
	return false;
}





Level::Level(const std::string &name,CL_ResourceManager * pResources): mpDocument(NULL)
{
	// Get the level file name from resources

	std::string path = CL_String::load("Game/LevelPath", pResources);
	std::string filename = CL_String::load("Levels/" + name, pResources);

	// Load the level
	LoadLevel ( path + filename );
}

Level::~Level()
{
	for(std::multimap<CL_Point,Tile*>::iterator i= mTileMap.begin(); i != mTileMap.end(); i++)
	{
		delete i->second;
	}
}
      

void Level::draw(uint levelX, uint levelY, CL_GraphicContext * pGC, uint windowX , uint windowY,
		uint windowWidth,
		uint windowHeight)
{

}
      
      
// Checks relevant tile and MO direction block information
bool Level::canMove(const CL_Rect &currently, const CL_Rect & destination) const
{
	return false;
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
	Party * pParty = Party::getInstance();
	uint pX = pParty->getLevelX();
	uint pY = pParty->getLevelY();
	
	uint p1Distance, p2Distance;

	
	p1Distance = std::max(std::abs( (long)pX - p1->getX()) , std::abs((long)pY - p1->getY()));
	p2Distance = std::max(std::abs( (long)pX - p2->getX()) , std::abs((long)pY - p2->getY()));

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

		// loadMo ( &currentMo );

		currentMo = currentMo.get_next_sibling().to_element();

		
	}

	std:: cout << "FOUND " << mocount << " MAPPABLE OBJECTS" << std::endl;


}


void Level::loadTile ( CL_DomElement * tileElement)
{
	CL_Point point;

	Tile * tile = new Tile ( tileElement );

	point.x = tile->getX();
	point.y = tile->getY();

	mTileMap.insert(std::make_pair(point, tile));
}
