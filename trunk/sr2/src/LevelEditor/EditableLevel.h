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


class EditableScriptElement : public StoneRing::ScriptElement
{
public:
    EditableScriptElement(bool isCondition)
        :StoneRing::ScriptElement(isCondition)
    {
    }
    virtual ~EditableScriptElement(){}
    virtual CL_DomElement createDomElement(CL_DomDocument&) const;

    void setScript(const std::string &);

    // May throw SteelException. Please catch and present
    void parse();
private:
    virtual void handleText(const std::string &);
    std::string mScript;
};


class EditableTilemap : public StoneRing::Tilemap
{
public:
    EditableTilemap(){}
    virtual ~EditableTilemap();

    void setMapName(const std::string & mapname);
    void setMapX(int x);
    void setMapY(int y);

    
};

class EditableEvent : public StoneRing::Event
{
public:
    EditableEvent(){}
    virtual ~EditableEvent(){}

    void setScript(ScriptElement *pScript){ mpScript = pScript; }
    virtual CL_DomElement createDomElement(CL_DomDocument&) const;
private:
};

class EditableSpriteRef : public StoneRing::SpriteRef
{
public:
    EditableSpriteRef(){}
    virtual ~EditableSpriteRef(){}
    void setSpriteRef( const std::string &ref);
    void setType( StoneRing::SpriteRef::eType dir);

};


class EditableTile : public StoneRing::Tile
{
public:
    EditableTile();
    virtual ~EditableTile();

    void setLevelX(int x);
    void setLevelY(int y);
    void setZOrder(int z);

    // Has to have one of these if it's new
    void setTilemap( const std::string &mapname, uint mapX, uint mapY);
    void setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eType direction );




    void setIsFloater();
    void setIsHot();
    void setSideBlock (int dirBlock );

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
    void setSideBlockAt(uint levelX, uint levelY, eSideBlock dir, bool bOn);

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




