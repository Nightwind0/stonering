//Infobar.cpp

#include "Infobar.h"


Infobar::Infobar( CL_Component *parent)
    :	 CL_Component(parent),tool_pos(NULL)
{


	set_size(parent->get_width(), 20);

	set_position(0, parent->get_height() - 50);

	text1 = new CL_Label(CL_Point(5,2), "Tool:", this);
	tool_text = new CL_Label(CL_Rect(35,2,100, 15), "Tile", this);


	setToolPos("0","0");
	text2 = new CL_Label(CL_Point(110,2), "Position:", this);
	tool_pos = new CL_Label(CL_Point(160,2), "(0000,0000)", this);

	slots.connect(sig_paint(), this, &Infobar::on_paint);

}


Infobar::~Infobar()
{
delete text1;
delete tool_text;
delete text2;
delete tool_pos;
}


void Infobar::on_paint()
{

	//component background color
//	CL_Display::fill_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::lightgrey);
	//component border color
	CL_Display::draw_rect(client_to_screen(CL_Rect(0, 0, get_width(), get_height())), CL_Color::grey);
	
	

}

void Infobar::on_window_resize(int,int)
{
	set_size(get_parent()->get_width(),20);
	set_position(0,get_parent()->get_height() - 20);
}


