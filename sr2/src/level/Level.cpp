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
#include "LevelFactory.h"
#include "ItemFactory.h"
#include "ItemManager.h"
#include "MonsterRegion.h"
#include "MonsterGroup.h"


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


std::list<Tile*>::const_iterator Tiles::getTilesBegin()const
{
    return mTiles.begin();
}

std::list<Tile*>::const_iterator Tiles::getTilesEnd()const
{
    return mTiles.end();
}
 
bool Tiles::handleElement(eElement element, Element * pElement)
{
    if(element == ETILE)
    {
        mTiles.push_back ( dynamic_cast<Tile*>(pElement) );
        return true;
    }

    return false;
}

void Tiles::loadAttributes(CL_DomNamedNodeMap * pAttributes)
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

std::list<MappableObject*>::const_iterator MappableObjects::getMappableObjectsBegin() const
{
    return mMappableObjects.begin();
}
std::list<MappableObject*>::const_iterator MappableObjects::getMappableObjectsEnd() const
{
    return mMappableObjects.end();
}


bool MappableObjects::handleElement(eElement element, Element * pElement)
{
    if(element == EMAPPABLEOBJECT)
    {
        mMappableObjects.push_back ( dynamic_cast<MappableObject*>(pElement) );
        return true;
    }

    return false;
}

void MappableObjects::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{

}


///////////////////////////////////////////////////////////////////////////
// LevelHeader
///////////////////////////////////////////////////////////////////////////

LevelHeader::LevelHeader():mpScript(NULL)
{
}

LevelHeader::~LevelHeader()
{
    delete mpScript;
}


void LevelHeader::executeScript() const
{
    if(NULL != mpScript)
        mpScript->executeScript();
}

bool LevelHeader::handleElement(eElement element, Element * pElement)
{
    if(element == ESCRIPT)
    {
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        return true;
    }
    return false;
}

void LevelHeader::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mnLevelWidth = getRequiredUint("width",pAttributes);
    mnLevelHeight = getRequiredUint("height",pAttributes);
    mMusic = getRequiredString("music",pAttributes);
    mbAllowsRunning = getRequiredBool("allowsRunning",pAttributes);
}

/////////////////////////////////////////////////////////////////////////////
// Level
// 
/////////////////////////////////////////////////////////////////////////////


Level::Level():mpMonsterRegions(NULL),mLevelWidth(0),mLevelHeight(0),mpScript(NULL),mpHeader(NULL)
{
}

void Level::invoke()
{
    if(mpHeader)
    {
        mpHeader->executeScript();
    }
}

bool Level::containsMappableObjects(const CL_Point &point) const
{
    if(mMOMap.find(point) != mMOMap.end())
    {
        return true;
    }
    else return false;
}

bool activeSolidMappableObject(const std::multimap<CL_Point,MappableObject*>::value_type &value)
{
    MappableObject * pMO = value.second;

    return pMO && pMO->isSolid() && pMO->evaluateCondition();
}

bool matchesMappableObject(const std::multimap<CL_Point,MappableObject*>::value_type &value,MappableObject *pMo)
{
    return value.second == pMo;
}

bool Level::containsSolidMappableObject(const CL_Point &point) const
{
    
//  MOMap::const_iterator i = std::find_if(mMOMap.lower_bound(point),mMOMap.upper_bound(point),activeSolidMappableObject);
    MOMap::const_iterator lower = mMOMap.lower_bound(point);
    MOMap::const_iterator upper = mMOMap.upper_bound(point);

    for(MOMap::const_iterator iter = lower; iter != upper; iter++)
    {
        if(iter->second && iter->second->isSolid() && iter->second->evaluateCondition())
            return true;
    }

    return false;

}

void Level::setMappableObjectAt(const CL_Point &point, MappableObject*  pMO)
{

    std::pair<CL_Point,MappableObject*> thePair(point,pMO);
    mMOMap.insert(thePair);

}

void Level::removeMappableObjectAt(const CL_Point &point, MappableObject *pMO)
{
    MOMapIter lower = mMOMap.lower_bound(point);
    MOMapIter upper = mMOMap.upper_bound(point);

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
        mMOMap.erase( *iter );
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
    for(int x=0;x<mLevelWidth;x++)
    {
        for(int y=0;y<mLevelHeight;y++)
        {
            for(std::list<Tile*>::iterator i = mTileMap[x][y].begin();
                i != mTileMap[x][y].end();
                i++)
            {
                delete *i;
            }
        
        }

    }

    delete mpPlayer;
}

