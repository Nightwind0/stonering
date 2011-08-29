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
,m_pHeader(NULL),m_player(0,0)
{
}

void Level::Invoke()
{
    if(m_pHeader)
    {
        m_pHeader->ExecuteScript();
    }
}

bool Level::Contains_Mappable_Objects(const CL_Point &point) const
{
    if(m_MO_map.find(point) != m_MO_map.end())
    {
        return true;
    }
    else return false;
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

bool Level::Contains_Solid_Mappable_Object(const CL_Point &point) const
{

//  MOMap::const_iterator i = std::find_if(m_MO_map.lower_bound(point),m_MO_map.upper_bound(point),activeSolidMappableObject);
    MOMap::const_iterator lower = m_MO_map.lower_bound(point);
    MOMap::const_iterator upper = m_MO_map.upper_bound(point);

    for(MOMap::const_iterator iter = lower; iter != upper; iter++)
    {
        if(iter->second && iter->second->IsSolid() && iter->second->EvaluateCondition())
            return true;
    }

    return false;

}

void Level::Set_Mappable_Object_At(const CL_Point &point, MappableObject*  pMO)
{

    std::pair<CL_Point,MappableObject*> thePair(point,pMO);
    m_MO_map.insert(thePair);

}

void Level::Remove_Mappable_Object_At(const CL_Point &point, MappableObject *pMO)
{
    MOMapIter lower = m_MO_map.lower_bound(point);
    MOMapIter upper = m_MO_map.upper_bound(point);

#ifndef NDEBUG
    if(lower == upper)
    {
        // This means it wasnt found
        std::cout << "We didn't find our guy where we expected." << std::endl;
    }
#endif

    std::list<MOMapIter> removals;

    for(MOMapIter findit = lower; findit != upper; findit++)
    {
        if ( findit->second == pMO)
        {
            removals.push_back (findit);
        }
    }

    for(std::list<MOMapIter>::const_iterator iter = removals.begin();
        iter != removals.end(); iter++)
    {
        m_MO_map.erase( *iter );
    }
}


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

        for(std::map<CL_Point,std::list<Tile*> >::iterator f = m_floater_map.begin();
            f != m_floater_map.end();
            f++)
        {

            // Possible optimization... instead of using is_overlapped, do a couple quick comparisons
            CL_Rect floaterRect(f->first.x,f->first.y,f->first.x + 32, f->first.y + 32);


            // Is this floater even on screen
            if(src.is_overlapped( floaterRect))

                for(std::list<Tile*>::iterator i = f->second.begin();
                    i != f->second.end();
                    i++)
                {
                    CL_Rect tileSrc(0,0,32,32);
                    CL_Rect tileDst ( exDst.left  + f->first.x * 32,
                                      exDst.top + f->first.y * 32,
                                      exDst.left + f->first.x * 32 + 32,
                                      exDst.top + f->first.y * 32 + 32);

                    Tile * pTile = *i;
                    if(pTile->EvaluateCondition())
                    {
                        pTile->Draw(tileSrc, tileDst , GC );

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


                            // Extra code for level editing
                            if(highlightHot && pTile->IsHot())
                            {
                                CL_Draw::fill(GC,tileDst, CL_Colorf(1.0,0.0f,0.0f,0.4));
                            }
                            if(indicateBlocks)
                            {
                                int block = pTile->GetDirectionBlock();

                                if(block & DIR_WEST)
                                {
                                    CL_Draw::fill(GC,CL_Rectf(tileDst.left,tileDst.top,tileDst.left + 8, tileDst.bottom), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
                                }
                                if(block & DIR_EAST)
                                {
                                    CL_Draw::fill(GC,CL_Rectf(tileDst.right - 8, tileDst.top, tileDst.right,tileDst.bottom),CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
                                }
                                if(block & DIR_NORTH)
                                {
                                    CL_Draw::fill(GC,CL_Rectf(tileDst.left, tileDst.top, tileDst.right, tileDst.top +8), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
                                }
                                if(block & DIR_SOUTH)
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






}


void Level::DrawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC)
{
    int cornerx = static_cast<int>(src.left / 32);
    int cornery = static_cast<int>(src.top / 32);

    int width = static_cast<int>((int)ceil((double)src.right/32.0)) - cornerx;
    int height = static_cast<int>((int)ceil((double)src.bottom/32.0)) - cornery;

    ++m_nFrameCount;

#if(!defined NDEBUG) && 0
    if(m_nFrameCount %2000 == 0)
    {
        std::cout << "X: " << cornerx <<  " Y: "  << cornery << std::endl;
        std::cout << "Width = " << width << " Height = " << height << std::endl;
        std::cout << "Right = " << cornerx + width << " Bottom = " << cornery + height << std::endl;
    }
#endif

    for(int y = 0;y<height;y++)
    {
        for(int x=0;x<width;x++)
        {
            CL_Point point(cornerx + x, cornery+ y);

            MOMapIter lower = m_MO_map.lower_bound(point);
            MOMapIter upper = m_MO_map.upper_bound(point);

            for(MOMapIter iter = lower; iter != upper; iter++)
            {
                MappableObject * pMO = iter->second;
                assert ( pMO != NULL );
#if 0

                GC.draw_rect(CL_Rect(point.x * 32 - src.left + dst.left,
                                       point.y * 32 + dst.top - src.top,
                                       point.x * 32 + 32 - src.left + dst.left,
                                       point.y * 32 + 32 - src.top + dst.top),CL_Color::beige);

#endif

                if(m_nFrameCount > pMO->GetFrameMarks()
                   && pMO->EvaluateCondition())
                {
                    assert(pMO != NULL);
                    pMO->MarkFrame();
                    CL_Rect moRect = pMO->GetPixelRect();
                    CL_Rect dstRect( moRect.left - src.left + dst.left, moRect.top + dst.top - src.top,
                                     moRect.left - src.left + dst.left +moRect.get_width(), moRect.top - src.top + dst.top + moRect.get_height());

                    pMO->Draw( moRect, dstRect, GC );
                }
            }
        }
    }
}


void Level::SetPlayerPos(const CL_Point &target)
{
    Set_Mappable_Object_At(target, &m_player);
}



void Level::Put_Mappable_Object_At_Current_Position(MappableObject *pMO)
{
    assert ( pMO != NULL );
    CL_Point cur_pos = pMO->GetPosition();
    uint width = pMO->GetCellWidth();
    uint height = pMO->GetCellHeight();

    for(uint x = cur_pos.x; x< width + cur_pos.x;x++)
    {
        for(uint y=cur_pos.y;y<height + cur_pos.y;y++)
        {
            Set_Mappable_Object_At(CL_Point(x,y),pMO);
        }
    }
}


void Level::MoveMappableObjects(const CL_Rect &src)
{

    // They can't change direction unless they are aligned.
    // They can't stop unless they are aligned.
    // Otherwise they can move as much as they want.
    // The timer should go off at the lowest time (the fastest mover)

    int cornerx = static_cast<int>(src.left / 32.0);
    int cornery = static_cast<int>(src.top / 32.0);

    int width = static_cast<int>(ceil((double)src.right/32.0)) - cornerx;
    int height = static_cast<int>(ceil((double)src.bottom/32.0)) - cornery;

    m_nMoveCount++;

    std::set<MOMapIter,LessMOMapIter> MOIters;

    for(int x = 0;x<width;x++)
    {
        for(int y=0;y<height;y++)
        {
            CL_Point point(cornerx + x, cornery+ y);

            MOMapIter lower = m_MO_map.lower_bound(point);
            MOMapIter upper = m_MO_map.upper_bound(point);

            // No list here.... move along
            for(MOMapIter iter = lower; iter!= upper; iter++)
            {
                MappableObject * pMo = iter->second;
                if(! pMo->EvaluateCondition()) continue; // Skip 'em

                MOIters.insert( iter );
            }
        }
    }

#ifndef NDEBUG
    bool playerFound = false;

#endif

    for(std::set<MOMapIter,LessMOMapIter>::iterator iMo = MOIters.begin();
        iMo != MOIters.end();iMo++)
    {
        MappableObject * pMo = (*iMo)->second;

#if 0
        if(pMo->getName() == "Player" && pMo->getDirection() != MappableObject::NONE)
            playerFound = true;
#endif

        CL_Point curPos = pMo->GetPosition();

        // Clear the MO from the map altogether, to be reinserted later
        pMo->SetOccupiedPoints(this, &Level::Remove_Mappable_Object_At);


        for(uint d=0;d<pMo->GetMovesPerDraw();d++)
        {
            if(pMo->GetDirection() != MappableObject::NONE)
            {
                if(pMo->IsAligned())
                {
                    std::list<CL_Point> intoPoints ;
                    MappableObject::CalculateEdgePoints(pMo->GetPositionAfterMove(), pMo->GetDirection(),
                                                        pMo->GetSize(), &intoPoints);

                    bool bPathBlocked = false;

                    // Make sure none of these points is occupied.
                    // Break this into a method so I can if() off it
                    for(std::list<CL_Point>::iterator iter = intoPoints.begin();
                        iter != intoPoints.end();
                        iter++)
                    {
                        if((*iter).x < cornerx || (*iter).y <cornery || (*iter).x >= cornerx+width || (*iter).y >= cornery+height
                           || (*iter).x <0 || (*iter).y <0 || (*iter).x >= m_LevelWidth || (*iter).y >= m_LevelHeight
                           || Contains_Solid_Mappable_Object(*iter)
                           ||
                           (Get_Cumulative_Direction_Block_At_Point(*iter) & MappableObject::ConvertDirectionToDirectionBlock(pMo->GetDirection()))
                           || (pMo->RespectsHotness() && Get_Cumulative_Hotness_At_Point(*iter))
                            )
                        {
#ifndef NDEBUG
                            if(pMo->GetName() == "Player" && gbDebugStop)
                            {
                                playerFound = true;
                            }
#endif
                            // No go. Change direction, so we can try again.
                            pMo->RandomNewDirection();
                            bPathBlocked = true;
                            break;
                        } // if blocked

                        // Not blocked!

                    }// all points

                    if(bPathBlocked) continue;




                }// Aligned

                pMo->Move();

                // We may have just become aligned
                if(pMo->IsAligned())
                {
                    pMo->MovedOneCell();

                    // If we wanted to step on every point that the player was stepping on, we could...
                    // In fact we could use setOccupiedPoints.
                    // But we're assuming that we take up only one square
                    if(pMo->Step())
                        Step(pMo->GetPosition());

                }

            }// if direction != NONE
            else
            {


                if(m_nMoveCount % 32 == 0)
                    pMo->MovedOneCell();
                else pMo->Idle();
            }
        }// For d

        pMo->SetOccupiedPoints(this, &Level::Set_Mappable_Object_At);

    }// for iMo

}


void Level::DrawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC)
{
    Draw(src,dst,GC, true);
}



// Checks relevant tile and MO direction block information
// Rects are in cells
bool Level::TryMove(const CL_Point &currently, const CL_Point & destination)
{
    //deprecated very soon...
    return true;
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
    MOMap::const_iterator lower = m_MO_map.lower_bound(target);
    MOMap::const_iterator upper = m_MO_map.upper_bound(target);


    for(MOMap::const_iterator iter = lower;
        iter != upper;
        iter++)
    {
        MappableObject * pMo = iter->second;

        if((pMo)->EvaluateCondition())
        {
            // This MO needs to be stepped on
            //(*i)->provokeEvents ( Event::STEP );
            (pMo)->ProvokeEvents(Event::STEP);
        }
    }
    
    IParty * pParty = IApplication::GetInstance()->GetParty();
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


    MOMap::const_iterator lower = m_MO_map.lower_bound(target);
    MOMap::const_iterator upper = m_MO_map.upper_bound(target);


    for(MOMap::const_iterator iter = lower;
        iter != upper;
        iter++)
    {
        MappableObject * pMo = iter->second;

        if((pMo)->EvaluateCondition())
        {
            if(!prod)
            {
                // This MO needs to be talked to
                (pMo)->ProvokeEvents ( Event::TALK );
            }
            else
            {
                // Prod!
                // (Can't prod things that aren't solid. They aren't in your way anyways)
                // And if it has no movement, prodding it does nothing.
                if((pMo)->IsSolid() && (pMo)->GetMovement() != NULL)
                {
                    (pMo)->Prod();
                }


            }

            return; // We only do the first one.

        }
    }

    if(m_bMarkedForDeath ) delete this;

}

// Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
void Level::Update(const CL_Rect & updateRect)
{
}


#ifndef NDEBUG
void Level::DumpMappableObjects() const
{

    std::cout << "=== Mappable Objects ===" << std::endl;

    for(MOMap::const_iterator iList = m_MO_map.begin();
        iList != m_MO_map.end();
        iList++)
    {

        std::cout << '{' << iList->first.x <<',' << iList->first.y << '}' << std::endl;

        MappableObject * pMO = iList->second;

        std::cout << '\t' << pMO->GetName();

        std::cout << " @ " << (pMO->GetX() / 32) << '(' << pMO->GetX() << ')'
                  <<','
                  << (pMO->GetY() / 32) << '(' << pMO->GetY() << ')';
        std::cout << std::endl;


    }


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



void Level::Load_Mo ( MappableObject * moElement )
{
    Put_Mappable_Object_At_Current_Position(moElement);
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
        m_floater_map[ point ].push_back ( tile );
        m_floater_map[ point ].sort( &Tile_Sort_Criterion );

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




