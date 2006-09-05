//editor.cpp  main function of program


#include "editor.h"
#include "IApplication.h"
#include "EditableLevel.h"


using namespace StoneRing;

bool gbDebugStop = false;

EditorMain::EditorMain():mpParty(NULL),mpLevelFactory(NULL),mInfo(NULL)
{
	mpParty = new EditorParty();
	mpLevelFactory = new EditableLevelFactory();
	instance = this;

}

EditorMain::~EditorMain()
{
	instance = 0;
	delete mTiles;
	delete mMap;
	delete mInfo;
}


EditorMain *EditorMain::instance = 0;


EditorMain app;

IApplication * IApplication::getInstance() 
{
	return &app;
}

 
CL_ResourceManager * EditorMain::getResources()const
{
	return mpResources;
}


StoneRing::IParty * EditorMain::getParty() const
{
	return mpParty;
}

LevelFactory * EditorMain::getLevelFactory() const
{
    return mpLevelFactory;
}
		
int EditorMain::getScreenWidth()const
{
	return 1024;
}

int EditorMain::getScreenHeight()const
{
	return 768;
}


CL_Rect EditorMain::getLevelRect() const
{
	// Allow for scrolling
	return CL_Rect(10, 30, 500, 590);
}

CL_Rect EditorMain::getDisplayRect() const
{
	return CL_Rect(10, 30, 500, 590);
}


bool EditorMain::canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot, bool isPlayer)
{
	// return mpLevel->canMove
	return true;
}