int Level::getCumulativeDirectionBlockAtPoint(const CL_Point &point) const
{
    int directionBlock =0;

    std::list<Tile*> tileList = mTileMap[point.x][point.y];

    for(std::list<Tile*>::const_iterator iter = tileList.begin();
        iter != tileList.end();
        iter++)
    {
        if((*iter)->evaluateCondition())
            directionBlock |= (*iter)->getDirectionBlock();
    }

    return directionBlock;
}

bool Level::getCumulativeHotnessAtPoint(const CL_Point &point) const
{

    std::list<Tile*> tileList = mTileMap[point.x][point.y];

    for(std::list<Tile*>::iterator iter = tileList.begin();
        iter != tileList.end();
        iter++)
    {
        if((*iter)->evaluateCondition())
            if((*iter)->isHot()) return true;
    }
        
    return false;

}
    

void Level::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC, bool floaters,
                 bool highlightHot, bool indicateBlocks)
{
    //    int maxSrcX = max( ceil(dst.get_width() / 32.0), mLevelWidth );
    //    int maxSrcY = max( ceil(dst.get_height() / 32.0), mLevelHeight);
    int cornerx = static_cast<int>(src.left / 32);
    int cornery = static_cast<int>(src.top / 32);

    int widthInTiles = static_cast<int>((int)ceil((double)src.right/32.0)) - cornerx;
    int heightInTiles = static_cast<int>((int)ceil((double)src.bottom/32.0)) - cornery;

    int widthInPx = widthInTiles * 32;
    int heightInPx = heightInTiles * 32;


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
    
        for(std::map<CL_Point,std::list<Tile*> >::iterator f = mFloaterMap.begin();
            f != mFloaterMap.end();
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
                    if(pTile->evaluateCondition())
                    {
                        pTile->draw(tileSrc, tileDst , pGC );

                    }
            
            
                }
        }
    
    }       
    else
    {
        // Regular tiles, not floaters
    
        for(int tileX = 0; tileX < widthInTiles; tileX++)
        {
            for(int tileY =0; tileY < heightInTiles; tileY++)
            {
        
                CL_Point p( src.left / 32 + tileX, src.top /32 + tileY);
        
        
                if(p.x >=0 && p.y >=0 && p.x < mLevelWidth && p.y < mLevelHeight)
                {
                    CL_Rect tileSrc(0,0,32,32);            
                    CL_Rect tileDst ( exDst.left  + (tileX << 5),
                                      exDst.top + (tileY << 5),
                                      exDst.left + (tileX << 5) + 32,
                                      exDst.top + (tileY << 5) + 32);

                           
                    std::list<Tile*>::iterator end = mTileMap[p.x][p.y].end();
                    for(std::list<Tile*>::iterator i = mTileMap[p.x][p.y].begin();
                        i != end;
                        i++)
                    {

               
                        Tile * pTile = *i;
                        if(pTile->evaluateCondition())
                        {
                            pTile->draw(tileSrc, tileDst , pGC );
                
                
                            // Extra code for level editing
                            if(highlightHot && pTile->isHot())
                            {
                                pGC->fill_rect(tileDst, CL_Color(255,0,0,160));
                            }
                            if(indicateBlocks)
                            {
                                int block = pTile->getDirectionBlock();
                            
                                if(block & DIR_WEST)
                                {
                                    pGC->fill_rect(CL_Rect(tileDst.left,tileDst.top,tileDst.left + 8, tileDst.bottom), CL_Color(0,255,255,120));
                                }
                                if(block & DIR_EAST)
                                {
                                    pGC->fill_rect(CL_Rect(tileDst.right - 8, tileDst.top, tileDst.right,tileDst.bottom),CL_Color(0,255,255,120));
                                }
                                if(block & DIR_NORTH)
                                {
                                    pGC->fill_rect(CL_Rect(tileDst.left, tileDst.top, tileDst.right, tileDst.top +8), CL_Color(0,255,255,120));
                                }
                                if(block & DIR_SOUTH)
                                {
                                    pGC->fill_rect(CL_Rect(tileDst.left,tileDst.bottom -8, tileDst.right, tileDst.bottom), CL_Color(0,255,255,120));
                                }
                            }
                        }
                    

                    

                    
                    
                    }
                }
            
            
            
            
            
            }
        }
    
    }
    
    


    

}       


