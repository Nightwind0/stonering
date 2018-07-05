//GridPoint.h

#ifndef GRIDPOINT_H
#define GRIDPOINT_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>
using namespace std;



class GridPoint : clan::Component
{
public:
    GridPoint(clan::Rect setrect, clan::Component *parent);

    virtual ~GridPoint();



    void on_paint();

private:

    clan::Rect rect;
    clan::SlotContainer slots;

};


#endif





