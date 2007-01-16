#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "IApplication.h"
#include "Level.h"
#include "Action.h"
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <functional>
#include <sstream>
#include "GraphicsManager.h"
#include "LevelFactory.h"
#include "ItemFactory.h"
#include "ItemManager.h"
#include "CharacterDefinition.h"
#include "LoadLevel.h"
#include "AttributeModifier.h"
#include "WeaponRef.h"
#include "ArmorRef.h"



template<typename MapType,
         typename KeyArgType,
         typename ValueArgType>
typename MapType::iterator
efficientAddOrUpdate(MapType &m,
                     const KeyArgType &k,
                     const ValueArgType &v)
{
    typename MapType::iterator lb = m.lower_bound(k);

    if(lb != m.end() &&
       !(m.key_comp()(k,lb->first)))
    {
        lb->second = v;
        return lb;
    }
    else
    {
        typedef typename MapType::value_type MVT;
        return m.insert(lb,MVT(k,v));
    }
}


std::string BoolToString( const bool &b)
{
    if(b) return "true";
    else return "false";
}




// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2)
{
	uint p1value = (p1.y  *  StoneRing::IApplication::getInstance()->getScreenWidth()) + p1.x;
	uint p2value = (p2.y  * StoneRing::IApplication::getInstance()->getScreenWidth()) + p2.x;
    
    return p1value < p2value;
}

bool operator < (const StoneRing::MappableObject::eDirection dir1, const StoneRing::MappableObject::eDirection dir2)
{
    return (int)dir1 < (int)dir2;
}



bool StoneRing::ItemRef::handleElement(eElement element, StoneRing::Element * pElement )
{
    switch(element)
    {
    case ENAMEDITEMREF:
        meType = NAMED_ITEM;
        mpNamedItemRef = dynamic_cast<NamedItemRef*>(pElement);
        break;
    case EWEAPONREF:
        meType = WEAPON_REF;
        mpWeaponRef = dynamic_cast<WeaponRef*>(pElement);
        break;
    case EARMORREF:
        meType = ARMOR_REF;
        mpArmorRef = dynamic_cast<ArmorRef*>(pElement);
        break;
    default:
        
        return false;
    }

	return true;
}

void StoneRing::ItemRef::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    
}

void StoneRing::ItemRef::loadFinished()
{

    const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    if(!mpNamedItemRef && !mpWeaponRef && !mpArmorRef)
    {
        throw CL_Error("Item Ref with no child");
    }
    
    mpItem = pItemManager->getItem ( *this ); 

}


StoneRing::ItemRef::ItemRef( ):
    mpNamedItemRef(NULL),mpWeaponRef(NULL),mpArmorRef(NULL)
{
 
  
}


CL_DomElement StoneRing::ItemRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc, std::string("itemRef"));


    switch(meType)
    {
    case NAMED_ITEM:
        element.append_child ( mpNamedItemRef->createDomElement(doc) );
        break;
    case WEAPON_REF:
        element.append_child ( mpWeaponRef->createDomElement(doc) );
        break;
    case ARMOR_REF:
        element.append_child ( mpArmorRef->createDomElement(doc) );
        break;
    }

    return element;

}

StoneRing::ItemRef::~ItemRef()
{
    delete mpNamedItemRef;
    delete mpWeaponRef;
    delete mpArmorRef;

}

#if 0
std::string StoneRing::ItemRef::getItemName() const
{
    switch ( meType )
    {
    case NAMED_ITEM:
        return mpNamedItemRef->getItemName();
        break;
    case WEAPON_REF:
        // Need a standard way to generate names from these suckers
        break;
    case ARMOR_REF:
        break;
    }
}
#endif

StoneRing::ItemRef::eRefType StoneRing::ItemRef::getType() const
{
    return meType;
}

StoneRing::NamedItemRef * StoneRing::ItemRef::getNamedItemRef() const
{
    return mpNamedItemRef;
}

StoneRing::WeaponRef * StoneRing::ItemRef::getWeaponRef() const
{
    return mpWeaponRef;
}

StoneRing::ArmorRef * StoneRing::ItemRef::getArmorRef() const
{
    return mpArmorRef;
}



StoneRing::NamedItemRef::NamedItemRef()
{
}

StoneRing::NamedItemRef::~NamedItemRef()
{
}


std::string StoneRing::NamedItemRef::getItemName()
{
    return mName;
}

void StoneRing::NamedItemRef::handleText(const std::string &text)
{
    mName = text;
}

CL_DomElement  StoneRing::NamedItemRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"namedItemRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}