int EditorMain::main(int argc, char **argv)
{
		// Create a console window for text-output if not available
		// Use printf or cout to display some text in your program
		CL_ConsoleWindow console("Console");
		console.redirect_stdio();

		try
		{
			CL_SetupCore::init();
			CL_SetupDisplay::init();
			CL_SetupGL::init();
			CL_SetupGUI::init();
			///////////////////Start my code//////////////
			

			// Create a display window
			CL_DisplayWindow display("SR2 - Editor", getScreenWidth(), getScreenHeight(), false, true);

			CL_ResourceManager gui_resources("gui.xml",new CL_Zip_Archive("guistylesilver.gui"),true);
			CL_StyleManager_Silver style(&gui_resources);
			CL_GUIManager gui(&style);
			mGui_manager = &gui;

			mGc = display.get_gc();
			CL_Window window(CL_Rect(0, 50, 640, 620), "Map", gui.get_client_area());
			CL_Window tileWindow(CL_Rect(650,75,900,380),"Tile Selector",gui.get_client_area());
			
			// Make sure our background is drawn under the GUI
			mSlots.connect(gui.sig_paint(),this, &EditorMain::on_paint);


			CL_Menu menu(&gui);
			CL_Menu windowMenu(window.get_client_area());
			CL_Menu tileMenu(tileWindow.get_client_area());

			// standard menu stuff
			menu.create_item("File/New");
			menu.create_item("File/Open");
			menu.create_item("File/Save");
			menu.create_item("File/Save As...");
			menu.create_item("File/Quit");
		

			//tools menu stuff
			windowMenu.create_item("Tools/Add Row");
			windowMenu.create_item("Tools/Add Column");
			windowMenu.create_item("Tools/-----");
			windowMenu.create_item("Tools/Add Tile");
			windowMenu.create_item("Tools/Set Hot");
			windowMenu.create_item("Tools/Direction Blocks -/North");
			windowMenu.create_item("Tools/Direction Blocks -/South");
			windowMenu.create_item("Tools/Direction Blocks -/East");
			windowMenu.create_item("Tools/Direction Blocks -/West");


			//various options 
			windowMenu.create_item("Options/Show Hot Tiles");
			windowMenu.create_item("Options/Show Direction Blocks");
			windowMenu.create_item("Options/Show Mappable Objects");
			windowMenu.create_item("Options/Hide Floaters");




			////get the tileset info from xml
			CL_ResourceManager* tsResources = new CL_ResourceManager ( "../../Media/resources.xml" );
			mpResources = tsResources;

		
			mpLevel = NULL;
			vector<string> tilemapnames = tsResources->get_all_resources("Tilemaps");

			vector<string> tempsets;

			string menutileset;

			////create menu from tileset info

			for(vector<string>::iterator iter = tilemapnames.begin(); iter != tilemapnames.end(); iter++)
			{
				menutileset = "TileSet/" + *iter;
				CL_MenuNode * pMenu = tileMenu.create_item(menutileset);
				
				mSlots.connect(pMenu->sig_clicked(), this, &EditorMain::on_tileset_change, *iter);

				tempsets.push_back(*iter);
				
			}
			tilemapnames = tempsets;

			menu.create_item("About/Info");


			// menu item slot connects

			mSlots.connect(menu.get_node("File/Quit")->sig_clicked(), this, &EditorMain::on_quit);

			mSlots.connect(menu.get_node("File/Save")->sig_clicked(), this, &EditorMain::on_save);
			mSlots.connect(menu.get_node("File/Save As...")->sig_clicked(), this, &EditorMain::on_save);
			mSlots.connect(menu.get_node("File/Open")->sig_clicked(), this, &EditorMain::on_load);
			mSlots.connect(menu.get_node("File/New")->sig_clicked(), this, &EditorMain::on_new);

			mSlots.connect(windowMenu.get_node("Tools/Add Row")->sig_clicked(), this, &EditorMain::on_add_row);
			mSlots.connect(windowMenu.get_node("Tools/Add Column")->sig_clicked(), this, &EditorMain::on_add_column);

			string tiletemp1 = "north";
			string tiletemp2 = "south";
			string tiletemp3 = "east";
			string tiletemp4 = "west";
			string tiletemp5 = "hot";
			string tiletemp6 = "tile";

			mSlots.connect(windowMenu.get_node("Tools/Direction Blocks -/North")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp1);
			mSlots.connect(windowMenu.get_node("Tools/Direction Blocks -/South")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp2);
			mSlots.connect(windowMenu.get_node("Tools/Direction Blocks -/East")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp3);
			mSlots.connect(windowMenu.get_node("Tools/Direction Blocks -/West")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp4);
			mSlots.connect(windowMenu.get_node("Tools/Set Hot")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp5);
			mSlots.connect(windowMenu.get_node("Tools/Add Tile")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp6);
			


			mSlots.connect(windowMenu.get_node("Options/Show Hot Tiles")->sig_clicked(), this, &EditorMain::on_show_hot);
			mSlots.connect(windowMenu.get_node("Options/Show Direction Blocks")->sig_clicked(), this, &EditorMain::on_show_blocks);


			//other random slot connects
			mSlots.connect(display.sig_window_close(), this, &EditorMain::on_quit);

			mTiles = new TileSelector(tileWindow.get_client_area(), tsResources);

			mTiles->set_position( tileWindow.get_client_area()->get_client_x(), tileWindow.get_client_area()->get_client_y() + windowMenu.get_height()); 
			mSlots.connect(tileWindow.sig_resize(),mTiles,&TileSelector::on_window_resize);

			mMap = new MapGrid(window.get_client_area(), display.get_gc(), mTiles);
			mMap->set_position( window.get_client_area()->get_client_x(), window.get_client_area()->get_client_y() + windowMenu.get_height());
			mSlots.connect(window.sig_resize(), mMap,&MapGrid::on_window_resize);

			mInfo = new Infobar( gui.get_client_area());

			mSlots.connect(gui.sig_resize(),mInfo, &Infobar::on_window_resize);
#if 0
//			cout<< "creating tileselector" << endl;
	

//			cout << "creating mapgrid" << endl;			
		


//			cout << "creating gridpoint" << endl;
			GridPoint gp(CL_Rect(510, 410, 691, 592), &gui);

			//info box (bottom of screen)
		//	info = new CL_InputBox(CL_Rect(5, 595, 505, 604), mMap->getCurrentTool(), &gui);
		//	info->enable(false);
		
		
#endif
cout << "all the creation stuff completed. about to run it." << endl;			



			mbQuit = false; //this quit gets triggered true by a signal from the menu
			while (!CL_Keyboard::get_keycode(CL_KEY_ESCAPE) && !mbQuit)
			{
				gui.show();

				CL_System::keep_alive();
				CL_Display::flip();
			}

			///////////////////End my code////////////////
			CL_SetupGUI::deinit();
			CL_SetupGL::deinit();
			CL_SetupDisplay::deinit();
			CL_SetupCore::deinit();
		}
		// Catch any errors from ClanLib
		catch (CL_Error err)
		{
			// Display the error message
			std::cout << err.message.c_str() << std::endl;
		}

		// Display console close message and wait for a key
		console.display_close_message();

		return 0;
}


void EditorMain::on_quit()
{
	int bttn=0;

	if(mpLevel != NULL)
	{
		bttn = CL_MessageBox::info("Warning", "Would you like to save the current level before quiting?", "Yes", "No", "Cancel", mGui_manager);
		
		if(bttn == 0)
		{
			on_save();
		}

	}

	if(bttn != 2)
	{
		mbQuit = true;
	}
}

void EditorMain::on_paint()
{
	CL_Display::clear(CL_Color::lightgrey);
}

void EditorMain::on_tileset_change(string userdata)
{
	mTiles->changeTS(userdata);
}


