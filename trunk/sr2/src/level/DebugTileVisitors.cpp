#include "DebugTileVisitors.h"
#include "GraphicsManager.h"

namespace StoneRing  {
    
TileDirectionBlockDrawer::TileDirectionBlockDrawer()
{

}

TileDirectionBlockDrawer::~TileDirectionBlockDrawer()
{

}

void TileDirectionBlockDrawer::accept ( CL_GraphicContext& GC, const CL_Point& tileDst, Tile* pTile )
{
    int block = pTile->GetDirectionBlock();

    if(block & BLK_WEST)
    {
        CL_Draw::fill(GC,CL_Rectf(tileDst.x,tileDst.y,tileDst.x + 8, tileDst.y + 32), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    }
    if(block & BLK_EAST)
    {
        CL_Draw::fill(GC,CL_Rectf(tileDst.x + 32 - 8, tileDst.y, tileDst.x + 32,tileDst.y + 32),CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    }
    if(block & BLK_NORTH)
    {
        CL_Draw::fill(GC,CL_Rectf(tileDst.x, tileDst.y, tileDst.x + 32, tileDst.y +8), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    }
    if(block & BLK_SOUTH)
    {
        CL_Draw::fill(GC,CL_Rectf(tileDst.x,tileDst.y + 32 -8, tileDst.x + 32, tileDst.y + 32), CL_Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    } 
}

TilePopsDrawer::TilePopsDrawer()
{

}

TilePopsDrawer::~TilePopsDrawer()
{

}

void TilePopsDrawer::accept ( CL_GraphicContext& GC, const CL_Point& top_left, Tile* pTile )
{
    if(m_indicator.is_null())
        m_indicator = GraphicsManager::CreateSprite("Sprites/System/Pops",false);    
    if(pTile->Pops())
        m_indicator.draw(GC,top_left.x,top_left.y);
}

TileFloaterDrawer::TileFloaterDrawer()
{

}

TileFloaterDrawer::~TileFloaterDrawer()
{

}

void TileFloaterDrawer::accept ( CL_GraphicContext& gc, const CL_Point& top_left, Tile* pTile )
{
    if(m_indicator.is_null())
        m_indicator = GraphicsManager::CreateSprite("Sprites/System/Floater",false);
    if(pTile->IsFloater())
        m_indicator.draw(gc,top_left.x,top_left.y + 16);
}



TileHotDrawer::TileHotDrawer()
{

}

TileHotDrawer::~TileHotDrawer()
{

}

void TileHotDrawer::accept ( CL_GraphicContext& gc, const CL_Point& top_left, Tile* pTile )
{
    if(pTile->IsHot()){
        CL_Rect rect = CL_Rect(top_left,CL_Size(32,32));
        CL_Draw::fill(gc,rect,CL_Colorf(1.0,0.0f,0.0f,0.4));
    }
}




};