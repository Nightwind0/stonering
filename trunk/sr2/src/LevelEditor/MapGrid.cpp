//MapGrid.cpp


#include "MapGrid.h"


MapGrid::MapGrid(CL_Rect setrect, CL_Component *parent)
:	rect(setrect), CL_Component(parent)
{


	set_position(rect.left, rect.top);
//	set_position(30, 30);
	set_size(rect.get_width(), rect.get_height());
//	set_size(50, 50);

	slots.connect(sig_paint(), this, &MapGrid::on_paint);

}


MapGrid::~MapGrid()
{
// do nothing
}


void MapGrid::on_paint()
{

	//component background color
	CL_Display::fill_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::lightgreen);
	//component border color
	CL_Display::draw_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::grey);


}