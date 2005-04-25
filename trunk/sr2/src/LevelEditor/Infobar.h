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



		void on_paint();

private:

		CL_Rect rect;
		CL_SlotContainer slots;

};

#endif

