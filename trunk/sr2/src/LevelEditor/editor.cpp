//editor.cpp  main function of program


#include "editor.h"

EditorMain::EditorMain()
{
	instance = this;
}

EditorMain::~EditorMain()
{
	instance = 0;
}


EditorMain *EditorMain::instance = 0;


EditorMain app;


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
			CL_DisplayWindow window("SR2 - Editor", 700, 600, false);

			
			CL_ResourceManager gui_resources((const std::string)"C:/Documents and Settings/Jon.Layton/My Documents/ClanLib-0.7.8-1/ClanLib-0.7/Resources/GUIStyleSilver/gui.xml");
			CL_StyleManager_Silver style(&gui_resources);
			CL_GUIManager gui(&style);
			gui_manager = &gui;

			
			// Make sure our background is drawn under the GUI
			CL_Slot slot_draw = gui.sig_paint().connect(this, &EditorMain::on_paint);

			CL_Menu menu(CL_Point(0,0), &gui);

			// standard menu stuff
			menu.create_item("File/New");
			menu.create_item("File/Open");
			menu.create_item("File/Save");
			menu.create_item("File/Save As...");
			menu.create_item("File/Quit");

			menu.create_item("TileSet/Load");

			// menu item signal connects
			CL_Slot slot_menu_quit = menu.get_node("File/Quit")->sig_clicked().connect(this, &EditorMain::on_quit);

			
			TileSelector tiles(CL_Rect(510, 30, 690, 400), &gui);
			
			MapGrid map(CL_Rect(10, 30, 500, 590), &gui);

			GridPoint gp(CL_Rect(510, 410, 690, 590), &gui);

		
			



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
		quit = true;
	}

	void EditorMain::on_paint()
	{
		CL_Display::clear(CL_Color::lightgrey);
	}





