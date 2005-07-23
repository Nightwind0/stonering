#ifndef SR_LEVEL_H
#define SR_LEVEL_H

#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "IApplication.h"
#include <set>
#include "Item.h"
#include "ItemRef.h"
#include "sr_defines.h"

using std::string;



#ifdef _MSC_VER
template <class T>
T abs( const T& a)
{
    if( a < 0) return -a;
    else return a;
}

#endif


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
	    DirectionBlock(CL_DomElement *pElement );
	    virtual ~DirectionBlock();

	    int getDirectionBlock() const;

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    int meDirectionBlock;
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
	    Tilemap(CL_DomElement *pElement);
	    virtual ~Tilemap();
      
	    inline ushort getMapX() const;
	    inline ushort getMapY() const;

	    inline CL_Surface * getTileMap() const;

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    CL_Surface *mpSurface;
	    ushort mX;
	    ushort mY;
	};


    class SpriteRef : public Element
	{
	public:
	    SpriteRef();
	    SpriteRef( CL_DomElement *pElement);
	    virtual ~SpriteRef();

	    enum eType {SPR_NONE, SPR_STILL, SPR_TWO_WAY, SPR_FOUR_WAY };

	    eType getType() const;
	    std::string getRef() const;

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    eType meType;
	    std::string mRef;
	};

    class Movement : public Element
	{
	public:
	    Movement();
	    Movement ( CL_DomElement * pElement );
	    virtual ~Movement();

	    enum eMovementType { MOVEMENT_NONE, MOVEMENT_WANDER, MOVEMENT_PACE_NS, MOVEMENT_PACE_EW };
	    enum eMovementSpeed { SLOW, FAST };

	    eMovementType getMovementType() const;
	    eMovementSpeed getMovementSpeed() const;

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
	    
	protected:
	    eMovementType meType;
	    eMovementSpeed meSpeed;
	};

    class Condition;

    class AttributeModifier : public Action
	{
	public:
	    AttributeModifier();
	    AttributeModifier (CL_DomElement *pElement);
	    virtual ~AttributeModifier();

	    enum eTarget { CURRENT, ALL };
	    enum eChangeTo { ADD, TO_MIN, TO_MAX };

	    virtual void invoke();

	    bool applicable() const;

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;


	protected:
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
	    HasGold( CL_DomElement *pElement);
	    virtual ~HasGold();

	    virtual bool evaluate();
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    uint mAmount;
	    bool mbNot;
	    eOperator meOperator;
      
	};

    class HasItem : public Check
	{
	public:
	    HasItem();
	    HasItem(CL_DomElement *pElement );
	    virtual ~HasItem();

	    virtual bool evaluate();
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
      
	protected:
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
	    DidEvent(CL_DomElement *pElement);
	    virtual ~DidEvent();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
	    virtual bool evaluate();
	protected:
	    bool mbNot;
	    std::string mEvent;
      
	};

    class Operator;

    class And : public Check
	{
	public:
	    And();
	    And(CL_DomElement * pElement);
	    virtual ~And();

	    virtual bool evaluate();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    ushort order();

	protected:
	    ushort mOrder;
	    std::list<Check*> mOperands;
	};

    class Or : public Check
	{
	public:
	    Or();
	    Or(CL_DomElement * pElement);
	    virtual ~Or();

	    virtual bool evaluate();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    ushort order();

	protected:
	    ushort mOrder;
	    std::list<Check*> mOperands;

	};

    class Operator : public Check
	{
	public:
	    Operator();
	    Operator(CL_DomElement *pElement);
	    virtual ~Operator();

	    virtual bool evaluate();
	    ushort order();
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
      
	protected:
	    ushort mOrder;
	    std::list<Check*> mOperands;
	};


    class Condition : public Element
	{
	public:
	    Condition();
	    Condition(CL_DomElement *pElement);
	    virtual ~Condition();

	    bool evaluate() const;

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    std::list<Check*> mChecks;
	};

    class Event : public Element
	{
	public:
	    Event();
	    Event(CL_DomElement *pElement);
	    virtual ~Event();

	    enum eTriggerType { STEP, TALK, ACT };

	    std::string getName() const;
	    eTriggerType getTriggerType();
	    bool repeatable();
	    inline bool remember();
	    bool invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

      
	protected:
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
	    PlayAnimation(CL_DomElement * pElement );
	    virtual ~PlayAnimation();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    std::string mAnimation;
	};

    class PlaySound : public Action
	{
	public:
	    PlaySound();
	    PlaySound(CL_DomElement *pElement );
	    virtual ~PlaySound();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    std::string mSound;
	};

    class LoadLevel : public Action
	{
	public:
	    LoadLevel();
	    LoadLevel(CL_DomElement *pElement);
	    virtual ~LoadLevel();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    std::string mName;
	    ushort mStartY;
	    ushort mStartX;
	};

    class StartBattle : public Action
	{
	public:
	    StartBattle();
	    StartBattle(CL_DomElement *pElement);
	    virtual ~StartBattle();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
      
	protected:
	    std::string mMonster;
	    ushort mCount;
	    bool mbIsBoss;
	};

    class InvokeShop : public Action
	{
	public:
	    InvokeShop();
	    InvokeShop(CL_DomElement *pElement);
	    virtual ~InvokeShop();

	    virtual void invoke();
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    std::string mShopType;
	};


    class Pause : public Action
	{
	public:
	    Pause();
	    Pause(CL_DomElement *pElement );
	    virtual ~Pause();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    uint mMs;
	};

    class Say : public Action
	{
	public:
	    Say();
	    Say (CL_DomElement *pElement );
	    virtual ~Say();

	    virtual void invoke();
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    std::string mSpeaker;
	    std::string mText;
	};

    class Give: public Action
	{
	public:
	    Give();
	    Give(CL_DomElement *pElement );
	    virtual ~Give();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    ItemRef *mpItemRef;
	    uint mCount;
	};

    class Take : public Action
	{
	public:
	    Take();
	    Take(CL_DomElement *pElement );
	    virtual ~Take();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    ItemRef *mpItemRef;
	    uint mCount;
	};

    class GiveGold : public Action
	{
	public:
	    GiveGold();
	    GiveGold( CL_DomElement *pElement );
	    virtual ~GiveGold();

	    virtual void invoke();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
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

	    virtual void update(bool bMove)=0;

	    virtual int getDirectionBlock() const=0;

	    virtual bool isTile() const=0;

	private:
	};


    class Option : public Element
    {
    public:
	Option();
	Option(CL_DomElement * pElement);
	virtual ~Option();
	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	virtual std::string getText() const;
	
	virtual bool evaluateCondition() const;

	virtual void choose();
    private:
	std::string mText;
	std::list<Action*> mActions;
	Condition * mpCondition;
	
    };

    class Choice : public Action
    {
    public:
	Choice();
	Choice(CL_DomElement * pElement);
	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	virtual void invoke();

	virtual std::string getText() const;

	virtual uint getOptionCount() const;

	Option * getOption(uint index );

	// To be called by application when choice is made.
	void chooseOption( uint index);

    private:
	std::vector<Option*> mOptions;
	std::string mText;
    };


    Action * createAction ( const std::string & action, CL_DomElement & pChild );

  
    union SpriteRefOrTilemap
    {
	SpriteRef* asSpriteRef;
	Tilemap * asTilemap;
    };


    class Tile : public Graphic
	{
	public:


	    Tile();
	    Tile(CL_DomElement *pElement);
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


	    virtual void update(bool bMove);
	    
	    virtual inline int getDirectionBlock() const;

	    virtual inline bool isTile() const;

	    

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:

	    enum eFlags { SPRITE = 1, FLOATER = 2, HOT = 4, BLK_NORTH = 8, BLK_SOUTH = 16, BLK_EAST = 32, BLK_WEST = 64 };

	    CL_Sprite *mpSprite;
	    SpriteRefOrTilemap mGraphic;
	    ushort mZOrder;
	    Condition *mpCondition;
	    AttributeModifier *mpAM;
	    ushort mX;
	    ushort mY;

	    unsigned char cFlags;

	};

    class MappableObject : public Graphic
	{
	public:
	    MappableObject();
	    MappableObject(CL_DomElement *pElement);
	    virtual ~MappableObject();

	    ushort getStartX() const;
	    ushort getStartY() const;


	    bool isSolid() const;

	    enum eMappableObjectType { NPC, SQUARE, CONTAINER, DOOR, WARP };
	    enum eSize { MO_SMALL, MO_MEDIUM, MO_LARGE, MO_TALL, MO_WIDE };

	    eSize getSize() const;

	    Movement * getMovement() const;

	    std::string getName() const;
	    virtual uint getX() const;
	    virtual uint getY() const;

	    virtual CL_Rect getRect();

	    virtual bool isSprite() const;

	    virtual int getDirectionBlock() const;


	    virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);

	    virtual void update(bool bMove);


	    virtual bool isTile() const;

	    void provokeEvents ( Event::eTriggerType trigger );

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    inline bool evaluateCondition() const;

	    enum eDirection { NONE, NORTH, SOUTH, EAST, WEST };

	    void prod();

	protected:

	    void randomNewDirection();
	    void pickOppositeDirection();
	    bool moveInCurrentDirection();
	    void setFrameForDirection();
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
	    long mTimeOfLastUpdate;
	    ushort mCountInCurDirection;
	    Movement *mpMovement;
	    Condition *mpCondition;
	    eMappableObjectType meType;

	    char cFlags;
      
	};
}
#ifdef _MSC_VER
using namespace StoneRing;
template<>
struct std::greater<MappableObject*>  : public binary_function<MappableObject* ,MappableObject*, bool>
{
    bool operator()(const MappableObject* &n1, const  MappableObject * &n2) const
	{
	    IApplication * pApp = IApplication::getInstance();

	    uint pX = pApp->getLevelRect().get_width() / 2;
	    uint pY = pApp->getLevelRect().get_height() / 2;

	
	    uint p1Distance, p2Distance;

	
	    p1Distance = max(abs( (long)pX - n1->getX()) , abs((long)pY - n1->getY()));
	    p2Distance = max(abs( (long)pX - n2->getX()) , abs((long)pY - n2->getY()));

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
	    virtual void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC, bool bMove = true);
	    virtual void drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC);
		
      
      
	    // Checks relevant tile and MO direction block information
	    bool canMove(const CL_Rect &currently, const CL_Rect & destination, bool noHot = false, bool isPlayer = false) const; 

	    // All AM's from tiles fire, as do any step events
	    virtual void step(const CL_Rect &dest, const CL_Rect &old);
      
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
	    virtual void talk(const CL_Rect &target, bool prod=false);

	    // Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
	    virtual void update(const CL_Rect & updateRect);

	    uint getWidth() const { return mLevelWidth; }
	    uint getHeight() const { return mLevelHeight; }

	    bool allowsRunning() const { return mbAllowsRunning; }

	    std::string getName() const { return mName; }
	protected:

