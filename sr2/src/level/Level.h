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
#include "MappableObject.h"

using std::string;


// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2);

namespace StoneRing {

	class WeaponRef;
	class ArmorRef;
	class NamedItemRef;
	class StartingEquipmentRef;
	class Condition;
	class Action;


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
		virtual eElement whichElement() const{ return EDIRECTIONBLOCK; }	
		int getDirectionBlock() const;

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);

		int meDirectionBlock;
	private:

	};


	class Tilemap : public Element
	{
	public:
		Tilemap();
		virtual ~Tilemap();
		virtual eElement whichElement() const{ return ETILEMAP; }	     
		ushort getMapX() const;
		ushort getMapY() const;

		CL_Surface * getTileMap() const;

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual bool handleElement(eElement element, Element * pElement );      
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
		virtual eElement whichElement() const{ return ESPRITEREF; }	
		enum eType {SPR_NONE, SPR_STILL, SPR_TWO_WAY, SPR_FOUR_WAY };

		eType getType() const;
		std::string getRef() const;

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual bool handleElement(eElement element, Element * pElement );      
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
		virtual eElement whichElement() const{ return EMOVEMENT; }	
		enum eMovementType { MOVEMENT_NONE, MOVEMENT_WANDER, MOVEMENT_PACE_NS, MOVEMENT_PACE_EW };
		enum eMovementSpeed { SLOW, MEDIUM, FAST };

		virtual eMovementType getMovementType() const;
		virtual eMovementSpeed getMovementSpeed() const;

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		eMovementType meType;
		eMovementSpeed meSpeed;
	};

	class PlayerMovement : public Movement
	{
	public:
		PlayerMovement(){}
		~PlayerMovement(){}

		virtual eMovementType getMovementType() const { return MOVEMENT_WANDER; }
		virtual eMovementSpeed getMovementSpeed() const { return MEDIUM; }
	private:
	};

	class Tile : public Graphic
	{
	public:
		Tile();
		virtual ~Tile(); 
		virtual eElement whichElement() const{ return ETILE; }	
		ushort getZOrder() const;

		bool isFloater() const;

		bool evaluateCondition() const;

		bool hasAM() const;

		void activate(); // Call any attributemodifier

		virtual uint getX() const;
		virtual uint getY() const;

		virtual CL_Rect getRect();

		virtual bool isSprite() const;

		bool isHot() const;

		bool pops() const;

		virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);
		virtual void update();
		virtual int getDirectionBlock() const;
		virtual bool isTile() const;
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		enum eFlags { SPRITE = 1, FLOATER = 2, HOT = 4, BLK_NORTH = 8, BLK_SOUTH = 16, BLK_EAST = 32, BLK_WEST = 64, POPS = 128};

		CL_Sprite *mpSprite;
		SpriteRefOrTilemap mGraphic;
		ushort mZOrder;
		Condition *mpCondition;
		AttributeModifier *mpAM;
		ushort mX;
		ushort mY;

		unsigned char cFlags;

	};


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

		void addPlayer(MappablePlayer * pPlayer);

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

		MappablePlayer * getPlayer()const { return mpPlayer; }

		void markForDeath() { mbMarkedForDeath = true; }


#ifndef NDEBUG
		void dumpMappableObjects() const;
#endif



	protected:


		typedef MOMap::value_type MOMapValueType;
		std::vector<std::vector<std::list<Tile*> > > mTileMap;
		// Needs to be a multimap
		std::map<CL_Point, std::list<Tile*> > mFloaterMap;
		MOMap mMOMap;

		// MO related operations
		bool containsMappableObjects(const CL_Point &point) const;
		bool containsSolidMappableObject(const CL_Point &point) const;
		void setMappableObjectAt(const CL_Point &point, MappableObject*  pMO);
		void putMappableObjectAtCurrentPosition(MappableObject *pMO);
		void removeMappableObjectAt(const CL_Point &point, MappableObject * pMO);

		// Sort tiles on zOrder
		static bool tileSortCriterion ( const Tile * p1, const Tile * p2);

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
		MappablePlayer * mpPlayer;
		bool mbMarkedForDeath;

	};



};

struct LessTile : public std::binary_function<const StoneRing::Tile*,
	const StoneRing::Tile*,bool>
{
	bool operator()(const StoneRing::Tile* n1, const StoneRing::Tile *n2) const
	{
		return n1->getZOrder() < n2->getZOrder();
	}
};




#endif



