#ifndef SR_EDITABLE_LEVEL_H
#define SR_EDITABLE_LEVEL_H

#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "IApplication.h"
#include "Level.h"

using std::string;

typedef unsigned int uint;
typedef unsigned short ushort;


using namespace StoneRing;

class EditableTilemap : public StoneRing::Tilemap
{
 public:
    EditableTilemap(const std::string &mapname, uint x, uint y);
    EditableTilemap(CL_DomElement *pElement);
    virtual ~EditableTilemap();
    
};


class EditableTile : public StoneRing::Tile
{
 public:
    EditableTile();
    EditableTile(CL_DomElement *pElement );
    ~EditableTile();

    void setLevelX(int x);
    void setLevelY(int y);
    void setZOrder(int z);

    // Has to have one of these if it's new
    void setTilemap( const std::string &mapname, uint mapX, uint mapY);
    void setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eDirection direction );


    void setIsFloater();
    void setIsHot();
    void setDirectionBlock (int dirBlock );
    

 private:
};



class EditableLevel : public StoneRing::Level
{
 public:
    EditableLevel(const std::string &name,CL_ResourceManager * pResources);
    virtual ~EditableLevel();
			
			
    virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC , bool floaters = false);
    virtual void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);
    virtual void drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC);

    std::list<Tile*> getTilesAt(uint levelX, uint levelY) const;
		
    void addTile ( Tile * pTile );
    void removeTile ( Tile * pTile );

    
    // All AM's from tiles fire, as do any step events
    virtual void step(uint levelX, uint levelY);
      
    // Any talk events fire (assuming they meet conditions)
    virtual void talk(uint levelX, uint levelY);

    // Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
    virtual void update(const CL_Rect & updateRect);

          
 protected:

}

#endif
