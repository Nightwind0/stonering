//editor.cpp  main function of program


#include "editor.h"
#include "IApplication.h"
#include "EditableLevel.h"
#include "filedialog.h"

using namespace StoneRing;

bool gbDebugStop = false;

EditorMain::EditorMain()
{
	mpParty = new EditorParty();
	mpLevelFactory = new EditableLevelFactory();
	instance = this;
	info = NULL;
	
}

EditorMain::~EditorMain()
{
	instance = 0;
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
	return 700;
}

int EditorMain::getScreenHeight()const
{
	return 615;
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
			CL_DisplayWindow display("SR2 - Editor", getScreenWidth(), getScreenHeight(), false);

			CL_ResourceManager gui_resources("gui.xml",new CL_Zip_Archive("guistylesilver.gui"),true);
			CL_StyleManager_Silver style(&gui_resources);
			CL_GUIManager gui(&style);
			gui_manager = &gui;

			gc = display.get_gc();
			CL_Window window(CL_Rect(0, 0, getScreenWidth(), getScreenHeight()), "Window with menu", &gui);

			
			// Make sure our background is drawn under the GUI
			slots.connect(gui.sig_paint(),this, &EditorMain::on_paint);

#if 1
			CL_Menu menu(window.get_client_area());

			// standard menu stuff
			menu.create_item("File/New");
			menu.create_item("File/Open");
			menu.create_item("File/Save");
			menu.create_item("File/Save As...");
			menu.create_item("File/Quit");
		

			//tools menu stuff
			menu.create_item("Tools/Add Row");
			menu.create_item("Tools/Add Column");
			menu.create_item("Tools/-----");
			menu.create_item("Tools/Add Tile");
			menu.create_item("Tools/Set Hot");
			menu.create_item("Tools/Direction Blocks -/North");
			menu.create_item("Tools/Direction Blocks -/South");
			menu.create_item("Tools/Direction Blocks -/East");
			menu.create_item("Tools/Direction Blocks -/West");


			//various options 
			menu.create_item("Options/Show Hot Tiles");
			menu.create_item("Options/Show Direction Blocks");
			menu.create_item("Options/Show Mappable Objects");
			menu.create_item("Options/Hide Floaters");




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
				CL_MenuNode * pMenu = menu.create_item(menutileset);
				
				slots.connect(pMenu->sig_clicked(), this, &EditorMain::on_tileset_change, *iter);

				tempsets.push_back(*iter);
				
			}
			tilemapnames = tempsets;

			menu.create_item("About/Info");


			// menu item slot connects

			slots.connect(menu.get_node("File/Quit")->sig_clicked(), this, &EditorMain::on_quit);

			slots.connect(menu.get_node("File/Save")->sig_clicked(), this, &EditorMain::on_save);
			slots.connect(menu.get_node("File/Save As...")->sig_clicked(), this, &EditorMain::on_save);
			slots.connect(menu.get_node("File/Open")->sig_clicked(), this, &EditorMain::on_load);
			slots.connect(menu.get_node("File/New")->sig_clicked(), this, &EditorMain::on_new);

			slots.connect(menu.get_node("Tools/Add Row")->sig_clicked(), this, &EditorMain::on_add_row);
			slots.connect(menu.get_node("Tools/Add Column")->sig_clicked(), this, &EditorMain::on_add_column);

			string tiletemp1 = "north";
			string tiletemp2 = "south";
			string tiletemp3 = "east";
			string tiletemp4 = "west";
			string tiletemp5 = "hot";
			string tiletemp6 = "tile";

			slots.connect(menu.get_node("Tools/Direction Blocks -/North")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp1);
			slots.connect(menu.get_node("Tools/Direction Blocks -/South")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp2);
			slots.connect(menu.get_node("Tools/Direction Blocks -/East")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp3);
			slots.connect(menu.get_node("Tools/Direction Blocks -/West")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp4);
			slots.connect(menu.get_node("Tools/Set Hot")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp5);
			slots.connect(menu.get_node("Tools/Add Tile")->sig_clicked(), this, &EditorMain::on_change_tool, tiletemp6);
			


			slots.connect(menu.get_node("Options/Show Hot Tiles")->sig_clicked(), this, &EditorMain::on_show_hot);
			slots.connect(menu.get_node("Options/Show Direction Blocks")->sig_clicked(), this, &EditorMain::on_show_blocks);

#endif
			//other random slot connects
			slots.connect(display.sig_window_close(), this, &EditorMain::on_quit);


//			cout<< "creating tileselector" << endl;
			tiles = new TileSelector(CL_Rect(510, 30, 691, 403), &gui, tsResources);

//			cout << "creating mapgrid" << endl;			
			map = new MapGrid(CL_Rect(5, 30, 505, 592), &gui, display.get_gc(), tiles);


//			cout << "creating gridpoint" << endl;
			GridPoint gp(CL_Rect(510, 410, 691, 592), &gui);

			//info box (bottom of screen)
		//	info = new CL_InputBox(CL_Rect(5, 595, 505, 604), map->getCurrentTool(), &gui);
		//	info->enable(false);
			info = new Infobar(CL_Rect(5, 595, 505, 612), &gui);
		
		
