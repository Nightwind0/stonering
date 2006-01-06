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
#include "Effect.h"

using std::string;




// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2);

namespace StoneRing {
    
        class WeaponRef;
        class ArmorRef;
        class NamedItemRef;

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
                        explicit DirectionBlock(int );
                        virtual ~DirectionBlock();

                        int getDirectionBlock() const;

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);

                        int meDirectionBlock;
                private:
		
                };


        // Things which take actions on the party
        class Action : public Element
                {
                public:
                        Action(){}
                        virtual ~Action(){}

                        virtual void invoke()=0;
                };


        // Things that evaluate by examining the party
        class Check : public Element
                {
                public:
                        Check(){}
                        virtual ~Check(){}

                        virtual bool evaluate()=0;
                };



    

        class Tilemap : public Element
                {
                public:
                        Tilemap();
                        virtual ~Tilemap();
      
                        inline ushort getMapX() const;
                        inline ushort getMapY() const;

                        inline CL_Surface * getTileMap() const;

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleElement(eElement element, Element * pElement );	
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        CL_Surface *mpSurface;
                        ushort mX;
                        ushort mY;
                };


        class SpriteRef : public Element
                {
                public:
                        SpriteRef();
                        virtual ~SpriteRef();

                        enum eType {SPR_NONE, SPR_STILL, SPR_TWO_WAY, SPR_FOUR_WAY };

                        eType getType() const;
                        std::string getRef() const;

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleElement(eElement element, Element * pElement );	
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        virtual void handleText(const std::string &text);
                        eType meType;
                        std::string mRef;
                };

        class Movement : public Element
                {
                public:
                        Movement();
                        virtual ~Movement();

                        enum eMovementType { MOVEMENT_NONE, MOVEMENT_WANDER, MOVEMENT_PACE_NS, MOVEMENT_PACE_EW };
                        enum eMovementSpeed { SLOW, MEDIUM, FAST };

                        eMovementType getMovementType() const;
                        eMovementSpeed getMovementSpeed() const;

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
	    
                protected:
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        eMovementType meType;
                        eMovementSpeed meSpeed;
                };

        class Condition;

        class AttributeModifier : public Action
                {
                public:
                        AttributeModifier();
                        virtual ~AttributeModifier();

                        enum eTarget { CURRENT, ALL, CASTER };
                        enum eChangeTo { ADD, TO_MIN, TO_MAX };

                        virtual void invoke();

                        bool applicable() const;

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;


                protected:
                        virtual void handleElement(eElement element, Element * pElement );	
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        std::list<Condition*> mConditions;
                        int mAdd;
                        std::string mAttribute;
                        eTarget  meTarget;
                        eChangeTo meChangeTo;
                };

        class HasGold : public Check
                {
                public:

                        enum eOperator{LT, GT, LTE, GTE, EQ};

                        HasGold();
                        virtual ~HasGold();

                        virtual bool evaluate();
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        virtual void handleText(const std::string &);
                        uint mAmount;
                        bool mbNot;
                        eOperator meOperator;
      
                };

        class HasItem : public Check
                {
                public:
                        HasItem();
                        virtual ~HasItem();

                        virtual bool evaluate();
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
      
                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        ItemRef * mpItemRef;
                        bool mbNot;
                        std::string mItem;
                        Item::eItemType mItemType;
                        uint mCount;
                };

        class DidEvent : public Check
                {
                public:
                        DidEvent();
                        virtual ~DidEvent();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
                        virtual bool evaluate();
                protected:
                        virtual void handleElement(eElement element, Element * pElement );	
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        virtual void handleText(const std::string &);
                        bool mbNot;
                        std::string mEvent;
      
                };

        class Operator;

        class And : public Check
                {
                public:
                        And();
                        virtual ~And();

                        virtual bool evaluate();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                        ushort order();

                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        ushort mOrder;
                        std::list<Check*> mOperands;
                };

        class Or : public Check
                {
                public:
                        Or();
                        virtual ~Or();

                        virtual bool evaluate();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                        ushort order();

                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        ushort mOrder;
                        std::list<Check*> mOperands;

                };

        class Operator : public Check
                {
                public:
                        Operator();
                        virtual ~Operator();

                        virtual bool evaluate();
                        ushort order();
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
      
                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        ushort mOrder;
                        std::list<Check*> mOperands;
                };


        class Condition : public Element
                {
                public:
                        Condition();
                        virtual ~Condition();

                        bool evaluate() const;

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        std::list<Check*> mChecks;
                };

        class Event : public Element
                {
                public:
                        Event();
                        virtual ~Event();

                        enum eTriggerType { STEP, TALK, ACT };

                        std::string getName() const;
                        eTriggerType getTriggerType();
                        bool repeatable();
                        inline bool remember();
                        bool invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

      
                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        std::string mName;
                        bool mbRepeatable;
                        bool mbRemember;
                        eTriggerType meTriggerType;
                        Condition *mpCondition;
                        std::list<Action*> mActions;

                };


        class PlayAnimation : public Action
                {
                public:
                        PlayAnimation();
                        virtual ~PlayAnimation();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleText(const std::string &);
                        std::string mAnimation;
                };

        class PlaySound : public Action
                {
                public:
                        PlaySound();
                        virtual ~PlaySound();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleText(const std::string &);
                        std::string mSound;
                };

        class LoadLevel : public Action
                {
                public:
                        LoadLevel();
                        virtual ~LoadLevel();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        std::string mName;
                        ushort mStartY;
                        ushort mStartX;
                };

        class StartBattle : public Action
                {
                public:
                        StartBattle();
                        virtual ~StartBattle();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
      
                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        std::string mMonster;
                        ushort mCount;
                        bool mbIsBoss;
                };

        class InvokeShop : public Action
                {
                public:
                        InvokeShop();
                        virtual ~InvokeShop();

                        virtual void invoke();
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        std::string mShopType;
                };


        class Pause : public Action
                {
                public:
                        Pause();
                        virtual ~Pause();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleText(const std::string &);
                        uint mMs;
                };

        class Say : public Action
                {
                public:
                        Say();
                        virtual ~Say();

                        virtual void invoke();
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        virtual void handleText(const std::string &);
                        std::string mSpeaker;
                        std::string mText;
                };

        class Give: public Action
                {
                public:
                        Give();
                        virtual ~Give();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleElement(eElement,Element*);
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        ItemRef *mpItemRef;
                        uint mCount;
                };

        class Take : public Action
                {
                public:
                        Take();
                        virtual ~Take();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        ItemRef *mpItemRef;
                        uint mCount;
                };

        class GiveGold : public Action
                {
                public:
                        GiveGold();
                        virtual ~GiveGold();

                        virtual void invoke();

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        uint mCount;
                };


        class Graphic : public Element
                {
                public:
                        Graphic();
                        virtual ~Graphic();

                        virtual uint getX() const=0;
                        virtual uint getY() const=0;

                        virtual CL_Rect getRect()=0;

                        virtual bool isSprite() const=0;

                        virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)=0;

                        virtual void update()=0;

                        virtual int getDirectionBlock() const=0;

                        virtual bool isTile() const=0;

                private:
                };


        class Option : public Element
                {
                public:
                        Option();
                        virtual ~Option();
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                        virtual std::string getText() const;
	
                        virtual bool evaluateCondition() const;

                        virtual void choose();
                protected:
                        virtual void handleElement(eElement element, Element * pElement );	
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                private:
                        std::string mText;
                        std::list<Action*> mActions;
                        Condition * mpCondition;
	
                };

        class Choice : public Action
                {
                public:
                        Choice();
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                        virtual void invoke();

                        virtual std::string getText() const;

                        virtual uint getOptionCount() const;

                        Option * getOption(uint index );

                        // To be called by application when choice is made.
                        void chooseOption( uint index);

                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        virtual void loadFinished();

                private:
                        std::vector<Option*> mOptions;
                        std::string mText;
                };


    

  
        union SpriteRefOrTilemap
        {
                SpriteRef* asSpriteRef;
                Tilemap * asTilemap;
        };


        class Tile : public Graphic
                {
                public:
                        Tile();
                        virtual ~Tile(); 

                        ushort getZOrder() const;

                        bool isFloater() const;

                        inline bool evaluateCondition() const;

                        inline bool hasAM() const;

                        inline void activate(); // Call any attributemodifier
	    
                        virtual inline  uint getX() const;
                        virtual inline uint getY() const;

                        virtual inline CL_Rect getRect();

                        virtual inline bool isSprite() const;

                        bool isHot() const;

                        virtual inline void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);


                        virtual void update();
	    
                        virtual inline int getDirectionBlock() const;

                        virtual inline bool isTile() const;

                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        enum eFlags { SPRITE = 1, FLOATER = 2, HOT = 4, BLK_NORTH = 8, BLK_SOUTH = 16, BLK_EAST = 32, BLK_WEST = 64};

                        CL_Sprite *mpSprite;
                        SpriteRefOrTilemap mGraphic;
                        ushort mZOrder;
                        Condition *mpCondition;
                        AttributeModifier *mpAM;
                        ushort mX;
                        ushort mY;

                        unsigned char cFlags;

                };

		//class 

        class MappableObject : public Graphic
                {
                public:
                        enum eMappableObjectType { NPC, SQUARE, CONTAINER, DOOR, WARP };
                        enum eSize { MO_SMALL, MO_MEDIUM, MO_LARGE, MO_TALL, MO_WIDE };
                        enum eDirection { NONE, NORTH, SOUTH, EAST, WEST };

                        MappableObject();
                        virtual ~MappableObject();

						virtual uint getX() const { return mX; }
						virtual uint getY() const { return mY; }

						virtual CL_Rect getRect() { return getPixelRect(); }
                        
                        CL_Point getStartPoint() const; // In cells
                        CL_Point getPosition() const { return CL_Point(mX / 32, mY / 32); }  // In cells
                        bool isSolid() const;
                        eSize getSize() const;
                        Movement * getMovement() const;
                        std::string getName() const;
                        bool isAligned() const; // Is aligned on cells (not moving between them)
                        virtual CL_Rect getPixelRect() const; // In pixels
                        virtual bool onScreen();
                        virtual bool isSprite() const;
                        virtual uint getCellHeight() const;
                        virtual uint getCellWidth() const;
						virtual uint getMovesPerDraw() const; // a factor of speed
                        
                        CL_Point getPositionAfterMove() const;

                        eDirection getDirection() const { return meDirection; }

                        virtual int getDirectionBlock() const;
                        virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);

                        // Move along in the current direction
                        virtual void move();

                        //Update is for drawing..
                        virtual void update();
                        virtual bool isTile() const;
                        virtual void provokeEvents ( Event::eTriggerType trigger );
                        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
                        inline bool evaluateCondition() const;

                        void prod();
                        
                        static void CalculateEdgePoints(const CL_Point &topleft, eDirection dir, eSize size, std::list<CL_Point> *pList);
                        static eDirection OppositeDirection(eDirection current_dir);
                        static int ConvertDirectionToDirectionBlock(eDirection dir);
                        virtual void randomNewDirection();

                        virtual void movedOneCell();

						uint getFrameMarks() const{return mnFrameMarks;}
						uint getMoveCount() const { return mnMoveCount;}

						void markFrame()  { ++mnFrameMarks; }
						void markMove() {++mnMoveCount; }
                protected:
                        virtual void handleElement(eElement element, Element * pElement );
                        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
                        virtual void loadFinished();
                        void pickOppositeDirection();
                        void setFrameForDirection();
                        static CL_Point calcCellDimensions(eSize size);
                        enum eFlags { SPRITE = 1, TILEMAP = 2, SOLID = 4 };

                        std::string mName;
                        CL_Sprite * mpSprite;
                        SpriteRefOrTilemap mGraphic;
                        std::list<Event*> mEvents;
			
                        eSize meSize;
                        eDirection meDirection;

                        bool mbStep; // step frame alternator
                        ushort mStartX;
                        ushort mStartY;
                        uint mX;
                        uint mY;
                        Movement *mpMovement;
                        Condition *mpCondition;
                        eMappableObjectType meType;
                        char cFlags;
                        ushort mnCellsMoved;
						uint mnFrameMarks;
						uint mnMoveCount;
                };
}
#ifdef _MSC_VER
using namespace StoneRing;