CL_DomElement  StoneRing::Tilemap::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"tilemap");

    element.set_attribute( "mapname" , GraphicsManager::getInstance()->lookUpMapWithSurface ( mpSurface ) );
    element.set_attribute( "mapx", IntToString ( mX ) );
    element.set_attribute( "mapy", IntToString ( mY ) );

    return element;
}

bool StoneRing::Tilemap::handleElement(eElement element, Element * pElement)
{
	return false;
}

void StoneRing::Tilemap::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string name = getRequiredString("mapname",pAttributes);
    mpSurface = GraphicsManager::getInstance()->getTileMap(name);
    mX = getRequiredInt("mapx",pAttributes);
    mY = getRequiredInt("mapy",pAttributes);
}


StoneRing::Tilemap::Tilemap():mpSurface(NULL)
{
    

}
 
StoneRing::Tilemap::~Tilemap()
{
}
      


CL_DomElement  StoneRing::SpriteRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"spriteRef");

    std::string dir;

    switch(meType)
    {
    case SPR_STILL:
        dir = "still";
        break;
    case SPR_TWO_WAY:
        dir = "twoway";
        break;
    case SPR_FOUR_WAY:
        dir = "fourway";
        break;
    case SPR_NONE:
        break;
    }

    if(dir.length())
    {
        element.set_attribute("type", dir);
    }

    CL_DomText text(doc,mRef);
    text.set_node_value( mRef );

    element.append_child ( text );

    return element;

}

bool StoneRing::SpriteRef::handleElement(eElement element, Element * pElement)
{
	return false;
}

void StoneRing::SpriteRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    if(hasAttr("type",pAttributes))
    {
        std::string direction = getString("type",pAttributes);
        
        if(direction == "still") meType = SPR_STILL;
        if(direction == "twoway") meType = SPR_TWO_WAY;
        if(direction == "fourway")  meType = SPR_FOUR_WAY;

    }

}

void StoneRing::SpriteRef::handleText(const std::string &text)
{
    mRef = text;
}


StoneRing::SpriteRef::SpriteRef( ):meType(SPR_NONE)
{
  
}

StoneRing::SpriteRef::~SpriteRef()
{
}

std::string StoneRing::SpriteRef::getRef() const
{
    return mRef;
}

StoneRing::SpriteRef::eType 
StoneRing::SpriteRef::getType() const
{
    return meType;
}




StoneRing::Event::Event():mbRepeatable(true),mbRemember(false),mpCondition(NULL)
{
}

CL_DomElement  StoneRing::Event::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"event");

    element.set_attribute("name", mName );

    std::string triggertype;

    switch( meTriggerType )
    {
    case STEP:
        triggertype = "step";
        break;
    case TALK:
        triggertype = "talk";
        break;
    case ACT:
        triggertype = "act";
        break;
    }

    element.set_attribute("triggerType", triggertype);

    if(!mbRepeatable) element.set_attribute("repeatable","false");

    if(mbRemember) element.set_attribute("remember","true");

    if(mpCondition )
    {
        CL_DomElement e = mpCondition->createDomElement(doc);

        element.append_child ( e );

    }

    for(std::list<Action*>::const_iterator i = mActions.begin();
        i != mActions.end();
        i++)
    {
		Element * pElement = dynamic_cast<Element*>(*i);
        CL_DomElement e = pElement->createDomElement(doc);
    
        element.append_child ( e );

    }


    return element;

}

void StoneRing::Event::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    std::string triggertype = getString("triggerType",pAttributes);

    if(triggertype == "step")
        meTriggerType = STEP;
    else if (triggertype == "talk")
        meTriggerType = TALK;
    else if (triggertype == "act")
        meTriggerType = ACT;
    else throw CL_Error(" Bad trigger type on event " + mName );

    mbRepeatable = getImpliedBool("repeatable",pAttributes,true);
    mbRemember = getImpliedBool("remember",pAttributes,false);
}

bool StoneRing::Event::handleElement(eElement element, Element *pElement)
{
    if(isAction(element))
    {
        mActions.push_back( dynamic_cast<Action*>(pElement));
    }
    else if(element == ECONDITION)
    {
        mpCondition = dynamic_cast<Condition*>(pElement);
    }
	else 
	{
		return false;
	}

	return true;
}


StoneRing::Event::~Event()
{
    for(std::list<Action*>::iterator i = mActions.begin();
        i != mActions.end();
        i++)
    {
        delete *i;
    }
}

std::string StoneRing::Event::getName() const
{
    return mName;
}

StoneRing::Event::eTriggerType StoneRing::Event::getTriggerType()
{
    return meTriggerType;
}

bool StoneRing::Event::repeatable()
{
    return mbRepeatable;
}

bool StoneRing::Event::remember()
{
    return mbRemember;
}
 
