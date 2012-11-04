#ifndef SR_LEVEL_H
#define SR_LEVEL_H

#include <ClanLib/core.h>
#include <string>
#include <cmath>
#include <map>
#include <list>
#include "IApplication.h"
#include <set>
#include "Item.h"
#include "ItemRef.h"
#include "sr_defines.h"
#include "MappableObject.h"
#include "Node.hxx"
#include <ClanLib/core.h>

using std::string;




namespace StoneRing {

class WeaponRef;
class ArmorRef;
class NamedItemRef;
class StartingEquipmentRef;
class ScriptElement;
class Action;
class MonsterRegions;
class MonsterRegion;

enum eSideBlock
{
    BLK_NORTH = 1,
    BLK_SOUTH = 2,
    BLK_WEST = 4,
    BLK_EAST = 8
};

class SideBlock : public Element
{
public:
    SideBlock();
    explicit SideBlock(int);
    virtual ~SideBlock();
    virtual eElement WhichElement() const {
        return EDIRECTIONBLOCK;
    }
    int GetSideBlock() const;
#if SR2_EDITOR
    CL_DomElement CreateDomElement(CL_DomDocument&)const;
#endif
protected:
    virtual void load_attributes(CL_DomNamedNodeMap attributes);

    int m_eSideBlock;
private:

};


class Tilemap : public Element
{
public:
    Tilemap();
    virtual ~Tilemap();
    virtual eElement WhichElement() const {
        return ETILEMAP;
    }
    inline ushort GetMapX() const {
        return m_X;
    }
    inline ushort GetMapY() const {
        return m_Y;
    }

    inline CL_Image GetTileMap() const {
        return m_image;
    }
#if SR2_EDITOR
    CL_DomElement CreateDomElement(CL_DomDocument&)const;
    void SetTilemap(CL_Image image,const CL_String& name, ushort X, ushort Y) {
        m_image = image;
        m_sprite_string = name;
        m_X = X;
        m_Y = Y;
    }
private:
    CL_String m_sprite_string;
#endif
protected:
    virtual bool handle_element(eElement element, Element * pElement );
    virtual void load_attributes(CL_DomNamedNodeMap attributes);
    CL_Image m_image;
    ushort m_X;
    ushort m_Y;
};

class Tile : public Graphic, public Element
{
public:
    enum eFlags { 
		TIL_SPRITE = 1, 
		TIL_FLOATER = (1<<1),
		TIL_WATER = (1<<2),
		TIL_HOT = (1<<3), 
		TIL_BLK_NORTH = (1<<4), 
		TIL_BLK_SOUTH = (1<<5),
		TIL_BLK_EAST = (1<<6),
		TIL_BLK_WEST = (1<<7),
		TIL_POPS = (1<<8)
	};
    class Visitor { 
    public:
        virtual void accept(CL_GraphicContext& gc, const CL_Point& top_left, Tile*) = 0;
    };
public:
    Tile();
    virtual ~Tile();
    virtual eElement WhichElement() const {
        return ETILE;
    }
    inline int GetZOrder() const {
        return (int(m_Y+m_ZOffset) * 2)*32 + (IsFloater()*100000);
    }

    inline bool IsFloater() const {
        return (cFlags & TIL_FLOATER) != 0;
    }
    bool EvaluateCondition() const;
    inline bool HasScript() const {
        return m_pScript != NULL;
    }

    void Activate(); // Call any script
	
	char GetMonsterRegion() const {
		return m_monster_region;
	}
    
    void Visit(Visitor* pVisitor, CL_GraphicContext& gc, const CL_Point& top_left) {
        pVisitor->accept(gc,top_left,this);
    }

    inline int GetX() const {
        return m_X;
    }
    inline int GetY() const {
        return m_Y;
    }

    virtual CL_Rect GetRect() const;

    inline bool IsSprite() const {
        return (cFlags & TIL_SPRITE) != 0;
    }