#undef abs

template <>
struct std::greater<MappableObject*>

{
        const bool operator()(const MappableObject* n1, const  MappableObject * n2) const
        {
	   
		
                IApplication * pApp = IApplication::getInstance();

                int pX = pApp->getLevelRect().left + pApp->getLevelRect().get_width() / 2;
                int pY = pApp->getLevelRect().top + pApp->getLevelRect().get_height() / 2;

                uint p1Distance, p2Distance;

	
                /*    p1Distance = max(abs( (long)pX - p1->getX()) , abs((long)pY - p1->getY()));
                      p2Distance = max(abs( (long)pX - p2->getX()) , abs((long)pY - p2->getY()));
                */
                int dx1 = abs((int)(pX - n1->getX()));
                int dy1 = abs((int)(pY - n1->getY()));
                int dx2 = abs((int)(pX - n2->getX()));
                int dy2 = abs((int)(pY - n2->getY()));

                p1Distance = (dx1 * dx1) + (dy1 * dy1);
                p2Distance = (dx2 * dx2) + (dy2 * dy2);

                return p1Distance < p2Distance;
		
	

        };
};


template<>
struct std::greater<Tile*>
{
        bool operator()(const Tile* n1, const Tile *n2) const
        {
                return n1->getZOrder() < n2->getZOrder();
        }
};


