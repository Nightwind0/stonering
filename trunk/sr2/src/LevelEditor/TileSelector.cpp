//TileSelector.cpp

#include "TileSelector.h"


TileSelector::TileSelector(CL_Rect setrect, CL_Component *parent, CL_ResourceManager* tsResources)
:	TSrect(setrect), CL_Component(parent), tsResources(tsResources)
{
//, TileSet tileset
// tileset(tileset),

	tsX = 0;
	tsY = 0;

	set_position(TSrect.left, TSrect.top);
	set_size(TSrect.get_width(), TSrect.get_height());

int tileX = 1;
int tileY = 1;

	SRCrect = new CL_Rect(32,0,64,32);
	DSTrect = new CL_Rect(tileX, tileY, tileX + 32, tileY + 32);

		
	scrollVert = new CL_ScrollBar(0, 0, false, this);
	scrollVert->set_position(get_width()-20, 0);
	scrollVert->set_width(20);
	scrollVert->set_height(get_height()-20);
	scrollVert->set_tracking(true);

	scrollHorz = new CL_ScrollBar(0, 0, true, this);
	scrollHorz->set_position(0, get_height()-20);
	scrollHorz->set_width(get_width()-20);
	scrollHorz->set_height(20);
	scrollHorz->set_tracking(true);


	list<string> tilemapnames = tsResources->get_all_resources("Tilemaps");
	
	//cur_tileset_lable = new CL_Label(CL_Point(20, 10), "X", this);
	tsMapName = *tilemapnames.begin();
	cur_tileset = new CL_Surface(tsMapName, tsResources);//*tilemaps.begin();

	scrollVert->set_min_value(0);
	scrollHorz->set_min_value(0);

	scrollVert->set_max_value((cur_tileset->get_height()/32)-12);
	scrollHorz->set_max_value((cur_tileset->get_width()/32)-6);

	slots.connect(sig_paint(), this, &TileSelector::on_paint);
	slots.connect(sig_mouse_up(), this, &TileSelector::on_select);

}


TileSelector::~TileSelector()
{
// does nothing
}


void TileSelector::on_paint()
{
	
	//component background color
	CL_Display::fill_rect(CL_Rect(0, 0, get_width() - scrollVert->get_width(), get_height() - scrollHorz->get_height()), CL_Color::white);
	//component border color
	CL_Display::draw_rect(CL_Rect(0, 0, get_width() - scrollVert->get_width()+1, get_height() - scrollHorz->get_height()+1), CL_Color::grey);

	//this rect shows which tile is selected.
	CL_Display::draw_rect(CL_Rect((tsX-scrollHorz->get_value())*32,(tsY-scrollVert->get_value())*32,(tsX-scrollHorz->get_value())*32+33,(tsY-scrollVert->get_value())*32+33), CL_Color::blue);

	draw();

  
}

void TileSelector::changeTS(string text)
{							   
//	cur_tileset_lable->set_text(text);

	tsMapName = text;

	cur_tileset = new CL_Surface(tsMapName , tsResources);
		
	scrollVert->set_max_value((cur_tileset->get_height()/32)-12);
	scrollHorz->set_max_value((cur_tileset->get_width()/32)-6);


}

void TileSelector::draw()
{
	int tsetWidth, tsetHeight, scrollX, scrollY;

	tsetWidth = min(5,cur_tileset->get_width()/32);//cur_tileset->get_width();
	tsetHeight = min(11,cur_tileset->get_height()/32);//cur_tileset->get_height();

	scrollX = scrollHorz->get_value()*32;
	scrollY = scrollVert->get_value()*32;

	for(int i = 0; i < tsetHeight; i++)
	{
		for(int j = 0; j < tsetWidth; j++)
		{
			SRCrect = new CL_Rect((j * 32)+scrollX, (i * 32)+scrollY,(j * 32)+scrollX + 32,(i * 32)+scrollY + 32);
			DSTrect = new CL_Rect((j * 32) + 1, (i * 32)+ 1, (j * 32) + 32, (i * 32) + 32);

			cur_tileset->draw(*SRCrect, *DSTrect);
		}
	}


}

void TileSelector::on_select(const CL_InputEvent &event)
{
	int clickX, clickY;

	clickX = event.mouse_pos.x;
	clickY = event.mouse_pos.y;

	if(clickX <160 && clickY<352)
	{
		tsX = (clickX/33) + scrollHorz->get_value();
		tsY = (clickY/33) + scrollVert->get_value();

		//cout << tsX << endl;


	}
}


