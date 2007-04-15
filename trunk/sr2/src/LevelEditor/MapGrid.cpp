//MapGrid.cpp


#include "MapGrid.h"
#include "editor.h"
#include <sstream>

using Editor::MapGrid;
using Editor::TileSelector;

MapGrid::MapGrid(CL_Component *parent, CL_GraphicContext *mgGC, TileSelector *TS)
    :   CL_Component(parent), mgGC(mgGC), TS(TS)
{
//EditableLevel *mpLevel,
//, mgLevel(null)

    hotflag = blocksflag = false;

    cur_tool = "tile";  

    mgLevel = NULL;

    set_size(get_parent()->get_width() - 20, get_parent()->get_height() - 50);

    mgScrollVert = new CL_ScrollBar(0, 0, false, get_parent());
    mgScrollVert->set_position(get_parent()->get_width() - 20, 40);
    mgScrollVert->set_width(20);
    mgScrollVert->set_height(get_parent()->get_height()-40);
    mgScrollVert->set_tracking(true);

    mgScrollHorz = new CL_ScrollBar(0, 0, true, get_parent());
    mgScrollHorz->set_position(0, get_parent()->get_height() - 20);
    mgScrollHorz->set_width(get_parent()->get_width()-20);
    mgScrollHorz->set_height(20);
    mgScrollHorz->set_tracking(true);

    mgScrollVert->set_min_value(0);
    mgScrollHorz->set_min_value(0);

    mgScrollVert->set_max_value(0);
    mgScrollHorz->set_max_value(0);


    slots.connect(sig_paint(), this, &MapGrid::on_paint);
    slots.connect(sig_mouse_up(), this, &MapGrid::on_Tool_Click);//on_placeTile
    slots.connect(sig_mouse_move(), this, &MapGrid::on_mouse_move);


}


MapGrid::~MapGrid()
{
// do nothing
//  delete mgScrollVert;
//  delete mgScrollHorz;
}

void MapGrid::on_window_resize(int,int)
{
    set_size(get_parent()->get_width() - 20, get_parent()->get_height() - 50);

    mgScrollVert->set_position(get_parent()->get_width() - 20, 20);
    mgScrollVert->set_width(20);
    mgScrollVert->set_height(get_parent()->get_height() - 40);
    mgScrollVert->set_tracking(true);

    mgScrollHorz->set_position(0, get_parent()->get_height() - 20);
    mgScrollHorz->set_width(get_parent()->get_width()-20);
    mgScrollHorz->set_height(20);
}

void MapGrid::on_mouse_move(const CL_InputEvent &event)
{
    std::ostringstream xStr;
    std::ostringstream yStr;

    int X = (event.mouse_pos.x/32) + mgScrollHorz->get_value();
    int Y = (event.mouse_pos.y/32) + mgScrollVert->get_value();

    xStr << X;
    yStr << Y;

    if(dynamic_cast<EditorMain*>(EditorMain::getInstance())->getInfo())
        dynamic_cast<EditorMain*>(EditorMain::getInstance())->getInfo()->setToolPos( xStr.str(), yStr.str() );
}


