/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "TileSelectorWindow.h"
#include "TileSelector.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include <Level.h>

#ifdef SR2_EDITOR

namespace StoneRing { 

TileSelectorWindow::TileSelectorWindow(CL_GUIComponent* owner, const CL_GUITopLevelDescription& desc):CL_Window(owner,desc)
{
	// TODO: Live within the get_client_area()
    m_pMenuBar = new CL_MenuBar(this);
    m_pMenuBar->set_geometry(CL_Rect(CL_Point(0,24),CL_Size(get_geometry().get_size().width,24)));
    m_pVertScroll = new CL_ScrollBar(this);
    m_pVertScroll->set_vertical();
    m_pVertScroll->set_min(0);
    m_pVertScroll->set_max(1);
    m_pVertScroll->set_geometry(CL_Rect(CL_Point(get_geometry().get_size().width-16,48),CL_Size(16,get_geometry().get_size().height-60)));
    m_pHorizScroll = new CL_ScrollBar(this);
    m_pHorizScroll->set_horizontal();
    m_pHorizScroll->set_min(0);
    m_pHorizScroll->set_max(1);
    m_pHorizScroll->set_geometry(CL_Rect(CL_Point(0,get_geometry().get_size().height-16),
                                         CL_Size(get_geometry().get_size().width,16)));
    
    m_pTileSelector = new TileSelector(this);
    m_pTileSelector->set_geometry(CL_Rect(CL_Point(0,48),CL_Size(get_geometry().get_size().width-16,get_geometry().get_size().height-70)));

    m_pHorizScroll->func_scroll().set(this,&TileSelectorWindow::on_horiz_scroll);
    m_pVertScroll->func_scroll().set(this,&TileSelectorWindow::on_vert_scroll);
    
    create_menu();
}

TileSelectorWindow::~TileSelectorWindow()
{
    delete m_pVertScroll;
    delete m_pHorizScroll;
    delete m_pMenuBar;
    delete m_pTileSelector;
}


void TileSelectorWindow::on_tilemap_change(CL_Image image)
{
	// TODO: Save and restore positions when switching around
	m_pVertScroll->set_position(0);
	m_pHorizScroll->set_position(0);
    m_pVertScroll->calculate_ranges(get_geometry().get_size().height,image.get_size().height);
    m_pHorizScroll->calculate_ranges(get_geometry().get_size().width,image.get_size().width);   
    request_repaint();
}

void TileSelectorWindow::on_tilemap_clicked ( int tilemap )
{
    for(int i=0;i<m_tilemap_items.size();i++){
        if(i != tilemap){
            m_tilemap_items[i].set_checked(false);
        }else{
            m_tilemap_items[i].set_checked(true);
        }
    }
    std::string tilename = m_tilemaps[tilemap];
    
    size_t slash = tilename.find_first_of('/',0);
    tilename.erase(0,slash+1);
    CL_Image image = GraphicsManager::GetTileMap(tilename);
    m_pTileSelector->SetTilemap(image,tilename);    
    on_tilemap_change(image);
	m_pTileSelector->request_repaint();
}

void TileSelectorWindow::on_horiz_scroll()
{
    CL_Point pt = m_pTileSelector->get_offset();
    pt.x = 0 - m_pHorizScroll->get_position();
    m_pTileSelector->set_offset(pt);
    request_repaint();
}

void TileSelectorWindow::on_vert_scroll()
{
    CL_Point pt = m_pTileSelector->get_offset();
    pt.y = 0 -m_pVertScroll->get_position();
    m_pTileSelector->set_offset(pt);
    request_repaint();
}

void TileSelectorWindow::SetMapEditor ( MapEditorState* state )
{
    m_pTileSelector->SetMapEditor(state);
}



void TileSelectorWindow::create_menu()
{
    CL_PopupMenu menu;
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    m_tilemaps = resources.get_resource_names_of_type("image","Tilemaps");    
    for(int i=0;i<m_tilemaps.size();i++){
        CL_PopupMenuItem item = menu.insert_item(m_tilemaps[i]);
        item.func_clicked().set(this,&TileSelectorWindow::on_tilemap_clicked,i);
        item.set_checkable(true);
        item.set_checked(false);
        m_tilemap_items.push_back(item);
    }
    m_pMenuBar->add_menu("Tile map",menu);
}



}

#endif