//TileSelector.cpp

#include "TileSelector.h"


TileSelector::TileSelector(CL_Rect setrect, CL_Component *parent, CL_ResourceManager* tsResources)
:	rect(setrect), CL_Component(parent), tsResources(tsResources)
{
//, TileSet tileset
// tileset(tileset),

	set_position(rect.left, rect.top);
	set_size(rect.get_width(), rect.get_height());

	cur_tileset_lable = new CL_Label(CL_Point(10, 10), "Select a TileSet", this);//gui_manager

////

	list<string> tilemapnames = tsResources->get_all_resources("Tilemaps");

	

	for(list<string>::iterator i = tilemapnames.begin();i != tilemapnames.end(); i++)
	{
		tilemaps.push_back( new CL_Surface(*i, &tsResources));
	}
////
	


	slots.connect(sig_paint(), this, &TileSelector::on_paint);

}


TileSelector::~TileSelector()
{
// do nothing
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
	cur_tileset_label.set_text(text);

}