    inline bool IsHot() const {
        return (cFlags & TIL_HOT) != 0;
    }

    inline bool Pops() const {
        return (cFlags & TIL_POPS) != 0;
    }
	void Draw(CL_GraphicContext& gc, const CL_Point& dst);
    
    virtual void Update();
    int GetSideBlock() const;
    inline bool IsTile() const {
        return true;
    }
    void SetStackOrder(ushort order){
		m_stack_order = order;
	}
	
    short GetZOffset() const { return m_ZOffset; }	
	
	ushort GetStackOrder() const { return m_stack_order; }

protected:
    virtual bool handle_element(eElement element, Element * pElement);
    virtual void load_attributes(CL_DomNamedNodeMap attributes);
    virtual void load_finished();
    void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC);	

    CL_Sprite m_sprite;
    Tilemap* m_tilemap;

    ScriptElement *m_pCondition;
    ScriptElement *m_pScript;
    ushort m_ZOffset;
    ushort m_X;
    ushort m_Y;
	ushort m_stack_order;
	char   m_monster_region;

    ushort cFlags;
#ifdef SR2_EDITOR
    std::string m_sprite_name;
public:
	CL_DomElement CreateDomElement(CL_DomDocument& doc)const;
	Tile* clone() const;
    void SetTileMap(Tilemap* pTileMap) {
        m_tilemap = pTileMap;
        cFlags &= ~TIL_SPRITE;
    }
    void SetMonsterRegion(char monster_region){
		m_monster_region = monster_region;
	}
    void SetPos(ushort x, ushort y) {
        m_X = x;
        m_Y = y;
    }
    void SetFloater() {
        cFlags |= TIL_FLOATER;
    }
    void SetFlag(int flag) {
        cFlags |= flag;
    }
    void UnsetFlag(int flag) {
        cFlags &= (~flag);
    }
    void SetZOffset(short z){
		if(z >= 0) m_ZOffset = z;
	}
	ushort GetFlags() const {
		return cFlags;
	}
	bool HasCondition() const { 
		return m_pCondition != NULL;
	}
	ScriptElement * GetCondition() const {
		return m_pCondition;
	}
	ScriptElement * GetScript() const {
		return m_pScript;
	}
#endif
};

class LevelHeader : public Element
{
public:
    LevelHeader();
    virtual ~LevelHeader();
    uint GetLevelWidth() const {
        return m_nLevelWidth;
    }
    uint GetLevelHeight() const {
        return m_nLevelHeight;
    }
    std::string GetMusic() const {
        return m_music;
    }
    bool AllowsRunning() const {
        return m_bAllowsRunning;
    }
    eElement WhichElement() const {
        return ELEVELHEADER;
    }
    void ExecuteScript() const;
#if SR2_EDITOR
    CL_DomElement CreateDomElement(CL_DomDocument& doc)const;
#endif
private:
    virtual bool handle_element(eElement element, Element * pElement);
    virtual void load_attributes(CL_DomNamedNodeMap attributes);
    ScriptElement * m_pScript;

    uint m_nLevelWidth;
    uint m_nLevelHeight;
    std::string m_music;
    bool m_bAllowsRunning;
};

class Tiles : public Element
{
public:
    Tiles();
    virtual ~Tiles();

    std::list<Tile*>::const_iterator GetTilesBegin()const;
    std::list<Tile*>::const_iterator GetTilesEnd()const;

    eElement WhichElement() const {
        return ETILES;
    }
private:
    virtual bool handle_element(eElement element, Element * pElement);
    virtual void load_attributes(CL_DomNamedNodeMap attributes);
protected:
    std::list<Tile*> m_tiles;
};



class MappableObjects : public Element
{
public:
    MappableObjects();
    virtual ~MappableObjects();

    std::list<MappableObject*>::const_iterator GetMappableObjectsBegin() const;
    std::list<MappableObject*>::const_iterator GetMappableObjectsEnd() const;

