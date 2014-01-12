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
#include "IApplication.h"
#include "Level.h"
#include "GraphicsManager.h"
#include "ItemManager.h"
#include "MonsterRegion.h"
#include "MonsterGroup.h"
#include "SoundManager.h"
#include "Party.h"





typedef unsigned int uint;


using namespace StoneRing;
using std::string;


#ifndef _WIN32
using std::max;
using std::min;
using std::abs;
#endif

///////////////////////////////////////////////////////////////////////////
// Tiles
///////////////////////////////////////////////////////////////////////////

Tiles::Tiles()
{
}

Tiles::~Tiles()
{
}


std::list<Tile*>::const_iterator Tiles::GetTilesBegin()const
{
    return m_tiles.begin();
}

std::list<Tile*>::const_iterator Tiles::GetTilesEnd()const
{
    return m_tiles.end();
}

bool Tiles::handle_element(eElement element, Element * pElement)
{
    if(element == ETILE)
    {
        m_tiles.push_back ( dynamic_cast<Tile*>(pElement) );
        return true;
    }

    return false;
}

void Tiles::load_attributes(clan::DomNamedNodeMap attributes)
{
}

///////////////////////////////////////////////////////////////////////////
// MappableObjects
///////////////////////////////////////////////////////////////////////////

bool GraphicDrawSort(Graphic* pObj1, Graphic* pObj2){
	short z1 = pObj1->GetZOrder();
	short z2 = pObj2->GetZOrder();
	if(z1 == z2){
		if(pObj1->GetRect().get_top_left().x == pObj2->GetRect().get_top_left().x){
			if(pObj1->IsTile() && pObj2->IsTile()){
				Tile* pTile1 = dynamic_cast<Tile*>(pObj1);
				Tile* pTile2 = dynamic_cast<Tile*>(pObj2);
				return pTile1->GetStackOrder() < pTile2->GetStackOrder();
			}else{
				// Compare by pointer, just for consistency
				return pObj1 < pObj2;
			}
		}else{
			return pObj1->GetRect().get_top_left().x < pObj2->GetRect().get_top_left().x;
		}
	}else{
		return z1 < z2;
	}
}

class QTNodeDrawer : public Level::MOQuadtree::OurNodeVisitor {
public:
    QTNodeDrawer(clan::Canvas gc, const clan::Point& offset, const Level& level):m_gc(gc),m_offset(offset),m_level(level){
    }
    virtual ~QTNodeDrawer(){
    }
    
    virtual bool Visit ( const Level::MOQuadtree::OurNode* pNode ){
        Quadtree::Geometry::Square<float> nodeSquare = pNode->GetSquare();
        clan::Rect rect(32*(nodeSquare.GetCenter().GetX() - nodeSquare.GetSize() / 2),
                     32*(nodeSquare.GetCenter().GetY() - nodeSquare.GetSize() / 2),
                     32*(nodeSquare.GetCenter().GetX() + nodeSquare.GetSize() / 2),
                     32*(nodeSquare.GetCenter().GetY() + nodeSquare.GetSize() / 2));
        rect.translate(m_offset);
#if !defined(NDEBUG)
        m_level.DrawDebugBox(m_gc,rect);
#endif
        return true;
    }
private:
    clan::Canvas m_gc;
    clan::Point m_offset;
    const Level &m_level;
};

class FindFloaters: public Level::TileQuadtree::OurVisitor { 
public:
    FindFloaters(){
    }
    virtual ~FindFloaters(){
    }
    virtual bool Visit(Tile *pTile, const Level::TileQuadtree::Node* pNode){
        m_tiles.push_back(pTile);
        return true;
    }
    std::list<Tile*>::const_iterator begin() const { return m_tiles.begin(); }
    std::list<Tile*>::const_iterator end() const { return m_tiles.end(); }
private:
    std::list<Tile*> m_tiles;
};

class FindPlayer : public Level::MOQuadtree::OurVisitor{
public:
	FindPlayer():m_player(NULL){}
	virtual ~FindPlayer(){}
    virtual bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
		MappablePlayer * player = dynamic_cast<MappablePlayer*>(pMO);
		if(player != NULL){
			m_player = player;
			return false;
		}else{
			return true;
		}
    }    
   	MappablePlayer* m_player;
};


class FindMappableObjects: public Level::MOQuadtree::OurVisitor{
public:
    FindMappableObjects(){
    }
    virtual ~FindMappableObjects(){
    }
    
    virtual bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
		m_mos.push_back(pMO);
        return true;
    }
    std::list<MappableObject*>::const_iterator begin() const {
        return m_mos.begin();
    }
    std::list<MappableObject*>::const_iterator end() const {
        return m_mos.end();
    }
    
    
    template <class Compare>
    void sort(Compare cmp){
        m_mos.sort(cmp);
	}
    
private:
    std::list<MappableObject*> m_mos;
};

class MappableObjectsPrinter: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectsPrinter(){}
    virtual ~MappableObjectsPrinter(){}
    virtual bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        clan::Rect spriteRect = pMO->GetSpriteRect();
        clan::Rect tileRect = pMO->GetTileRect();
        std::cout << '\t' << pMO->GetName();

        std::cout << " @ " << tileRect.get_top_left().x << ',' << tileRect.get_top_left().y 
                  <<' '
                  << tileRect.get_width() << 'x' << tileRect.get_height();
        std::cout << std::endl;
        return true;
    }
};

