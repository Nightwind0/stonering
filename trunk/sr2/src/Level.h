#ifndef SR_LEVEL_H
#define SR_LEVEL_H

#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "Application.h"

using std::string;

typedef unsigned int uint;
typedef unsigned short ushort;




// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2);

namespace StoneRing {

	enum eDirectionBlock
		{
			DIR_NORTH = 1,
			DIR_SOUTH = 2,
			DIR_WEST = 4,
			DIR_EAST = 8
		};

	class DirectionBlock
		{
		public:
			DirectionBlock(CL_DomElement *pElement );
			~DirectionBlock();

			eDirectionBlock getDirectionBlock() const;
		private:
			eDirectionBlock meDirectionBlock;
		};


	// Things which take actions on the party
	class Action
		{
		public:
			Action(){}
			virtual ~Action(){}

			virtual void invoke()=0;
		};


	// Things that evaluate by examining the party
	class Check
		{
		public:
			Check(){}
			virtual ~Check(){}

			virtual bool evaluate()=0;
		};


	class ItemRef
		{
		public:
			ItemRef(CL_DomElement *pElement );
			~ItemRef();

			std::string getItemName();
			Item::eItemType getItemType();

		private:
			std::string mItem;
			Item::eItemType meType;
		};

	class Tilemap
		{
		public:
			Tilemap(CL_DomElement *pElement);
			~Tilemap();
      
			ushort getMapX() const;
			ushort getMapY() const;

			std::string getMapName() const;


		private:
			std::string mMapName;
			ushort mX;
			ushort mY;
		};


	class SpriteRef
		{
		public:
			SpriteRef( CL_DomElement *pElement);
			~SpriteRef();

			std::string getRef() const;

		private:
			std::string mRef;
		};

	class Condition;

	class AttributeModifier : public Action
		{
		public:
			AttributeModifier (CL_DomElement *pElement);
			~AttributeModifier();


			virtual void invoke();

		private:
			std::list<Condition*> mConditions;
			int mAdd;
			std::string mAttribute;
			std::string mTarget;
		};

	class HasGold : public Check
		{
		public:

			enum eOperator{LT, GT, LTE, GTE, EQ};

			HasGold( CL_DomElement *pElement);
			~HasGold();

			virtual bool evaluate();

		private:
			uint mAmount;
			bool mbNot;
			eOperator meOperator;
      
		};

	class HasItem : public Check
		{
		public:
			HasItem(CL_DomElement *pElement );
			~HasItem();

			virtual bool evaluate();

      
		private:
			ItemRef * mpItemRef;
			bool mbNot;
			std::string mItem;
			Item::eItemType mItemType;
		};

	class DidEvent : public Check
		{
		public:
			DidEvent(CL_DomElement *pElement);
			~DidEvent();

			virtual bool evaluate();
		private:
			bool mbNot;
			std::string mEvent;
      
		};

	class Operator;

	class And : public Check
		{
		public:
			And(CL_DomElement * pElement);
			~And();

			virtual bool evaluate();

			ushort order();

		private:
			ushort mOrder;
			std::list<Check*> mOperands;
		};

	class Or : public Check
		{
		public:
			Or(CL_DomElement * pElement);
			~Or();

			virtual bool evaluate();

			ushort order();

		private:
			ushort mOrder;
			std::list<Check*> mOperands;

		};

	class Operator : public Check
		{
		public:
			Operator(CL_DomElement *pElement);
			~Operator();

			virtual bool evaluate();
			ushort order();
      
		private:
			ushort mOrder;
			std::list<Check*> mOperands;
		};


	class Condition 
		{
		public:
			Condition(CL_DomElement *pElement);
			~Condition();

			bool evaluate() const;

		private:
			std::list<Check*> mChecks;
		};

	class Event 
		{
		public:
			Event(CL_DomElement *pElement);
			~Event();

			enum eTriggerType { STEP, TALK, ACT };

			std::string getName() const;
			eTriggerType getTriggerType();
			bool repeatable();
			bool invoke();
      
		private:
			std::string mName;
			bool mbRepeatable;
			eTriggerType meTriggerType;
			Condition *mpCondition;
			std::list<Action*> mActions;

		};


	class PlayAnimation : public Action
		{
		public:
			PlayAnimation(CL_DomElement * pElement );
			~PlayAnimation();

			virtual void invoke();
		private:
			std::string mAnimation;
		};

	class PlaySound : public Action
		{
		public:
			PlaySound(CL_DomElement *pElement );
			~PlaySound();

			virtual void invoke();
		private:
			std::string mSound;
		};

	class LoadLevel : public Action
		{
		public:
			LoadLevel(CL_DomElement *pElement);
			~LoadLevel();

			virtual void invoke();
		private:
			std::string mName;
			ushort mStartY;
			ushort mStartX;
		};

	class StartBattle : public Action
		{
		public:
			StartBattle(CL_DomElement *pElement);
			~StartBattle();

			virtual void invoke();

      
		private:
			std::string mMonster;
			ushort mCount;
			bool mbIsBoss;
		};

	class InvokeShop : public Action
		{
		public:
			InvokeShop(CL_DomElement *pElement);
			~InvokeShop();

