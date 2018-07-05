//MapGrid.h

#ifndef MAPGRID_H
#define MAPGRID_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>

#include "EditorElements.h"
#include "TileSelector.h"

using namespace std;

namespace Editor{

class MapGrid : public clan::Component
{
public:
    MapGrid( clan::Component *parent, clan::Canvas *mgGC, TileSelector *TS);
        
    virtual ~MapGrid();

    void set_Level(Editor::Level *pLevel);
    void save_Level(string filename);

    void switchTool(string toolname);

    void on_Tool_Click(const clan::InputEvent &event);
    void on_setHot(const clan::InputEvent &event);
    void on_setNorth(const clan::InputEvent &event);
    void on_setSouth(const clan::InputEvent &event);
    void on_setEast(const clan::InputEvent &event);
    void on_setWest(const clan::InputEvent &event);
    void on_placeTile(const clan::InputEvent &event);
    void on_paint();
    void on_dir_change(const string &new_dir);
    void on_mouse_move(const clan::InputEvent &event);
    void on_window_resize(int, int);

    void more_rows(int r = 1);
    void more_columns(int c = 1);

    void toggle_hot();
    void toggle_blocks();

    string getCurrentTool(){return cur_tool;}

    bool get_hotflag(){return hotflag;}
    bool get_blocksflag(){return blocksflag;}

private:

    //this should probably be an enum.
    string cur_tool;  //valid values: tile, hot, north, south, east, west

    clan::Rect rect;
    clan::SlotContainer slots;

    Editor::Level *mgLevel;

    clan::Canvas *mgGC;

    clan::ScrollBar *mgScrollVert;
    clan::ScrollBar *mgScrollHorz;

    int mgX, mgY;
    TileSelector *TS;

    clan::FileDialog *openLevel;

    bool hotflag, blocksflag;

        
};

};
#endif