#endif

namespace StoneRing{
        class Level
                {
                public:
                        Level();
                        Level(const std::string &name,CL_ResourceManager * pResources);
                        Level(CL_DomDocument &document);
                        virtual ~Level();
      
                        void load ( CL_DomDocument &document);

                        virtual CL_DomElement createDomElement(CL_DomDocument&) const;

                        virtual void draw(const CL_Rect &src, const CL_Rect &dst,
                                          CL_GraphicContext * pGC , bool floaters = false,
                                          bool highlightHot=false,bool indicateBlocks = false);
                        virtual void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);
                        virtual void drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC);
		
                        void moveMappableObjects(const CL_Rect &src);
      
                        // Checks relevant tile and MO direction block information
                        // And mark occupied and unoccupied if move is successful
                        bool tryMove(const CL_Point &currently, const CL_Point & destination );

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
                        virtual void talk(const CL_Point &target,  bool prod=false);

                        // Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
                        virtual void update(const CL_Rect & updateRect);

                        uint getWidth() const { return mLevelWidth; }
                        uint getHeight() const { return mLevelHeight; }

                        bool allowsRunning() const { return mbAllowsRunning; }

                        std::string getName() const { return mName; }

#ifndef NDEBUG
                        void dumpMappableObjects() const;
#endif



                protected:

                        //			std::map<CL_Point, std::list<Tile*> > mTileMap;
                        //	    std::list<Tile*> ** mTileMap;

			typedef std::map<CL_Point,std::list<MappableObject*> > MappableObjectMap;
                        std::vector<std::vector<std::list<Tile*> > > mTileMap;
                        std::map<CL_Point, std::list<Tile*> > mFloaterMap;
                        MappableObjectMap mMOMap;

			// MO related operations
			bool containsMappableObjects(const CL_Point &point) const;
			bool containsSolidMappableObject(const CL_Point &point) const;
			void setMappableObjectAt(const CL_Point &point, MappableObject*  pMO);
			void removeMappableObjectFrom(const CL_Point &point, MappableObject *pMO);
			void putMappableObjectAtCurrentPosition(MappableObject *pMO);
			void removeMappableObjectFromCurrentPosition(MappableObject *pMO);
      
                        // Sort MO's in order to bring close ones to the top
                        static bool moSortCriterion( const MappableObject *p1, const MappableObject * p2);

                        // Sort tiles on zOrder
                        static bool tileSortCriterion ( const Tile * p1, const Tile * p2);

                        // Sort to bring the close ones near the top, or partition to bring nearby ones up.
//                        std::list<MappableObject *> mMappableObjects; 
                        void LoadLevel( const std::string &filename );
                        void LoadLevel ( CL_DomDocument &document );

                        void loadTile ( CL_DomElement * tileElement);
                        void loadMo ( CL_DomElement * moElement );

                        // All AM's from tiles fire, as do any step events
                        virtual void step(const CL_Point &destination);
      
			// Tile related operations
                        // Call attribute modifiers on tiles at this location
                        void activateTilesAt ( uint x, uint y );
                        int getCumulativeDirectionBlockAtPoint(const CL_Point &point) const;
                        bool getCumulativeHotnessAtPoint(const CL_Point &point) const;

                        CL_ResourceManager * mpResources;
                        CL_DomDocument * mpDocument;

                        std::string mMusic;
                        std::string mName;
                        uint mLevelWidth;
                        uint mLevelHeight;
                        bool mbAllowsRunning;
						mutable uint mnFrameCount;
						uint mnMoveCount;
			

                };
}

#endif
