//TileSelector.h

#ifndef TILESELECTOR_H
#define TILESELECTOR_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>
using namespace std;



class TileSelector : CL_Component
{
public:
		TileSelector(CL_Rect setrect, CL_Component *parent);
//, TileSet tileset
		~TileSelector();



		void on_paint();

private:

		CL_Rect rect;
		CL_SlotContainer slots;

};


#endif