void Level::drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    int cornerx = static_cast<int>(src.left / 32);
    int cornery = static_cast<int>(src.top / 32);

    int width = static_cast<int>((int)ceil((double)src.right/32.0)) - cornerx;
    int height = static_cast<int>((int)ceil((double)src.bottom/32.0)) - cornery;
        
    ++mnFrameCount;

#ifndef NDEBUG
    if(mnFrameCount %2000 == 0)
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

            MOMapIter lower = mMOMap.lower_bound(point);
            MOMapIter upper = mMOMap.upper_bound(point);

            for(MOMapIter iter = lower; iter != upper; iter++)
            {
                MappableObject * pMO = iter->second;
                assert ( pMO != 0 );
#if 0

                pGC->draw_rect(CL_Rect(point.x * 32 - src.left + dst.left,
                                       point.y * 32 + dst.top - src.top,
                                       point.x * 32 + 32 - src.left + dst.left,
                                       point.y * 32 + 32 - src.top + dst.top),CL_Color::beige);
        
#endif

                if(mnFrameCount > pMO->getFrameMarks()
                   && pMO->evaluateCondition())
                {
                    pMO->markFrame();
                    CL_Rect moRect = pMO->getPixelRect();
                    CL_Rect dstRect( moRect.left - src.left + dst.left, moRect.top + dst.top - src.top,
                                     moRect.left - src.left + dst.left +moRect.get_width(), moRect.top - src.top + dst.top + moRect.get_height());
                                        
                    pMO->draw( moRect, dstRect, pGC );            
                }
            }
        }
    }
}

    


void Level::putMappableObjectAtCurrentPosition(MappableObject *pMO)
{
    CL_Point cur_pos = pMO->getPosition();
    uint width = pMO->getCellWidth();
    uint height = pMO->getCellHeight();

    for(int x = cur_pos.x; x< width + cur_pos.x;x++)
    {
        for(int y=cur_pos.y;y<height + cur_pos.y;y++)
        {
            setMappableObjectAt(CL_Point(x,y),pMO);
        }
    }
}


void Level::addPlayer(StoneRing::MappablePlayer *pPlayer)
{
    mpPlayer = pPlayer;
    putMappableObjectAtCurrentPosition(pPlayer);
}


