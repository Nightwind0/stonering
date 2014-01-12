//Infobar.h

#ifndef INFOBAR_H
#define INFOBAR_H

#include <ClanLib/gui.h>
#include <ClanLib/display.h>
#include <iostream>
using namespace std;

namespace Editor{

class Infobar : clan::Component
{
public:
    Infobar( clan::Component *parent);

    virtual ~Infobar();

    void setToolText(string txt){ tool_text->set_text(txt);}
    void setToolPos(string x, string y)
    {
        pos = "";
        pos += "("+x+","+y+")";
            
        if(tool_pos)
            tool_pos->set_text(pos);
    }

    void on_paint();
    void on_window_resize(int,int);

private:

    clan::Rect rect;
    clan::SlotContainer slots;
    clan::Label* text1;
    clan::Label* tool_text;
    clan::Label* text2;
    clan::Label* tool_pos;

    string pos;
};
};
#endif





