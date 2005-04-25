//Infobar.cpp

#include "Infobar.h"


Infobar::Infobar(CL_Rect setrect, CL_Component *parent)
:	rect(setrect), CL_Component(parent)
{


	set_position(rect.left, rect.top);
//	set_position(30, 30);
	set_size(rect.get_width(), rect.get_height());
//	set_size(50, 50);

	slots.connect(sig_paint(), this, &Infobar::on_paint);

}


Infobar::~Infobar()
{
// do nothing
}


void Infobar::on_paint()
{

	//component background color
	CL_Display::fill_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::lightgrey);
	//component border color
	CL_Display::draw_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::grey);

	//Tool info
	CL_Label(CL_Rect(2, 2, get_width(), get_height()), "info", this);

}