    eElement WhichElement() const {
        return EMAPPABLEOBJECTS;
    }

private:
    virtual bool handle_element(eElement element, Element * pElement);
    virtual void load_attributes(CL_DomNamedNodeMap attributes);
protected:
    std::list<MappableObject*> m_mappable_objects;
};

class Level : public Element
{
public:
    Level();
    virtual ~Level();
    void Load(const std::string &name, CL_ResourceManager& resources);
    void Invoke(); // run invoke script if any
    eElement WhichElement() const {
        return ELEVEL;
    }

    virtual void Draw(const CL_Rect &src, const CL_Rect &dst,
                      CL_GraphicContext& GC , bool mappable_objects = true,  bool draw_borders=false, bool draw_debug=false);
    virtual void AddTileVisitor(Tile::Visitor * pVisitor);
    virtual void RemoveTileVisitor(Tile::Visitor * pVisitor);
    void MoveMappableObjects(const CL_Rect &src);

    void AddMappableObject(MappableObject* pMO);
    void RemoveMappableObject(MappableObject* pMO);

    bool CanMove(MappableObject* pMO, const CL_Rect& tiles_currently, const CL_Rect& tiles_destination)const;
    // Checks relevant tile and MO direction block information
    // And mark occupied and unoccupied if move is successful
    bool Move(MappableObject* pMO, const CL_Rect &tiles_currently, const CL_Rect& tiles_destination);

    // Any talk events fire (assuming they meet conditions)
    // "target" describes the region which the player is talking to.
    // In other words, if the player is facing west,the game engine will select a rectangle of a size it finds
    // appropriate which is directly west of the player.
    // The level will select the FIRST MO in the list an execute the talk on that MO.
    // In theory, because of the MO sorting, this will be the closest MO.

    // If prod = true, it makes the MO that you would normally talk to,
    // pick a random new direction (if it has movement).
    // This is intended as a way to get people that are blocking you
    // out of your way. You prod them until they head in some direction which helps you.
    // Or.. you know... you could just prod people for fun.
    virtual void Talk(const CL_Point &target,  bool prod=false);

    // Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
    virtual void Update(const CL_Rect & updateRect);

    uint GetWidth() const {
        return m_LevelWidth;
    }
    uint GetHeight() const {
        return m_LevelHeight;
    }
    bool AllowsRunning() const {
        return m_bAllowsRunning;
    }
    std::string GetName() const {
        return m_name;
    }
    std::string GetMusic() const {
        return m_music;
    }
    std::string GetResourceName() const {
        return m_resource_name;
    }
    
    MappablePlayer* GetPlayer() const;
    

    void MarkForDeath() {
        m_bMarkedForDeath = true;
    }

    void SerializeState(std::ostream& out);
    void DeserializeState(std::istream& in);
    void FreezeMappableObjects();
    void UnfreezeMappableObjects();
    MappableObject* GetMappableObjectByName(const std::string& name) const;
    void LoadFromFile(const std::string &path);
#ifndef NDEBUG
    void DumpMappableObjects() const;
    void DrawMOQuadtree(CL_GraphicContext gc, const CL_Point& offset) const;
    void DrawDebugBox(CL_GraphicContext gc, const CL_Rect& rect)const;
    void AddPathTile(const CL_Point& pt);
    void ClearPath();
#endif
    typedef Quadtree::RootNode<MappableObject*,4,float> MOQuadtree;
    typedef Quadtree::RootNode<Tile*,4,float> TileQuadtree;
protected:


    std::vector<std::vector<std::list<Tile*> > > m_tiles;

    MonsterRegions *m_pMonsterRegions;

    // Element virtuals
    virtual bool handle_element(eElement element, Element * pElement);
    virtual void load_attributes(CL_DomNamedNodeMap attributes);
    virtual void load_finished();
	
