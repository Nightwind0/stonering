#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "IApplication.h"
#include "EditableLevel.h"
#include "GraphicsManager.h"


using std::string;

typedef unsigned int uint;
typedef unsigned short ushort;


using namespace StoneRing;



EditableTilemap::EditableTilemap(const std::string &mapname, uint x, uint y)
{
    mpSurface = GraphicsManager::getInstance()->getTileMap( mapname );
    mX = x;
    mY = y;
}
EditableTilemap::EditableTilemap(CL_DomElement *pElement):Tilemap(pElement)
{
}

EditableTilemap::~EditableTilemap()
{
    
}


EditableSpriteRef::EditableSpriteRef( const std::string &ref, StoneRing::SpriteRef::eDirection dir )
{
    mRef = ref;
    meDirection = dir;
}
EditableSpriteRef::EditableSpriteRef(CL_DomElement * pElement ):SpriteRef(pElement)
{
}

EditableSpriteRef::~EditableSpriteRef()
{
}



EditableTile::EditableTile()
{
}

EditableTile::EditableTile(CL_DomElement *pElement ):Tile(pElement)
{

}

EditableTile::~EditableTile()
{
}

void EditableTile::setLevelX(int x)
{
    mX = x;
}

void EditableTile::setLevelY(int y)
{
    mY = y;
}

void EditableTile::setZOrder(int z)
{
    mZOrder = z;
}

// Has to have one of these if it's new
void EditableTile::setTilemap( const std::string &mapname, uint mapX, uint mapY)
{
    // TODO: Factory method...
    mGraphic.asTilemap = new EditableTilemap( mapname,mapX,mapY );   
}

void EditableTile::setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eDirection direction )
{
    GraphicsManager * GM = GraphicsManager::getInstance();
    mGraphic.asSpriteRef = new EditableSpriteRef (spriteRef, direction   );
}


void EditableTile::setIsFloater()
{
    cFlags |= FLOATER;
}

void EditableTile::setIsHot()
{
    cFlags |= HOT;
}

void EditableTile::setDirectionBlock (int dirBlock )
{
    if(dirBlock & StoneRing::DIR_NORTH)
	cFlags |= BLK_NORTH;
    if(dirBlock & StoneRing::DIR_SOUTH)
	cFlags |= DIR_SOUTH;
    if(dirBlock & StoneRing::DIR_WEST)
	cFlags |= DIR_WEST;
    if(dirBlock & StoneRing::DIR_EAST)
	cFlags |= DIR_EAST;
}
    


void EditableLevel::loadTile ( CL_DomElement * tileElement)
{

    CL_Point point;



    // TODO: We need a factory which we get from IApplication,
    // and then we call pFactory->createTile ( tileElement );
    // and it could give us back a Tile, or an EditableTile
    // Then we wouldn't have to overload this method at all

    EditableTile * tile = new EditableTile ( tileElement );

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

EditableLevel::EditableLevel(const std::string &name,CL_ResourceManager * pResources):Level(name,pResources)
{
}
EditableLevel::~EditableLevel()
{
}
			
void EditableLevel::drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
}
 

		
void EditableLevel::addTile ( Tile * pTile )
{
    if( pTile->isFloater())
    {
	mFloaterMap[ CL_Point(pTile->getX(),pTile->getY()) ].push_back(pTile);
    }
    else
    {
	mTileMap[ pTile->getX() ][ pTile->getY()].push_back ( pTile );	
    }
}

void EditableLevel::removeTile ( Tile * pTile )
{
    if(pTile->isFloater())
    {
	if(mFloaterMap.count( CL_Point(pTile->getX(), pTile->getY() ) ))
	{
	    mFloaterMap[CL_Point(pTile->getX(),pTile->getY())].remove( pTile );
	}
    }
    else
    {
	mTileMap[pTile->getX()][pTile->getY()].remove (pTile );
    }
}
 
std::list<Tile*> EditableLevel::getTilesAt(uint levelX, uint levelY) const
{

	std::list<Tile *> tiles = mTileMap[ levelX][levelY];

	if(mFloaterMap.count( CL_Point(levelX, levelY )))
	{
		
			
	
	}

	return tiles;

}
    


