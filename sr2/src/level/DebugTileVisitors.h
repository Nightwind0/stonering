#if !defined(__DEBUG_TILE_VISITORS_H__)
#define __DEBUG_TILE_VISITORS_H__
#include "Level.h"

namespace StoneRing {

	class TileDirectionBlockDrawer : public Tile::Visitor {
	public:
		TileDirectionBlockDrawer ( );
		virtual ~TileDirectionBlockDrawer ( );
		virtual void accept(CL_GraphicContext& gc, const CL_Point& top_left, Tile* pTile);
	private:
	};
	
	class TilePopsDrawer : public Tile::Visitor {
	public:
		TilePopsDrawer ();
		virtual ~TilePopsDrawer();
		virtual void accept(CL_GraphicContext& gc, const CL_Point& top_left, Tile* pTile);
	private:
		CL_Sprite m_indicator;
	};
	
	class TileHotDrawer : public Tile::Visitor { 
	public:
		TileHotDrawer ( );
		virtual ~TileHotDrawer ( );
		virtual void accept(CL_GraphicContext& gc, const CL_Point& top_left, Tile* pTile);
	private:
	};
	
	class TileFloaterDrawer : public Tile::Visitor {
	public:
		TileFloaterDrawer ( );
		virtual ~TileFloaterDrawer ( );
		virtual void accept(CL_GraphicContext& gc, const CL_Point& top_left, Tile* pTile);
	private:
		CL_Sprite m_indicator;
	};
	
		
};

#endif