//MapGrid.cpp


#include "MapGrid.h"


MapGrid::MapGrid(CL_Rect setrect, CL_Component *parent, CL_GraphicContext *mgGC, TileSelector *TS)
:	rect(setrect), CL_Component(parent), mgGC(mgGC), TS(TS)
{
//EditableLevel *mpLevel,
//, mgLevel(null)

	hotflag = blocksflag = false;

	cur_tool = "tile";  

	mgLevel = NULL;

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


	mgScrollVert->set_max_value(0);
	mgScrollHorz->set_max_value(0);


	slots.connect(sig_paint(), this, &MapGrid::on_paint);
	slots.connect(sig_mouse_up(), this, &MapGrid::on_Tool_Click);//on_placeTile

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


	if(mgLevel)  //we dont want to try drawing an empty level
	{
		CL_Rect dst(0,0,min((unsigned int)15,mgLevel->getWidth())*32-20, min((unsigned int)17,mgLevel->getHeight())*32-20);
		CL_Rect src(mgScrollX,mgScrollY, mgScrollX+dst.get_width(), mgScrollY+dst.get_height());

		mgLevel->draw(src, dst, mgGC, false, hotflag, blocksflag);
	
		//when dan fixes the mappables to work in the editor just uncomment this line.
		//mgLevel->drawMappableObjects(src,dst,mgGC);
	}


}

void MapGrid::on_placeTile(const CL_InputEvent &event)
{
	if(mgLevel)  //nasty things might happen if we tried adding tiles to nonexistant levels
	{
		int clickX, clickY;

		clickX = event.mouse_pos.x;
		clickY = event.mouse_pos.y;

		if(clickX <get_width()-20 && clickY<get_height()-20)
		{
			mgX = (clickX/32) + mgScrollHorz->get_value();
			mgY = (clickY/32) + mgScrollVert->get_value();

			cout << "Tile (" << mgX << "," << mgY << ")" << endl;


			if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
			{
				// Zap mode, delete the topmost tile
				std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );

				std::list<Tile*>::iterator lastGuy = TileList.end();

				// Since it's currently actually one PAST the lastguy.
				lastGuy--;

				mgLevel->removeTile( *lastGuy );
				

			}
			else
			{
				// Default mode is add mode
				EditableTile *pTile = new EditableTile();
				//cout << TS->get_tsMapName() << endl;
				pTile->setTilemap(TS->get_tsMapName(), TS->get_tsX(), TS->get_tsY());
				
				int ZOrder = -1;
				std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );
				for( std::list<Tile*>::iterator iter = TileList.begin(); iter != TileList.end(); iter++)
				{
				
				if( (*iter)->getZOrder() > ZOrder) 
					ZOrder = (*iter)->getZOrder();
				
				}
				ZOrder++;
				
				pTile->setZOrder( ZOrder );
				pTile->setLevelX ( mgX );
				pTile->setLevelY ( mgY );
				
				mgLevel->addTile( pTile );
			}

		}
	}
}


void MapGrid::set_Level(EditableLevel *mpLevel)
{
	mgLevel = mpLevel;

	mgScrollVert->set_max_value((mgLevel->getHeight())-17);
	mgScrollHorz->set_max_value((mgLevel->getWidth())-15);
}


void MapGrid::save_Level(string filename)
{
	CL_DomDocument newdoc;


	newdoc.append_child ( mgLevel->createDomElement(newdoc) );


	newdoc.save( new CL_OutputSource_File( filename ), true,false );

}

void MapGrid::more_rows(int r)
{
	if(mgLevel != NULL)
	{
		mgLevel->addRows(r);

		mgScrollVert->set_max_value((mgLevel->getHeight())-17);
	}

}

void MapGrid::more_columns(int c)
{
	if(mgLevel != NULL)
	{
		mgLevel->addColumns(c);

		mgScrollHorz->set_max_value((mgLevel->getWidth())-15);
	}
}

void MapGrid::toggle_hot()
{
	if(hotflag)
		hotflag = false;
	else
		hotflag = true;

}

void MapGrid::toggle_blocks()
{
	if(blocksflag)
		blocksflag = false;
	else
		blocksflag = true;

}

