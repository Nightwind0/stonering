//MapGrid.h

#ifndef MAPGRID_H
#define MAPGRID_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>

#include "EditableLevel.h"

using namespace std;



class MapGrid : CL_Component
{
public:
		MapGrid(CL_Rect setrect, CL_Component *parent, EditableLevel *mpLevel, CL_GraphicContext *mgGC);

		~MapGrid();


		void on_placeTile(const CL_InputEvent &event);
		void on_paint();

private:

		CL_Rect rect;
		CL_SlotContainer slots;

		EditableLevel *mgLevel;

		CL_GraphicContext *mgGC;

		CL_ScrollBar *mgScrollVert;
		CL_ScrollBar *mgScrollHorz;

		int mgX, mgY;

};


#endif

