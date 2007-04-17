
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
        virtual eElement whichElement() const{ return EMAPPABLEOBJECT; }    
        virtual uint getX() const { return mX; }
        virtual uint getY() const { return mY; }

        virtual CL_Rect getRect() { return getPixelRect(); }

        CL_Point getStartPoint() const; // In cells
        CL_Point getPosition() const { return CL_Point(mX / 32, mY / 32); }  // In cells

        virtual bool isSolid() const;
        virtual eSize getSize() const;
        Movement * getMovement() const;
        std::string getName() const;
        bool isAligned() const; // Is aligned on cells (not moving between them)
        virtual CL_Rect getPixelRect() const; // In pixels
        virtual bool isSprite() const;
        virtual uint getCellHeight() const;
        virtual uint getCellWidth() const;
        virtual uint getMovesPerDraw() const; // a factor of speed
        virtual bool respectsHotness() const{ return true; }

        typedef void (Level::*LevelPointMethod)(const CL_Point&,MappableObject*);

        void setOccupiedPoints(Level * pLevel, LevelPointMethod method);
        CL_Point getPositionAfterMove() const;
        virtual eDirection getDirection() const { return meDirection; }
        virtual int getDirectionBlock() const;
        virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);

        // Move along in the current direction
        virtual void move();

        //Update is for drawing..
        virtual void update();
        virtual bool isTile() const;
        virtual void provokeEvents ( Event::eTriggerType trigger );
        bool evaluateCondition() const;

        void prod();
        virtual bool step() const { return false; }

        static void CalculateEdgePoints(const CL_Point &topleft, eDirection dir, eSize size, std::list<CL_Point> *pList);
        //  static eDirection OppositeDirection(eDirection current_dir);
        static int ConvertDirectionToDirectionBlock(eDirection dir);
        virtual void randomNewDirection();
        virtual void movedOneCell();
        virtual void idle(){} // Wait while direction is none.

        uint getFrameMarks() const{return mnFrameMarks;}
        void markFrame()  { ++mnFrameMarks; }
    protected:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        virtual void loadFinished();
        void pickOppositeDirection();
        virtual void setFrameForDirection();
        virtual bool deleteSprite() const { return true; }
        static CL_Point calcCellDimensions(eSize size);
        enum eFlags { SPRITE = 1, TILEMAP = 2, SOLID = 4 };

        std::string mName;
        CL_Sprite * mpSprite;
        SpriteRefOrTilemap mGraphic;
        std::list<Event*> mEvents;

        eSize meSize;
        eDirection meDirection;
        eDirection meFacingDirection;


        bool mbStep; // step frame alternator
        ushort mStartX;
        ushort mStartY;
        uint mX;
        uint mY;
        Movement *mpMovement;
        ScriptElement *mpCondition;
        eMappableObjectType meType;
        char cFlags;
        ushort mnCellsMoved;
        uint mnFrameMarks;
        ushort mnStepsUntilChange;
    };


    typedef std::multimap<CL_Point,MappableObject*> MOMap;
    typedef MOMap::iterator MOMapIter;


    class MappablePlayer : public MappableObject
    {
    public:
        MappablePlayer(uint startX, uint startY);
        virtual ~MappablePlayer();
        virtual bool isSolid() const { return true; }
        virtual bool isSprite() const { return true; }
        virtual uint getMovesPerDraw() const;
        CL_Point getPointInFront() const;
        virtual bool isTile() const { return false; }
        virtual void setNextDirection(eDirection newDir);
        virtual void clearNextDirection() { mbHasNextDirection = false; meDirection = NONE; }
        virtual void randomNewDirection();
        virtual void movedOneCell();
        virtual void idle();
        void setSprite(CL_Sprite *pSprite) { mpSprite = pSprite; }
        void setRunning(bool running);
        virtual bool respectsHotness()const{ return false; }
        virtual uint getLevelX() const { return mX;}
        virtual uint getLevelY() const { return mY;}
        virtual void matchFacingDirection(MappablePlayer * pOther) { meFacingDirection = pOther->meFacingDirection; }
        virtual void resetLevelX(uint x) { mX = x * 32;}
        virtual void resetLevelY(uint y) { mY = y * 32;}
    private:
        virtual bool handleElement(eElement element, Element * pElement ){ return false;}
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes){}
        virtual void loadFinished(){}
        virtual void setFrameForDirection();
        virtual bool deleteSprite() const { return false; }

        virtual bool step() const { return true; }


        eDirection meNextDirection;
        bool mbHasNextDirection;
        bool mbRunning;
    };



    struct LessMOMapIter : public std::binary_function<const MOMapIter&,const MOMapIter&,bool>
    {
        bool operator()(const MOMapIter &i1, const MOMapIter &i2)
            {
                return i1->second < i2->second;
            }
    };
};

#endif



