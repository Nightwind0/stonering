//TileSelector.cpp

#include "TileSelector.h"


TileSelector::TileSelector( CL_Component *parent, CL_ResourceManager* tsResources)
    :  CL_Component(parent), tsResources(tsResources)
{
//, TileSet tileset
// tileset(tileset),
	//set_size(get_parent()->get_width(), get_parent()->get_height());
	tsX = 0;
	tsY = 0;
	CL_Component * pWindow = get_parent();
	CL_Component * pClient = pWindow;

	cur_tileset = NULL;

	set_parent(pClient);
	//set_position(0,0);
	set_size(get_parent()->get_width() - 20,get_parent()->get_height() - 20);

	int tileX = 1;
	int tileY = 1;
	
	scrollVert = new CL_ScrollBar(false,pWindow);
	pWindow->add_child(scrollVert);

	
	scrollVert->set_size(20,pWindow->get_height() - 40);	
	scrollVert->set_position(pWindow->get_width()-20,40);

	//scrollVert->set_width(20);
	//scrollVert->set_height(get_parent()->get_height());
	scrollVert->set_tracking(true);

	scrollHorz = new CL_ScrollBar(true, pWindow);
	scrollHorz->set_position(0,  pWindow->get_height()-40);
	scrollHorz->set_width(pWindow->get_width()-20);
	scrollHorz->set_height(20);
	scrollHorz->set_tracking(true);

	pWindow->add_child(scrollHorz);

	
	std::vector<string> tilemapnames = tsResources->get_all_resources("Tilemaps");
	
	//cur_tileset_lable = new CL_Label(CL_Point(20, 10), "X", this);
	tsMapName = *tilemapnames.begin();
	cur_tileset = new CL_Surface(tsMapName, tsResources);//*tilemaps.begin();

	scrollVert->set_min_value(0);
	scrollHorz->set_min_value(0);

	scrollVert->set_max_value((cur_tileset->get_height()/32)- (get_width() /33));
	scrollHorz->set_max_value((cur_tileset->get_width()/32)- (get_height() / 33 ));

	slots.connect(sig_paint(), this, &TileSelector::on_paint);
	slots.connect(sig_mouse_up(), this, &TileSelector::on_select);


}


TileSelector::~TileSelector()
{
// does nothing

//	delete scrollVert;
//	delete scrollHorz;
//	delete cur_tileset;
}

void TileSelector::on_window_resize(int,int)
{
	CL_Component * pWindow = get_parent();
	set_size(get_parent()->get_width() - 20,get_parent()->get_height() - 20);

	scrollHorz->set_position(0, pWindow->get_height()-20);
	scrollHorz->set_width(pWindow->get_width()-20);
	scrollHorz->set_height(20);

	scrollVert->set_size(20,pWindow->get_height() - 40);	
	scrollVert->set_position(pWindow->get_width()-20,40);
}

void TileSelector::on_paint()
{
	//find_preferred_size();
//	set_position(get_parent()->get_screen_rect());	
	//set_size(get_parent()->get_width() - scrollVert->get_width(),
	//	get_parent()->get_height() - scrollHorz->get_height());
	
	//component background color
	CL_Display::fill_rect(get_screen_rect(), CL_Color::lightgrey);
	//CL_Display::fill_rect(CL_Rect(0, 0, get_width() , get_height()), CL_Color::blue);
	//component border color
	//CL_Display::draw_rect(client_to_screen(CL_Rect(0, 20, get_width() - scrollVert->get_width()+1, get_height() - scrollHorz->get_height()+1)), CL_Color::red);

	int sX = tsX - scrollHorz->get_value();
	int sY = tsY - scrollVert->get_value();
	//this rect shows which tile is selected.
	if(sX >= 0 && sY >= 0 && sX < (get_width()/33) && sY < (get_height()/33))
		CL_Display::draw_rect(client_to_screen(CL_Rect(sX*32,sY*32 , sX * 32 + 33, sY * 32 + 33)), CL_Color::cyan);

	draw();

  
}

void TileSelector::changeTS(string text)
{							   
//	cur_tileset_lable->set_text(text);

	tsMapName = text;

	delete cur_tileset;
	cur_tileset = new CL_Surface(tsMapName , tsResources);
		
	scrollVert->set_max_value((cur_tileset->get_height()/32)-11);
	scrollHorz->set_max_value((cur_tileset->get_width()/32)-5);

	
}

void TileSelector::draw()
{
	int tsetWidth, tsetHeight, scrollX, scrollY;

	tsetWidth = min(get_width() / 33,cur_tileset->get_width()/32);//cur_tileset->get_width();
	tsetHeight = min(get_height() / 33,cur_tileset->get_height()/32);//cur_tileset->get_height();

	scrollX = scrollHorz->get_value()*32;
	scrollY = scrollVert->get_value()*32;

	for(int i = 0; i < tsetHeight; i++)
	{
		for(int j = 0; j < tsetWidth; j++)
		{
			CL_Rect src((j * 32)+scrollX, (i * 32)+scrollY,(j * 32)+scrollX + 32,(i * 32)+scrollY + 32);
			CL_Rect dst = client_to_screen(CL_Rect((j * 32) + 1, (i * 32)+ 1, (j * 32) + 32, (i * 32) + 32));

			cur_tileset->draw(src, dst);


		}
	}


}

void TileSelector::on_select(const CL_InputEvent &event)
{
	int clickX, clickY;

	clickX = event.mouse_pos.x;
	clickY = event.mouse_pos.y;


	tsX = (clickX/33) + scrollHorz->get_value();
	tsY = (clickY/33) + scrollVert->get_value();

	std::cout << tsX << ',' << tsY << std::endl;

	
}




