//editor.cpp  main function of program


#include "editor.h"
#include "IApplication.h"
#include "EditableLevel.h"

using namespace StoneRing;

EditorMain::EditorMain()
{
	mpParty = new EditorParty();
	mpLevelFactory = new EditableLevelFactory();
	instance = this;
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
	return 600;
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


bool EditorMain::canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot)
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
			CL_DisplayWindow window("SR2 - Editor", getScreenWidth(), getScreenHeight(), false);

			CL_ResourceManager gui_resources("gui.xml",new CL_Zip_Archive("guistylesilver.gui"),true);
			CL_StyleManager_Silver style(&gui_resources);
			CL_GUIManager gui(&style);
			gui_manager = &gui;

			gc = window.get_gc();
			

			
			// Make sure our background is drawn under the GUI
			CL_Slot slot_draw = gui.sig_paint().connect(this, &EditorMain::on_paint);

			CL_Menu menu(CL_Point(0,0), &gui);

			// standard menu stuff
			menu.create_item("File/New");
			menu.create_item("File/Open");
			menu.create_item("File/Save");
			menu.create_item("File/Save As...");
			menu.create_item("File/Quit");


			//get the tileset info from xml
			CL_ResourceManager* tsResources = new CL_ResourceManager ( "../../Media/resources.xml" );

			mpResources = tsResources;

			std::string startinglevel =  CL_String::load("Game/StartLevel",mpResources);
		

			std::string path = CL_String::load("Game/LevelPath", mpResources);
			std::string filename = CL_String::load("Levels/" + startinglevel, mpResources);

			std::string leveldocfilename = "../../"+ path + filename;

			CL_InputSource_File file(leveldocfilename);

			CL_DomDocument doc;

			doc.load(&file);

			mpLevel = new EditableLevel();

			mpLevel->load(doc);
			


			list<string> tilemapnames = tsResources->get_all_resources("Tilemaps");

			list<string> tempsets;

			string menutileset;

			while(!tilemapnames.empty())
			{
				menutileset = "TileSet/" + tilemapnames.front();
				menu.create_item(menutileset);
				
				slots.connect(menu.get_node(menutileset)->sig_clicked(), this, &EditorMain::on_tileset_change, tilemapnames.front());

				tempsets.push_back(tilemapnames.front());
				tilemapnames.pop_front();
			}
			tilemapnames = tempsets;


			// menu item slot connects
			slots.connect(menu.get_node("File/Quit")->sig_clicked(), this, &EditorMain::on_quit);


			//other random slot connects
			slots.connect(window.sig_window_close(), this, &EditorMain::on_quit);



			tiles = new TileSelector(CL_Rect(510, 30, 691, 403), &gui, tsResources);
			
			MapGrid map(CL_Rect(5, 30, 505, 594), &gui, mpLevel, window.get_gc());

			GridPoint gp(CL_Rect(510, 410, 691, 590), &gui);

		
			



			quit = false; //this quit gets triggered true by a signal from the menu
			while (!CL_Keyboard::get_keycode(CL_KEY_ESCAPE) && !quit)
			{
				gui.show();

//				CL_Rect dst(10,30,min((unsigned int)500,mpLevel->getWidth()*32), min((unsigned int)590,mpLevel->getWidth()*32));
//				CL_Rect src(0,0,dst.get_width(),dst.get_height());

//				window.get_gc()->push_cliprect( dst );
//				mpLevel->draw(src,dst, window.get_gc(),false);
//				window.get_gc()->pop_cliprect();

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
		quit = true;
	}

	void EditorMain::on_paint()
	{
		CL_Display::clear(CL_Color::lightgrey);
	}

	void EditorMain::on_tileset_change(string userdata)
	{
		tiles->changeTS(userdata);
	}


