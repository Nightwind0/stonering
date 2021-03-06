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
#include "GraphicsManager.h"

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
bool operator < (const clan::Point &p1, const clan::Point &p2)
{
    uint p1value = (p1.y  *  IApplication::GetInstance()->GetDisplayRect().get_width()) + p1.x;
    uint p2value = (p2.y  * IApplication::GetInstance()->GetDisplayRect().get_width()) + p2.x;

    return p1value < p2value;
}



bool Tilemap::handle_element(Element::eElement element, Element * pElement)
{
    return false;
}

void Tilemap::load_attributes(clan::DomNamedNodeMap attributes)
{
    std::string name = get_required_string("mapname",attributes);
    m_image = GraphicsManager::GetTileMap(name);
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


SideBlock::SideBlock():m_eSideBlock(0)
{
}

SideBlock::SideBlock(int i )
{
    m_eSideBlock = i;
}



void SideBlock::load_attributes(clan::DomNamedNodeMap attributes)
{
    bool north = get_required_bool("north",attributes);
    bool south = get_required_bool("south",attributes);
    bool east =  get_required_bool("east",attributes);
    bool west =  get_required_bool("west",attributes);

    if (north) m_eSideBlock |= BLK_NORTH;
    if (south) m_eSideBlock |= BLK_SOUTH;
    if (east) m_eSideBlock |= BLK_EAST;
    if (west) m_eSideBlock |= BLK_WEST;
}


SideBlock::~SideBlock()
{
}

int SideBlock::GetSideBlock() const
{
    return m_eSideBlock;
}


Tile::Tile():m_pCondition(NULL),m_pScript(NULL),m_ZOffset(0),cFlags(0),m_monster_region(-1)
{
}


void Tile::load_attributes(clan::DomNamedNodeMap attributes)
{
    m_X = get_required_int("xpos",attributes);
    m_Y = get_required_int("ypos",attributes);

    m_ZOffset = get_implied_int("zoffset",attributes,0);

    bool floater = get_implied_bool("floater",attributes,false);
    bool hot = get_implied_bool("hot",attributes,false);
    bool pops = get_implied_bool("pops",attributes,false);
	bool fence = get_implied_bool("fence",attributes,false);
	
	if(fence) m_ZOffset = 1;
	
	m_monster_region = get_implied_int("monster_region",attributes,-1);

    if (floater) cFlags |= TIL_FLOATER;
    if (hot) cFlags |= TIL_HOT;
}


bool Tile::handle_element(Element::eElement element, Element * pElement)
{
    switch (element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ETILEMAP:
        m_tilemap = dynamic_cast<Tilemap*>(pElement);
		m_image =  clan::Subtexture(m_tilemap->GetTileMap(), clan::Rect(clan::Point(m_tilemap->GetMapX()*32,m_tilemap->GetMapY()*32),clan::Size(32,32)));
        cFlags &= ~TIL_SPRITE;
        break;
    case ESPRITEREF:
    {
        SpriteRef* ref = dynamic_cast<SpriteRef*>(pElement);
        cFlags |= TIL_SPRITE;

        // Actually create the ref'd sprite here.
        // And assign to mpSprite
        m_sprite = GraphicsManager::CreateSprite( ref->GetRef() );
#ifdef SR2_EDITOR
        m_sprite_name = ref->GetRef();
#endif
        break;
    }//ESPRITEREF
    case ECONDITIONSCRIPT:
        m_pCondition = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EDIRECTIONBLOCK:
    {
        SideBlock *block = dynamic_cast<SideBlock*>(pElement);

        int db = block->GetSideBlock();

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
    if (m_image.is_null() && m_sprite.is_null()) throw XMLException("Tile didn't have tilemap or sprite ref.");
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
    delete m_pCondition;;
    //if(!IsSprite()) delete m_tilemap;
}

bool Tile::EvaluateCondition() const
{
    if ( m_pCondition )
        return m_pCondition->EvaluateCondition();
    else return true;
}


clan::Rect Tile::GetRect() const
{
    return clan::Rect(clan::Point(m_X*32,m_Y*32),clan::Size(32,32));
}


void Tile::Draw(clan::Canvas& gc, const clan::Point& dst)
{
	draw(clan::Rect(0,0,32,32),clan::Rect(dst + clan::Point(m_X*32,m_Y*32),clan::Size(32,32)),gc);
}


void Tile::draw(const clan::Rect &src, const clan::Rect &dst, clan::Canvas& GC)
{
    // Get the graphic guy
    // Get our tilemap or sprite
    // Blit it

    // static GraphicsManager * GM = GraphicsManager::GetInstance();

    if ( !IsSprite() )
    {
        /*clan::Subtexture tilemap = m_tilemap->GetTileMap();

        //        void draw(  const clan::Rect& src, const clan::Rect& dest, clan::Canvas* context = 0);
        int mapx, mapy;
        mapx = m_tilemap->GetMapX();
        mapy = m_tilemap->GetMapY();
		clan::Rect srcRect(clan::Point(mapx*32,mapy*32),clan::Size(32,32));


        tilemap.draw(GC,srcRect,dst);
        */
		clan::Image image(m_image);
		image.draw(GC, dst);
    }
    else
    {
        Update();
        m_sprite.draw(GC, dst);
    }

}

void Tile::Update()
{
    if (IsSprite()) m_sprite.update(1); // calc actual ms?
}

int Tile::GetSideBlock() const
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



#ifdef SR2_EDITOR

clan::DomElement Tile::CreateDomElement(clan::DomDocument& doc)const
{
    clan::DomElement element(doc,"tile");

    element.set_attribute("xpos", IntToString ( m_X ) );
    element.set_attribute("ypos", IntToString ( m_Y ) );
    if(m_ZOffset >0 ) element.set_attribute("zoffset", IntToString (m_ZOffset ) );
    if(IsFloater()) element.set_attribute("floater", "true");
    if(IsHot())     element.set_attribute("hot", "true");
	
	if(m_monster_region >= 0){
		element.set_attribute("monster_region",IntToString(m_monster_region));
	}

    if(IsSprite())
    {
        element.set_attribute("sprite",m_sprite_name);
    }
    else
    {
       element.append_child(m_tilemap->CreateDomElement(doc));
    }

    if(m_pCondition)
    {
        element.append_child(m_pCondition->CreateDomElement(doc));
    }
    if( GetSideBlock() > 0)
    {
        SideBlock block( GetSideBlock() );
        clan::DomElement dirEl = block.CreateDomElement(doc);

        element.append_child( dirEl );
    }
    

    return element;
}


Tile* Tile::clone() const {
	Tile * pTile = new Tile();
	if(IsSprite())
		pTile->m_sprite = GraphicsManager::CreateSprite(m_sprite_name,false);
	if(m_tilemap){
		pTile->m_tilemap = new Tilemap();
		*pTile->m_tilemap = *m_tilemap;
	}
	if(m_pCondition)
		*pTile->m_pCondition = *m_pCondition;
	if(m_pScript)
		*pTile->m_pScript = *m_pScript;
	pTile->m_ZOffset = m_ZOffset;
	pTile->m_X = m_X;
	pTile->m_Y = m_Y;
	pTile->m_monster_region = m_monster_region;
	pTile->cFlags = cFlags;
	pTile->m_sprite_name = m_sprite_name;
	return pTile;
}


clan::DomElement SideBlock::CreateDomElement(clan::DomDocument& doc)const
{
    clan::DomElement element(doc,"block");
    element.set_attribute("north", (m_eSideBlock & BLK_NORTH )?"true":"false");
    element.set_attribute("south", (m_eSideBlock & BLK_SOUTH)?"true":"false");
    element.set_attribute("east",  (m_eSideBlock & BLK_EAST )?"true":"false" );
    element.set_attribute("west",  (m_eSideBlock & BLK_WEST )?"true":"false" );

    return element;
}

clan::DomElement Tilemap::CreateDomElement(clan::DomDocument& doc)const
{
    clan::DomElement element(doc,"tilemap");
    element.set_attribute("mapname",m_sprite_string);
    element.set_attribute("mapx",IntToString(m_X));
    element.set_attribute("mapy",IntToString(m_Y));
    return element;
}

void LevelHeader::SetLevelHeight(uint height)
{
	m_nLevelHeight = height;
}

void LevelHeader::SetLevelWidth(uint width)
{
	m_nLevelWidth = width;
}

void LevelHeader::SetAllowsRunning(bool allowed)
{
	m_bAllowsRunning = allowed;
}

void LevelHeader::SetMusic(const std::string& music)
{
	m_music = music;
}

clan::DomElement LevelHeader::CreateDomElement(clan::DomDocument& doc)const
{
    clan::DomElement levelHeader(doc,"levelHeader");
    levelHeader.set_attribute("music", m_music );
    levelHeader.set_attribute("width", IntToString(m_nLevelWidth) );
    levelHeader.set_attribute("height", IntToString(m_nLevelHeight) );
    levelHeader.set_attribute("allowsRunning", m_bAllowsRunning? "true" : "false"); 
    if(m_pScript){
        levelHeader.append_child(m_pScript->CreateDomElement(doc));
    }
    return levelHeader;
}
#endif
}