void Level::moveMappableObjects(const CL_Rect &src)
{

    // They can't change direction unless they are aligned.
    // They can't stop unless they are aligned. 
    // Otherwise they can move as much as they want.
    // The timer should go off at the lowest time (the fastest mover)
          
    int cornerx = static_cast<int>(src.left / 32.0);
    int cornery = static_cast<int>(src.top / 32.0);

    int width = static_cast<int>(ceil((double)src.right/32.0)) - cornerx;
    int height = static_cast<int>(ceil((double)src.bottom/32.0)) - cornery;
        
    mnMoveCount++;

    std::set<MOMapIter,LessMOMapIter> MOIters;

    for(int x = 0;x<width;x++)
    {
        for(int y=0;y<height;y++)
        {
            CL_Point point(cornerx + x, cornery+ y);

            MOMapIter lower = mMOMap.lower_bound(point);
            MOMapIter upper = mMOMap.upper_bound(point);
        
            // No list here.... move along
            for(MOMapIter iter = lower; iter!= upper; iter++)
            {
                MappableObject * pMo = iter->second;
                if(! pMo->evaluateCondition()) continue; // Skip 'em

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

        CL_Point curPos = pMo->getPosition();

        // Clear the MO from the map altogether, to be reinserted later

            
        pMo->setOccupiedPoints(this, &Level::removeMappableObjectAt);
    
        
        for(uint d=0;d<pMo->getMovesPerDraw();d++)
        {
            if(pMo->getDirection() != MappableObject::NONE)
            {
                if(pMo->isAligned())
                {
                    std::list<CL_Point> intoPoints ;
                    MappableObject::CalculateEdgePoints(pMo->getPositionAfterMove(), pMo->getDirection(),
                                                        pMo->getSize(), &intoPoints);

                    bool bPathBlocked = false;
                                        
                    // Make sure none of these points is occupied.
                    // Break this into a method so I can if() off it
                    for(std::list<CL_Point>::iterator iter = intoPoints.begin();
                        iter != intoPoints.end();
                        iter++)
                    {
                        if((*iter).x < cornerx || (*iter).y <cornery || (*iter).x >= cornerx+width || (*iter).y >= cornery+height
                           || (*iter).x <0 || (*iter).y <0 || (*iter).x >= mLevelWidth || (*iter).y >= mLevelHeight
                           ||containsSolidMappableObject(*iter)
                           ||
                           (getCumulativeDirectionBlockAtPoint(*iter) & MappableObject::ConvertDirectionToDirectionBlock(pMo->getDirection()))
                           || (pMo->respectsHotness() && getCumulativeHotnessAtPoint(*iter))
                            )
                        {
#ifndef NDEBUG
                            if(pMo->getName() == "Player" && gbDebugStop)
                            {
                                playerFound = true;
                            }
#endif
                            // No go. Change direction, so we can try again.
                            pMo->randomNewDirection();
                            bPathBlocked = true;
                            break;
                        } // if blocked

                        // Not blocked!

                    }// all points    

                    if(bPathBlocked) continue;


                  

                }// Aligned

                pMo->move();

                // We may have just become aligned
                if(pMo->isAligned())
                {
                    pMo->movedOneCell();

                    // If we wanted to step on every point that the player was stepping on, we could...
                    // In fact we could use setOccupiedPoints.
                    // But we're assuming that we take up only one square
                    if(pMo->step())
                        step(pMo->getPosition());

                }

            }// if direction != NONE
            else
            {

                
                if(mnMoveCount % 32 == 0)
                    pMo->movedOneCell();
                else pMo->idle();
            }
        }// For d

        pMo->setOccupiedPoints(this, &Level::setMappableObjectAt);
    
    }// for iMo

    if(mbMarkedForDeath) delete this;
        
}
 

void Level::drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC)
{
    draw(src,dst,pGC, true);
}


      
// Checks relevant tile and MO direction block information
// Rects are in cells
bool Level::tryMove(const CL_Point &currently, const CL_Point & destination)
{
    //deprecated very soon...
    return true;
}

// All AM's from tiles fire, as do any step events
void Level::step(const CL_Point &target)
{
    // First, check for a battle here (events come after)
    if(mpMonsterRegions)
    {
        MonsterRegion *pRegion = mpMonsterRegions->getApplicableRegion(target.x,target.y);
        if(pRegion != NULL)
        {
            float rate = pRegion->getEncounterRate();
            float value = ((float)rand() / ((float)RAND_MAX + 1)); // Generate number between 0 and 1.0

            if(value < rate)
            {
                // Then we have an encounter.
                // Let the region pick a group for us to battle
                MonsterGroup * pGroup = pRegion->getMonsterGroup();

                IApplication::getInstance()->startBattle (pGroup->getMonsters());
            }
        }
    }
    // Second, process any MO step events you may have triggered
    MOMap::const_iterator lower = mMOMap.lower_bound(target);
    MOMap::const_iterator upper = mMOMap.upper_bound(target);
    

    for(MOMap::const_iterator iter = lower;
        iter != upper;
        iter++)
    {
        MappableObject * pMo = iter->second;

        if((pMo)->evaluateCondition())
        {
            // This MO needs to be stepped on
            //(*i)->provokeEvents ( Event::STEP );
            (pMo)->provokeEvents(Event::STEP);
        }
    }



    
    activateTilesAt(target.x,target.y);


}


void Level::activateTilesAt ( uint x, uint y )
{
    std::list<Tile*> tileList = mTileMap[x][y];

    for(std::list<Tile*>::const_iterator iter = tileList.begin();
        iter != tileList.end();
        iter++)
    {
        Tile *pTile = *iter;
        if(pTile->evaluateCondition())
        {
            if ( pTile->hasScript() )
            {
                (*iter)->activate();
            }
            else if ((*iter)->pops() )
            {
                IApplication::getInstance()->pop(false);
            }
        }
    }
    
}
      
// Any talk events fire (assuming they meet conditions)
void Level::talk(const CL_Point &target, bool prod)
{


    MOMap::const_iterator lower = mMOMap.lower_bound(target);
    MOMap::const_iterator upper = mMOMap.upper_bound(target);
    

    for(MOMap::const_iterator iter = lower;
        iter != upper;
        iter++)
    {
        MappableObject * pMo = iter->second;

        if((pMo)->evaluateCondition())
        {
            if(!prod)
            {
                // This MO needs to be talked to
                (pMo)->provokeEvents ( Event::TALK );
            }
            else
            {
                // Prod!
                // (Can't prod things that aren't solid. They aren't in your way anyways)
                // And if it has no movement, prodding it does nothing.
                if((pMo)->isSolid() && (pMo)->getMovement() != NULL)
                {
                    (pMo)->prod();
                }

        
            }
        
            return; // We only do the first one.
        
        }
    }

    if(mbMarkedForDeath ) delete this;

}

// Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
void Level::update(const CL_Rect & updateRect)
{
}
      
 
#ifndef NDEBUG
void Level::dumpMappableObjects() const
{

    IApplication * pApp = IApplication::getInstance();


    std::cout << "=== Mappable Objects ===" << std::endl;

    for(MOMap::const_iterator iList = mMOMap.begin();
        iList != mMOMap.end();
        iList++)
    {

        std::cout << '{' << iList->first.x <<',' << iList->first.y << '}' << std::endl;

        MappableObject * pMO = iList->second;

        std::cout << '\t' << pMO->getName();
            
        std::cout << " @ " << (pMO->getX() / 32) << '(' << pMO->getX() << ')'
                  <<',' 
                  << (pMO->getY() / 32) << '(' << pMO->getY() << ')';
        std::cout << std::endl;
    
            
    }


}
#endif


// Sort tiles on zOrder
bool Level::tileSortCriterion ( const Tile * p1, const Tile * p2)
{
    return p1->getZOrder() < p2->getZOrder();
}

void Level::loadFromFile(const std::string &filename)
{
    CL_DomDocument doc;
    CL_InputSource_File file(filename);
    doc.load(&file);

    CL_DomElement levelNode = doc.named_item("level").to_element();
    Element::load(&levelNode);
}

void Level::load(const std::string &name, CL_ResourceManager *pResources)
{
    std::string path = CL_String::load("Game/LevelPath", pResources);
    std::string filename = CL_String::load("Levels/" + name, pResources);

    loadFromFile(path + filename);
}

bool Level::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ELEVELHEADER:
        mpHeader = dynamic_cast<LevelHeader*>(pElement);

        // Based on the header, lets preallocate our tiles.
        mLevelWidth = mpHeader->getLevelWidth();
        mLevelHeight = mpHeader->getLevelHeight();
        mbAllowsRunning = mpHeader->allowsRunning();
        mMusic = mpHeader->getMusic();
        mTileMap.resize( mLevelWidth );

        for(int x=0;x< mLevelWidth; x++)
        {
            mTileMap[x].resize ( mLevelHeight );
        }
        mpHeader->executeScript();
        break;
    case ETILES:
        {
            Tiles *pTiles = dynamic_cast<Tiles*>(pElement);
            for(std::list<Tile*>::const_iterator it = pTiles->getTilesBegin();
                it != pTiles->getTilesEnd(); it++)
                loadTile ( *it );
            break;
        }
    case EMAPPABLEOBJECTS:
        {
            MappableObjects *pMOs = dynamic_cast<MappableObjects*>(pElement);
            for(std::list<MappableObject*>::const_iterator it = pMOs->getMappableObjectsBegin();
                it != pMOs->getMappableObjectsEnd(); it++)
                loadMo ( *it );
        break;
        }
    case EMONSTERREGIONS:
        {
            if(mpMonsterRegions != NULL) throw CL_Error("Already have monster regions on this level");

            mpMonsterRegions = dynamic_cast<MonsterRegions*>(pElement);
            break;
        }
    default:
        return false;
    }
    return true;
}

