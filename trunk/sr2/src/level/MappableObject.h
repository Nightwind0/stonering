
#ifndef MAPPABLE_OBJECT_H
#define MAPPABLE_OBJECT_H

#include "sr_defines.h"
#include "Graphic.h"
#include "Event.h"

namespace StoneRing {

    class Movement;
    class Level;

    class MappableObject : public Graphic
    {
    public:
        enum eMappableObjectType 
        { 
            NPC, SQUARE, CONTAINER, DOOR, WARP, PLAYER 
        };
        enum eSize 
        { 
            MO_SMALL, MO_MEDIUM, MO_LARGE, MO_TALL, MO_WIDE             
        };
        enum eDirection 
        { 
            NONE, NORTH, SOUTH, EAST, WEST             
        };

        MappableObject();
        virtual ~MappableObject();
        
        virtual eElement WhichElement() const{ return EMAPPABLEOBJECT; }

        virtual CL_Rect GetTileRect() const; 
        virtual void Draw(CL_GraphicContext& GC, const CL_Point& offset);
        virtual void Move(Level& level);
        virtual bool IsSolid() const;
        virtual eSize GetSize() const;
        virtual std::string GetName() const;
        virtual bool IsSprite() const;
        virtual bool IsTile() const;
        virtual int GetDirectionBlock() const;        
        // Update sprite, etc
        virtual void Update();
        virtual void ProvokeEvents ( Event::eTriggerType trigger );
        virtual bool Step() const { return false; }
        
        bool EvaluateCondition() const;
        void Prod();

        CL_Point GetPosition() const { return CL_Point(m_X / 32, m_Y / 32); }  // In tiles
        bool IsAligned() const; // Is aligned on cells (not moving between them)

        virtual bool RespectsHotness() const{ return true; }
        // Graphic api
        virtual int GetX() const { return m_X; }
        virtual int GetY() const { return m_Y; }
        // In pixels
        virtual CL_Rect GetRect() const ;
        static void CalculateEdgePoints(const CL_Point &topleft, eDirection dir, eSize size, std::list<CL_Point> *pList);        
        static CL_Vec2<int> DirectionToVector(eDirection dir);
        static int ConvertDirectionToDirectionBlock(MappableObject::eDirection dir);        
    protected:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        virtual void load_finished();

        void Pick_Opposite_Direction();
        virtual void Set_Frame_For_Direction();
        virtual bool Delete_Sprite() const { return true; }
        virtual void Random_New_Direction();
        uint get_moves_per_draw()const;
        // returns whether to keep moving
        bool single_move(Level& level);
        



        //  static eDirection OppositeDirection(eDirection current_dir);
        virtual void StopMovement();
        virtual void MovedOneCell();
        virtual void Idle(); // Wait while direction is none.
        static CL_Point calcTileDimensions(eSize size);
        enum eFlags { SPRITE = 1, TILEMAP = 2, SOLID = 4 };
        std::string m_name;
        CL_Sprite  m_sprite;
        SpriteRefOrTilemap m_graphic;
        std::list<Event*> m_events;

        eSize m_eSize;
        eDirection m_eDirection;
        eDirection m_eFacingDirection;
        
        uint m_nStep; // step frame alternator
        ushort m_StartX;
        ushort m_StartY;
        int m_X; // pixels
        int m_Y; // pixels
        uint m_nHeight; // in tiles
        uint m_nWidth; // in tiles
        Movement *m_pMovement;
        ScriptElement *m_pCondition;
        eMappableObjectType m_eType;
        char cFlags;
        ushort m_nTilesMoved;
        ushort m_nStepsUntilChange;
    };


    class MappablePlayer : public MappableObject
    {
    public:
        MappablePlayer(uint startX, uint startY);
        virtual ~MappablePlayer();
        virtual bool IsSolid() const { return true; }
        virtual bool IsSprite() const { return true; }
        virtual uint GetMovesPerDraw() const;
        CL_Point GetPointInFront() const;
        virtual bool IsTile() const { return false; }
        virtual void SetNextDirection(eDirection newDir);
	virtual void StopMovement();
        virtual void ClearNextDirection();
        virtual eDirection GetDirection();
        virtual void MovedOneCell();
        virtual void Idle();
        void SetSprite(CL_Sprite sprite) { m_sprite = sprite; }
        void SetRunning(bool running);
        virtual bool RespectsHotness()const{ return false; }
        virtual uint GetLevelX() const { return m_X;}
        virtual uint GetLevelY() const { return m_Y;}
        virtual void MatchFacingDirection(MappablePlayer * pOther) { m_eFacingDirection = pOther->m_eFacingDirection; }
        virtual void ResetLevelX(uint x) { m_X = x * 32;}
        virtual void ResetLevelY(uint y) { m_Y = y * 32;}
        virtual bool Step() const { return true; }
        void SerializeState(std::ostream& out);
        void DeserializeState(std::istream& in);
    private:
        virtual bool handle_element(eElement, Element* ){ return false;}
        virtual void load_attributes(CL_DomNamedNodeMap){}
        virtual void load_finished(){}
        virtual void set_frame_for_direction();
        virtual bool delete_sprite() const { return false; }
        virtual void Random_New_Direction();
        eDirection m_eNextDirection;
        bool m_bHasNextDirection;
        bool m_bRunning;
    };

}

#endif