void MapGrid::on_Tool_Click(const CL_InputEvent &event)
{
		
	if(cur_tool == "hot")
		on_setHot(event);
	else if(cur_tool == "north")
		on_setNorth(event);
	else if(cur_tool == "south")
		on_setSouth(event);
	else if(cur_tool == "east")
		on_setEast(event);
	else if(cur_tool == "west")
		on_setWest(event);
	else
		on_placeTile(event);
}


void MapGrid::switchTool(string toolname)
{
	if(toolname == "hot")
		cur_tool = toolname;
	else if(toolname == "north")
		cur_tool = toolname;
	else if(toolname == "south")
		cur_tool = toolname;
	else if(toolname == "east")
		cur_tool = toolname;
	else if(toolname == "west")
		cur_tool = toolname;
	else
		cur_tool = "tile";

}

void MapGrid::on_setHot(const CL_InputEvent &event)
{
	if(mgLevel)  //nasty things might happen if we set stuff on nonexistant levels
	{
		int clickX, clickY;

		clickX = event.mouse_pos.x;
		clickY = event.mouse_pos.y;

		if(clickX <get_width()-20 && clickY<get_height()-20)
		{
			mgX = (clickX/32) + mgScrollHorz->get_value();
			mgY = (clickY/32) + mgScrollVert->get_value();

			cout << "Hot (" << mgX << "," << mgY << ")" << endl;


			if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
			{
				// Anti-Mode.  UNset the value if its set.
				std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );


				

			}
			else
			{
				// Default mode is set mode
				
			}

		}
	}
}

void MapGrid::on_setNorth(const CL_InputEvent &event)
{
	if(mgLevel)  //nasty things might happen if we set stuff on nonexistant levels
	{
		int clickX, clickY;

		clickX = event.mouse_pos.x;
		clickY = event.mouse_pos.y;

		if(clickX <get_width()-20 && clickY<get_height()-20)
		{
			mgX = (clickX/32) + mgScrollHorz->get_value();
			mgY = (clickY/32) + mgScrollVert->get_value();

			cout << "North (" << mgX << "," << mgY << ")" << endl;


			if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
			{
				// Anti-Mode.  UNset the value if its set.
				std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );


				

			}
			else
			{
				// Default mode is set mode
				
			}

		}
	}
}

void MapGrid::on_setSouth(const CL_InputEvent &event)
{
	if(mgLevel)  //nasty things might happen if we set stuff on nonexistant levels
	{
		int clickX, clickY;

		clickX = event.mouse_pos.x;
		clickY = event.mouse_pos.y;

		if(clickX <get_width()-20 && clickY<get_height()-20)
		{
			mgX = (clickX/32) + mgScrollHorz->get_value();
			mgY = (clickY/32) + mgScrollVert->get_value();

			cout << "South (" << mgX << "," << mgY << ")" << endl;


			if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
			{
				// Anti-Mode.  UNset the value if its set.
				std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );


				

			}
			else
			{
				// Default mode is set mode
				
			}

		}
	}
}

void MapGrid::on_setEast(const CL_InputEvent &event)
{
	if(mgLevel)  //nasty things might happen if we set stuff on nonexistant levels
	{
		int clickX, clickY;

		clickX = event.mouse_pos.x;
		clickY = event.mouse_pos.y;

		if(clickX <get_width()-20 && clickY<get_height()-20)
		{
			mgX = (clickX/32) + mgScrollHorz->get_value();
			mgY = (clickY/32) + mgScrollVert->get_value();

			cout << "East (" << mgX << "," << mgY << ")" << endl;


			if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
			{
				// Anti-Mode.  UNset the value if its set.
				std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );


				

			}
			else
			{
				// Default mode is set mode
				
			}

		}
	}
}

void MapGrid::on_setWest(const CL_InputEvent &event)
{
	if(mgLevel)  //nasty things might happen if we set stuff on nonexistant levels
	{
		int clickX, clickY;

		clickX = event.mouse_pos.x;
		clickY = event.mouse_pos.y;

		if(clickX <get_width()-20 && clickY<get_height()-20)
		{
			mgX = (clickX/32) + mgScrollHorz->get_value();
			mgY = (clickY/32) + mgScrollVert->get_value();

			cout << "West (" << mgX << "," << mgY << ")" << endl;


			if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
			{
				// Anti-Mode.  UNset the value if its set.
				std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );


				

			}
			else
			{
				// Default mode is set mode
				
			}

		}
	}
}