void Level::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
#ifndef NDEBUG
    std::cout << "LEVEL NAME = " << mName << std::endl;
#endif
}

void Level::loadFinished()
{
    if(mpHeader == NULL) throw CL_Error("Level was missing levelHeader.");
    

    mnFrameCount = mnMoveCount = 0 ;

    mbMarkedForDeath = false;
    mpPlayer = NULL;

}



void Level::loadMo ( MappableObject * moElement )
{
    putMappableObjectAtCurrentPosition(moElement);
}


void Level::loadTile ( Tile * tile)
{
    CL_Point point;
    if(tile->getX() >= mLevelWidth)
    {
        delete tile;
        throw CL_Error("Tile found beyond range of indicated level width.");
    }
    else if (tile->getY() >= mLevelHeight)
    {
        delete tile;
        throw CL_Error("Tile found beyond range of indicated level height.");
    }

    point.x = tile->getX();
    point.y = tile->getY();
    
    if( tile->isFloater() )
    {
#ifndef NDEBUG
        std::cout << "Placing floater at: " << point.x << ',' << point.y << std::endl;
#endif
        mFloaterMap[ point ].push_back ( tile );

        mFloaterMap[ point ].sort( &tileSortCriterion );

    }
    else
    {
        mTileMap[ point.x ][point.y].push_back ( tile );

        // Sort by ZOrder, so that they display correctly
        mTileMap[ point.x ][point.y].sort( &tileSortCriterion );
    }
}