			virtual void invoke();
		private:
			std::string mShopType;
		};


	class Pause : public Action
		{
		public:
			Pause(CL_DomElement *pElement );
			~Pause();

			virtual void invoke();
		private:
			uint mMs;
		};

	class Say : public Action
		{
		public:
			Say (CL_DomElement *pElement );
			~Say();

			virtual void invoke();
		private:
			std::string mSpeaker;
			std::string mText;
		};

	class Give: public Action
		{
		public:
			Give(CL_DomElement *pElement );
			~Give();

			virtual void invoke();
		private:
			ItemRef *mpItemRef;
			uint mCount;
		};

	class Take : public Action
		{
		public:
			Take(CL_DomElement *pElement );
			~Take();

			virtual void invoke();

		private:
			ItemRef *mpItemRef;
			uint mCount;
		};

	class GiveGold : public Action
		{
		public:
			GiveGold( CL_DomElement *pElement );
			~GiveGold();

			virtual void invoke();

		private:
			uint mCount;
		};


	class Graphic
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


  
	union SpriteRefOrTilemap
	{
		SpriteRef* asSpriteRef;
		Tilemap * asTilemap;
	};


	class Tile : public Graphic
		{
		public:



			Tile(CL_DomElement *pElement);
			~Tile(); 

			ushort getZOrder() const;

			bool isFloater() const;

			bool evaluateCondition() const;


			virtual uint getX() const;
			virtual uint getY() const;

			virtual CL_Rect getRect();

			virtual bool isSprite() const;

			virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);


			virtual void update();

			virtual int getDirectionBlock() const;

			virtual bool isTile() const;


		private:

			enum eFlags { SPRITE = 1, FLOATER = 2, HAS_COND = 4, BLK_NORTH = 8, BLK_SOUTH = 16, BLK_EAST = 32, BLK_WEST = 64 };

			CL_Sprite *mpSprite;
			SpriteRefOrTilemap mGraphic;
			ushort mZOrder;
			Condition *mpCondition;
			AttributeModifier *mpAM;
			ushort mX;
			ushort mY;

			char cFlags;

		};

	class MappableObject : public Graphic
		{
		public:
			MappableObject(CL_DomElement *pElement);
			~MappableObject();

			ushort getStartX() const;
			ushort getStartY() const;

			enum eMovementType { MOVEMENT_NONE, MOVEMENT_WANDER };
			enum eMappableObjectType { NPC, SQUARE, CONTAINER, DOOR, WARP };

			eMovementType getMovementType() const;

			std::string getName() const;
			virtual uint getX() const;
			virtual uint getY() const;

			virtual CL_Rect getRect();

			virtual bool isSprite() const;


			virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);

			virtual void update();

			virtual int getDirectionBlock() const;

			virtual bool isTile() const;

			void provokeEvents ( Event::eTriggerType trigger );

		private:

			enum eFlags { SPRITE = 1, BLK_NORTH = 2, BLK_SOUTH = 4, BLK_EAST = 8, BLK_WEST = 16, TILEMAP = 32 };

			std::string mName;
			std::list<CL_Sprite *>mSprites;
			SpriteRefOrTilemap mGraphic;
			std::list<Event*> mEvents;
      
			ushort mStartX;
			ushort mStartY;
			uint mX;
			uint mY;
			eMovementType meMovementType;
			eMappableObjectType meType;

			char cFlags;
      
		};



	class Level
		{
		public:
			Level(const std::string &name,CL_ResourceManager * pResources);
			~Level();
      

			void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC , bool floaters = false);
			void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);
			void drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC);
		
      
      
			// Checks relevant tile and MO direction block information
			bool canMove(const CL_Rect &currently, const CL_Rect & destination) const; 

			// All AM's from tiles fire, as do any step events
			void step(uint levelX, uint levelY);
      
			// Any talk events fire (assuming they meet conditions)
			void talk(uint levelX, uint levelY);

			// Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
			void update(const CL_Rect & updateRect);

			uint getWidth() const { return mLevelWidth; }
			uint getHeight() const { return mLevelHeight; }
      
		private:
//			std::multimap<CL_Point, Tile *> mTileMap;
//			std::multimap<CL_Point, Tile *> mFloaterMap;
			std::map<CL_Point, std::list<Tile*> > mTileMap;
			std::map<CL_Point, std::list<Tile*> > mFloaterMap;
      
			// Sort MO's in order to bring close ones to the top
			static bool moSortCriterion( const MappableObject *p1, const MappableObject * p2);

			// Sort tiles on zOrder
			static bool tileSortCriterion ( const Tile * p1, const Tile * p2);

			// Sort to bring the close ones near the top, or partition to bring nearby ones up.
			std::list<MappableObject *> mMappableObjects; 
			void LoadLevel( const std::string &filename );

			void loadTile ( CL_DomElement * tileElement);
			void loadMo ( CL_DomElement * moElement );

			CL_ResourceManager * mpResources;
			CL_DomDocument * mpDocument;

			std::string mMusic;

			uint mLevelWidth;
			uint mLevelHeight;
		};
}

#endif
