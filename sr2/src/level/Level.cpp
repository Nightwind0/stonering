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


#ifndef _MSC_VER
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

void Tiles::load_attributes(CL_DomNamedNodeMap attributes)
{
}

///////////////////////////////////////////////////////////////////////////
// MappableObjects
///////////////////////////////////////////////////////////////////////////

bool MappableObjectDrawSort(MappableObject* pObj1, MappableObject* pObj2){
    int y1 = pObj1->GetPixelPosition().y;
    int y2 = pObj2->GetPixelPosition().y;
    if(pObj1->IsFlying())
        y1 += IApplication::GetInstance()->GetDisplayRect().get_height();
    if(pObj2->IsFlying())
        y2 += IApplication::GetInstance()->GetDisplayRect().get_height();
    
    return y1 < y2;
}

class QTNodeDrawer : public Level::MOQuadtree::OurNodeVisitor {
public:
    QTNodeDrawer(CL_GraphicContext gc, const CL_Point& offset, const Level& level):m_gc(gc),m_offset(offset),m_level(level){
    }
    virtual ~QTNodeDrawer(){
    }
    
    virtual bool Visit ( const Level::MOQuadtree::OurNode* pNode ){
        Quadtree::Geometry::Square<float> nodeSquare = pNode->GetSquare();
        CL_Rect rect(32*(nodeSquare.GetCenter().GetX() - nodeSquare.GetSize() / 2),
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
    CL_GraphicContext m_gc;
    CL_Point m_offset;
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
        CL_Rect spriteRect = pMO->GetSpriteRect();
        CL_Rect tileRect = pMO->GetTileRect();
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
    ContainsSolidMappableObjects(MappableObject* pMO,const CL_Rect& destRect):m_pMO(pMO),m_destRect(destRect),m_contains(false){}
    virtual ~ContainsSolidMappableObjects(){}
    
    virtual bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        // TODO is this contains right?
        if(pMO != m_pMO && pMO->EvaluateCondition() && (pMO->GetTileRect() == m_destRect || pMO->GetTileRect().is_overlapped(m_destRect)) && pMO->IsSolid()){
            m_contains = true;
            return false;
        }else return true;
    }    
    bool DidContainSolidMO() const { return m_contains; }
private:
    MappableObject* m_pMO;
    CL_Rect m_destRect;
    bool m_contains;
};


class MappableObjectDrawer: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectDrawer(const CL_GraphicContext& gc, const CL_Point& offset):m_gc(gc),m_offset(offset){
    }
    virtual ~MappableObjectDrawer(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        if(pMO->EvaluateCondition()){
            // Magically make a circle from a square
            Quadtree::Geometry::Circle<float> circle = pNode->GetSquare();
            CL_Draw::circle(m_gc,circle.GetCenter().GetX() * 32 + m_offset.x,
                            circle.GetCenter().GetY()*32 + m_offset.y,
                            circle.GetRadius()*32,
                            CL_Colorf(0.5f+ (1.05 * float(pNode->GetDepth())),1.0f,0.0f,0.1f));
        }
        return true;
    }
private:
    CL_GraphicContext m_gc;
    CL_Point m_offset;
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

void MappableObjects::load_attributes(CL_DomNamedNodeMap attributes)
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

void LevelHeader::load_attributes(CL_DomNamedNodeMap attributes)
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
,m_pHeader(NULL),m_player(0,0),m_mo_quadtree(NULL),m_floater_quadtree(NULL)
{

}


void Level::Invoke()
{
    if(m_pHeader)
    {
        m_pHeader->ExecuteScript();
    }
}


bool activeSolidMappableObject(const std::multimap<CL_Point,MappableObject*>::value_type &value)
{
    MappableObject * pMO = value.second;

    return pMO && pMO->IsSolid() && pMO->EvaluateCondition();
}

bool matchesMappableObject(const std::multimap<CL_Point,MappableObject*>::value_type &value,MappableObject *pMo)
{
    return value.second == pMo;
}

#if 0
bool Level::Contains_Solid_Mappable_Object(const CL_Point &point) const
{
        ContainsSolidMappableObjects contains(point);
        Quadtree::Geometry::Vector<float> center(
        //m_mappleObjects.Traverse(
        m_mo_quadtree->Traverse(
    
}
#endif


#if 0
Level::Level(const std::string &name,CL_ResourceManager * pResources): mpDocument(NULL)
{
    srand(time(0));
    // Get the level file name from resources

    std::string path = CL_String::load("Game/LevelPath", pResources);
    std::string filename = CL_String::load("Levels/" + name, pResources);

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
    delete m_floater_quadtree;
}

int Level::Get_Cumulative_Direction_Block_At_Point(const CL_Point &point) const
{
    int directionBlock =0;

    std::list<Tile*> tileList = m_tiles[point.x][point.y];

    for(std::list<Tile*>::const_iterator iter = tileList.begin();
        iter != tileList.end();
        iter++)
    {
        if((*iter)->EvaluateCondition())
            directionBlock |= (*iter)->GetDirectionBlock();
    }

    return directionBlock;
}

bool Level::Get_Cumulative_Hotness_At_Point(const CL_Point &point) const
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


void Level::Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC, bool floaters,
                 bool highlightHot, bool indicateBlocks)
{
    //    int maxSrcX = max( ceil(dst.get_width() / 32.0), mLevelWidth );
    //    int maxSrcY = max( ceil(dst.get_height() / 32.0), mLevelHeight);
    int cornerx = static_cast<int>(src.left / 32);
    int cornery = static_cast<int>(src.top / 32);

    uint widthInTiles = static_cast<int>((int)ceil((double)src.right/32.0)) - cornerx;
    uint heightInTiles = static_cast<int>((int)ceil((double)src.bottom/32.0)) - cornery;

    uint widthInPx = widthInTiles * 32;
    uint heightInPx = heightInTiles * 32;


    CL_Rect exDst = dst; // expanded Dest

    // Make it as big as the full source tiles
    // (It maintains the top/left position when you do this)
    exDst.set_size(CL_Size( widthInPx, heightInPx ));

    // Move the top and left position out

    int newleftDelta = src.left - ((src.left / 32) * 32);
    int newtopDelta = src.top - ((src.top / 32) * 32);

    exDst.left -= newleftDelta;
    exDst.right -= newleftDelta;
    exDst.top -= newtopDelta;
    exDst.bottom -= newtopDelta;



    if(floaters)
    {
        CL_Point offset(dst.left-src.left,dst.bottom-src.bottom);
        Quadtree::Geometry::Vector<float> center(src.get_center().x,src.get_center().y);
        Quadtree::Geometry::Rect<float> rect(center,src.get_width(),src.get_height());
        FindFloaters finder;
        m_floater_quadtree->Traverse(finder,rect);
        for(std::list<Tile*>::const_iterator f = finder.begin();
            f != finder.end();
            f++)
        {
            Tile * floater = *f;
            // Possible optimization... instead of using is_overlapped, do a couple quick comparisons
            CL_Rect floaterRect(floater->GetX(),floater->GetY(),floater->GetX() + 32, floater->GetY() + 32);


            // Is this floater even on screen
            if(src.is_overlapped(floaterRect)){
                CL_Rect tileSrc(0,0,32,32);
                CL_Rect tileDst ( exDst.left  + floater->GetX() * 32,
                                    exDst.top + floater->GetY() * 32,
                                    exDst.left + floater->GetX() * 32 + 32,
                                    exDst.top + floater->GetY() * 32 + 32);

                if(floater->EvaluateCondition())
                {
                    floater->Draw(tileSrc, tileDst , GC );

                }
            }

        }

    }
    else
    {
        // Regular tiles, not floaters

        for(uint tileX = 0; tileX < widthInTiles; tileX++)
        {
            for(uint tileY =0; tileY < heightInTiles; tileY++)
            {

                CL_Point p( src.left / 32 + tileX, src.top /32 + tileY);


                if(p.x >=0 && p.y >=0 && p.x < m_LevelWidth && p.y < m_LevelHeight)
                {
                    CL_Rect tileSrc(0,0,32,32);
                    CL_Rect tileDst ( exDst.left  + (tileX << 5),
                                      exDst.top + (tileY << 5),
                                      exDst.left + (tileX << 5) + 32,
                                      exDst.top + (tileY << 5) + 32);


                    std::list<Tile*>::iterator end = m_tiles[p.x][p.y].end();
                    for(std::list<Tile*>::iterator i = m_tiles[p.x][p.y].begin();
                        i != end;
                        i++)
                    {


                        Tile * pTile = *i;
                        if(pTile->EvaluateCondition())
                        {
                            pTile->Draw(tileSrc, tileDst , GC );

#ifndef NDEBUG
                            if(indicateBlocks && m_pathPoints.count(CL_Point(tileX,tileY))){
                                CL_Draw::fill(GC,tileDst,CL_Colorf(0.8f,0.0f,0.8f,0.25f));
                            }
#endif

                            // Extra code for level editing
                            if(highlightHot && pTile->IsHot())
                            {
                                CL_Rect rect = tileDst;
                                rect.translate(dst.get_top_left());
                                CL_Draw::fill(GC,rect, CL_Colorf(1.0,0.0f,0.0f,0.4));
                            }                            
                            if(indicateBlocks)
                            {

                                int block = pTile->GetDirectionBlock();

                                if(block & BLK_WEST)
                                {
                                    CL_Draw::fill(GC,CL_Rectf(tileDst.left,tileDst.top,tileDst.left + 8, tileDst.bottom), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
                                }
                                if(block & BLK_EAST)
                                {
                                    CL_Draw::fill(GC,CL_Rectf(tileDst.right - 8, tileDst.top, tileDst.right,tileDst.bottom),CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
                                }
                                if(block & BLK_NORTH)
                                {
                                    CL_Draw::fill(GC,CL_Rectf(tileDst.left, tileDst.top, tileDst.right, tileDst.top +8), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
                                }
                                if(block & BLK_SOUTH)
                                {
                                    CL_Draw::fill(GC,CL_Rectf(tileDst.left,tileDst.bottom -8, tileDst.right, tileDst.bottom), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
                                }
                            }
                        } 






                    }
                }





            }
        }

    }


#ifndef NDEBUG
    if(indicateBlocks){
        DrawMOQuadtree(GC, dst.get_top_left() - src.get_top_left());
    }
#endif
}


void Level::DrawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC, bool bDrawDebug)
{
    CL_Point offset(dst.left-src.left,dst.bottom-src.bottom);
    Quadtree::Geometry::Vector<float> center(src.get_center().x,src.get_center().y);
    Quadtree::Geometry::Rect<float> rect(center,src.get_width(),src.get_height());
    FindMappableObjects finder;
    
    m_mo_quadtree->Traverse(finder,rect);    
    finder.sort(MappableObjectDrawSort);
    
    for(std::list<MappableObject*>::const_iterator iter = finder.begin();
        iter != finder.end(); iter++){
        (*iter)->Draw(GC,offset);
#if !defined(NDEBUG)
        if(bDrawDebug){
            CL_Rect tileRect = (*iter)->GetTileRect();
            CL_Rect spriteRect(tileRect.get_top_left() * 32, tileRect.get_size() * 32);
       
            spriteRect.translate(offset);
            CL_Draw::box(GC,spriteRect,CL_Colorf(1.0f,1.0f,0.0f,0.5f));
        }
#endif
    }
    
#if !defined(NDEBUG)
    if(bDrawDebug){
        MappableObjectDrawer drawer(GC,offset);
        m_mo_quadtree->Traverse(drawer,rect);
    }
#endif

    ++m_nFrameCount;
}


void Level::SetPlayerPos(const CL_Point &target)
{
    Add_Mappable_Object(&m_player);
}



void Level::Move_Mappable_Object(MappableObject *pMO, Direction dir, const CL_Rect& pixel_from, const CL_Rect& pixel_to)
{
    assert ( pMO != NULL );
    Quadtree::Geometry::Vector<float> from_center(pixel_from.get_center().x,pixel_from.get_center().y);
    Quadtree::Geometry::Rect<float> from_rect(from_center,pixel_from.get_width(),pixel_from.get_height());
    
    Quadtree::Geometry::Vector<float> to_center(pixel_to.get_center().x,pixel_to.get_center().y);
    Quadtree::Geometry::Rect<float> to_rect(to_center,pixel_to.get_width(),pixel_to.get_height());
    m_mo_quadtree->MoveObject(pMO,from_rect,to_rect);
    if(pMO->DoesStep())
        Step(pMO->GetPosition() + dir.ToScreenVector());
}

void Level::Add_Mappable_Object ( MappableObject* pMO)
{
    assert ( pMO );
    CL_Rect rect = pMO->GetTileRect();
    Quadtree::Geometry::Rect<float> location(
        Quadtree::Geometry::Vector<float>(rect.get_center().x,rect.get_center().y),
                                          rect.get_width(),rect.get_height());
    
    m_mo_quadtree->Add(location,pMO);
    
}


bool Level::CanMove ( MappableObject* pObject, const CL_Rect& tiles_currently, const CL_Rect& tiles_destination ) const
{
    Direction dir;
    CL_Point topleft = tiles_currently.get_top_left();
    CL_Point dest_topleft = tiles_destination.get_top_left();
    CL_Vec2<float> vector;
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
    
    std::list<CL_Point> edge;
    pObject->CalculateEdgePoints(topleft,dir,&edge);
    for(std::list<CL_Point>::const_iterator iter = edge.begin();
        iter != edge.end(); iter++){
        CL_Point tile = *iter;
        CL_Point newtile = tile + vector;
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


bool Level::Move(MappableObject* pObject, const CL_Rect& tiles_currently, const CL_Rect& tiles_destination)
{
    Direction dir;
    CL_Point topleft = tiles_currently.get_top_left();
    CL_Point dest_topleft = tiles_destination.get_top_left();
    CL_Vec2<float> vector;
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


bool Level::Check_Direction_Block ( MappableObject * pMo, Direction dir, const CL_Point& tile, const CL_Point& dest_tile )const
{
    if(dest_tile.x <0 || dest_tile.y <0 || dest_tile.x >= m_LevelWidth || dest_tile.y >= m_LevelHeight)
        return false;
    if(!pMo->IsFlying()
        &&
        (Get_Cumulative_Direction_Block_At_Point(dest_tile) & MappableObject::ConvertDirectionToDirectionBlock(dir))
        || (pMo->RespectsHotness() && Get_Cumulative_Hotness_At_Point(dest_tile))
        ){
        return false;
    }
    
    return true;
}



void Level::MoveMappableObjects(const CL_Rect &src)
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


void Level::DrawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC)
{
    Draw(src,dst,GC, true);
}


// All AM's from tiles fire, as do any step events
void Level::Step(const CL_Point &target)
{
    // First, check for a battle here (events come after)
    if(m_pMonsterRegions != NULL)
    {
        MonsterRegion *pRegion = m_pMonsterRegions->GetApplicableRegion(target.x,target.y);
        if(pRegion != NULL)
        {
            float rate = pRegion->GetEncounterRate();
            float value = ((float)rand() / ((float)RAND_MAX + 1)); // Generate number between 0 and 1.0

            if(value < rate)
            {
                // Then we have an encounter.
                // Let the region pick a group for us to battle
                MonsterGroup * pGroup = pRegion->GetMonsterGroup();

                IApplication::GetInstance()->StartBattle (*pGroup, pRegion->GetBackdrop());
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
            else if ((*iter)->Pops() )
            {
                IApplication::GetInstance()->PopLevelStack(false);
            }
        }
    }

}

// Any talk events fire (assuming they meet conditions)
void Level::Talk(const CL_Point &target, bool prod)
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
            CL_Rect rect = pMo->GetTileRect();
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


            }


        }
    }

    if(m_bMarkedForDeath ) delete this;

}

// Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
void Level::Update(const CL_Rect & updateRect)
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

void Level::DrawDebugBox ( CL_GraphicContext gc, const CL_Rect& pixelRect ) const
{
    CL_Draw::box(gc,pixelRect,CL_Colorf(1.0f,1.0f,1.0f,0.5f));
}

void Level::DrawMOQuadtree(CL_GraphicContext gc, const CL_Point& offset) const
{
    QTNodeDrawer drawer(gc,offset,*this);    
    m_mo_quadtree->TraverseNodes(
        drawer);
}

void Level::Create_Floater_Quadtree()
{
    m_floater_quadtree = new  TileQuadtree(Quadtree::Geometry::Square<float>(Quadtree::Geometry::Vector<float>(m_LevelWidth/2,m_LevelHeight/2),
                                                                     (m_LevelWidth > m_LevelHeight?m_LevelWidth:m_LevelHeight)));
}

void Level::AddPathTile ( const CL_Point& pt )
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

void Level::LoadFromFile(const std::string &filename)
{
    CL_DomDocument doc;
    CL_File file(filename);
    doc.load(file);

    CL_DomElement levelNode = doc.named_item("level").to_element();
    Element::Load(levelNode);
}

void Level::Load(const std::string &name, CL_ResourceManager& resources)
{
    std::string path = CL_String_load("Game/LevelPath", resources);
    std::string filename = CL_String_load("Levels/" + name, resources);

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
        Create_Floater_Quadtree();
        
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
            if(m_pMonsterRegions != NULL) throw CL_Exception("Already have monster regions on this level");

            m_pMonsterRegions = dynamic_cast<MonsterRegions*>(pElement);
            break;
        }
    default:
        return false;
    }
    return true;
}

void Level::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
#ifndef NDEBUG
    std::cout << "LEVEL NAME = " << m_name << std::endl;
#endif
}

void Level::load_finished()
{
    if(m_pHeader == NULL) throw CL_Exception("Level was missing levelHeader.");

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
    CL_Rect rect = pMO->GetTileRect();
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
    CL_Point point;
    if(tile->GetX() >= m_LevelWidth)
    {
        delete tile;
        throw CL_Exception("Tile found beyond range of indicated level width.");
    }
    else if (tile->GetY() >= m_LevelHeight)
    {
        delete tile;
        throw CL_Exception("Tile found beyond range of indicated level height.");
    }

    point.x = tile->GetX();
    point.y = tile->GetY();

    if( tile->IsFloater() )
    {
#ifndef NDEBUG
        std::cout << "Placing floater at: " << point.x << ',' << point.y << std::endl;
#endif
        CL_Rect rect = CL_Rect(point.x*32,point.y*32,(point.x+1)*32,(point.y+1)*32);
        Quadtree::Geometry::Rect<float> location(
        Quadtree::Geometry::Vector<float>(rect.get_center().x,rect.get_center().y),
                                          rect.get_width(),rect.get_height());
        m_floater_quadtree->Add(location,tile);
        //m_floater_map[ point ].sort( &Tile_Sort_Criterion );

    }
    else
    {
        m_tiles[ point.x ][point.y].push_back ( tile );
        // Sort by ZOrder, so that they display correctly
        m_tiles[ point.x ][point.y].sort( &Tile_Sort_Criterion );
    }
}

void Level::SerializeState ( std::ostream& out )
{
    m_player.SerializeState(out);   
}

void Level::DeserializeState ( std::istream& in )
{
    m_player.DeserializeState(in);
}


#ifdef SR2_EDITOR

class MappableObjectXMLWriter: public Level::MOQuadtree::OurVisitor {
public:
    MappableObjectXMLWriter(CL_DomElement& parent,CL_DomDocument& doc):m_parent(parent),m_doc(doc){}
    virtual ~MappableObjectXMLWriter(){}
    bool Visit(MappableObject* pMO, const Level::MOQuadtree::Node* pNode){
        MappableObjectElement * pMo = dynamic_cast<MappableObjectElement*>(pMO);
        if(pMo)
            m_parent.append_child(pMo->CreateDomElement(m_doc));
        return true;
    }
private:
    CL_DomElement& m_parent;
    CL_DomDocument& m_doc;
};


Level::Level(uint width, uint height):m_pMonsterRegions(NULL),m_LevelWidth(width),m_LevelHeight(height),m_pScript(NULL),
m_pHeader(NULL),m_player(0,0),m_mo_quadtree(NULL){
    m_tiles.resize( m_LevelWidth );

    for(uint x=0;x< m_LevelWidth; x++)
    {
        m_tiles[x].resize ( m_LevelHeight );
    }
    
    Create_MOQuadtree();
    Create_Floater_Quadtree();
}

void Level::GrowLevelTo(uint width, uint height)
{
    m_LevelWidth = std::max(width,m_LevelWidth);
    m_LevelHeight = std::max(height,m_LevelHeight);
    
    m_tiles.resize( m_LevelWidth );

    for(uint x=0;x< m_LevelWidth; x++)
    {
        m_tiles[x].resize ( m_LevelHeight );
    }
    
    resize_mo_quadtree();
    resize_floater_quadtree();
}

void Level::AddTile ( Tile* pTile )
{
    if(pTile->GetX() >= m_LevelWidth || pTile->GetY() >= m_LevelHeight )
        return;

    if( pTile->IsFloater())
    {
        CL_Rect rect = CL_Rect(pTile->GetX()*32,pTile->GetY()*32,(pTile->GetX()+1)*32,(pTile->GetY()+1)*32);
        Quadtree::Geometry::Rect<float> location(
        Quadtree::Geometry::Vector<float>(rect.get_center().x,rect.get_center().y),
                                          rect.get_width(),rect.get_height());
        m_floater_quadtree->Add(location,pTile);
    }
    else
    {
        m_tiles[ pTile->GetX() ][ pTile->GetY()].push_back ( pTile );  
    }

}

Tile* Level::PopTileAtPos(const CL_Point& pos)
{
    if(pos.x >= m_LevelWidth || pos.y >= m_LevelHeight )
        return NULL;
    
    if(m_tiles[pos.x][pos.y].empty()) 
        return NULL;
    
    Tile * pTile = m_tiles[pos.x][pos.y].back();
    m_tiles[pos.x][pos.y].pop_back();
    return pTile;
}

std::list<Tile*> Level::GetTilesAt(const CL_Point& pos) const
{
    return m_tiles[pos.x][pos.y];
}

void Level::RemoveFloater(const CL_Point& pos, Tile* pFloater)
{
    CL_Rect rect = CL_Rect(pos.x*32,pos.y*32,(pos.x+1)*32,(pos.y+1)*32);
    Quadtree::Geometry::Rect<float> location(
    Quadtree::Geometry::Vector<float>(rect.get_center().x,rect.get_center().y),
                                          rect.get_width(),rect.get_height());
    m_floater_quadtree->Add(location,pFloater);
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

void Level::resize_floater_quadtree()
{
   FindFloaters finder;
   m_floater_quadtree->TraverseAll(finder);
   
   Create_Floater_Quadtree();
   
   for(std::list<Tile*>::const_iterator it = finder.begin();
       it != finder.end(); it++){
       AddTile(*it);
   }
}

CL_DomElement Level::CreateDomElement(CL_DomDocument& doc) const 
{
    CL_DomElement element(doc,"level");
    element.set_attribute("name",m_name);

    CL_DomElement mappableObjects(doc,"mappableObjects");
    
    if(!m_pHeader){
        CL_DomElement levelHeader(doc,"levelHeader");
        levelHeader.set_attribute("width",IntToString(m_LevelWidth));
        levelHeader.set_attribute("height",IntToString(m_LevelHeight));
        levelHeader.set_attribute("music",m_music);
        levelHeader.set_attribute("allowsRunning",m_bAllowsRunning?"true":"false");
        element.append_child(levelHeader);
    }else{
        element.append_child( m_pHeader->CreateDomElement(doc) );
    }
    
    CL_DomElement tiles(doc,"tiles");
    for(int x=0;x<m_tiles.size();x++){
        for(int y=0;y<m_tiles[x].size();y++){
            for(std::list<Tile*>::const_iterator it = m_tiles[x][y].begin(); it != m_tiles[x][y].end(); it++)
            {
                tiles.append_child((*it)->CreateDomElement(doc));
            }
        }
    }
    
    FindFloaters finder;
    m_floater_quadtree->TraverseAll(finder);

    for(std::list<StoneRing::Tile*>::const_iterator jj = finder.begin();
            jj != finder.end();
            jj++)
    {
        CL_DomElement floaterEl = (*jj)->CreateDomElement(doc);
        tiles.append_child ( floaterEl );
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
    CL_DomDocument newdoc;
    CL_File  os;

    if(!force && !os.open(filename,CL_File::create_new,CL_File::access_write)){
        return false;
    }else if(force){
        os.open(filename,CL_File::create_always,CL_File::access_write);
    }


    newdoc.append_child ( CreateDomElement(newdoc) );

    newdoc.save( os, true );

    os.close();
    
    return true;
}

#endif