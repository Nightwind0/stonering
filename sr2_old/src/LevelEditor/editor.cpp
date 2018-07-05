//editor.cpp  main function of program


#include "editor.h"
#include "IApplication.h"
#include "EditorElementFactory.h"
#include "EditorElements.h"

using namespace StoneRing;
using Editor::EditorMain;

bool gbDebugStop = false;

EditorMain::EditorMain():
mpResources(NULL),mpParty(NULL),mpLevelFactory(NULL),mInfo(NULL)
{
    mpParty = new EditorParty();
    mpLevelFactory = new EditorElementFactory();

}

EditorMain::~EditorMain()
{

//    delete mTiles;
    //   delete mMap;
    // delete mInfo;
    delete mpParty;
    delete mpLevelFactory;
    delete mpResources;
    
}




EditorMain app;

IApplication * IApplication::getInstance() 
{
    return &app;
}

 
clan::ResourceManager * EditorMain::getResources()const
{
    return mpResources;
}


StoneRing::Party * EditorMain::getParty() const
{
    return mpParty;
}
        
int EditorMain::getScreenWidth()const
{
    return 1000;

}

int EditorMain::getScreenHeight()const
{

    return 600;

}


clan::Rect EditorMain::getLevelRect() const
{
    // Allow for scrolling
    return clan::Rect(10, 30, 500, 590);
}

clan::Rect EditorMain::getDisplayRect() const
{
    return clan::Rect(10, 30, 500, 590);
}


bool EditorMain::canMove(const clan::Rect &currently, const clan::Rect &destination, bool noHot, bool isPlayer)
{
    // return mpLevel->canMove
    return true;
}

AstScript * EditorMain::loadScript(const std::string &name, const std::string &script)
{
    return mInterpreter.prebuildAst(name,script);
}

SteelType EditorMain::runScript(AstScript *pScript)
{
    return mInterpreter.runAst ( pScript );
}

SteelType EditorMain::runScript(AstScript *pScript, const ParameterList &params)
{
    return mInterpreter.runAst ( pScript, params );
}

int EditorMain::main(int argc, char **argv)
{
    // Create a console window for text-output if not available
    // Use printf or cout to display some text in your program
    clan::ConsoleWindow console("Console");
    console.redirect_stdio();

    try
    {
        clan::SetupCore setup_core;
        clan::SetupDisplay setup_display;
        clan::SetupGL setup_gl;
        clan::SetupGUI setup_gui;
        ///////////////////Start my code//////////////
            

        // Create a display window
        clan::DisplayWindow display("SR2 - Editor", getScreenWidth(), getScreenHeight(), false, true);

        clan::Zip_Archive * gui_zip = new clan::Zip_Archive("guistylesilver.gui");
        clan::ResourceManager gui_resources("gui.xml",gui_zip,true);
        clan::StyleManager_Silver style(&gui_resources);
        clan::GUIManager gui(&style);
        mGui_manager = &gui;


        //mGc = display.get_gc();
        clan::Window window(clan::Rect(0, 50, 640, 620), "Map", gui.get_client_area());
        clan::Window tileWindow(clan::Rect(650,75,900,380),"Tile Selector",gui.get_client_area());
            
        // Make sure our background is drawn under the GUI
        mSlots.connect(gui.sig_paint(),this, &EditorMain::on_paint);


        clan::Menu menu(&gui);
        clan::Menu windowMenu(window.get_client_area());
        clan::Menu tileMenu(tileWindow.get_client_area());

        clan::Slot slot_quit = display.sig_window_close().connect(this, &EditorMain::on_quit);
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
        clan::ResourceManager* tsResources = new clan::ResourceManager ( "../../Media/resources.xml" );
        mpResources = tsResources;
        mAppUtils.loadGameItemsAndSkills("../../",mpResources);

        
        mpLevel = NULL;
        vector<string> tilemapnames = tsResources->get_all_resources("Tilemaps");

        vector<string> tempsets;

        string menutileset;

        ////create menu from tileset info

        for(vector<string>::iterator iter = tilemapnames.begin(); iter != tilemapnames.end(); iter++)
        {
            menutileset = "TileSet/" + *iter;
            clan::MenuNode * pMenu = tileMenu.create_item(menutileset);
                
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
//          cout<< "creating tileselector" << endl;
    

//          cout << "creating mapgrid" << endl;         
        


//          cout << "creating gridpoint" << endl;
        GridPoint gp(clan::Rect(510, 410, 691, 592), &gui);

        //info box (bottom of screen)
        //  info = new clan::InputBox(clan::Rect(5, 595, 505, 604), mMap->getCurrentTool(), &gui);
        //  info->enable(false);
        
        
#endif
        cout << "all the creation stuff completed. about to run it." << endl;           
            
            
        gui.run();
    
//  delete tsResources;
//  delete gui_zip;


    }
    // Catch any errors from ClanLib
    catch (clan::Error err)
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
        bttn = clan::MessageBox::info("Warning", "Would you like to save the current level before quiting?", "Yes", "No", "Cancel", mGui_manager);
        
        if(bttn == 0)
        {
            on_save();
        }

    }

    if(bttn != 2)
    {
        mGui_manager->quit();
    }
}

