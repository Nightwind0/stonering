//Infobar.h

#ifndef INFOBAR_H
#define INFOBAR_H

#include <ClanLib/gui.h>
#include <ClanLib/display.h>
using namespace std;


class Infobar : CL_Component
{
public:
		Infobar(CL_Rect setrect, CL_Component *parent);

		~Infobar();

		void setToolText(string txt){ tool_text->set_text(txt);}
		void setToolPos(string x, string y){ pos = "("+x+","+y+")";}

		void on_paint();

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

