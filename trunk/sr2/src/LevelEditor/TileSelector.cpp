//TileSelector.cpp

#include "TileSelector.h"


TileSelector::TileSelector(CL_Rect setrect, CL_Component *parent, CL_ResourceManager* tsResources)
:	TSrect(setrect), CL_Component(parent), tsResources(tsResources)
{
//, TileSet tileset
// tileset(tileset),

	set_position(TSrect.left, TSrect.top);
	set_size(TSrect.get_width(), TSrect.get_height());

int tileX = 1;
int tileY = 1;

	SRCrect = new CL_Rect(32,0,64,32);
	DSTrect = new CL_Rect(tileX, tileY, tileX + 32, tileY + 32);



	list<string> tilemapnames = tsResources->get_all_resources("Tilemaps");
	
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

	//cur_tileset->draw(*SRCrect, *DSTrect);//20,20);//
	draw();
}

void TileSelector::setCurLable(string text)
{							   
	cur_tileset_lable->set_text(text);

	cur_tileset = new CL_Surface(text , tsResources);

}

void TileSelector::draw()
{
	int tsetWidth, tsetHeight;

	tsetWidth = cur_tileset->get_width();
	tsetHeight = cur_tileset->get_height();

	for(int i = 0; i < tsetHeight / 32; i++)
	{
		for(int j = 0; j < tsetWidth / 32; j++)
		{
			SRCrect = new CL_Rect((j * 32), (i * 32),(j * 32) + 32,(i * 32) + 32);
			DSTrect = new CL_Rect((j * 32) + 1, (i * 32)+ 1, (j * 32) + 32, (i * 32) + 32);

			cur_tileset->draw(*SRCrect, *DSTrect);
		}
	}




}

