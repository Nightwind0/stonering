//GridPoint.cpp


#include "GridPoint.h"


GridPoint::GridPoint(CL_Rect setrect, CL_Component *parent)
:	rect(setrect), CL_Component(parent)
{


	set_position(rect.left, rect.top);
//	set_position(30, 30);
	set_size(rect.get_width(), rect.get_height());
//	set_size(50, 50);

	slots.connect(sig_paint(), this, &GridPoint::on_paint);

}


GridPoint::~GridPoint()
{
// do nothing
}


void GridPoint::on_paint()
{

	//component background color
	CL_Display::fill_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::lightgrey);
	//component border color
	CL_Display::draw_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::grey);


}

