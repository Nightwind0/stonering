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
}

AttributeModifier::~AttributeModifier()
{
}



void AttributeModifier::invoke()
{
}



HasGold::HasGold( CL_DomElement *pElement)
{
}

HasGold::~HasGold()
{
}

bool HasGold::evaluate()
{
}

 
 
HasItem::HasItem(CL_DomElement *pElement )
{
}

HasItem::~HasItem()
{
}

bool HasItem::evaluate()
{
}

 
DidEvent::DidEvent(CL_DomElement *pElement)
{
}

DidEvent::~DidEvent()
{
}

bool DidEvent::evaluate()
{
}


And::And(CL_DomElement * pElement)
{
}
And::~And()
{
}

bool And::evaluate()
{
}

ushort And::order()
{
}

 
Or::Or(CL_DomElement * pElement)
{
}
Or::~Or()
{
}

bool Or::evaluate()
{
}

ushort Or::order()
{
}

 
Operator::Operator(CL_DomElement *pElement)
{
}
Operator::~Operator()
{
}

bool Operator::evaluate()
{
}

ushort Operator::order()
{
	return 0;
}
      
 

Condition::Condition(CL_DomElement *pElement)
{
}

Condition::~Condition()
{
}

bool Condition::evaluate() const
{
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
}
 
PlayAnimation::~PlayAnimation()
{
}

void PlayAnimation::invoke()
{
}
 


PlaySound::PlaySound(CL_DomElement *pElement )
{
}

PlaySound::~PlaySound()
{
}

void PlaySound::invoke()
{
}
 
LoadLevel::LoadLevel(CL_DomElement *pElement)
{
}
LoadLevel::~LoadLevel()
{
}

void LoadLevel::invoke()
{
}



StartBattle::StartBattle(CL_DomElement *pElement)
{
}
 
StartBattle::~StartBattle()
{
}

void StartBattle::invoke()
{
}

 
InvokeShop::InvokeShop(CL_DomElement *pElement)
{
}
 
InvokeShop::~InvokeShop()
{
}

void InvokeShop::invoke()
{
}

 
Pause::Pause(CL_DomElement *pElement )
{
}
Pause::~Pause()
{
}

void Pause::invoke()
{
}
 
Say::Say (CL_DomElement *pElement )
{
}

Say::~Say()
{
}

void Say::invoke()
{
}
 
Give::Give(CL_DomElement *pElement )
{
}
Give::~Give()
{
}

void Give::invoke()
{
}
 
Take::Take(CL_DomElement *pElement )
{
}
Take::~Take()
{
}

void Take::invoke()
{
}

 

 
GiveGold::GiveGold( CL_DomElement *pElement )
{
}
GiveGold::~GiveGold()
{
}

void GiveGold::invoke()
{
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

Tile::Tile(CL_DomElement *pElement):mpTilemap(NULL),mpSprite(NULL),mpCondition(NULL), mpAM(NULL)
{

#if 1

	CL_DomNamedNodeMap attributes = pElement->get_attributes();

	if(attributes.get_length() == 0) throw CL_Error("Error reading attributes in tile");

	CL_DomAttr xpos = attributes.get_named_item ( "xpos" ).to_attr();

	if(xpos.is_null() || !xpos.get_specified()) throw CL_Error("Error reading xpos in tile");

	CL_DomAttr ypos = attributes.get_named_item ( "ypos" ).to_attr();

	if(ypos.is_null() || !ypos.get_specified()) throw CL_Error("Error reading ypos in tile");
	mX = atoi ( xpos.get_value().c_str() );
	mY = atoi ( ypos.get_value().c_str() );

	CL_DomAttr zorder = attributes.get_named_item("zorder").to_attr();

	if(zorder.get_specified())
	  mZOrder = atoi ( attributes.get_named_item( "zorder").to_attr().get_node_value().c_str() );
	else mZOrder = 0;

	CL_DomElement child = pElement->get_first_child().to_element();

	while( !child.is_null() )
	{
		if( child.get_node_name() == "tilemap" )
		{
			mpTilemap = new Tilemap( &child );
		}
		else if (child.get_node_name() == "spriteRef" )
		{
			mpSprite = new SpriteRef ( &child );
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

			meDirectionBlock = block.getDirectionBlock();
		}
	}

#endif

}

Tile::~Tile()
{
	delete mpAM;
	delete mpCondition;
	delete mpSprite;
	delete mpTilemap;
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
	return CL_Rect(mX, mY, mX + 32, mY + 32 );
}

bool Tile::isSprite() const
{
	return mpSprite != NULL;
}

void Tile::draw(uint targetX, uint targetY, CL_GraphicContext *pGC)
{
	
}

void Tile::update()
{
//	if(mpSprite) mpSprite->update();
}

eDirectionBlock Tile::getDirectionBlock() const
{
	return meDirectionBlock;
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

eDirectionBlock MappableObject::getDirectionBlock() const
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
