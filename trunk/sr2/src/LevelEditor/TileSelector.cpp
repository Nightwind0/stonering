//TileSelector.cpp

#include "TileSelector.h"


TileSelector::TileSelector(CL_Rect setrect, CL_Component *parent, CL_ResourceManager* tsResources)
:	TSrect(setrect), CL_Component(parent), tsResources(tsResources)
{
//, TileSet tileset
// tileset(tileset),

	set_position(TSrect.left, TSrect.top);
	set_size(TSrect.get_width(), TSrect.get_height());

	SRCrect = new CL_Rect(0,0,32,32);
	DSTrect = new CL_Rect(TSrect);



////

	list<string> tilemapnames = tsResources->get_all_resources("Tilemaps");

	

	for(list<string>::iterator i = tilemapnames.begin();i != tilemapnames.end(); i++)
	{
		
		tilemaps.push_back( new CL_Surface(*i, tsResources));//"Tilemaps/wall1", tsResources));
	}
////
	
	cur_tileset_lable = new CL_Label(CL_Point(20, 10), tilemapnames.front(), this);
	cur_tileset = new CL_Surface(*tilemapnames.begin(), tsResources);//*tilemaps.begin();


	slots.connect(sig_paint(), this, &TileSelector::on_paint);

}


TileSelector::~TileSelector()
{
// does nothing
}


void TileSelector::on_paint()
{


	//component background color
	CL_Display::fill_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::white);
	//component border color
	CL_Display::draw_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::grey);


}

void TileSelector::setCurLable(string text)
{							   
	cur_tileset_lable->set_text(text);



	tilemaps.front()->draw(*SRCrect, *DSTrect);

}

void TileSelector::draw()
{
	

}

