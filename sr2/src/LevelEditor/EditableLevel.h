#ifndef SR_EDITABLE_LEVEL_H
#define SR_EDITABLE_LEVEL_H

#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "IApplication.h"
#include "Level.h"
#include "LevelFactory.h"

using std::string;

typedef unsigned int uint;
typedef unsigned short ushort;


using namespace StoneRing;

class EditableLevelFactory : public LevelFactory
{
 public:
    EditableLevelFactory(){}
    ~EditableLevelFactory(){}

    virtual Tile * createTile() const;
    virtual Tile * createTile(CL_DomElement *pElement)const;
    // virtual MappableObject * createMappableObject() const;
    //virtual MappableObject * createMappableObject(CL_DomElement *pElement) const;
    virtual Tilemap * createTilemap() const;
    virtual Tilemap * createTilemap(CL_DomElement *pElement) const;
    virtual SpriteRef * createSpriteRef() const;
    virtual SpriteRef * createSpriteRef(CL_DomElement * pElement) const;




 private:
};


class EditableTilemap : public StoneRing::Tilemap
{
 public:
    EditableTilemap(){}
    EditableTilemap(CL_DomElement *pElement);
    virtual ~EditableTilemap();

    void setMapName(const std::string & mapname);
    void setMapX(int x);
    void setMapY(int y);

    
};

class EditableSpriteRef : public StoneRing::SpriteRef
{
 public:
    EditableSpriteRef(){}
    EditableSpriteRef(CL_DomElement * pElement );
    virtual ~EditableSpriteRef();

    void setSpriteRef( const std::string &ref);
    void setDirection( StoneRing::SpriteRef::eDirection dir);

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

    void setNorthBlock(bool bOn);
    void setSouthBlock(bool bOn);
    void setEastBlock(bool bOn);
    void setWestBlock(bool bOn);
    void setNotHot();

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


    // Operates on ALL tiles at a location. For finer control, one must operate on the tiles individually.
    // bOn of true turns the direction block on for the specified direction,
    // false will turn it off.
    void setDirectionBlockAt(uint levelX, uint levelY, eDirectionBlock dir, bool bOn);

    void setHotAt(uint levelX, uint levelY, bool bHot);
		
    void addTile ( Tile * pTile );
    void removeTile ( Tile * pTile );

    void addRows(int rows);
    void addColumns(int columns);

    void setName(const std::string &name);
    void setMusic(const std::string &music);

          
 protected:



};

#endif