	void draw_floor_tiles(CL_GraphicContext& gc, const CL_Rect& src, const CL_Rect& dst);
	void draw_object_layer(CL_GraphicContext& gc, const CL_Rect& src, const CL_Rect& dst, bool draw_mos, bool draw_debug, bool draw_borders);


    // MO related operations
    bool Contains_Mappable_Objects(const CL_Point &point) const;
    bool Contains_Solid_Mappable_Object(const CL_Point &point) const;
    bool Check_Direction_Block(MappableObject* pMO, Direction dir,const CL_Point &tile, const CL_Point &dest_tile)const;
	MOQuadtree::Vector Translate_Point(const MOQuadtree::Vector& vec);

    void Move_Mappable_Object(MappableObject* pMO, Direction dir, const CL_Rect& from, const CL_Rect& to);
    void Add_Mappable_Object(MappableObject* pMO);

    // Sort tiles on zOrder
    static bool Tile_Sort_Criterion ( const Tile * p1, const Tile * p2 );
	
	// returns rect in tiles, not pixels
	CL_Rect calc_tile_bounds(const CL_Rect& src_pixels, const CL_Rect& dst_pixels) const;

    void Load_Tile ( Tile * tileElement );
    void Load_Mo ( MappableObject * moElement );

    // All AM's from tiles fire, as do any step events
    virtual void Step(const CL_Point &destination);

    // Tile related operations
    // Call attribute modifiers on tiles at this location
    void Activate_Tiles_At ( uint x, uint y );
    int Get_Cumulative_Side_Block_At_Point
(const CL_Point &point) const;
    bool Get_Cumulative_Hotness_At_Point(const CL_Point &point) const;
    void Create_MOQuadtree();

    CL_DomDocument  m_document;
    ScriptElement *m_pScript;
    LevelHeader *m_pHeader;
    std::string m_music;
    std::string m_name;
    uint m_LevelWidth;
    uint m_LevelHeight;
    bool m_bAllowsRunning;
    mutable uint m_nFrameCount;
    uint m_nMoveCount;
    bool m_bMarkedForDeath;
    MOQuadtree *m_mo_quadtree;
    std::string m_resource_name;
    std::set<Tile::Visitor*> m_tile_visitors;
#ifndef NDEBUG
    struct InteractPoint {
        CL_Point m_point;
        uint m_creationTime;
    };
    std::list<InteractPoint> m_interactPoints;
    std::set<CL_Point> m_pathPoints;
#endif
    //mutable CL_Mutex m_mo_mutex;
#ifdef SR2_EDITOR
public:
    Level(uint width, uint height);
    void GrowLevelTo(uint width, uint height);
    bool TilesAt(const CL_Point& loc)const;
    void AddTile(Tile * pTile);
	void AddTilesAt(const CL_Point& loc, const std::list<Tile*>& tiles, bool overwrite);
    Tile* PopTileAtPos(const CL_Point& loc);
    std::list<Tile*> GetTilesAt(const CL_Point& loc) const;
    std::list<MappableObject*> GetMappableObjectsAt(const CL_Point& loc)const;
	const MonsterRegions* GetMonsterRegions() const {return m_pMonsterRegions;}
	void RemoveMonsterRegion(MonsterRegion* pRegion);

	void AddMonsterRegion(MonsterRegion* region);
	void SetMusic(const std::string& music);
	void SetAllowsRunning(bool);
    void SetHotAt(uint levelX, uint levelY, bool bHot);
    bool WriteXML(const std::string& filename, bool force)const;
    void resize_mo_quadtree();
    CL_DomElement CreateDomElement(CL_DomDocument& doc) const;
#endif	
};



}

struct LessTile : public std::binary_function<const StoneRing::Tile*,
            const StoneRing::Tile*,bool>
{
    bool operator()(const StoneRing::Tile* n1, const StoneRing::Tile *n2) const
    {
        return n1->GetZOrder() < n2->GetZOrder();
    }
};




#endif





