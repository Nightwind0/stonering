#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
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
#include "SpriteDefinition.h"
#include "WeaponRef.h"
#include "ArmorRef.h"
#include "IApplication.h"
#include "Level.h"

using StoneRing::Element;


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
    uint p1value = (p1.y  *  StoneRing::IApplication::GetInstance()->GetScreenWidth()) + p1.x;
    uint p2value = (p2.y  * StoneRing::IApplication::GetInstance()->GetScreenWidth()) + p2.x;

    return p1value < p2value;
}

bool operator < (const StoneRing::MappableObject::eDirection dir1, const StoneRing::MappableObject::eDirection dir2)
{
    return (int)dir1 < (int)dir2;
}


bool StoneRing::Tilemap::handle_element(Element::eElement element, Element * pElement)
{
    return false;
}

void StoneRing::Tilemap::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string name = get_required_string("mapname",pAttributes);
    m_pSurface = GraphicsManager::GetInstance()->GetTileMap(name);
    m_X = get_required_int("mapx",pAttributes);
    m_Y = get_required_int("mapy",pAttributes);
}


StoneRing::Tilemap::Tilemap():m_pSurface(NULL)
{


}

StoneRing::Tilemap::~Tilemap()
{
}

void StoneRing::Movement::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    if(has_attribute("speed",pAttributes))
    {
        std::string speed = get_string("speed",pAttributes);

        if(speed == "medium")
        {
            m_eSpeed = MEDIUM;
        }
        else if(speed == "slow")
        {
            m_eSpeed = SLOW;
        }
        else if(speed == "fast")
        {
            m_eSpeed = FAST;
        }
        else throw CL_Error("Error, movement speed must be fast, medium or slow.");

    }

    std::string type = get_required_string("movementType",pAttributes);

    if(type == "wander")
    {
        m_eType = MOVEMENT_WANDER;
    }
    else if(type == "paceNS")
    {
        m_eType = MOVEMENT_PACE_NS;
    }
    else if(type == "paceEW")
    {
        m_eType = MOVEMENT_PACE_EW;
    }
    else if(type == "script")
    {
        m_eType = MOVEMENT_SCRIPT;
    }
    else if(type == "none")
    {
        // Why would they ever....
        m_eType = MOVEMENT_NONE;
    }

}

StoneRing::Movement::Movement ( ):m_eType(MOVEMENT_NONE),m_eSpeed(SLOW),m_pScript(NULL)
{

}

StoneRing::Movement::~Movement()
{
    delete m_pScript;
}


bool StoneRing::Movement::handle_element(Element::eElement element, Element *pElement)
{
    if(element == ESCRIPT)
        m_pScript= dynamic_cast<ScriptElement*>(pElement);
    else return false;

    return true;
}

StoneRing::Movement::eMovementType StoneRing::Movement::GetMovementType() const
{
    return m_eType;
}

StoneRing::Movement::eMovementSpeed StoneRing::Movement::GetMovementSpeed() const
{
    return m_eSpeed;
}

StoneRing::Graphic::Graphic()
{
}

StoneRing::Graphic::~Graphic()
{
}


StoneRing::DirectionBlock::DirectionBlock():m_eDirectionBlock(0)
{
}

StoneRing::DirectionBlock::DirectionBlock(int i )
{
    m_eDirectionBlock = i;
}



void StoneRing::DirectionBlock::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    bool north = get_required_bool("north",pAttributes);
    bool south = get_required_bool("south",pAttributes);
    bool east =  get_required_bool("east",pAttributes);
    bool west =  get_required_bool("west",pAttributes);

    if(north) m_eDirectionBlock |= DIR_NORTH;
    if(south) m_eDirectionBlock |= DIR_SOUTH;
    if(east) m_eDirectionBlock |= DIR_EAST;
    if(west) m_eDirectionBlock |= DIR_WEST;
}


StoneRing::DirectionBlock::~DirectionBlock()
{
}

int StoneRing::DirectionBlock::GetDirectionBlock() const
{
    return m_eDirectionBlock;
}


StoneRing::Tile::Tile():m_pSprite(NULL),m_pCondition(NULL),m_pScript(NULL),m_ZOrder(0),cFlags(0)
{
}


void StoneRing::Tile::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_X = get_required_int("xpos",pAttributes);
    m_Y = get_required_int("ypos",pAttributes);

    m_ZOrder = get_implied_int("zorder",pAttributes,0);

    bool floater = get_implied_bool("floater",pAttributes,false);
    bool hot = get_implied_bool("hot",pAttributes,false);
    bool pops = get_implied_bool("pops",pAttributes,false);

    if(floater) cFlags |= FLOATER;
    if(hot) cFlags |= HOT;
    if(pops) cFlags |= POPS;

}


bool StoneRing::Tile::handle_element(Element::eElement element, Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ETILEMAP:
        m_Graphic.asTilemap = dynamic_cast<Tilemap*>(pElement);
        break;
    case ESPRITEREF:
    {
        GraphicsManager * GM = GraphicsManager::GetInstance();
        m_Graphic.asSpriteRef = dynamic_cast<SpriteRef*>(pElement);
        cFlags |= SPRITE;

        // Actually create the ref'd sprite here.
        // And assign to mpSprite
        m_pSprite = GM->CreateSprite( m_Graphic.asSpriteRef->GetRef() );
        break;
    }//ESPRITEREF
    case ECONDITIONSCRIPT:
        m_pCondition = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EDIRECTIONBLOCK:
    {
        DirectionBlock *block = dynamic_cast<DirectionBlock*>(pElement);

        int db = block->GetDirectionBlock();

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

void StoneRing::Tile::load_finished()
{
    if(m_Graphic.asSpriteRef == NULL) throw CL_Error("Tile didn't have tilemap or sprite ref.");
}

void StoneRing::Tile::Activate() // Call any attributemodifier
{
    // Run script
    if(m_pScript)
        m_pScript->ExecuteScript();
}

StoneRing::Tile::~Tile()
{
    delete m_pScript;
    delete m_pCondition;
    delete m_pSprite;

    if( IsSprite() )
        delete m_Graphic.asSpriteRef;
    else delete m_Graphic.asTilemap;
}

bool StoneRing::Tile::EvaluateCondition() const
{
    if( m_pCondition )
        return m_pCondition->EvaluateCondition();
    else return true;
}


CL_Rect StoneRing::Tile::GetRect()
{
    return CL_Rect(m_X , m_Y , m_X + 1, m_Y +1);
}



void StoneRing::Tile::Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    // Get the graphic guy
    // Get our tilemap or sprite
    // Blit it

    // static GraphicsManager * GM = GraphicsManager::GetInstance();

    if( !IsSprite() )
    {
        CL_Surface * tilemap = m_Graphic.asTilemap->GetTileMap();

        //        void draw(  const CL_Rect& src, const CL_Rect& dest, CL_GraphicContext* context = 0);
        int mapx, mapy;
        mapx = m_Graphic.asTilemap->GetMapX();
        mapy = m_Graphic.asTilemap->GetMapY();
        CL_Rect srcRect((mapx << 5) + src.left, (mapy << 5) + src.top,
                        (mapx << 5) + src.right, (mapy << 5) + src.bottom);


        tilemap->draw(srcRect, dst, pGC);

    }
    else
    {
        Update();
        m_pSprite->draw( dst, pGC );
    }

}

void StoneRing::Tile::Update()
{
    if(IsSprite()) m_pSprite->update();
}

int StoneRing::Tile::GetDirectionBlock() const
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







