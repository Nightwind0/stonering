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
    
}

void EditableTile::setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eDirection direction )
{
}


void EditableTile::setIsFloater()
{
}

void EditableTile::setIsHot()
{
}

void EditableTile::setDirectionBlock (int dirBlock )
{
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
}

void EditableLevel::removeTile ( Tile * pTile )
{
}
 
std::list<Tile*> EditableLevel::getTilesAt(uint levelX, uint levelY) const
{
}
    


