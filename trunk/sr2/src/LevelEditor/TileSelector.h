//TileSelector.h

#ifndef TILESELECTOR_H
#define TILESELECTOR_H


#include <ClanLib/gui.h>
#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <string>
using namespace std;

namespace Editor{

class TileSelector : public clan::Component
{
public:
    TileSelector(clan::Component *parent, clan::ResourceManager* tsResources);
    //, TileSet tileset
    virtual ~TileSelector();

    void changeTS(string text);

    void on_paint();
    void draw();
    void on_select(const clan::InputEvent &event);
    void on_window_resize(int,int);

    int get_tsX() {return tsX;}
    int get_tsY() {return tsY;}
    string get_tsMapName() {return tsMapName.substr(tsMapName.rfind('/')+1, tsMapName.length()-1);}

private:

    clan::Rect TSrect;
    clan::Rect* SRCrect;
    clan::Rect* DSTrect;

    clan::ScrollBar *scrollVert;
    clan::ScrollBar *scrollHorz;
        

    clan::SlotContainer slots;
    clan::ResourceManager* tsResources;
    list<clan::Surface*> tilemaps;

    clan::Label *cur_tileset_lable;
    clan::Surface* cur_tileset;

    int tsX, tsY;
    string tsMapName;

};
};

#endif





