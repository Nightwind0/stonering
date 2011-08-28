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

using std::string;


// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2);

namespace StoneRing {

    class WeaponRef;
    class ArmorRef;
    class NamedItemRef;
    class StartingEquipmentRef;
    class ScriptElement;
    class Action;
    class MonsterRegions;

    enum eDirectionBlock
    {
        DIR_NORTH = 1,
        DIR_SOUTH = 2,
        DIR_WEST = 4,
        DIR_EAST = 8
    };

    class DirectionBlock : public Element
    {
    public:
        DirectionBlock();
        explicit DirectionBlock(int);
        virtual ~DirectionBlock();
        virtual eElement WhichElement() const{ return EDIRECTIONBLOCK; }
        int GetDirectionBlock() const;

    protected:
        virtual void load_attributes(CL_DomNamedNodeMap attributes);

        int m_eDirectionBlock;
    private:

    };


    class Tilemap : public Element
    {
    public:
        Tilemap();
        virtual ~Tilemap();
        virtual eElement WhichElement() const{ return ETILEMAP; }
        inline ushort GetMapX() const { return m_X; }
        inline ushort GetMapY() const { return m_Y; }

        inline CL_Sprite  GetTileMap() const { return m_sprite; }

    protected:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        CL_Sprite m_sprite;
        ushort m_X;
        ushort m_Y;
    };

    class Movement : public Element
    {
    public:
        Movement();
        virtual ~Movement();
        virtual eElement WhichElement() const{ return EMOVEMENT; }
        enum eMovementType { MOVEMENT_NONE, MOVEMENT_WANDER, MOVEMENT_PACE_NS, MOVEMENT_PACE_EW, MOVEMENT_SCRIPT};
        enum eMovementSpeed { SLOW, MEDIUM, FAST };

        virtual eMovementType GetMovementType() const;
        virtual eMovementSpeed GetMovementSpeed() const;

    protected:
        virtual bool handle_element(eElement element,Element *pElement);
        virtual void load_attributes(CL_DomNamedNodeMap  attributes);
        eMovementType m_eType;
        eMovementSpeed m_eSpeed;
        ScriptElement* m_pScript;
    };

    class PlayerMovement : public Movement
    {
    public:
        PlayerMovement(){}
        ~PlayerMovement(){}

        virtual eMovementType GetMovementType() const { return MOVEMENT_WANDER; }
        virtual eMovementSpeed GetMovementSpeed() const { return MEDIUM; }
    private:
    };

    class Tile : public Graphic
    {
    protected:
        enum eFlags { SPRITE = 1, FLOATER = 2, HOT = 4, BLK_NORTH = 8, BLK_SOUTH = 16, BLK_EAST = 32, BLK_WEST = 64, POPS = 128};
    public:
        Tile();
        virtual ~Tile();
        virtual eElement WhichElement() const{ return ETILE; }
        inline ushort GetZOrder() const { return m_ZOrder; }

        inline bool IsFloater() const { return (cFlags & FLOATER) != 0; }
        bool EvaluateCondition() const;
        inline bool HasScript() const { return m_pScript != NULL; }

        void Activate(); // Call any attributemodifier

        inline uint GetX() const { return m_X; }
        inline uint GetY() const { return m_Y; }

        CL_Rect GetRect();

        inline bool IsSprite() const { return (cFlags & SPRITE) != 0; }

        inline bool IsHot() const { return (cFlags & HOT) != 0; }

        inline bool Pops() const { return (cFlags & POPS) != 0; }

        void Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC);
        virtual void Update();
        int GetDirectionBlock() const;
        inline bool IsTile() const { return true; }

    protected:
        virtual bool handle_element(eElement element, Element * pElement);
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        virtual void load_finished();

        CL_Sprite m_sprite;
        SpriteRefOrTilemap m_Graphic;

        ScriptElement *m_pCondition;
        ScriptElement *m_pScript;
        ushort m_ZOrder;
        ushort m_X;
        ushort m_Y;

        unsigned char cFlags;

    };

    class LevelHeader : public Element
    {
    public:
        LevelHeader();
        virtual ~LevelHeader();
        uint GetLevelWidth() const { return m_nLevelWidth; }
        uint GetLevelHeight() const { return m_nLevelHeight; }
        std::string GetMusic() const { return m_music; }
        bool AllowsRunning() const { return m_bAllowsRunning; }
        eElement WhichElement() const { return ELEVELHEADER; }
        void ExecuteScript() const;
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

        eElement WhichElement() const { return ETILES; }
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

        eElement WhichElement() const { return EMAPPABLEOBJECTS; }

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
        void LoadFromFile(const std::string &filename);
        void Load(const std::string &name, CL_ResourceManager& resources);
        void Invoke(); // run invoke script if any
        eElement WhichElement() const { return ELEVEL; }

        virtual void Draw(const CL_Rect &src, const CL_Rect &dst,
                          CL_GraphicContext& GC , bool floaters = false,
                          bool highlightHot=false,bool indicateBlocks = false);
        virtual void DrawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC);
        virtual void DrawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC);

        void MoveMappableObjects(const CL_Rect &src);

        // Checks relevant tile and MO direction block information
        // And mark occupied and unoccupied if move is successful
        bool TryMove(const CL_Point &currently, const CL_Point & destination );

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

        uint GetWidth() const { return m_LevelWidth; }
        uint GetHeight() const { return m_LevelHeight; }
        bool AllowsRunning() const { return m_bAllowsRunning; }
        std::string GetName() const { return m_name; }

        // Player interfaces
        MappablePlayer * GetPlayer() { return &m_player; }
        void SetPlayerPos(const CL_Point &target);

        void MarkForDeath() { m_bMarkedForDeath = true; }

        void SerializeState(std::ostream& out);
        void DeserializeState(std::istream& in);
#ifndef NDEBUG
        void DumpMappableObjects() const;
#endif
    protected:

        typedef MOMap::value_type MOMapValueType;
        std::vector<std::vector<std::list<Tile*> > > m_tiles;
        // Needs to be a multimap
        std::map<CL_Point, std::list<Tile*> > m_floater_map;
        MOMap m_MO_map;
        MonsterRegions *m_pMonsterRegions;

        // Element virtuals
        virtual bool handle_element(eElement element, Element * pElement);
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        virtual void load_finished();

        // MO related operations
        bool Contains_Mappable_Objects(const CL_Point &point) const;
        bool Contains_Solid_Mappable_Object(const CL_Point &point) const;
        void Set_Mappable_Object_At(const CL_Point &point, MappableObject*  pMO);
        void Put_Mappable_Object_At_Current_Position(MappableObject *pMO);
        void Remove_Mappable_Object_At(const CL_Point &point, MappableObject * pMO);

        // Sort tiles on zOrder
        static bool Tile_Sort_Criterion ( const Tile * p1, const Tile * p2 );

        void Load_Tile ( Tile * tileElement );
        void Load_Mo ( MappableObject * moElement );

        // All AM's from tiles fire, as do any step events
        virtual void Step(const CL_Point &destination);

        // Tile related operations
        // Call attribute modifiers on tiles at this location
        void Activate_Tiles_At ( uint x, uint y );
        int Get_Cumulative_Direction_Block_At_Point(const CL_Point &point) const;
        bool Get_Cumulative_Hotness_At_Point(const CL_Point &point) const;

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
        MappablePlayer m_player;
        bool m_bMarkedForDeath;

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





