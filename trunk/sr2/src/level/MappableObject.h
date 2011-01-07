
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
        enum eMappableObjectType { NPC, SQUARE, CONTAINER, DOOR, WARP, PLAYER };
        enum eSize { MO_SMALL, MO_MEDIUM, MO_LARGE, MO_TALL, MO_WIDE };
        enum eDirection { NONE, NORTH, SOUTH, EAST, WEST };

        MappableObject();
        virtual ~MappableObject();
        virtual eElement WhichElement() const{ return EMAPPABLEOBJECT; }
        virtual uint GetX() const { return m_X; }
        virtual uint GetY() const { return m_Y; }

        virtual CL_Rect GetRect() { return GetPixelRect(); }

        CL_Point GetStartPoint() const; // In cells
        CL_Point GetPosition() const { return CL_Point(m_X / 32, m_Y / 32); }  // In cells

        virtual bool IsSolid() const;
        virtual eSize GetSize() const;
        Movement * GetMovement() const;
        std::string GetName() const;
        bool IsAligned() const; // Is aligned on cells (not moving between them)
        virtual CL_Rect GetPixelRect() const; // In pixels
        virtual bool IsSprite() const;
        virtual uint GetCellHeight() const;
        virtual uint GetCellWidth() const;
        virtual uint GetMovesPerDraw() const; // a factor of speed
        virtual bool RespectsHotness() const{ return true; }

        typedef void (Level::*LevelPointMethod)(const CL_Point&,MappableObject*);

        void SetOccupiedPoints(Level * pLevel, LevelPointMethod method);
        CL_Point GetPositionAfterMove() const;
        virtual eDirection GetDirection() const { return m_eDirection; }
        virtual int GetDirectionBlock() const;
        virtual void Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC);

        // Move along in the current direction
        virtual void Move();

        //Update is for drawing..
        virtual void Update();
        virtual bool IsTile() const;
        virtual void ProvokeEvents ( Event::eTriggerType trigger );
        bool EvaluateCondition() const;

        void Prod();
        virtual bool Step() const { return false; }

        static void CalculateEdgePoints(const CL_Point &topleft, eDirection dir, eSize size, std::list<CL_Point> *pList);
        //  static eDirection OppositeDirection(eDirection current_dir);
        static int ConvertDirectionToDirectionBlock(eDirection dir);
        virtual void RandomNewDirection();
        virtual void MovedOneCell();
        virtual void Idle(); // Wait while direction is none.

        uint GetFrameMarks() const{return m_nFrameMarks;}
        void MarkFrame()  { ++m_nFrameMarks; }
    protected:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        virtual void load_finished();
        void Pick_Opposite_Direction();
        virtual void Set_Frame_For_Direction();
        virtual bool Delete_Sprite() const { return true; }
        static CL_Point calcCellDimensions(eSize size);
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
        uint m_X;
        uint m_Y;
        Movement *m_pMovement;
        ScriptElement *m_pCondition;
        eMappableObjectType m_eType;
        char cFlags;
        ushort m_nCellsMoved;
        uint m_nFrameMarks;
        ushort m_nStepsUntilChange;
    };


    typedef std::multimap<CL_Point,MappableObject*> MOMap;
    typedef MOMap::iterator MOMapIter;


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
        virtual void RandomNewDirection();
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
    private:
        virtual bool handle_element(eElement, Element* ){ return false;}
        virtual void load_attributes(CL_DomNamedNodeMap){}
        virtual void load_finished(){}
        virtual void set_frame_for_direction();
        virtual bool delete_sprite() const { return false; }



        eDirection m_eNextDirection;
        bool m_bHasNextDirection;
        bool m_bRunning;
    };



    struct LessMOMapIter : public std::binary_function<const MOMapIter&,const MOMapIter&,bool>
    {
        bool operator()(const MOMapIter &i1, const MOMapIter &i2)
            {
                return i1->second < i2->second;
            }
    };
}

#endif



