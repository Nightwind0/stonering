//TileSelector.h

#ifndef TILESELECTOR_H
#define TILESELECTOR_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <string>
using namespace std;



class TileSelector : public CL_Component
{
public:
		TileSelector(CL_Component *parent, CL_ResourceManager* tsResources);
//, TileSet tileset
		~TileSelector();

		void changeTS(string text);

		void on_paint();
		void draw();
		void on_select(const CL_InputEvent &event);

		int get_tsX() {return tsX;}
		int get_tsY() {return tsY;}
		string get_tsMapName() {return tsMapName.substr(tsMapName.rfind('/')+1, tsMapName.length()-1);}

private:

		CL_Rect TSrect;
		CL_Rect* SRCrect;
		CL_Rect* DSTrect;

		CL_ScrollBar *scrollVert;
		CL_ScrollBar *scrollHorz;
		

		CL_SlotContainer slots;
		CL_ResourceManager* tsResources;
		list<CL_Surface*> tilemaps;

		CL_Label *cur_tileset_lable;
		CL_Surface* cur_tileset;

		int tsX, tsY;
		string tsMapName;

};


#endif