class ContainsSolidMappableObjects: public Level::MOQuadtree::OurVisitor{
public:
    ContainsSolidMappableObjects(MappableObject* pMO,const clan::Rect& destRect):m_pMO(pMO),m_destRect(destRect),m_contains(false){}
    virtual ~ContainsSolidMappableObjects(){}
    
    virtual bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        // TODO is this contains right?
        if(pMO != m_pMO && pMO->EvaluateCondition() && 
				(pMO->GetTileRect() == m_destRect || pMO->GetTileRect().is_overlapped(m_destRect))
				&& pMO->IsSolid() && m_pMO->IsFlying() == pMO->IsFlying()){
			// Note: You only collide with one object at a time
			if(m_pMO->DoesStep()) // Only if the collider (not collidee) is the player
				pMO->ProvokeEvents(Event::COLLIDE);
            m_contains = true;
            return false;
        }else return true;
    }    
    bool DidContainSolidMO() const { return m_contains; }
private:
    MappableObject* m_pMO;
    clan::Rect m_destRect;
    bool m_contains;
};


class MappableObjectDrawer: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectDrawer(const clan::Canvas& gc, const clan::Point& offset):m_gc(gc),m_offset(offset){
    }
    virtual ~MappableObjectDrawer(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        if(pMO->EvaluateCondition()){
            // Magically make a circle from a square
            Quadtree::Geometry::Circle<float> circle = pNode->GetSquare();
            m_gc.fill_circle(circle.GetCenter().GetX() * 32 + m_offset.x,
                            circle.GetCenter().GetY()*32 + m_offset.y,
                            circle.GetRadius()*32,
                            clan::Colorf(0.5f+ (1.05 * float(pNode->GetDepth())),1.0f,0.0f,0.1f));
        }
        return true;
    }
private:
    clan::Canvas m_gc;
    clan::Point m_offset;
};

class MappableObjectUpdater: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectUpdater(){}
    virtual ~MappableObjectUpdater(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        if(pMO->EvaluateCondition()){
            pMO->Update();
        }
        return true;
    }
};

class MappableObjectFreezer: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectFreezer(){}
    virtual ~MappableObjectFreezer(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        pMO->PushNavigator(new StillNavigator(*pMO));
        return true;
    }
};

class MappableObjectNavigatorPopper: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectNavigatorPopper(){}
    virtual ~MappableObjectNavigatorPopper(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode) {
        delete pMO->PopNavigator();
        return true;
    }    
};

class MappableObjectFindByName: public Level::MOQuadtree::OurVisitor{
public:
    MappableObjectFindByName(const std::string& name):m_pObject(NULL),m_name(name){}
    virtual ~MappableObjectFindByName(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        if(pMO->GetName() == m_name){
            m_pObject = pMO;
            return false;
        }
        return true;
    }
    MappableObject* GetObject() const { return m_pObject; }
private:
    MappableObject* m_pObject;
    std::string m_name;
};

class MappableObjectCollector: public Level::MOQuadtree::OurVisitor{
public:
    MappableObjectCollector(std::list<MappableObject*>& list, const clan::Point& point):m_list(list),m_point(point){
    }
    virtual ~MappableObjectCollector(){}
    bool Visit(MappableObject * pMO, const Level::MOQuadtree::Node* pNode){
        if(pMO->GetTileRect().contains(m_point))
            m_list.push_back(pMO);
        return true;
    }
private:
    clan::Point m_point;
    std::list<MappableObject*>& m_list;
};
MappableObjects::MappableObjects()
{
}

MappableObjects::~MappableObjects()
{
}

std::list<MappableObject*>::const_iterator MappableObjects::GetMappableObjectsBegin() const
{
    return m_mappable_objects.begin();
}
std::list<MappableObject*>::const_iterator MappableObjects::GetMappableObjectsEnd() const
{
    return m_mappable_objects.end();
}


bool MappableObjects::handle_element(Element::eElement element, Element * pElement)
{
    if(element == EMAPPABLEOBJECT)
    {
        m_mappable_objects.push_back ( dynamic_cast<MappableObject*>(pElement) );
        return true;
    }

    return false;
}

void MappableObjects::load_attributes(clan::DomNamedNodeMap attributes)
{

}


///////////////////////////////////////////////////////////////////////////
// LevelHeader
///////////////////////////////////////////////////////////////////////////

LevelHeader::LevelHeader():m_pScript(NULL)
{
}

LevelHeader::~LevelHeader()
{
    delete m_pScript;
}


void LevelHeader::ExecuteScript() const
{
    if(NULL != m_pScript)
        m_pScript->ExecuteScript();
}

bool LevelHeader::handle_element(eElement element, Element * pElement)
{
    if(element == ESCRIPT)
    {
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        return true;
    }
    return false;
}