cout << "all the creation stuff completed. about to run it." << endl;			



			quit = false; //this quit gets triggered true by a signal from the menu
			while (!CL_Keyboard::get_keycode(CL_KEY_ESCAPE) && !quit)
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
		bttn = CL_MessageBox::info("Warning", "Would you like to save the current level before quiting?", "Yes", "No", "Cancel", gui_manager);
		
		if(bttn == 0)
		{
			on_save();
		}

	}

	if(bttn != 2)
	{
		quit = true;
	}
}

void EditorMain::on_paint()
{
	CL_Display::clear(CL_Color::lightgrey);
}

void EditorMain::on_tileset_change(string userdata)
{
	tiles->changeTS(userdata);
}


void EditorMain::on_save()
{
    string filename = CL_FileDialog::open("", "*.xml", gui_manager);;  //= SR_FileDialog::save("", "*.xml", gui_manager);

//	    cout << filename << endl;
	if(filename != "")
		map->save_Level(filename);
}

void EditorMain::on_load()
{
	int bttn=0;

	if(mpLevel != NULL)
	{
		bttn = CL_MessageBox::info("Warning", "Would you like to save the current level before opening a new one?", "Yes", "No", "Cancel", gui_manager);
		
		if(bttn == 0)
		{
			on_save();
		}

	}

	if(bttn != 2)
	{
		string filename = CL_FileDialog::open("", "*.xml", gui_manager);

		if(filename != "")
		{
			CL_InputSource_File file(filename);

			CL_DomDocument doc;
			doc.load(&file);

			delete mpLevel;

			mpLevel = new EditableLevel();

			mpLevel->load(doc);

			map->set_Level(mpLevel);
		}
	}

}


void EditorMain::on_new()
{
	
	int bttn;

	if(mpLevel != NULL)
	{
		bttn = CL_MessageBox::info("Warning", "Would you like to save the current level before creating a new one?", "Yes", "No", "Cancel", gui_manager);
		
		if(bttn == 0)
		{
			on_save();
		}

	}

	if(bttn != 2)
	{
		CL_InputDialog new_dlg("Create New Level", "Ok", "Cancel", "", gui_manager);

		CL_InputBox *lvlName = new_dlg.add_input_box("Level Name:", "", 150);
		CL_InputBox *lvlMusic = new_dlg.add_input_box("Music:", "", 150);
		CL_InputBox *lvlWidth = new_dlg.add_input_box("Width (in tiles):", "10", 150);
		CL_InputBox *lvlHeight = new_dlg.add_input_box("Height (in tiles):", "10", 150);

		// Connecting signals, to allow only numbers
//		slots.connect(lvlWidth->sig_validate_character(), this, &App::validator_numbers);
//		slots.connect(lvlHeight->sig_validate_character(), this, &App::validator_numbers);

		new_dlg.run();
	

		if(mpLevel != NULL)
			delete mpLevel;	

		mpLevel = new EditableLevel();

		mpLevel->setName(lvlName->get_text());
		mpLevel->setMusic(lvlMusic->get_text());
		mpLevel->addColumns(atoi(lvlWidth->get_text().c_str()));
		mpLevel->addRows(atoi(lvlHeight->get_text().c_str()));

		map->set_Level(mpLevel);
	
	}

}

void EditorMain::on_add_row()
{
	map->more_rows(1);
}

void EditorMain::on_add_column()
{
	map->more_columns(1);
}

void EditorMain::on_show_hot()
{
	map->toggle_hot();
}

void EditorMain::on_show_blocks()
{
	map->toggle_blocks();
}

void EditorMain::on_change_tool(string newtool)
{
	if(newtool == "hot")
	{
		info->setToolText("Hot");
		map->switchTool(newtool);
		if(!map->get_hotflag())
			map->toggle_hot();
		if(map->get_blocksflag())
			map->toggle_blocks();
	}
	else if(newtool == "north")
	{
		info->setToolText("Dir Block N");
		map->switchTool(newtool);
		if(!map->get_blocksflag())
			map->toggle_blocks();
		if(map->get_hotflag())
			map->toggle_hot();
	}
	else if(newtool == "south")
	{
		info->setToolText("Dir Block S");
		map->switchTool(newtool);
		if(!map->get_blocksflag())
			map->toggle_blocks();
		if(map->get_hotflag())
			map->toggle_hot();
	}
	else if(newtool == "east")
	{
		info->setToolText("Dir Block E");
		map->switchTool(newtool);
		if(!map->get_blocksflag())
			map->toggle_blocks();
		if(map->get_hotflag())
			map->toggle_hot();
	}
	else if(newtool == "west")
	{
		info->setToolText("Dir Block W");
		map->switchTool(newtool);
		if(!map->get_blocksflag())
			map->toggle_blocks();
		if(map->get_hotflag())
			map->toggle_hot();
	}
	else
	{
		info->setToolText("Tile");
		map->switchTool("tile");
		if(map->get_hotflag())
			map->toggle_hot();
		if(map->get_blocksflag())
			map->toggle_blocks();

	}
	//info->set_text(map->getCurrentTool());

}
