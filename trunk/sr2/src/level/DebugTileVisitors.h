#if !defined(__DEBUG_TILE_VISITORS_H__)
#define __DEBUG_TILE_VISITORS_H__
#include "Level.h"

namespace StoneRing {

	class TileSideBlockDrawer : public Tile::Visitor {
	public:
		TileSideBlockDrawer ( );
		virtual ~TileSideBlockDrawer ( );
		virtual void accept(clan::Canvas& gc, const clan::Point& top_left, Tile* pTile);
	private:
	};
	

	
	class TileHotDrawer : public Tile::Visitor { 
	public:
		TileHotDrawer ( );
		virtual ~TileHotDrawer ( );
		virtual void accept(clan::Canvas& gc, const clan::Point& top_left, Tile* pTile);
	private:
	};
	
	class TileFloaterDrawer : public Tile::Visitor {
	public:
		TileFloaterDrawer ( );
		virtual ~TileFloaterDrawer ( );
		virtual void accept(clan::Canvas& gc, const clan::Point& top_left, Tile* pTile);
	private:
		clan::Sprite m_indicator;
	};

	class TileZOrderDrawer : public Tile::Visitor {
	public:
		TileZOrderDrawer ( );
		virtual ~TileZOrderDrawer ( );
		virtual void accept(clan::Canvas& gc, const clan::Point& top_left, Tile* pTile);
	private:
		Font m_font;
	};
		
	
	class TileMonsterRegionDrawer : public Tile::Visitor { 
	public:
		TileMonsterRegionDrawer( );
		virtual ~TileMonsterRegionDrawer ( );
		virtual void accept(clan::Canvas& gc, const clan::Point& top_left, Tile* pTile);
	private:
		clan::Colorf get_color(char id)const;
		Font m_font;
	};
	
		
};

#endif