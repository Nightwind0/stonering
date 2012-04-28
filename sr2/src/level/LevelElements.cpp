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

namespace StoneRing { 


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
    uint p1value = (p1.y  *  IApplication::GetInstance()->GetDisplayRect().get_width()) + p1.x;
    uint p2value = (p2.y  * IApplication::GetInstance()->GetDisplayRect().get_width()) + p2.x;

    return p1value < p2value;
}



bool Tilemap::handle_element(Element::eElement element, Element * pElement)
{
    return false;
}

void Tilemap::load_attributes(CL_DomNamedNodeMap attributes)
{
    std::string name = get_required_string("mapname",attributes);
    m_sprite = GraphicsManager::GetTileMap(name);
    m_X = get_required_int("mapx",attributes);
    m_Y = get_required_int("mapy",attributes);
#if SR2_EDITOR
    m_sprite_string = name;
#endif
}


Tilemap::Tilemap()
{


}

Tilemap::~Tilemap()
{
}

Graphic::Graphic()
{
}

Graphic::~Graphic()
{
}


DirectionBlock::DirectionBlock():m_eDirectionBlock(0)
{
}

DirectionBlock::DirectionBlock(int i )
{
    m_eDirectionBlock = i;
}



void DirectionBlock::load_attributes(CL_DomNamedNodeMap attributes)
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


DirectionBlock::~DirectionBlock()
{
}

int DirectionBlock::GetDirectionBlock() const
{
    return m_eDirectionBlock;
}


Tile::Tile():m_pCondition(NULL),m_pScript(NULL),m_ZOrder(0),cFlags(0)
{
}


void Tile::load_attributes(CL_DomNamedNodeMap attributes)
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


bool Tile::handle_element(Element::eElement element, Element * pElement)
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

void Tile::load_finished()
{
    if (m_Graphic.asSpriteRef == NULL) throw CL_Exception("Tile didn't have tilemap or sprite ref.");
}

void Tile::Activate() // Call any attributemodifier
{
    // Run script
    if (m_pScript)
        m_pScript->ExecuteScript();
}

Tile::~Tile()
{
    delete m_pScript;
    delete m_pCondition;
    if ( IsSprite() )
        delete m_Graphic.asSpriteRef;
    else delete m_Graphic.asTilemap;
}

bool Tile::EvaluateCondition() const
{
    if ( m_pCondition )
        return m_pCondition->EvaluateCondition();
    else return true;
}


CL_Rect Tile::GetRect() const
{
    return CL_Rect(m_X , m_Y , m_X + 1, m_Y +1);
}



void Tile::Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC)
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

void Tile::Update()
{
    if (IsSprite()) m_sprite.update();
}

int Tile::GetDirectionBlock() const
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



#if SR2_EDITOR

CL_DomElement Tile::CreateDomElement(CL_DomDocument& doc)const
{
    CL_DomElement element(doc,"tile");

    element.set_attribute("xpos", IntToString ( m_X ) );
    element.set_attribute("ypos", IntToString ( m_Y ) );
    if(m_ZOrder >0 ) element.set_attribute("zorder", IntToString (m_ZOrder ) );
    if(IsFloater()) element.set_attribute("floater", "true");
    if(IsHot())     element.set_attribute("hot", "true");
    if(Pops())      element.set_attribute("pops","true");

    if(IsSprite())
    {
        element.append_child(m_Graphic.asSpriteRef->CreateDomElement(doc));
    }
    else
    {
       element.append_child(m_Graphic.asTilemap->CreateDomElement(doc));
    }

    if(m_pCondition)
    {
        element.append_child(m_pCondition->CreateDomElement(doc));
    }
    if( GetDirectionBlock() > 0)
    {
        DirectionBlock block( GetDirectionBlock() );
        CL_DomElement dirEl = block.CreateDomElement(doc);

        element.append_child( dirEl );
    }
    

    return element;
}


CL_DomElement DirectionBlock::CreateDomElement(CL_DomDocument& doc)const
{
    CL_DomElement element(doc,"directionBlock");
    element.set_attribute("north", (m_eDirectionBlock & BLK_NORTH )?"true":"false");
    element.set_attribute("south", (m_eDirectionBlock & BLK_SOUTH)?"true":"false");
    element.set_attribute("east",  (m_eDirectionBlock & BLK_EAST )?"true":"false" );
    element.set_attribute("west",  (m_eDirectionBlock & BLK_WEST )?"true":"false" );

    return element;
}

CL_DomElement Tilemap::CreateDomElement(CL_DomDocument& doc)const
{
    CL_DomElement element(doc,"tilemap");
    element.set_attribute("mapname",m_sprite_string);
    element.set_attribute("mapx",IntToString(m_X));
    element.set_attribute("mapy",IntToString(m_Y));
    return element;
}

CL_DomElement LevelHeader::CreateDomElement(CL_DomDocument& doc)const
{
    CL_DomElement levelHeader(doc,"levelHeader");
    levelHeader.set_attribute("music", m_music );
    levelHeader.set_attribute("width", IntToString(m_nLevelWidth) );
    levelHeader.set_attribute("height", IntToString(m_nLevelHeight) );
    levelHeader.set_attribute("allowsRunning", m_bAllowsRunning? "true" : "false"); 
    if(m_pScript){
        levelHeader.append_child(m_pScript->CreateDomElement(doc));
    }
    return levelHeader;
}
}
#endif

