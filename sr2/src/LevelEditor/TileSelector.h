//TileSelector.h

#ifndef TILESELECTOR_H
#define TILESELECTOR_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <string>
using namespace std;



class TileSelector : CL_Component
{
public:
		TileSelector(CL_Rect setrect, CL_Component *parent, CL_ResourceManager* tsResources);
//, TileSet tileset
		~TileSelector();

		void setCurLable(string text);

		void on_paint();

private:

		CL_Rect rect;
		CL_SlotContainer slots;
		CL_ResourceManager* tsResources;
		string cur_tileset;
		list<CL_Surface*> tilemaps;

		CL_Label * cur_tileset_lable;

};


#endif