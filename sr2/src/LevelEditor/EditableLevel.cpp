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


Tile * EditableLevelFactory::createTile() const
{
    return new EditableTile();
}

Tile * EditableLevelFactory::createTile(CL_DomElement *pElement)const
{
    return new EditableTile(pElement);
}

/*MappableObject * EditableLevelFactory::createMappableObject() const
{
    return new EditableMappableObject();
}
MappableObject * EditableLevelFactory::createMappableObject(CL_DomElement *pElement) const
{
    return new EditableMappableObject(pElement);
}
*/
Tilemap * EditableLevelFactory::createTilemap() const
{
    return new EditableTilemap();
}
Tilemap * EditableLevelFactory::createTilemap(CL_DomElement *pElement) const
{
    return new EditableTilemap( pElement );
}
SpriteRef * EditableLevelFactory::createSpriteRef() const
{
    return new EditableSpriteRef();
}

SpriteRef * EditableLevelFactory::createSpriteRef(CL_DomElement * pElement) const
{
    return new EditableSpriteRef(pElement );
}



EditableTilemap::EditableTilemap(CL_DomElement *pElement):Tilemap(pElement)
{
}

EditableTilemap::~EditableTilemap()
{
    
}

void EditableTilemap::setMapName(const std::string & mapname)
{
    mpSurface = GraphicsManager::getInstance()->getTileMap(mapname); 
}
void EditableTilemap::setMapX(int x)
{
    mX = x;
}
void EditableTilemap::setMapY(int y)
{
    mY = y;
}



EditableSpriteRef::EditableSpriteRef(CL_DomElement * pElement ):SpriteRef(pElement)
{
}

EditableSpriteRef::~EditableSpriteRef()
{
}

void EditableSpriteRef::setSpriteRef( const std::string &ref)
{
    mRef = ref;
}

void EditableSpriteRef::setDirection( StoneRing::SpriteRef::eDirection dir)
{
    meDirection = dir;
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
    mGraphic.asTilemap = new EditableTilemap();   

    ((EditableTilemap*)mGraphic.asTilemap)->setMapName(mapname);
    ((EditableTilemap*)mGraphic.asTilemap)->setMapX(mapX);
    ((EditableTilemap*)mGraphic.asTilemap)->setMapY(mapY);

}

void EditableTile::setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eDirection direction )
{
    GraphicsManager * GM = GraphicsManager::getInstance();
    mGraphic.asSpriteRef = new EditableSpriteRef ();

    ((EditableSpriteRef*)mGraphic.asSpriteRef)->setSpriteRef ( spriteRef );
    ((EditableSpriteRef*)mGraphic.asSpriteRef)->setDirection( direction );
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
    




EditableLevel::EditableLevel(const std::string &name,CL_ResourceManager * pResources):Level(name,pResources)
{
}
EditableLevel::~EditableLevel()
{
}
			
void EditableLevel::drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
}
 

void EditableLevel::addRows(int rows)
{
    for(std::vector<std::vector<std::list<Tile*> > >::iterator iter= mTileMap.begin();
	    iter != mTileMap.end();
	iter++)
    {
	iter->resize ( iter->size() + rows );
    }

    mLevelHeight+=rows;
}

void EditableLevel::addColumns(int columns)
{
    int orig_size = mTileMap.size();
    mTileMap.resize( orig_size + columns );

    for(int x = orig_size-1; x < mTileMap.size(); x++)
    {
	mTileMap[x].resize ( mLevelHeight );
    }

    mLevelWidth+=columns;
}

void EditableLevel::setName(const std::string &name)
{
    mName = name;
}

		
void EditableLevel::addTile ( Tile * pTile )
{

    if(pTile->getX() >= mLevelWidth || pTile->getY() >= mLevelHeight )
	return;

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

	
	std::map<CL_Point,std::list<Tile*> >::const_iterator iter = mFloaterMap.find ( CL_Point(levelX,levelY));

	if(iter != mFloaterMap.end())
	{
	    for(std::list<Tile*>::const_iterator i = iter->second.begin();
		i != iter->second.end();
		i++)
	    {
		tiles.push_back ( *i );
	    }
	}
	


	return tiles;

}
    


