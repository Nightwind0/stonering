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

class EditableSpriteRef : public StoneRing::SpriteRef
{
 public:
    EditableSpriteRef( const std::string &ref, StoneRing::SpriteRef::eDirection dir );
    EditableSpriteRef(CL_DomElement * pElement );
    virtual ~EditableSpriteRef();
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
    EditableLevel(){}
    EditableLevel(const std::string &name,CL_ResourceManager * pResources);
    virtual ~EditableLevel();
			

    virtual void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);


    std::list<Tile*> getTilesAt(uint levelX, uint levelY) const;
		
    void addTile ( Tile * pTile );
    void removeTile ( Tile * pTile );

    

          
 protected:

    virtual void loadTile ( CL_DomElement * tileElement);

};

#endif