//			std::map<CL_Point, std::list<Tile*> > mTileMap;
//	    std::list<Tile*> ** mTileMap;
	    std::vector<std::vector<std::list<Tile*> > > mTileMap;
	    std::map<CL_Point, std::list<Tile*> > mFloaterMap;
      
	    // Sort MO's in order to bring close ones to the top
	    static bool moSortCriterion( const MappableObject *p1, const MappableObject * p2);

	    // Sort tiles on zOrder
	    static bool tileSortCriterion ( const Tile * p1, const Tile * p2);

	    // Sort to bring the close ones near the top, or partition to bring nearby ones up.
	    std::list<MappableObject *> mMappableObjects; 
	    void LoadLevel( const std::string &filename );
	    void LoadLevel ( CL_DomDocument &document );

	    void loadTile ( CL_DomElement * tileElement);
	    void loadMo ( CL_DomElement * moElement );

	    // Call attribute modifiers on tiles at this location
	    void activateTilesAt ( uint x, uint y );

	    CL_ResourceManager * mpResources;
	    CL_DomDocument * mpDocument;

	    std::string mMusic;
	    std::string mName;
	    uint mLevelWidth;
	    uint mLevelHeight;
	    bool mbAllowsRunning;

	    std::multiset<CL_Point> mLastSteppedTiles;
	};
}

#endif
