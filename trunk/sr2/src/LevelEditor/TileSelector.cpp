//TileSelector.cpp

#include "TileSelector.h"


TileSelector::TileSelector(CL_Rect setrect, CL_Component *parent)
:	rect(setrect), CL_Component(parent)
{
//, TileSet tileset
// tileset(tileset),

	set_position(rect.left, rect.top);
	set_size(rect.get_width(), rect.get_height());


/*////////////
	CL_ResourceManager manager("resource.xml");
	list<string> tilemapnames = manager.get_all_resources("Tilemaps");

	list<CL_Surface*> tilemaps;

	for(list<string>::iterator i = tilemapnames.begin();i != tilemapnames.end(); i++)
	{
		tilemaps.push_back( new CL_Surface ( *i, &manager ) ) ;
	}
*/////////



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