#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "../Application.h"
#include "EditableLevel.h"
#include "../GraphicsManager.h"


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

~EditableTilemap::EditableTilemap()
{
    
}


EditableTile::EditableTile():mpSprite(0),mZOrder(0),cFlags(0)
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

void EditableTile::setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eDirection direction );


void EditableTile::setIsFloater();
void EditableTile::setIsHot();
void EditableTile::setDirectionBlock (int dirBlock );
    



class EditableLevel : public StoneRing::Level
{
 public:
    EditableLevel(const std::string &name,CL_ResourceManager * pResources);
    virtual ~EditableLevel();
			
			
    virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC , bool floaters = false);
    virtual void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);
    virtual void drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC);
		
    void addTile ( Tile * pTile );
    void removeTile ( Tile * pTile );
    std::list<Tile*> getTilesAt(uint levelX, uint levelY) const;
    
    // All AM's from tiles fire, as do any step events
    virtual void step(uint levelX, uint levelY);
      
    // Any talk events fire (assuming they meet conditions)
    virtual void talk(uint levelX, uint levelY);

    // Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
    virtual void update(const CL_Rect & updateRect);

          
 protected:

}

#endif
