#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include <iostream>
#include "IApplication.h"
#include "EditableLevel.h"
#include "GraphicsManager.h"


using std::string;


typedef unsigned int uint;
typedef unsigned short ushort;

using namespace std;

using namespace StoneRing;


Element * EditableLevelFactory::createTile() const
{
    return new EditableTile();
}

/*MappableObject * EditableLevelFactory::createMappableObject() const
  {
  return new EditableMappableObject();
  }
  MappableObject * EditableLevelFactory::createMappableObject(clan::DomElement *pElement) const
  {
  return new EditableMappableObject(pElement);
  }
*/

Element * EditableLevelFactory::createEvent() const
{
    return new EditableEvent();
}

Element * EditableLevelFactory::createScriptElement() const
{
    return new EditableScriptElement(false);
}

Element * EditableLevelFactory::createConditionScript() const
{
    return new EditableScriptElement(true);
}


Element * EditableLevelFactory::createTilemap() const
{
    return new EditableTilemap();
}

Element * EditableLevelFactory::createSpriteRef() const
{
    return new EditableSpriteRef();
}

clan::DomElement EditableScriptElement::createDomElement(clan::DomDocument &doc) const
{
    clan::DomElement  element(doc, isConditionScript()?"conditionScript":"script");


    element.set_attribute("id",mId);

    clan::DomText text ( doc, mScript );
    text.set_node_value ( mScript );

    element.append_child ( text );

    return element;
}

void EditableScriptElement::setScript(const std::string &script)
{
    mScript = script;
}

void EditableScriptElement::parse()
{
    mpScript = IApplication::getInstance()->loadScript(mId,mScript);
}

void EditableScriptElement::handleText(const std::string &script)
{
    mScript = script;
}

clan::DomElement EditableEvent::createDomElement(clan::DomDocument &doc) const
{
    clan::DomElement  element(doc,"event");

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

    if(mpCondition)
    {
        clan::DomElement e = mpCondition->createDomElement(doc);
        element.append_child ( e );
    }

    if(mpScript)
    {
        clan::DomElement e = mpScript->createDomElement(doc);
        element.append_child ( e );
    }

    return element;
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



void EditableSpriteRef::setSpriteRef( const std::string &ref)
{
    mRef = ref;
}

void EditableSpriteRef::setType( StoneRing::SpriteRef::eType dir)
{
    meType = dir;
}


EditableTile::EditableTile()
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

void EditableTile::setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eType direction )
{
    GraphicsManager * GM = GraphicsManager::getInstance();
    mGraphic.asSpriteRef = new EditableSpriteRef ();

    ((EditableSpriteRef*)mGraphic.asSpriteRef)->setSpriteRef ( spriteRef );
    ((EditableSpriteRef*)mGraphic.asSpriteRef)->setType( direction );
}


void EditableTile::setIsFloater()
{
    cFlags |= FLOATER;
}

void EditableTile::setIsHot()
{
    cFlags |= HOT;
}

void EditableTile::setSideBlock (int dirBlock )
{
    if(dirBlock & StoneRing::DIR_NORTH)
        cFlags |= TBK_NORTH;
    if(dirBlock & StoneRing::DIR_SOUTH)
        cFlags |= DIR_SOUTH;
    if(dirBlock & StoneRing::DIR_WEST)
        cFlags |= DIR_WEST;
    if(dirBlock & StoneRing::DIR_EAST)
        cFlags |= DIR_EAST;
}
    

void EditableTile::setNotHot()
{
    cFlags &= ~HOT;
}

void EditableTile::setNorthBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= TBK_NORTH;
    }
    else
    {
        cFlags &= ~TBK_NORTH;
    }
}

void EditableTile::setSouthBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= BLK_SOUTH;
    }
    else
    {
        cFlags &= ~BLK_SOUTH;
    }
}

void EditableTile::setEastBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= BLK_EAST;
    }
    else
    {
        cFlags &= ~BLK_EAST;
    }
}

void EditableTile::setWestBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= BLK_WEST;
    }
    else
    {
        cFlags &= ~BLK_WEST;
    }
}




EditableLevel::EditableLevel(const std::string &name,clan::ResourceManager * pResources):Level(name,pResources)
{
}
EditableLevel::~EditableLevel()
{
}
            
void EditableLevel::drawMappableObjects(const clan::Rect &src, const clan::Rect &dst, clan::Canvas *pGC)
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
    int orig_size = static_cast<int>(mTileMap.size());
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

void EditableLevel::setMusic(const std::string &music)
{
    mMusic = music;
}

        
void EditableLevel::addTile ( Tile * pTile )
{

    if(pTile->getX() >= mLevelWidth || pTile->getY() >= mLevelHeight )
        return;

    if( pTile->isFloater())
    {
        mFloaterMap[ clan::Point(pTile->getX(),pTile->getY()) ].push_back(pTile);
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
        if(mFloaterMap.count( clan::Point(pTile->getX(), pTile->getY() ) ))
        {
            mFloaterMap[clan::Point(pTile->getX(),pTile->getY())].remove( pTile );
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

    
    std::map<clan::Point,std::list<Tile*> >::const_iterator iter = mFloaterMap.find ( clan::Point(levelX,levelY));

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
    


// Operates on ALL tiles at a location. For finer control, one must operate on the tiles individually.
// bOn of true turns the direction block on for the specified direction,
// false will turn it off.
void EditableLevel::setSideBlockAt(uint levelX, uint levelY, eSideBlock dir, bool bOn)
{
    
    std::list<Tile*> tiles = getTilesAt(levelX,levelY);

    for(std::list<Tile*>::iterator iter = tiles.begin();
        iter != tiles.end();
        iter++)
    {
        switch(dir)
        {
        case DIR_NORTH:
            static_cast<EditableTile*>(*iter)->setNorthBlock(bOn);
            break;
        case DIR_SOUTH:
            static_cast<EditableTile*>(*iter)->setSouthBlock(bOn);
            break;
        case DIR_EAST:
            static_cast<EditableTile*>(*iter)->setEastBlock(bOn);
            break;
        case DIR_WEST:
            static_cast<EditableTile*>(*iter)->setWestBlock(bOn);
            break;
        }
    }
}

void EditableLevel::setHotAt(uint levelX, uint levelY, bool bHot)
{
    std::list<Tile*> tiles = getTilesAt(levelX,levelY);

    for(std::list<Tile*>::iterator iter = tiles.begin();
        iter != tiles.end();
        iter++)
    {
        if(bHot)
            static_cast<EditableTile*>(*iter)->setIsHot();
        else static_cast<EditableTile*>(*iter)->setNotHot();
    }
}




