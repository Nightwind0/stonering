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

    if (lb != m.end() &&
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
    if (b) return "true";
    else return "false";
}




// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2)
{
    uint p1value = (p1.y  *  StoneRing::IApplication::GetInstance()->GetDisplayRect().get_width()) + p1.x;
    uint p2value = (p2.y  * StoneRing::IApplication::GetInstance()->GetDisplayRect().get_width()) + p2.x;

    return p1value < p2value;
}



bool StoneRing::Tilemap::handle_element(Element::eElement element, Element * pElement)
{
    return false;
}

void StoneRing::Tilemap::load_attributes(CL_DomNamedNodeMap attributes)
{
    std::string name = get_required_string("mapname",attributes);
    m_sprite = GraphicsManager::GetTileMap(name);
    m_X = get_required_int("mapx",attributes);
    m_Y = get_required_int("mapy",attributes);
}


StoneRing::Tilemap::Tilemap()
{


}

StoneRing::Tilemap::~Tilemap()
{
}

void StoneRing::Movement::load_attributes(CL_DomNamedNodeMap attributes)
{
    if (has_attribute("speed",attributes))
    {
        std::string speed = get_string("speed",attributes);

        if (speed == "medium")
        {
            m_eSpeed = MEDIUM;
        }
        else if (speed == "slow")
        {
            m_eSpeed = SLOW;
        }
        else if (speed == "fast")
        {
            m_eSpeed = FAST;
        }
        else throw CL_Exception("Error, movement speed must be fast, medium or slow.");

    }

    std::string type = get_required_string("movementType",attributes);

    if (type == "wander")
    {
        m_eType = MOVEMENT_WANDER;
    }
    else if (type == "paceNS")
    {
        m_eType = MOVEMENT_PACE_NS;
    }
    else if (type == "paceEW")
    {
        m_eType = MOVEMENT_PACE_EW;
    }
    else if (type == "script")
    {
        m_eType = MOVEMENT_SCRIPT;
    }
    else if (type == "none")
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
    if (element == ESCRIPT)
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



void StoneRing::DirectionBlock::load_attributes(CL_DomNamedNodeMap attributes)
{
    bool north = get_required_bool("north",attributes);
    bool south = get_required_bool("south",attributes);
    bool east =  get_required_bool("east",attributes);
    bool west =  get_required_bool("west",attributes);

    if (north) m_eDirectionBlock |= BLK_NORTH;
    if (south) m_eDirectionBlock |= BLK_SOUTH;
    if (east) m_eDirectionBlock |= BLK_EAST;
    if (west) m_eDirectionBlock |= BLK_WEST;
}


StoneRing::DirectionBlock::~DirectionBlock()
{
}

int StoneRing::DirectionBlock::GetDirectionBlock() const
{
    return m_eDirectionBlock;
}


StoneRing::Tile::Tile():m_pCondition(NULL),m_pScript(NULL),m_ZOrder(0),cFlags(0)
{
}


void StoneRing::Tile::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_X = get_required_int("xpos",attributes);
    m_Y = get_required_int("ypos",attributes);

    m_ZOrder = get_implied_int("zorder",attributes,0);

    bool floater = get_implied_bool("floater",attributes,false);
    bool hot = get_implied_bool("hot",attributes,false);
    bool pops = get_implied_bool("pops",attributes,false);

    if (floater) cFlags |= TIL_FLOATER;
    if (hot) cFlags |= TIL_HOT;
    if (pops) cFlags |= TIL_POPS;

}


bool StoneRing::Tile::handle_element(Element::eElement element, Element * pElement)
{
    switch (element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ETILEMAP:
        m_Graphic.asTilemap = dynamic_cast<Tilemap*>(pElement);
        break;
    case ESPRITEREF:
    {
        m_Graphic.asSpriteRef = dynamic_cast<SpriteRef*>(pElement);
        cFlags |= TIL_SPRITE;

        // Actually create the ref'd sprite here.
        // And assign to mpSprite
        m_sprite = GraphicsManager::CreateSprite( m_Graphic.asSpriteRef->GetRef() );
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

        if (db & BLK_NORTH)
            cFlags |= TIL_BLK_NORTH;
        if (db & BLK_SOUTH)
            cFlags |= TIL_BLK_SOUTH;
        if (db & BLK_EAST)
            cFlags |= TIL_BLK_EAST;
        if (db & BLK_WEST)
            cFlags |= TIL_BLK_WEST;

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
    if (m_Graphic.asSpriteRef == NULL) throw CL_Exception("Tile didn't have tilemap or sprite ref.");
}

void StoneRing::Tile::Activate() // Call any attributemodifier
{
    // Run script
    if (m_pScript)
        m_pScript->ExecuteScript();
}

StoneRing::Tile::~Tile()
{
    delete m_pScript;
    delete m_pCondition;
    if ( IsSprite() )
        delete m_Graphic.asSpriteRef;
    else delete m_Graphic.asTilemap;
}

bool StoneRing::Tile::EvaluateCondition() const
{
    if ( m_pCondition )
        return m_pCondition->EvaluateCondition();
    else return true;
}


CL_Rect StoneRing::Tile::GetRect() const
{
    return CL_Rect(m_X , m_Y , m_X + 1, m_Y +1);
}



void StoneRing::Tile::Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC)
{
    // Get the graphic guy
    // Get our tilemap or sprite
    // Blit it

    // static GraphicsManager * GM = GraphicsManager::GetInstance();

    if ( !IsSprite() )
    {
        CL_Sprite tilemap = m_Graphic.asTilemap->GetTileMap();

        //        void draw(  const CL_Rect& src, const CL_Rect& dest, CL_GraphicContext* context = 0);
        int mapx, mapy;
        mapx = m_Graphic.asTilemap->GetMapX();
        mapy = m_Graphic.asTilemap->GetMapY();
        CL_Rect srcRect((mapx << 5) + src.left, (mapy << 5) + src.top,
                        (mapx << 5) + src.right, (mapy << 5) + src.bottom);


        tilemap.draw(GC,srcRect, dst);

    }
    else
    {
        Update();
        m_sprite.draw(GC, dst);
    }

}

void StoneRing::Tile::Update()
{
    if (IsSprite()) m_sprite.update();
}

int StoneRing::Tile::GetDirectionBlock() const
{

    int block = 0;

    if ( cFlags & TIL_BLK_NORTH)
        block |= BLK_NORTH;
    if ( cFlags & TIL_BLK_SOUTH)
        block |= BLK_SOUTH;
    if ( cFlags & TIL_BLK_EAST)
        block |= BLK_EAST;
    if ( cFlags & TIL_BLK_WEST)
        block |= BLK_WEST;

    return block;
}







