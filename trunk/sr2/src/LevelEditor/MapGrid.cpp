//MapGrid.cpp


#include "MapGrid.h"


MapGrid::MapGrid(CL_Rect setrect, CL_Component *parent, EditableLevel *mpLevel, CL_GraphicContext *mgGC, TileSelector *TS)
:	rect(setrect), CL_Component(parent), mgLevel(mpLevel), mgGC(mgGC), TS(TS)
{

	set_position(rect.left, rect.top);
	set_size(rect.get_width(), rect.get_height());


	mgScrollVert = new CL_ScrollBar(0, 0, false, this);
	mgScrollVert->set_position(get_width()-20, 0);
	mgScrollVert->set_width(20);
	mgScrollVert->set_height(get_height()-20);
	mgScrollVert->set_tracking(true);

	mgScrollHorz = new CL_ScrollBar(0, 0, true, this);
	mgScrollHorz->set_position(0, get_height()-20);
	mgScrollHorz->set_width(get_width()-20);
	mgScrollHorz->set_height(20);
	mgScrollHorz->set_tracking(true);

	mgScrollVert->set_min_value(0);
	mgScrollHorz->set_min_value(0);

	mgScrollVert->set_max_value((mgLevel->getHeight())-17);
	mgScrollHorz->set_max_value((mgLevel->getWidth())-15);


	slots.connect(sig_paint(), this, &MapGrid::on_paint);
	slots.connect(sig_mouse_up(), this, &MapGrid::on_placeTile);

}


MapGrid::~MapGrid()
{
// do nothing
}


void MapGrid::on_paint()
{
	int mgScrollX, mgScrollY;

	mgScrollX = mgScrollHorz->get_value()*32;
	mgScrollY = mgScrollVert->get_value()*32;


	//component background color
	CL_Display::fill_rect(CL_Rect(0, 0, get_width()-20, get_height()-20), CL_Color::lightgreen);
	//component border color
	CL_Display::draw_rect(CL_Rect(0, 0, get_width(), get_height()), CL_Color::grey);

	CL_Rect dst(0,0,min((unsigned int)15,mgLevel->getWidth())*32-20, min((unsigned int)17,mgLevel->getHeight())*32-20);
	CL_Rect src(mgScrollX,mgScrollY, mgScrollX+dst.get_width(), mgScrollY+dst.get_height());

	mgLevel->draw(src, dst, mgGC, false);

	//when dan fixes the mappables to work in the editor just uncomment this line.
	//mgLevel->drawMappableObjects(src,dst,mgGC);

}

void MapGrid::on_placeTile(const CL_InputEvent &event)
{
	int clickX, clickY;

	clickX = event.mouse_pos.x;
	clickY = event.mouse_pos.y;

	if(clickX <get_width()-20 && clickY<get_height()-20)
	{
		mgX = (clickX/32) + mgScrollHorz->get_value();
		mgY = (clickY/32) + mgScrollVert->get_value();

		cout << "(" << mgX << "," << mgY << ")" << endl;

		//EditableTile pTile;
		cout << TS->get_tsMapName() << endl;
		//pTile.setTilemap(TS->get_tsMapName(), TS->get_tsX(), TS->get_tsY());
	}
}