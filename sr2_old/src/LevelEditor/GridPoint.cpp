//GridPoint.cpp


#include "GridPoint.h"


GridPoint::GridPoint(clan::Rect setrect, clan::Component *parent)
:   rect(setrect), clan::Component(parent)
{


    set_position(rect.left, rect.top);
//  set_position(30, 30);
    set_size(rect.get_width(), rect.get_height());
//  set_size(50, 50);

    slots.connect(sig_paint(), this, &GridPoint::on_paint);

}


GridPoint::~GridPoint()
{
// do nothing
}


void GridPoint::on_paint()
{

    //component background color
    clan::Display::fill_rect(clan::Rect(0, 0, get_width(), get_height()), clan::Color::lightgrey);
    //component border color
    clan::Display::draw_rect(clan::Rect(0, 0, get_width(), get_height()), clan::Color::grey);


}