void LevelHeader::load_attributes(clan::DomNamedNodeMap attributes)
{
    m_nLevelWidth = get_required_uint("width",attributes);
    m_nLevelHeight = get_required_uint("height",attributes);
    m_music = get_required_string("music",attributes);
    m_bAllowsRunning = get_required_bool("allowsRunning",attributes);
}

/////////////////////////////////////////////////////////////////////////////
// Level
//
/////////////////////////////////////////////////////////////////////////////


Level::Level():m_pMonsterRegions(NULL),m_LevelWidth(0),m_LevelHeight(0),m_pScript(NULL)
,m_pHeader(NULL),m_mo_quadtree(NULL)
{

}


void Level::Invoke()
{
    if(m_pHeader)
    {
         m_pHeader->ExecuteScript();
    }
}


bool activeSolidMappableObject(const std::multimap<clan::Point,MappableObject*>::value_type &value)
{
    MappableObject * pMO = value.second;

    return pMO && pMO->IsSolid() && pMO->EvaluateCondition();
}

bool matchesMappableObject(const std::multimap<clan::Point,MappableObject*>::value_type &value,MappableObject *pMo)
{
    return value.second == pMo;
}

#if 0
bool Level::Contains_Solid_Mappable_Object(const clan::Point &point) const
{
        ContainsSolidMappableObjects contains(point);
        Quadtree::Geometry::Vector<float> center(
        //m_mappleObjects.Traverse(
        m_mo_quadtree->Traverse(
    
}
#endif


#if 0
Level::Level(const std::string &name,clan::ResourceManager * pResources): mpDocument(NULL)
{
    srand(time(0));
    // Get the level file name from resources

    std::string path = clan::String::load("Game/LevelPath", pResources);
    std::string filename = clan::String::load("Levels/" + name, pResources);

    // Load the level
    LoadLevel ( path + filename );
}
#endif

Level::~Level()
{
    for(uint x=0;x<m_LevelWidth;x++)
    {
        for(uint y=0;y<m_LevelHeight;y++)
        {
            for(std::list<Tile*>::iterator i = m_tiles[x][y].begin();
                i != m_tiles[x][y].end();
                i++)
            {
                delete *i;
            }

        }

    }
    delete m_mo_quadtree;
}

int Level::Get_Cumulative_Side_Block_At_Point
(const clan::Point &point) const
{
    int block =0;

    std::list<Tile*> tileList = m_tiles[point.x][point.y];

    for(std::list<Tile*>::const_iterator iter = tileList.begin();
        iter != tileList.end();
        iter++)
    {
        if((*iter)->EvaluateCondition())
            block |= (*iter)->GetSideBlock();
    }

    return block;
}

bool Level::Get_Cumulative_Hotness_At_Point(const clan::Point &point) const
{

    std::list<Tile*> tileList = m_tiles[point.x][point.y];

    for(std::list<Tile*>::iterator iter = tileList.begin();
        iter != tileList.end();
        iter++)
    {
        if((*iter)->EvaluateCondition())
            if((*iter)->IsHot()) return true;
    }

    return false;

}

clan::Rect Level::calc_tile_bounds( const clan::Rect& src_pixels, const clan::Rect& dst_pixels ) const
{
	return clan::Rect(clan::Point(src_pixels.get_top_left().x/32,src_pixels.get_top_left().y/32),clan::Size(src_pixels.get_width()/32+1,src_pixels.get_height()/32+1));
}

void Level::Draw(const clan::Rect& src, const clan::Rect& dst, clan::Canvas& GC, bool draw_mos, bool draw_borders, bool draw_debug)
{
	draw_floor_tiles(GC,src,dst);
	draw_object_layer(GC,src,dst,draw_mos,draw_borders, draw_debug);
	//draw_flying_layer(GC,src,dst,
}

void Level::draw_floor_tiles(clan::Canvas& gc, const clan::Rect &src, const clan::Rect &dst)
{
    clan::Point offset(dst.left-src.left,dst.bottom-src.bottom);    
	clan::Rect tile_bounds = calc_tile_bounds(src,dst);

    // Regular tiles, not floaters
    for(int tileX = tile_bounds.left; tileX <= tile_bounds.right; tileX++)
    {
        for(int tileY =tile_bounds.top; tileY <= tile_bounds.bottom; tileY++)
        {
            clan::Point p(tileX,tileY);

            if(p.x >=0 && p.y >=0 && p.x < m_LevelWidth && p.y < m_LevelHeight)
            {
                std::list<Tile*>::iterator end = m_tiles[p.x][p.y].end();
                for(std::list<Tile*>::iterator i = m_tiles[p.x][p.y].begin();
                    i != end;
                    i++)
                {
                    Tile * pTile = *i;
					if(pTile->GetZOffset() != 0)
						continue;
                    if(pTile->IsFloater())
                        continue;                    
                    if(pTile->EvaluateCondition())
                    {
						pTile->Draw(gc,offset);
                        for(std::set<Tile::Visitor*>::const_iterator iter = m_tile_visitors.begin();
                            iter != m_tile_visitors.end();iter++){
                            pTile->Visit(*iter,gc,offset);
                        }
                    }
                }
            }
        }
    }

    

}

void Level::AddTileVisitor ( Tile::Visitor* pVisitor )
{
    m_tile_visitors.insert(pVisitor);
}

void Level::RemoveTileVisitor ( Tile::Visitor* pVisitor )
{
    m_tile_visitors.erase(pVisitor);
}





void Level::draw_object_layer(clan::Canvas& gc, const clan::Rect &src, const clan::Rect &dst, bool bDrawMos, bool bDrawBorders, bool bDrawDebug)
{
    clan::Point offset(dst.left-src.left,dst.bottom-src.bottom);

    Quadtree::Geometry::Vector<float> center(src.get_center().x/32,src.get_center().y/32);
    Quadtree::Geometry::Rect<float> rect(center,src.get_width()/32,src.get_height()/32);
    FindMappableObjects finder;
	
	std::list<Graphic*> graphics;
	// Add any fence tiles here
	clan::Rect tile_bounds = calc_tile_bounds(src,dst);
	for(int x=tile_bounds.left;x<=tile_bounds.right;x++){
		for(int y=tile_bounds.top;y<=tile_bounds.bottom;y++){
			if(x >=0 && y >= 0 && x < m_LevelWidth && y < m_LevelHeight){ 
				for(std::list<Tile*>::const_iterator it = m_tiles[x][y].begin(); it != m_tiles[x][y].end(); it++){
					if(((*it)->GetZOffset() || (*it)->IsFloater()) && (*it)->EvaluateCondition()){
						graphics.push_back(*it);
					}
				}
			}
		}
	}
    
    if(bDrawMos){
		m_mo_quadtree->Traverse(finder,rect);
		for(std::list<MappableObject*>::const_iterator it = finder.begin(); it != finder.end(); it++){
			if(!(*it)->IsFlying())
				graphics.push_back(*it);
		}
	}
	// Add any fence tile graphics
    graphics.sort(GraphicDrawSort);
    
    for(std::list<Graphic*>::const_iterator iter = graphics.begin();
        iter != graphics.end(); iter++){

		(*iter)->Draw(gc,offset);
#if !defined(NDEBUG)
        if(bDrawBorders && !(*iter)->IsTile()){
            clan::Rect tileRect = (*iter)->GetRect();
       
            tileRect.translate(offset);
            gc.draw_box(tileRect,clan::Colorf(1.0f,1.0f,0.0f,0.5f));
        }else if((*iter)->IsTile()){
			Tile* pTile = dynamic_cast<Tile*>(*iter);
			if(pTile){
				for(std::set<Tile::Visitor*>::const_iterator iter = m_tile_visitors.begin();
					iter != m_tile_visitors.end();iter++){
					pTile->Visit(*iter,gc,offset);
				}				
			}
		}
#endif
		
    }
    
    /* Draw flyers */
    for(std::list<MappableObject*>::const_iterator iter = finder.begin();
        iter != finder.end(); iter++){
		if((*iter)->IsFlying()){
			(*iter)->Draw(gc,offset);
#if !defined(NDEBUG)
			if(bDrawBorders){
				clan::Rect tileRect = (*iter)->GetTileRect();
				clan::Rect spriteRect(tileRect.get_top_left() * 32, tileRect.get_size() * 32);
		
				spriteRect.translate(offset);
				gc.draw_box(spriteRect,clan::Colorf(1.0f,0.0f,1.0f,0.5f));
			}
#endif
		}
    }
    
#if !defined(NDEBUG)
    if(bDrawDebug){
        MappableObjectDrawer drawer(gc,offset);
        m_mo_quadtree->Traverse(drawer,rect);
    }
#endif

    ++m_nFrameCount;
}




void Level::Move_Mappable_Object(MappableObject *pMO, Direction dir, const clan::Rect& tiles_from, const clan::Rect& tiles_to)
{
    assert ( pMO != NULL );
    Quadtree::Geometry::Vector<float> from_center(tiles_from.get_center().x,tiles_from.get_center().y);
    Quadtree::Geometry::Rect<float> from_rect(from_center,tiles_from.get_width(),tiles_from.get_height());
    
    Quadtree::Geometry::Vector<float> to_center(tiles_to.get_center().x,tiles_to.get_center().y);
    Quadtree::Geometry::Rect<float> to_rect(to_center,tiles_to.get_width(),tiles_to.get_height());
    m_mo_quadtree->MoveObject(pMO,from_rect,to_rect);
    if(pMO->DoesStep())
        Step(pMO->GetPosition() + dir.ToScreenVector());
}

void Level::Add_Mappable_Object ( MappableObject* pMO)
{
    assert ( pMO );
    clan::Rect rect = pMO->GetTileRect();
    Quadtree::Geometry::Rect<float> location(
        Quadtree::Geometry::Vector<float>(rect.get_center().x,rect.get_center().y),
                                          rect.get_width(),rect.get_height());
    
    m_mo_quadtree->Add(location,pMO);
    pMO->Placed();    
}


bool Level::CanMove ( MappableObject* pObject, const clan::Rect& tiles_currently, const clan::Rect& tiles_destination ) const
{
    Direction dir;
    clan::Point topleft = tiles_currently.get_top_left();
    clan::Point dest_topleft = tiles_destination.get_top_left();
    clan::Vec2<int> vector;
    if(topleft.x < dest_topleft.x){
        dir = Direction::EAST;
    }else if(topleft.x > dest_topleft.x){
        dir = Direction::WEST;
    }else if(topleft.y < dest_topleft.y){
        dir = Direction::SOUTH;
    }else{
        dir = Direction::NORTH;
    }
    
    vector = dir.ToScreenVector();
    
    std::list<clan::Point> edge;
    pObject->CalculateEdgePoints(topleft,dir,&edge);
    for(std::list<clan::Point>::const_iterator iter = edge.begin();
        iter != edge.end(); iter++){
        clan::Point tile = *iter;
        clan::Point newtile = tile + vector;
        if(!Check_Direction_Block(pObject,dir,tile,newtile)){
            return false;
        }
    }
    
    
    if(pObject->IsSolid()){    
        Quadtree::Geometry::Vector<float> center(tiles_destination.get_center().x,tiles_destination.get_center().y);
        Quadtree::Geometry::Rect<float>  rect(center,tiles_destination.get_width(),tiles_destination.get_height());
        
        ContainsSolidMappableObjects solidmos(pObject,tiles_destination);
        m_mo_quadtree->Traverse(solidmos,rect);
        
        if(solidmos.DidContainSolidMO()){
            return false;
        }
    }
    return true;
}


bool Level::Move(MappableObject* pObject, const clan::Rect& tiles_currently, const clan::Rect& tiles_destination)
{
    Direction dir;
    clan::Point topleft = tiles_currently.get_top_left();
    clan::Point dest_topleft = tiles_destination.get_top_left();
    clan::Vec2<float> vector;
    if(topleft.x < dest_topleft.x){
        dir = Direction::EAST;
    }else if(topleft.x > dest_topleft.x){
        dir = Direction::WEST;
    }else if(topleft.y < dest_topleft.y){
        dir = Direction::SOUTH;
    }else{
        dir = Direction::NORTH;
    }
    
    if(CanMove(pObject,tiles_currently,tiles_destination))
        Move_Mappable_Object(pObject,dir,tiles_currently,tiles_destination); 
    else return false;
    
    return true;
}


bool Level::Check_Direction_Block ( MappableObject * pMo, Direction dir, const clan::Point& tile, const clan::Point& dest_tile )const
{
    if(dest_tile.x <0 || dest_tile.y <0 || dest_tile.x >= m_LevelWidth || dest_tile.y >= m_LevelHeight)
        return false;
    if(!pMo->IsFlying()
        &&
        (Get_Cumulative_Side_Block_At_Point(tile) & MappableObject::ConvertDirectionToSideBlock(dir.opposite()))
        ||
        (Get_Cumulative_Side_Block_At_Point(dest_tile) & MappableObject::ConvertDirectionToSideBlock(dir))
        || (pMo->RespectsHotness() && Get_Cumulative_Hotness_At_Point(dest_tile))
        ){
        return false;
    }
    
    return true;
}



void Level::MoveMappableObjects(const clan::Rect &src)
{
    Quadtree::Geometry::Vector<float> center(src.get_center().x,src.get_center().y);
    Quadtree::Geometry::Rect<float> rect(center,src.get_width(),src.get_height());
    
    // Oh yeah. I can't move them with a visitor because you can't traverse and move
    // at the same time.
    FindMappableObjects finder;
    
    m_mo_quadtree->Traverse(finder,rect);
    
    for(std::list<MappableObject*>::const_iterator iter = finder.begin();
        iter != finder.end(); iter++){
        (*iter)->Move(*this);
    }
}


// All AM's from tiles fire, as do any step events
void Level::Step(const clan::Point &target)
{
    // First, check for a battle here (events come after)
    if(m_pMonsterRegions != NULL)
    {
		if(!m_tiles[target.x][target.y].empty()){
			MonsterRegion * pRegion = m_pMonsterRegions->GetMonsterRegion(m_tiles[target.x][target.y].back()->GetMonsterRegion());
			if(pRegion){
				float r = ranf();
				if(r < pRegion->GetEncounterRate()){
					MonsterGroup * pGroup = pRegion->GetMonsterGroup();
					IApplication::GetInstance()->StartBattle (*pGroup, pRegion->GetBackdrop());		
				}
			}
		}
        
    }
    // Second, process any MO step events you may have triggered
    Quadtree::Geometry::Vector<float> center(0.5f + float(target.x),0.5f + float(target.y));
    Quadtree::Geometry::Square<float> square(center,1);
    FindMappableObjects finder;
    m_mo_quadtree->Traverse(finder,square);


    for(std::list<MappableObject*>::const_iterator iter = finder.begin();
        iter != finder.end();
        iter++)
    {
        MappableObject * pMo = *iter;
        if(!RectContains(pMo->GetTileRect(),target))
            continue;

        if((pMo)->EvaluateCondition())
        {
            // This MO needs to be stepped on
            //(*i)->provokeEvents ( Event::STEP );
            (pMo)->ProvokeEvents(Event::STEP);
        }
    }
    
    Party * pParty = IApplication::GetInstance()->GetParty();
    for(int i=0;i<pParty->GetCharacterCount();i++)
    {
        Character *pChar = dynamic_cast<Character*>(pParty->GetCharacter(i));
        pChar->StatusEffectRound();
    }

    Activate_Tiles_At(target.x,target.y);
}


void Level::Activate_Tiles_At ( uint x, uint y )
{
    std::list<Tile*> tileList = m_tiles[x][y];

    for(std::list<Tile*>::const_iterator iter = tileList.begin();
        iter != tileList.end();
        iter++)
    {
        Tile *pTile = *iter;
        if(pTile->EvaluateCondition())
        {
            if ( pTile->HasScript() )
            {
                (*iter)->Activate();
            }
        }
    }

}

// Any talk events fire (assuming they meet conditions)
void Level::Talk(const clan::Point &target, bool prod)
{
    Quadtree::Geometry::Vector<float> center(0.0f + float(target.x),0.0f + float(target.y));
    Quadtree::Geometry::Square<float> square(center,1);
    FindMappableObjects finder;
    m_mo_quadtree->Traverse(finder,square);

    for(std::list<MappableObject*>::const_iterator iter = finder.begin();
        iter != finder.end();
        iter++)
    {
        MappableObject * pMo = *iter;
        if(!RectContains<int>(pMo->GetTileRect(),target)){
#ifndef NDEBUG
            clan::Rect rect = pMo->GetTileRect();
            std::cout << '(' << rect.get_top_left().x << ',' << rect.get_top_left().y << "),"
                        << '(' << rect.get_width() << 'x' << rect.get_height() << ')'
                        << " Point: " << target.x << ',' << target.y
                        << std::endl;
#endif
            continue;
        }
       

        if((pMo)->EvaluateCondition())
        {
            if(!prod)
            {
                // This MO needs to be talked to
                if((pMo)->ProvokeEvents ( Event::TALK ))
                    break; // we only do one
            }
            else
            {
                // Prod!
                // (Can't prod things that aren't solid. They aren't in your way anyways)
                // And if it has no movement, prodding it does nothing.
                if((pMo)->IsSolid())
                {
                    (pMo)->Prod();
                }
                
                if((pMo)->ProvokeEvents ( Event::ACT ))
					break; // policy is to only do the first one with a true condition

            }


        }
    }

    if(m_bMarkedForDeath ) delete this;

}

// Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
void Level::Update(const clan::Rect & updateRect)
{
    Quadtree::Geometry::Vector<float> center(updateRect.get_center().x,updateRect.get_center().y);
    Quadtree::Geometry::Rect<float> rect(center,updateRect.get_width(),updateRect.get_height());
    
    MappableObjectUpdater updater;
    m_mo_quadtree->Traverse(updater,rect);    
}

void Level::FreezeMappableObjects()
{
    MappableObjectFreezer freezer;
    m_mo_quadtree->TraverseAll(freezer);
}

void Level::UnfreezeMappableObjects()
{
    MappableObjectNavigatorPopper popper;
    m_mo_quadtree->TraverseAll(popper);
}

MappableObject* Level::GetMappableObjectByName(const std::string &name) const
{
    MappableObjectFindByName finder(name);
    m_mo_quadtree->TraverseAll(finder);
    return finder.GetObject();
}



#if  !defined(NDEBUG) 
void Level::DumpMappableObjects() const
{
    MappableObjectsPrinter printVisitor;
    
    m_mo_quadtree->TraverseAll(printVisitor);    
}

void Level::DrawDebugBox ( clan::Canvas gc, const clan::Rect& pixelRect ) const
{
    gc.draw_box(pixelRect,clan::Colorf(1.0f,1.0f,1.0f,0.5f));
}

void Level::DrawMOQuadtree(clan::Canvas gc, const clan::Point& offset) const
{
    QTNodeDrawer drawer(gc,offset,*this);    
    m_mo_quadtree->TraverseNodes(
        drawer);
}


void Level::AddPathTile ( const clan::Point& pt )
{
    m_pathPoints.insert(pt);
}

void Level::ClearPath()
{
    m_pathPoints.clear();
}




#endif


// Sort tiles on zOrder
bool Level::Tile_Sort_Criterion ( const Tile * p1, const Tile * p2)
{
    return p1->GetZOrder() < p2->GetZOrder();
}

void Level::LoadFromFile(const std::string &filename, bool resource)
{
    clan::DomDocument doc;
    clan::IODevice file;
	if(resource) file = IApplication::GetInstance()->OpenResource(filename);
	else {	
		file = clan::File(filename);
	}
    doc.load(file);

    clan::DomElement levelNode = doc.named_item("level").to_element();
    Element::Load(levelNode);
}

void Level::Load(const std::string &name, clan::ResourceManager& resources)
{
    std::string path = String_load("Game/LevelPath", resources);
    std::string filename = String_load("Levels/" + name, resources);

    m_resource_name = name;
    LoadFromFile(path + filename);
}

bool Level::handle_element(Element::eElement element, Element * pElement)
{
    switch(element)
    {
    case ELEVELHEADER:
        m_pHeader = dynamic_cast<LevelHeader*>(pElement);

        // Based on the header, lets preallocate our tiles.
        m_LevelWidth = m_pHeader->GetLevelWidth();
        m_LevelHeight = m_pHeader->GetLevelHeight();
        
        Create_MOQuadtree();
        
        m_bAllowsRunning = m_pHeader->AllowsRunning();
        m_music = m_pHeader->GetMusic();
        m_tiles.resize( m_LevelWidth );

        for(uint x=0;x< m_LevelWidth; x++)
        {
            m_tiles[x].resize ( m_LevelHeight );
        }
        break;
    case ETILES:
        {
            Tiles *pTiles = dynamic_cast<Tiles*>(pElement);
            for(std::list<Tile*>::const_iterator it = pTiles->GetTilesBegin();
                it != pTiles->GetTilesEnd(); it++)
                Load_Tile ( *it );
            break;
        }
    case EMAPPABLEOBJECTS:
        {
            MappableObjects *pMOs = dynamic_cast<MappableObjects*>(pElement);
            for(std::list<MappableObject*>::const_iterator it = pMOs->GetMappableObjectsBegin();
                it != pMOs->GetMappableObjectsEnd(); it++)
                Load_Mo ( *it );
        break;
        }
    case EMONSTERREGIONS:
        {
            if(m_pMonsterRegions != NULL) 
				throw XMLException("Already have monster regions on this level");

            m_pMonsterRegions = dynamic_cast<MonsterRegions*>(pElement);
            break;
        }
    default:
        return false;
    }
    return true;
}

void Level::load_attributes(clan::DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
#ifndef NDEBUG
    std::cout << "LEVEL NAME = " << m_name << std::endl;
#endif
}

void Level::load_finished()
{
    if(m_pHeader == NULL) throw XMLException("Level was missing levelHeader.");

    m_nFrameCount = m_nMoveCount = 0 ;
    m_bMarkedForDeath = false;
}

void Level::Create_MOQuadtree()
{
    // m_mappleObjects(Quadtree::Geometry::Square<int>(Quadtree::Geometry::Vector<int>(m_LevelWidth/2,m_LevelHeight/2), 
    //m_LevelWidth > m_LevelHeight?m_LevelWidth:m_LevelHeight)    
    if(m_mo_quadtree) delete m_mo_quadtree;
    m_mo_quadtree = new MOQuadtree(Quadtree::Geometry::Square<float>(Quadtree::Geometry::Vector<float>(m_LevelWidth/2,m_LevelHeight/2),
                                                                     (m_LevelWidth > m_LevelHeight?m_LevelWidth:m_LevelHeight)));
}


void Level::AddMappableObject ( MappableObject* pMO )
{
    Add_Mappable_Object(pMO);
}

void Level::RemoveMappableObject ( MappableObject* pMO )
{
    clan::Rect rect = pMO->GetTileRect();
    Quadtree::Geometry::Rect<float> location(
        Quadtree::Geometry::Vector<float>(rect.get_center().x,rect.get_center().y),
                                          rect.get_width(),rect.get_height());    
    
    m_mo_quadtree->Remove(location,pMO);   
}



void Level::Load_Mo ( MappableObject * moElement )
{
    Add_Mappable_Object(moElement);
}


void Level::Load_Tile ( Tile * tile)
{
    clan::Point point;
    if(tile->GetX() >= m_LevelWidth)
    {
        delete tile;
        throw XMLException("Tile found beyond range of indicated level width.");
    }
    else if (tile->GetY() >= m_LevelHeight)
    {
        delete tile;
        throw XMLException("Tile found beyond range of indicated level height.");
    }

    point.x = tile->GetX();
    point.y = tile->GetY();
	
	tile->SetStackOrder(m_tiles[point.x][point.y].size());

    m_tiles[ point.x ][point.y].push_back ( tile );

}

void Level::SerializeState ( std::ostream& out )
{ 
}

void Level::DeserializeState ( std::istream& in )
{

}


MappablePlayer* Level::GetPlayer() const
{
	FindPlayer finder;
	m_mo_quadtree->TraverseAll(finder);
	return finder.m_player;
}

#ifdef SR2_EDITOR

class MappableObjectXMLWriter: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectXMLWriter(clan::DomElement& parent,clan::DomDocument& doc):m_parent(parent),m_doc(doc){}
    virtual ~MappableObjectXMLWriter(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        MappableObject * pMo = dynamic_cast<MappableObject*>(pMO);
        if(pMo)
            m_parent.append_child(pMo->CreateDomElement(m_doc));
        return true;
    }
private:
    clan::DomElement& m_parent;
    clan::DomDocument& m_doc;
};


Level::Level(uint width, uint height):m_pMonsterRegions(NULL),m_LevelWidth(width),m_LevelHeight(height),m_pScript(NULL),
m_pHeader(NULL),m_mo_quadtree(NULL){
    m_tiles.resize( m_LevelWidth );

    for(uint x=0;x< m_LevelWidth; x++)
    {
        m_tiles[x].resize ( m_LevelHeight );
    }
    
    Create_MOQuadtree();
}

void Level::GrowLevelTo(uint width, uint height)
{
    m_LevelWidth = max(width,m_LevelWidth);
    m_LevelHeight = max(height,m_LevelHeight);
    
    m_tiles.resize( m_LevelWidth );

    for(uint x=0;x< m_LevelWidth; x++)
    {
        m_tiles[x].resize ( m_LevelHeight );
    }
    
    resize_mo_quadtree();
}

void Level::AddTile ( Tile* pTile )
{
    if(pTile->GetX() >= m_LevelWidth || pTile->GetY() >= m_LevelHeight )
        return;

    m_tiles[ pTile->GetX() ][ pTile->GetY()].push_back ( pTile );      
}

Tile* Level::PopTileAtPos(const clan::Point& pos)
{
    if(pos.x >= m_LevelWidth || pos.y >= m_LevelHeight )
        return NULL;
    
    if(m_tiles[pos.x][pos.y].empty()) 
        return NULL;
    
    Tile * pTile = m_tiles[pos.x][pos.y].back();
    m_tiles[pos.x][pos.y].pop_back();
    return pTile;
}

std::list<Tile*> Level::GetTilesAt(const clan::Point& pos) const
{
    return m_tiles[pos.x][pos.y];
}

bool Level::TilesAt(const clan::Point& pos) const 
{
    return !m_tiles[pos.x][pos.y].empty();
}






void Level::resize_mo_quadtree()
{
    FindMappableObjects finder;
    m_mo_quadtree->TraverseAll(finder);
    
    Create_MOQuadtree();
    
    for(std::list<MappableObject*>::const_iterator it = finder.begin();
        it != finder.end(); it++){
        AddMappableObject(*it);
    }
}


std::list<MappableObject*> Level::GetMappableObjectsAt(const clan::Point& point) const 
{
    std::list<MappableObject*> list;
    MappableObjectCollector collector(list,point);
    Quadtree::Geometry::Rect<float> location(
        Quadtree::Geometry::Vector<float>(point.x,point.y),1.0f,1.0f);    
    m_mo_quadtree->Traverse(collector,location);
   
    
    return list;
}

clan::DomElement Level::CreateDomElement(clan::DomDocument& doc) const 
{
    clan::DomElement element(doc,"level");
    element.set_attribute("name",m_name);
	

    clan::DomElement mappableObjects(doc,"mappableObjects");
    if(!m_pHeader){
		const_cast<Level*>(this)->m_pHeader = new LevelHeader();
		m_pHeader->SetMusic(m_music);
		m_pHeader->SetAllowsRunning(m_bAllowsRunning);
	}
	m_pHeader->SetLevelWidth(m_LevelWidth);
	m_pHeader->SetLevelHeight(m_LevelHeight);	
	element.append_child(m_pHeader->CreateDomElement(doc));
  
    clan::DomElement tiles(doc,"tiles");
    for(int x=0;x<m_tiles.size();x++){
        for(int y=0;y<m_tiles[x].size();y++){
            for(std::list<Tile*>::const_iterator it = m_tiles[x][y].begin(); it != m_tiles[x][y].end(); it++)
            {
                tiles.append_child((*it)->CreateDomElement(doc));
            }
        }
    }
    
  

    
    MappableObjectXMLWriter mo_writer(mappableObjects,doc);
    m_mo_quadtree->TraverseAll(mo_writer);
    
    element.append_child(mappableObjects);
    if(m_pMonsterRegions)
        element.append_child(m_pMonsterRegions->CreateDomElement(doc));    
    element.append_child(tiles);
    
    return element;
}

bool Level::WriteXML(const std::string& filename, bool force)const{
    clan::DomDocument newdoc;
    clan::File  os;

    if(!force && !os.open(filename,clan::File::create_new,clan::File::access_write)){
        return false;
    }else if(force){
        os.open(filename,clan::File::create_always,clan::File::access_write);
    }


    newdoc.append_child ( CreateDomElement(newdoc) );

    newdoc.save( os, true );

    os.close();
    
    return true;
}

void Level::AddMonsterRegion(MonsterRegion* region)
{
	if(!m_pMonsterRegions)
		m_pMonsterRegions = new MonsterRegions();
	m_pMonsterRegions->AddMonsterRegion(region);
}

void Level::RemoveMonsterRegion(MonsterRegion* region)
{
	assert(m_pMonsterRegions);
	m_pMonsterRegions->RemoveMonsterRegion(region);
}

void Level::AddTilesAt(const clan::Point& point, const std::list<Tile*>& tiles, bool overwrite)
{
	if(point.x < GetWidth() && point.y < GetHeight()){
		if(overwrite) {
			for(std::list<Tile*>::iterator it = m_tiles[point.x][point.y].begin(); 
				it != m_tiles[point.x][point.y].end();it++){
				delete *it;
			}
			m_tiles[point.x][point.y].clear();
		}
		for(std::list<Tile*>::const_iterator it = tiles.begin(); it != tiles.end(); it++){
			Tile * pTile = (*it)->clone();
			pTile->SetPos(point.x,point.y);
			AddTile(pTile);
		}		
	}
}

void Level::SetMusic(const std::string& music)
{
	m_music = music;
}

void Level::SetAllowsRunning(bool allowed)
{
	m_bAllowsRunning = allowed;
}


#endif