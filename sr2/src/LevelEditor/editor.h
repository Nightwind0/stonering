//editor.h


#ifndef EDITOR_H
#define EDITOR_H

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include <ClanLib/gui.h>
#include <ClanLib/guistylesilver.h>

#include "TileSelector.h"
#include "MapGrid.h"
#include "GridPoint.h"


class EditorMain : public CL_ClanApplication
{
public:
	EditorMain();
	~EditorMain();

	static EditorMain *instance;

private:
	int main(int argc, char **argv);
	
	CL_GUIManager *gui_manager;
	CL_ComponentManager *component_manager;

	void on_quit();
	
	void on_paint();
	void on_tileset_change(string userdata);

	bool quit;
	string menuitem;

	CL_SlotContainer slots;
};

#endif