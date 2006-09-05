//GridPoint.h

#ifndef GRIDPOINT_H
#define GRIDPOINT_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>
using namespace std;



class GridPoint : CL_Component
{
public:
		GridPoint(CL_Rect setrect, CL_Component *parent);

		~GridPoint();



		void on_paint();

private:

		CL_Rect rect;
		CL_SlotContainer slots;

};


#endif



