//MapGrid.h

#ifndef MAPGRID_H
#define MAPGRID_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>
using namespace std;



class MapGrid : CL_Component
{
public:
		MapGrid(CL_Rect setrect, CL_Component *parent);

		~MapGrid();



		void on_paint();

private:

		CL_Rect rect;
		CL_SlotContainer slots;

};


#endif

