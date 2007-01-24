//Infobar.h

#ifndef INFOBAR_H
#define INFOBAR_H

#include <ClanLib/gui.h>
#include <ClanLib/display.h>
#include <iostream>
using namespace std;


class Infobar : CL_Component
{
public:
    Infobar( CL_Component *parent);

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

    CL_Rect rect;
    CL_SlotContainer slots;
    CL_Label* text1;
    CL_Label* tool_text;
    CL_Label* text2;
    CL_Label* tool_pos;

    string pos;
};

#endif



