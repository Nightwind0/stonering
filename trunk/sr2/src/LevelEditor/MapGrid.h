//MapGrid.h

#ifndef MAPGRID_H
#define MAPGRID_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>

#include "EditableLevel.h"
#include "TileSelector.h"

using namespace std;



class MapGrid : public CL_Component
{
public:
		MapGrid(CL_Rect setrect, CL_Component *parent, CL_GraphicContext *mgGC, TileSelector *TS);

		~MapGrid();

		void set_Level(EditableLevel *mpLevel);
		void save_Level(string filename);

		void on_placeTile(const CL_InputEvent &event);
		void on_paint();
		void on_dir_change(const string &new_dir);

		void more_rows(int r = 1);
		void more_columns(int c = 1);

		void toggle_hot();
		void toggle_blocks();

private:

		CL_Rect rect;
		CL_SlotContainer slots;

		EditableLevel *mgLevel;

		CL_GraphicContext *mgGC;

		CL_ScrollBar *mgScrollVert;
		CL_ScrollBar *mgScrollHorz;

		int mgX, mgY;
		TileSelector *TS;

		CL_FileDialog *openLevel;

		bool hotflag, blocksflag;

};


#endif