void MapGrid::on_paint()
{
    int mgScrollX, mgScrollY;
    
    mgScrollX = mgScrollHorz->get_value()*32;
    mgScrollY = mgScrollVert->get_value()*32;

    int widthInTiles = get_width() / 32  ;
    int heightInTiles = get_height() / 32 ;


    //component background color
    CL_Display::fill_rect(client_to_screen(CL_Rect(0, 0, get_width(), get_height())), CL_Color::lightgreen);
    //component border color
    CL_Display::draw_rect(client_to_screen(CL_Rect(0, 0, get_width(), get_height())), CL_Color::grey);


    if(mgLevel)  //we dont want to try drawing an empty level
    {
        CL_Rect dst(0,0,min((unsigned int)widthInTiles,mgLevel->getWidth())*32, min((unsigned int)heightInTiles,mgLevel->getHeight())*32);
        CL_Rect src(mgScrollX,mgScrollY, mgScrollX+dst.get_width(), mgScrollY+dst.get_height());

        mgLevel->draw(src, client_to_screen(dst), mgGC, false, hotflag, blocksflag);
    
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

        if(clickX <get_width() && clickY<get_height())
        {
            mgX = (clickX/32) + mgScrollHorz->get_value();
            mgY = (clickY/32) + mgScrollVert->get_value();

            cout << "Tile (" << mgX << "," << mgY << ")" << endl;

            if(mgX >= mgLevel->getWidth() ||
               mgY >= mgLevel->getHeight())
            {
                cout << "Not a valid location..." << endl;
                return;
            }


            if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
            {
                // Zap mode, delete the topmost tile
                std::list<StoneRing::Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );

                if(TileList.size() > 0)
                {

                    std::list<StoneRing::Tile*>::iterator lastGuy = TileList.end();

                    // Since it's currently actually one PAST the lastguy.
                    lastGuy--;

                    mgLevel->removeTile( *lastGuy );
                }
                

            }
            else
            {
                // Default mode is add mode
                Editor::Tile *pTile = new Editor::Tile();
                //cout << TS->get_tsMapName() << endl;
                pTile->setTilemap(TS->get_tsMapName(), TS->get_tsX(), TS->get_tsY());
                
                int ZOrder = -1;
                std::list<StoneRing::Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );
                for( std::list<StoneRing::Tile*>::iterator iter = TileList.begin(); iter != TileList.end(); iter++)
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


void MapGrid::set_Level(Editor::Level *pLevel)
{
    mgLevel = pLevel;

    int widthInTiles = get_width() / 32 ;
    int heightInTiles = get_height() / 32 ;

    mgScrollVert->set_max_value((mgLevel->getHeight())-heightInTiles);
    mgScrollHorz->set_max_value((mgLevel->getWidth())-widthInTiles);
}


void MapGrid::save_Level(string filename)
{
    CL_DomDocument newdoc;
    CL_OutputSource_File  os(filename);

    os.open();


    newdoc.append_child ( mgLevel->createDomElement(newdoc) );


    newdoc.save( &os, false, true );

    os.close();


}

void MapGrid::more_rows(int r)
{
    if(mgLevel != NULL)
    {
        mgLevel->addRows(r);

        mgScrollVert->set_max_value((mgLevel->getHeight())- (get_height()/32 ));
    }

}

void MapGrid::more_columns(int c)
{
    if(mgLevel != NULL)
    {
        mgLevel->addColumns(c);

        mgScrollHorz->set_max_value((mgLevel->getWidth())- (get_width() / 32 ));
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

        if(clickX <get_width() && clickY<get_height())
        {
            mgX = (clickX/32) + mgScrollHorz->get_value();
            mgY = (clickY/32) + mgScrollVert->get_value();

            cout << "Hot (" << mgX << "," << mgY << ")" << endl;


            if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
            {
                // Anti-Mode.  UNset the value if its set.
                //std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );

                mgLevel->setHotAt(mgX, mgY, false);
                
            }
            else
            {
                // Default mode is set mode
                mgLevel->setHotAt(mgX, mgY, true);
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

        if(clickX <get_width() && clickY<get_height())
        {
            mgX = (clickX/32) + mgScrollHorz->get_value();
            mgY = (clickY/32) + mgScrollVert->get_value();

            cout << "North (" << mgX << "," << mgY << ")" << endl;


            if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
            {
                // Anti-Mode.  UNset the value if its set.
                //std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );

                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_NORTH, false);
                
            }
            else
            {
                // Default mode is set mode
                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_NORTH, true);
                
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

        if(clickX <get_width() && clickY<get_height())
        {
            mgX = (clickX/32) + mgScrollHorz->get_value();
            mgY = (clickY/32) + mgScrollVert->get_value();

            cout << "South (" << mgX << "," << mgY << ")" << endl;


            if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
            {
                // Anti-Mode.  UNset the value if its set.
                //std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );

                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_SOUTH, false);
                
            }
            else
            {
                // Default mode is set mode
                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_SOUTH, true);
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

        if(clickX <get_width() && clickY<get_height())
        {
            mgX = (clickX/32) + mgScrollHorz->get_value();
            mgY = (clickY/32) + mgScrollVert->get_value();

            cout << "East (" << mgX << "," << mgY << ")" << endl;


            if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
            {
                // Anti-Mode.  UNset the value if its set.
                //std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );

                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_EAST, false);
        
            }
            else
            {
                // Default mode is set mode
                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_EAST, true);
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

        if(clickX <get_width() && clickY<get_height())
        {
            mgX = (clickX/32) + mgScrollHorz->get_value();
            mgY = (clickY/32) + mgScrollVert->get_value();

            cout << "West (" << mgX << "," << mgY << ")" << endl;


            if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ))
            {
                // Anti-Mode.  UNset the value if its set.
                //std::list<Tile*> TileList = mgLevel->getTilesAt( mgX, mgY );

                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_WEST, false);
                
            }
            else
            {
                // Default mode is set mode
                mgLevel->setDirectionBlockAt(mgX, mgY, StoneRing::DIR_WEST, true);
            }

        }
    }
}




