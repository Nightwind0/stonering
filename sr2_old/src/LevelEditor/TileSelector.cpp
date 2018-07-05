//TileSelector.cpp

#include "TileSelector.h"

using Editor::TileSelector;

TileSelector::TileSelector( clan::Component *parent, clan::ResourceManager* tsResources)
    :  clan::Component(parent), tsResources(tsResources)
{

    tsX = 0;
    tsY = 0;
    clan::Component * pWindow = get_parent();
    clan::Component * pClient = pWindow;

    cur_tileset = NULL;

    set_parent(pClient);
    set_size(get_parent()->get_width() - 20,get_parent()->get_height() - 20);

    int tileX = 1;
    int tileY = 1;
    
    scrollVert = new clan::ScrollBar(false,pWindow);
    pWindow->add_child(scrollVert);
    
	scrollVert->set_size(20,pWindow->get_height() - 20);	
	scrollVert->set_position(pWindow->get_width()-20,20);

    scrollVert->set_tracking(true);

    scrollHorz = new clan::ScrollBar(true, pWindow);
	scrollHorz->set_position(0,  pWindow->get_height()-20);
    scrollHorz->set_width(pWindow->get_width()-20);
    scrollHorz->set_height(20);
    scrollHorz->set_tracking(true);

    pWindow->add_child(scrollHorz);
    
    std::vector<string> tilemapnames = tsResources->get_all_resources("Tilemaps");
    
    tsMapName = *tilemapnames.begin();
    cur_tileset = new clan::Surface(tsMapName, tsResources);//*tilemaps.begin();

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

}

void TileSelector::on_window_resize(int,int)
{
    clan::Component * pWindow = get_parent();
    set_size(get_parent()->get_width() - 20,get_parent()->get_height() - 20);

    scrollHorz->set_position(0, pWindow->get_height()-20);
    scrollHorz->set_width(pWindow->get_width()-20);
    scrollHorz->set_height(20);

    scrollVert->set_size(20,pWindow->get_height() - 40);    
    scrollVert->set_position(pWindow->get_width()-20,40);
}

void TileSelector::on_paint()
{
    //component background color
    clan::Display::fill_rect(get_screen_rect(), clan::Color::lightgrey);
 
    int sX = tsX - scrollHorz->get_value();
    int sY = tsY - scrollVert->get_value();
    //this rect shows which tile is selected.
    if(sX >= 0 && sY >= 0 && sX < (get_width()/33) && sY < (get_height()/33))
        clan::Display::draw_rect(client_to_screen(clan::Rect(sX*32,sY*32 , sX * 32 + 33, sY * 32 + 33)), clan::Color::cyan);

    draw();
}

void TileSelector::changeTS(string text)
{                              
    tsMapName = text;

    delete cur_tileset;
    cur_tileset = new clan::Surface(tsMapName , tsResources);
        
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
            clan::Rect src((j * 32)+scrollX, (i * 32)+scrollY,(j * 32)+scrollX + 32,(i * 32)+scrollY + 32);
            clan::Rect dst = client_to_screen(clan::Rect((j * 32) + 1, (i * 32)+ 1, (j * 32) + 32, (i * 32) + 32));

            cur_tileset->draw(src, dst);
        }
    }


}

void TileSelector::on_select(const clan::InputEvent &event)
{
    int clickX, clickY;

    clickX = event.mouse_pos.x;
    clickY = event.mouse_pos.y;


    tsX = (clickX/33) + scrollHorz->get_value();
    tsY = (clickY/33) + scrollVert->get_value();

    std::cout << tsX << ',' << tsY << std::endl;

    
}






