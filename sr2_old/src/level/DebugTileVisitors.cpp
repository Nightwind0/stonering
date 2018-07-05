#include "DebugTileVisitors.h"
#include "GraphicsManager.h"

namespace StoneRing  {
    
TileSideBlockDrawer::TileSideBlockDrawer()
{

}

TileSideBlockDrawer::~TileSideBlockDrawer()
{

}

void TileSideBlockDrawer::accept ( clan::Canvas& gc, const clan::Point& tileDst, Tile* pTile )
{
    int block = pTile->GetSideBlock();
	clan::Point pt = pTile->GetRect().get_top_left();

    if(block & BLK_WEST)
    {
        gc.fill_rect(clan::Rectf(pt.x + tileDst.x,pt.y + tileDst.y,pt.x + tileDst.x + 8, pt.y+tileDst.y + 32), clan::Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    }
    if(block & BLK_EAST)
    {
        gc.fill_rect(clan::Rectf(pt.x+tileDst.x + 32 - 8, pt.y+tileDst.y, pt.x+tileDst.x + 32,pt.y+tileDst.y + 32),clan::Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    }
    if(block & BLK_NORTH)
    {
        gc.fill_rect(clan::Rectf(pt.x+tileDst.x,pt.y+tileDst.y, pt.x+tileDst.x + 32, pt.y+tileDst.y +8), clan::Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    }
    if(block & BLK_SOUTH)
    {
        gc.fill_rect(clan::Rectf(pt.x+tileDst.x,pt.y+tileDst.y + 32 -8, pt.x+tileDst.x + 32, pt.y+tileDst.y + 32), clan::Colorf(0.0f,1.0f,1.0f,(float)120/255.0f));
    } 
}


TileFloaterDrawer::TileFloaterDrawer()
{

}

TileFloaterDrawer::~TileFloaterDrawer()
{

}

void TileFloaterDrawer::accept ( clan::Canvas& gc, const clan::Point& top_left, Tile* pTile )
{
	clan::Rect rect = pTile->GetRect();
    if(m_indicator.is_null())
        m_indicator = GraphicsManager::CreateSprite("Sprites/System/Floater",false);
    if(pTile->IsFloater())
        m_indicator.draw(gc,top_left.x+rect.get_top_left().x,rect.get_top_left().y+top_left.y + 16);
}




TileHotDrawer::TileHotDrawer()
{

}

TileHotDrawer::~TileHotDrawer()
{

}

void TileHotDrawer::accept ( clan::Canvas& gc, const clan::Point& top_left, Tile* pTile )
{
	clan::Point tile_pt = pTile->GetRect().get_top_left();
    if(pTile->IsHot()){
        clan::Rect rect = clan::Rect(top_left+tile_pt,clan::Size(32,32));
        gc.fill_rect(rect,clan::Colorf(1.0,0.0f,0.0f,0.4));
    }
}

TileMonsterRegionDrawer::TileMonsterRegionDrawer()
{
	m_font = GraphicsManager::GetFont("sm_white");
}

TileMonsterRegionDrawer::~TileMonsterRegionDrawer()
{

}

clan::Colorf TileMonsterRegionDrawer::get_color( char id ) const
{
	struct RGB {
		float r;
		float g;
		float b;
	};
	const RGB colors[] =  {
		{0.0f,0.0f,0.0f},
		{1.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f},
		{0.0f,0.0f,1.0f},
		{0.0f,1.0f,1.0f},
		{1.0f,1.0f,0.0f},
		{1.0f,0.0f,1.0f},
		{1.0f,1.0f,1.0f},
		{0.5f,0.0f,0.0f},
		{0.0f,0.5f,0.0f},
		{0.0f,0.0f,0.5f},
		{0.0f,0.5f,0.5f},
		{0.5f,0.5f,0.0f},
		{0.5f,0.0f,0.5f},
		{0.5f,0.5f,0.5f},
		{0.25f,0.0f,0.0f},
		{0.0f,0.25f,0.0f},
		{0.0f,0.0f,0.25f},
		{0.0f,0.25f,0.25f},
		{0.25f,0.25f,0.0f},
		{0.25f,0.0f,0.25f},
		{0.25f,0.25f,0.25f},		
		{1.0f,0.25f,0.25f},
		{0.25f,1.0f,0.25f},
		{0.25f,0.25f,1.0f},
		{0.25f,1.0f,1.0f},
		{1.0f,1.0f,0.25f},
		{1.0f,0.25f,1.0f}		
	};
	
	int index = id % (sizeof(colors) / sizeof(RGB));
	return clan::Colorf(colors[index].r, colors[index].g,colors[index].b);
}


void TileMonsterRegionDrawer::accept( clan::Canvas& gc, const clan::Point& top_left, Tile* pTile )
{
	if(pTile->GetMonsterRegion() >= 0){
		clan::Pointf point(top_left.x,top_left.y);
		point += pTile->GetRect().get_top_left();
		point += clan::Point(16,24);
		m_font.set_color(get_color(pTile->GetMonsterRegion()));
		m_font.draw_text(gc,point, IntToString(pTile->GetMonsterRegion()));
	}
}

TileZOrderDrawer::TileZOrderDrawer()
{
	m_font = GraphicsManager::GetFont("mm_white");
}

TileZOrderDrawer::~TileZOrderDrawer()
{

}

void TileZOrderDrawer::accept( clan::Canvas& gc, const clan::Point& top_left, Tile* pTile )
{
	if(pTile->GetZOffset() != 0){
		clan::Pointf point(top_left.x,top_left.y);
		point += pTile->GetRect().get_top_left();
		std::string zorder = "+" + IntToString(pTile->GetZOffset());
		m_font.draw_text(gc,point, zorder,Font::TOP_LEFT);
	}
}





};