bool StoneRing::Event::invoke()
{
    
    if(mpCondition && !mpCondition->evaluate() ) return false;

	StoneRing::IApplication::getInstance()->getParty()->doEvent ( mName, mbRemember );

    for(std::list<Action*>::iterator i = mActions.begin();
        i != mActions.end();
        i++)
    {
        Action *action = *i;

        action->invoke();
    }

    return true;
}
      

CL_DomElement StoneRing::Movement::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"movement") ;

    
    std::string movementType;

    switch( meType )
    {
    case MOVEMENT_WANDER:
        movementType = "wander";
        break;
    case MOVEMENT_PACE_NS:
        movementType = "paceNS";
        break;
    case MOVEMENT_PACE_EW:
        movementType = "paceEW";
        break;
    case MOVEMENT_NONE:
        movementType = "none";
        break;
    }


    element.set_attribute("movementType", movementType );

    std::string speed;

    switch(meSpeed)
    {
    case SLOW:
        speed = "slow";
        break;
    case FAST:
        speed = "fast";
        break;
    }

    element.set_attribute("speed", speed );

    return element;

}

void StoneRing::Movement::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    if(hasAttr("speed",pAttributes))
    {
        std::string speed = getString("speed",pAttributes);

        if(speed == "medium")
        {
            meSpeed = MEDIUM;
        }
        else if(speed == "slow")
        {
            meSpeed = SLOW;
        }
        else if(speed == "fast")
        {
            meSpeed = FAST;
        }
        else throw CL_Error("Error, movement speed must be fast or slow.");
            
    }

    std::string type = getRequiredString("movementType",pAttributes);

    if(type == "wander")
    {   
        meType = MOVEMENT_WANDER;
    }
    else if(type == "paceNS")
    {
        meType = MOVEMENT_PACE_NS;
    }
    else if(type == "paceEW")
    {
        meType = MOVEMENT_PACE_EW;
    }
    else if(type == "none")
    {
        // Why would they ever....
        meType = MOVEMENT_NONE;
    }

}

StoneRing::Movement::Movement ( ):meType(MOVEMENT_NONE),meSpeed(SLOW)
{
   
}

StoneRing::Movement::~Movement()
{
}


StoneRing::Movement::eMovementType StoneRing::Movement::getMovementType() const
{
    return meType;
}

StoneRing::Movement::eMovementSpeed StoneRing::Movement::getMovementSpeed() const
{
    return meSpeed;
}


void StoneRing::LoadLevel::invoke()
{
    IApplication::getInstance()->loadLevel ( mName, mStartX, mStartY );
}


StoneRing::Graphic::Graphic()
{
}

StoneRing::Graphic::~Graphic()
{
}


StoneRing::DirectionBlock::DirectionBlock():meDirectionBlock(0)
{
}

StoneRing::DirectionBlock::DirectionBlock(int i )
{
    meDirectionBlock = i;
}

CL_DomElement  StoneRing::DirectionBlock::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"directionBlock");

    element.set_attribute("north", BoolToString (meDirectionBlock & DIR_NORTH ));
    element.set_attribute("south", BoolToString (meDirectionBlock & DIR_SOUTH));
    element.set_attribute("east", BoolToString ( meDirectionBlock & DIR_EAST ) );
    element.set_attribute("west", BoolToString ( meDirectionBlock & DIR_WEST ) );

    return element;
}

void StoneRing::DirectionBlock::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    bool north = getRequiredBool("north",pAttributes);
    bool south = getRequiredBool("south",pAttributes);
    bool east =  getRequiredBool("east",pAttributes);
    bool west =  getRequiredBool("west",pAttributes);

    if(north) meDirectionBlock |= DIR_NORTH;
    if(south) meDirectionBlock |= DIR_SOUTH;
    if(east) meDirectionBlock |= DIR_EAST;
    if(west) meDirectionBlock |= DIR_WEST;
}


StoneRing::DirectionBlock::~DirectionBlock()
{
}

int StoneRing::DirectionBlock::getDirectionBlock() const
{
    return meDirectionBlock;
}


StoneRing::Tile::Tile():mpSprite(NULL),mpCondition(NULL),mpAM(NULL),mZOrder(0),cFlags(0)
{
}