void EditorMain::on_save()
{
	
//	mpDialog = new CL_FileDialog("Save","","*.xml",mGui_manager);

	//	string path = dialog.get_path();
	string filename = CL_FileDialog::save(mGui_manager);

//	    cout << filename << endl;
	if(filename != "")
		mMap->save_Level(filename);
	else
	{
		CL_MessageBox::info("You didn't enter a file name. You (sadly) have to hit Enter after typing in the name.",mGui_manager);
	}
}

void EditorMain::on_load()
{
	int bttn=0;

	if(mpLevel != NULL)
	{
		bttn = CL_MessageBox::info("Warning", "Would you like to save the current level before opening a new one?", "Yes", "No", "Cancel", mGui_manager);
		
		if(bttn == 0)
		{
			on_save();
		}

	}

	if(bttn != 2)
	{
		string filename = CL_FileDialog::open("", "*.xml", mGui_manager);

		if(filename != "")
		{
			CL_InputSource_File file(filename);

			CL_DomDocument doc;
			doc.load(&file);

			delete mpLevel;

			mpLevel = new EditableLevel();

			mpLevel->load(doc);

			mMap->set_Level(mpLevel);
		}
	}

}


void EditorMain::on_new()
{
	
	int bttn=0;

	if(mpLevel != NULL)
	{
		bttn = CL_MessageBox::info("Warning", "Would you like to save the current level before creating a new one?", "Yes", "No", "Cancel", mGui_manager);
		
		if(bttn == 0)
		{
			on_save();
		}

	}

	if(bttn != 2)
	{
		CL_InputDialog new_dlg("Create New Level", "Ok", "Cancel", "", mGui_manager);

		CL_InputBox *lvlName = new_dlg.add_input_box("Level Name:", "", 150);
		CL_InputBox *lvlMusic = new_dlg.add_input_box("Music:", "", 150);
		CL_InputBox *lvlWidth = new_dlg.add_input_box("Width (in tiles):", "10", 150);
		CL_InputBox *lvlHeight = new_dlg.add_input_box("Height (in tiles):", "10", 150);

		// Connecting signals, to allow only numbers
//		mSlots.connect(lvlWidth->sig_validate_character(), this, &App::validator_numbers);
//		mSlots.connect(lvlHeight->sig_validate_character(), this, &App::validator_numbers);

		new_dlg.run();
	

		if(mpLevel != NULL)
			delete mpLevel;	

		mpLevel = new EditableLevel();

		mpLevel->setName(lvlName->get_text());
		mpLevel->setMusic(lvlMusic->get_text());
		mpLevel->addColumns(atoi(lvlWidth->get_text().c_str()));
		mpLevel->addRows(atoi(lvlHeight->get_text().c_str()));

		mMap->set_Level(mpLevel);
	
	}

}

void EditorMain::on_add_row()
{
	mMap->more_rows(1);
}

void EditorMain::on_add_column()
{
	mMap->more_columns(1);
}

void EditorMain::on_show_hot()
{
	mMap->toggle_hot();
}

void EditorMain::on_show_blocks()
{
	mMap->toggle_blocks();
}

void EditorMain::on_change_tool(string newtool)
{
	if(newtool == "hot")
	{
		mInfo->setToolText("Hot");
		mMap->switchTool(newtool);
		if(!mMap->get_hotflag())
			mMap->toggle_hot();
		if(mMap->get_blocksflag())
			mMap->toggle_blocks();
	}
	else if(newtool == "north")
	{
		mInfo->setToolText("Dir Block N");
		mMap->switchTool(newtool);
		if(!mMap->get_blocksflag())
			mMap->toggle_blocks();
		if(mMap->get_hotflag())
			mMap->toggle_hot();
	}
	else if(newtool == "south")
	{
		mInfo->setToolText("Dir Block S");
		mMap->switchTool(newtool);
		if(!mMap->get_blocksflag())
			mMap->toggle_blocks();
		if(mMap->get_hotflag())
			mMap->toggle_hot();
	}
	else if(newtool == "east")
	{
		mInfo->setToolText("Dir Block E");
		mMap->switchTool(newtool);
		if(!mMap->get_blocksflag())
			mMap->toggle_blocks();
		if(mMap->get_hotflag())
			mMap->toggle_hot();
	}
	else if(newtool == "west")
	{
		mInfo->setToolText("Dir Block W");
		mMap->switchTool(newtool);
		if(!mMap->get_blocksflag())
			mMap->toggle_blocks();
		if(mMap->get_hotflag())
			mMap->toggle_hot();
	}
	else
	{
		mInfo->setToolText("Tile");
		mMap->switchTool("tile");
		if(mMap->get_hotflag())
			mMap->toggle_hot();
		if(mMap->get_blocksflag())
			mMap->toggle_blocks();

	}
	//info->set_text(mMap->getCurrentTool());

}