void EditorMain::on_paint()
{
    clan::Display::clear(clan::Color::lightgrey);
}

void EditorMain::on_tileset_change(string userdata)
{
    mTiles->changeTS(userdata);
}


void EditorMain::on_save()
{
    
//  mpDialog = new clan::FileDialog("Save","","*.xml",mGui_manager);

    //  string path = dialog.get_path();
    string filename = clan::FileDialog::save(mGui_manager);

//      cout << filename << endl;
    if(filename != "")
        mMap->save_Level(filename);
    else
    {
        clan::MessageBox::info("You didn't enter a file name. You (sadly) have to hit Enter after typing in the name.",mGui_manager);
    }
}

void EditorMain::on_load()
{
    int bttn=0;

    if(mpLevel != NULL)
    {
        bttn = clan::MessageBox::info("Warning", "Would you like to save the current level before opening a new one?", "Yes", "No", "Cancel", mGui_manager);
        
        if(bttn == 0)
        {
            on_save();
        }

    }

    if(bttn != 2)
    {
        string filename = clan::FileDialog::open("", "*.xml", mGui_manager);

        if(filename != "")
        {
            delete mpLevel;

            mpLevel = new Editor::Level();

            mpLevel->loadFromFile(filename);

            mMap->set_Level(mpLevel);
        }
    }

}


void EditorMain::on_new()
{
    
    int bttn=0;

    if(mpLevel != NULL)
    {
        bttn = clan::MessageBox::info("Warning", "Would you like to save the current level before creating a new one?", "Yes", "No", "Cancel", mGui_manager);
        
        if(bttn == 0)
        {
            on_save();
        }

    }

    if(bttn != 2)
    {
        clan::InputDialog new_dlg("Create New Level", "Ok", "Cancel", "", mGui_manager);

        clan::InputBox *lvlName = new_dlg.add_input_box("Level Name:", "", 150);
        clan::InputBox *lvlMusic = new_dlg.add_input_box("Music:", "", 150);
        clan::InputBox *lvlWidth = new_dlg.add_input_box("Width (in tiles):", "10", 150);
        clan::InputBox *lvlHeight = new_dlg.add_input_box("Height (in tiles):", "10", 150);

        // Connecting signals, to allow only numbers
//      mSlots.connect(lvlWidth->sig_validate_character(), this, &App::validator_numbers);
//      mSlots.connect(lvlHeight->sig_validate_character(), this, &App::validator_numbers);

        new_dlg.run();
    

        if(mpLevel != NULL)
            delete mpLevel; 

        mpLevel = new Editor::Level();

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