CL_DomElement  StoneRing::Tile::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement element(doc,"tile");


    element.set_attribute("xpos", IntToString ( mX ) );
    element.set_attribute("ypos", IntToString ( mY ) );
    if(mZOrder >0 ) element.set_attribute("zorder", IntToString (mZOrder ) );
    if(isFloater()) element.set_attribute("floater", "true");
    if(isHot())     element.set_attribute("hot", "true");
	if(pops())      element.set_attribute("pops","true");

    if(isSprite())
    {
        CL_DomElement  spriteEl = mGraphic.asSpriteRef->createDomElement(doc);

        element.append_child ( spriteEl );

    }
    else
    {
        CL_DomElement  tilemapEl = mGraphic.asTilemap->createDomElement(doc);

        element.append_child (  tilemapEl );

    }

    if(mpCondition)
    {
        CL_DomElement  condEl = mpCondition->createDomElement(doc);

        element.append_child ( condEl );

    }

    if(mpAM)
    {
        CL_DomElement  amEl = mpAM->createDomElement(doc);

        element.append_child ( amEl );

    }

    if( getDirectionBlock() > 0)
    {
        DirectionBlock block( getDirectionBlock() );

        CL_DomElement  dirEl = block.createDomElement(doc);

        element.append_child( dirEl );

    }
    

    return element;

    
}

void StoneRing::Tile::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mX = getRequiredInt("xpos",pAttributes);
    mY = getRequiredInt("ypos",pAttributes);

    mZOrder = getImpliedInt("zorder",pAttributes,0);

    bool floater = getImpliedBool("floater",pAttributes,false);
    bool hot = getImpliedBool("hot",pAttributes,false);
	bool pops = getImpliedBool("pops",pAttributes,false);

    if(floater) cFlags |= FLOATER;
    if(hot) cFlags |= HOT;
	if(pops) cFlags |= POPS;

}


bool StoneRing::Tile::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ETILEMAP:
        mGraphic.asTilemap = dynamic_cast<Tilemap*>(pElement);
        break;
    case ESPRITEREF:
    {
        GraphicsManager * GM = GraphicsManager::getInstance();
        mGraphic.asSpriteRef = dynamic_cast<SpriteRef*>(pElement);
        cFlags |= SPRITE;

        // Actually create the ref'd sprite here.
        // And assign to mpSprite
        mpSprite = GM->createSprite( mGraphic.asSpriteRef->getRef() );
        break;
    }//ESPRITEREF
    case ECONDITION:
        mpCondition = dynamic_cast<Condition*>(pElement);
        break;
    case EATTRIBUTEMODIFIER:
        mpAM = dynamic_cast<AttributeModifier*>(pElement);
        break;
    case EDIRECTIONBLOCK:
    {
        DirectionBlock *block = dynamic_cast<DirectionBlock*>(pElement);

        int db = block->getDirectionBlock();

        // This is all done to make tile's take up less space in memory
            
        if(db & DIR_NORTH)
            cFlags |= BLK_NORTH;
        if(db & DIR_SOUTH)
            cFlags |= BLK_SOUTH;
        if(db & DIR_EAST)
            cFlags |= BLK_EAST;
        if(db & DIR_WEST)
            cFlags |= BLK_WEST;

        delete block;
        
        break;
    }
	default:
		return false;
    }

	return true;
}

void StoneRing::Tile::activate() // Call any attributemodifier
{
    mpAM->invoke();   
}

StoneRing::Tile::~Tile()
{
    delete mpAM;
    delete mpCondition;
    delete mpSprite;

    if( isSprite() )
        delete mGraphic.asSpriteRef;
    else delete mGraphic.asTilemap;
}

bool StoneRing::Tile::evaluateCondition() const
{
    if( mpCondition ) return mpCondition->evaluate();

    else return true;
}


CL_Rect StoneRing::Tile::getRect()
{
    return CL_Rect(mX , mY , mX + 1, mY +1);
}



void StoneRing::Tile::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    // Get the graphic guy
    // Get our tilemap or sprite
    // Blit it

    static GraphicsManager * GM = GraphicsManager::getInstance();

    if( !isSprite() )
    {
        CL_Surface * tilemap = mGraphic.asTilemap->getTileMap();

        //        void draw(  const CL_Rect& src, const CL_Rect& dest, CL_GraphicContext* context = 0);
        int mapx, mapy;
        mapx = mGraphic.asTilemap->getMapX();
        mapy = mGraphic.asTilemap->getMapY();
        CL_Rect srcRect((mapx << 5) + src.left, (mapy << 5) + src.top,
                        (mapx << 5) + src.right, (mapy << 5) + src.bottom);

        
        tilemap->draw(srcRect, dst, pGC);
        
    }
    else
    {
        update();
        mpSprite->draw( dst, pGC );
    }

}

void StoneRing::Tile::update()
{
    if(isSprite()) mpSprite->update();
}

int StoneRing::Tile::getDirectionBlock() const
{

    int block = 0;

    if( cFlags & BLK_NORTH)
        block |= DIR_NORTH;
    if( cFlags & BLK_SOUTH)
        block |= DIR_SOUTH;
    if( cFlags & BLK_EAST)
        block |= DIR_EAST;
    if( cFlags & BLK_WEST)
        block |= DIR_WEST;

    return block;
}





