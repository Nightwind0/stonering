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


#include "TileEditor.h"
#include "ScriptEditWindow.h"

namespace StoneRing { 
	
TileEditorWindow::TileSelectGrid::TileSelectGrid(CL_GUIComponent* parent):CL_GUIComponent(parent),m_pWindow(NULL)
{
	func_render().set(this,&TileEditorWindow::TileSelectGrid::on_render);
    func_input_released().set(this,&TileEditorWindow::TileSelectGrid::on_click);	
	m_selected_num = -1;
}

TileEditorWindow::TileSelectGrid::~TileSelectGrid()
{

}

void TileEditorWindow::TileSelectGrid::set_edit_window( TileEditorWindow* pWindow )
{
	m_pWindow = pWindow;
}


bool TileEditorWindow::TileSelectGrid::on_click( const CL_InputEvent& event )
{
	CL_Point pt = event.mouse_pos / 32;
	int num = pt.y * 4 + pt.x;
	if(num >= 0 && num < m_tiles.size()){
		m_selected_num = num;
		m_pWindow->selection_changed();
		request_repaint();
	}
}

void TileEditorWindow::TileSelectGrid::delete_selected()
{
	if(m_selected_num >= 0){
		m_tiles.erase(m_tiles.begin()+m_selected_num);
		if(m_tiles.empty())
			m_selected_num = -1;
		m_pWindow->selection_changed();
		request_repaint();
	}
}

void TileEditorWindow::TileSelectGrid::move_down()
{
	if(m_selected_num >= 0){
		if(m_selected_num +1 < m_tiles.size()){
			Tile * tmp = m_tiles[m_selected_num];
			m_tiles[m_selected_num] = m_tiles[m_selected_num+1];
			m_tiles[m_selected_num+1] = tmp;
			m_pWindow->selection_changed();
		}
	}
}

void TileEditorWindow::TileSelectGrid::move_up()
{
	if(m_selected_num > 0){
		Tile * tmp = m_tiles[m_selected_num-1];
		m_tiles[m_selected_num-1] = m_tiles[m_selected_num];
		m_tiles[m_selected_num] = tmp;
		m_pWindow->selection_changed();
	}
}



void TileEditorWindow::TileSelectGrid::free_tiles()
{
	for(std::vector<Tile*>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++){
		delete *it;
	}	
}

void TileEditorWindow::TileSelectGrid::set_tiles( const std::list< Tile* >& tiles )
{
	for(std::list<Tile*>::const_iterator it = tiles.begin(); it != tiles.end(); it++){
		m_tiles.push_back ( (*it)->clone() );
	}
}


void TileEditorWindow::TileSelectGrid::on_render( CL_GraphicContext& gc, const CL_Rect& rect )
{
	CL_Draw::fill(gc,rect,CL_Colorf(0.0f,0.0f,0.0f));
	for(int i=0;i<m_tiles.size();i++){
		int y = i / 4;
		int x = i - (y*4);
		CL_Point pt(x*32,y*32);
		pt -= m_tiles[i]->GetRect().get_top_left();
		m_tiles[i]->Draw(gc,pt);
		m_tiles[i]->Visit(&m_block_drawer,gc,pt);
		m_tiles[i]->Visit(&m_hot_drawer,gc,pt);
		m_tiles[i]->Visit(&m_region_drawer,gc,pt);
		m_tiles[i]->Visit(&m_zorder_drawer,gc,pt);
		if(i == m_selected_num){
			pt += m_tiles[i]->GetRect().get_top_left();
			CL_Draw::box(gc,CL_Rect(pt,CL_Size(32,32)),CL_Colorf(0.0f,1.0f,1.0f));
		}
	}
}

Tile* TileEditorWindow::TileSelectGrid::get_selected_tile() const
{
	if(m_selected_num >= 0){
		return m_tiles[m_selected_num];
	}
	return NULL;
}




TileEditorWindow::TileEditorWindow(CL_GUIComponent* owner, const CL_GUITopLevelDescription &desc):CL_Window(owner,desc), m_grid(this) {
	func_close().set(this,&TileEditorWindow::on_close);
	m_grid.set_edit_window(this);
	m_grid.set_geometry(CL_Rect(CL_Point(24,24),CL_Size(144,144)));
	m_save = new CL_PushButton(this);
	m_save->set_geometry(CL_Rect(CL_Point(12,380),CL_Size(128,32)));
	m_save->func_clicked().set(this,&TileEditorWindow::on_save);
	m_save->set_text("Done");
	
	m_delete_selected = new CL_PushButton(this);
	m_delete_selected->set_geometry(CL_Rect(CL_Point(264,64+24+24),CL_Size(128,32)));
	m_delete_selected->set_text("Delete Tile");
	m_delete_selected->func_clicked().set(this,&TileEditorWindow::on_delete_tile);
	
	m_edit_condition = new CL_PushButton(this);
	m_edit_condition->set_geometry(CL_Rect(CL_Point(264,12+24-12),CL_Size(128,32)));
	m_edit_condition->set_text("Edit Condition");
	m_edit_condition->func_clicked().set(this,&TileEditorWindow::on_edit_condition);
	
	m_edit_script = new CL_PushButton(this);
	m_edit_script->set_geometry(CL_Rect(CL_Point(264,32+12+24),CL_Size(128,32)));
	m_edit_script->set_text("Edit Script");
	m_edit_script->func_clicked().set(this,&TileEditorWindow::on_edit_script);
	
	m_block_north = new CL_CheckBox(this);
	m_block_north->set_geometry(CL_Rect(CL_Point(224+64,64+32+24+32),CL_Size(100,24)));
	m_block_north->set_text("North");
	m_block_north->func_state_changed().set(this,&TileEditorWindow::on_block_north_changed);

	m_block_south = new CL_CheckBox(this);
	m_block_south->set_geometry(CL_Rect(CL_Point(224+64,64+32+24+48+32),CL_Size(100,24)));
	m_block_south->set_text("South");
	m_block_south->func_state_changed().set(this,&TileEditorWindow::on_block_south_changed);
	
	m_block_west = new CL_CheckBox(this);
	m_block_west->set_geometry(CL_Rect(CL_Point(264,64+32+24+24+32),CL_Size(64,24)));
	m_block_west->set_text("West");
	m_block_west->func_state_changed().set(this,&TileEditorWindow::on_block_west_changed);
	
	m_block_east = new CL_CheckBox(this);
	m_block_east->set_geometry(CL_Rect(CL_Point(200+64+64,64+32+24+24+32),CL_Size(64,24)));
	m_block_east->set_text("West");
	m_block_east->func_state_changed().set(this,&TileEditorWindow::on_block_east_changed);
	
	m_hot = new CL_CheckBox(this);
	m_hot->set_geometry(CL_Rect(CL_Point(264,64+32+24+24+24+64),CL_Size(64,24)));
	m_hot->set_text("Hot");
	m_hot->func_state_changed().set(this,&TileEditorWindow::on_hot_changed);
	
	m_floater = new CL_CheckBox(this);
	m_floater->set_geometry(CL_Rect(CL_Point(264,64+32+24+24+64+32+32),CL_Size(64,24)));
	m_floater->set_text("Floater");
	m_floater->func_state_changed().set(this,&TileEditorWindow::on_floater_changed);
	
	m_water = new CL_CheckBox(this);
	m_water->set_geometry(CL_Rect(CL_Point(264,64+32+24+24+64+32+12),CL_Size(64,24)));
	m_water->set_text("Water");
	m_water->func_state_changed().set(this,&TileEditorWindow::on_water_changed);	
	
	m_move_up = new CL_PushButton(this);
	m_move_up->set_geometry(CL_Rect(CL_Point(144+48,(144-24)/2),CL_Size(48,24)));
	m_move_up->set_text("Up");
	m_move_up->func_clicked().set(this,&TileEditorWindow::on_move_up);

	m_move_down = new CL_PushButton(this);
	m_move_down->set_geometry(CL_Rect(CL_Point(144+48,48+(144-48)/2),CL_Size(48,24)));
	m_move_down->set_text("Down");
	m_move_down->func_clicked().set(this,&TileEditorWindow::on_move_down);	
	

	m_monster_region = new CL_ComboBox(this);
	m_monster_region->set_geometry(CL_Rect(CL_Point(264,300),CL_Size(100,24)));
	m_monster_region->func_selection_changed().set(this,&TileEditorWindow::on_select_monster_region);

//	CL_Spin*       m_zorder;
	m_zorder = new CL_Spin(this);
	m_zorder->set_geometry(CL_Rect(CL_Point(248+64,232+100),CL_Size(64,24)));
	m_zorder->set_ranges(0,100);
	m_zorder->func_value_changed().set(this,&TileEditorWindow::on_zorder_change);
	
	m_zorder_label = new CL_Label(this);
	m_zorder_label->set_geometry(CL_Rect(CL_Point(264,232+100),CL_Size(64,24)));
	m_zorder_label->set_text("ZOffset");

	
	populate_monster_region_list();
	sync_from_selected();
}

TileEditorWindow::~TileEditorWindow() {
	delete m_save;
	delete m_delete_selected;
	delete m_block_east;
	delete m_block_west;
	delete m_block_north;
	delete m_block_south;
	delete m_edit_condition;
	delete m_edit_script;
	delete m_hot;
	delete m_move_up;
	delete m_move_down;
	delete m_monster_region;
	delete m_zorder;
	delete m_zorder_label;
	delete m_floater;
	delete m_water;
	m_grid.free_tiles();
}

void TileEditorWindow::TileSelectGrid::get_tiles( std::list< Tile* >& tiles ) const
{
	for(std::vector<Tile*>::const_iterator it = m_tiles.begin(); it != m_tiles.end(); it++){
		tiles.push_back(*it);
	}
}


void TileEditorWindow::selection_changed()
{
	sync_from_selected();
}



void TileEditorWindow::populate_monster_region_list()
{
	CL_PopupMenu menu;
	menu.insert_item("-1");
	for(std::list<int>::const_iterator it = m_monster_regions.begin();
		it != m_monster_regions.end(); it++){
	    CL_PopupMenuItem item = menu.insert_item(IntToString(*it));
		item.set_id(*it);
	}
	m_monster_region->set_popup_menu(menu);	
}

void TileEditorWindow::on_select_monster_region( int selection )
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		int c = StringToInt(m_monster_region->get_item(selection));
		pTile->SetMonsterRegion(c);
	}
}

void TileEditorWindow::on_delete_tile()
{
	m_grid.delete_selected();
}

void TileEditorWindow::on_move_down()
{
	m_grid.move_down();
}

void TileEditorWindow::on_move_up()
{
	m_grid.move_up();
}



bool TileEditorWindow::on_close()
{

	exit_with_code(-1);
	return true;
}

void TileEditorWindow::on_save()
{
	exit_with_code(0);
}

void TileEditorWindow::set_tiles( const std::list< Tile* >& tiles )
{
	m_grid.set_tiles(tiles);
}




void TileEditorWindow::sync_from_selected()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		m_block_east->set_checked(pTile->GetFlags() & Tile::TIL_BLK_EAST);
		m_block_west->set_checked(pTile->GetFlags() & Tile::TIL_BLK_WEST);
		m_block_north->set_checked(pTile->GetFlags() & Tile::TIL_BLK_NORTH);
		m_block_south->set_checked(pTile->GetFlags() & Tile::TIL_BLK_SOUTH);
		m_hot->set_checked(pTile->GetFlags() & Tile::TIL_HOT);
		// TODO: Water, floater
		
		// todo: delete script, delete condition
		if(pTile->HasScript())
			m_edit_script->set_text("Edit Script");
		else
			m_edit_script->set_text("Add Script");
		if(pTile->HasCondition())
			m_edit_condition->set_text("Edit Condition");
		else
			m_edit_condition->set_text("Add Condition");
		
		m_zorder->set_value(pTile->GetZOffset());
		
		m_monster_region->set_text(IntToString(pTile->GetMonsterRegion()));
	}
}

void TileEditorWindow::on_block_east_changed()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		if(m_block_east->is_checked())
			pTile->SetFlag(Tile::TIL_BLK_EAST);
		else
			pTile->UnsetFlag(Tile::TIL_BLK_EAST);
	}
	m_grid.request_repaint();
}

void TileEditorWindow::on_block_west_changed()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		if(m_block_west->is_checked())
			pTile->SetFlag(Tile::TIL_BLK_WEST);
		else
			pTile->UnsetFlag(Tile::TIL_BLK_WEST);
	}
	m_grid.request_repaint();
}

void TileEditorWindow::on_block_north_changed()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		if(m_block_north->is_checked())
			pTile->SetFlag(Tile::TIL_BLK_NORTH);
		else
			pTile->UnsetFlag(Tile::TIL_BLK_NORTH);
	}
	m_grid.request_repaint();
}

void TileEditorWindow::on_block_south_changed()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		if(m_block_south->is_checked())
			pTile->SetFlag(Tile::TIL_BLK_SOUTH);
		else
			pTile->UnsetFlag(Tile::TIL_BLK_SOUTH);
	}
	m_grid.request_repaint();
}

void TileEditorWindow::on_edit_condition()
{
	CL_GUIManager mgr = get_gui_manager();
	CL_GUITopLevelDescription desc("Condition",CL_Size(800,650),false);
	desc.set_dialog_window(true);
	ScriptEditWindow script_window(&mgr,desc);
	script_window.SetIsCondition(true);
	script_window.set_draggable(true);
	Tile * pTile = m_grid.get_selected_tile();
	assert(pTile);
	if(pTile->HasCondition()){
		script_window.SetScript(pTile->GetCondition());
	}else{
		script_window.CreateScript();
	}
	script_window.exec();
}

void TileEditorWindow::on_edit_script()
{
	CL_GUIManager mgr = get_gui_manager();
	CL_GUITopLevelDescription desc("Script",CL_Size(800,650),false);
	desc.set_dialog_window(true);
	ScriptEditWindow script_window(&mgr,desc);
	script_window.SetIsCondition(true);
	script_window.set_draggable(true);
	Tile * pTile = m_grid.get_selected_tile();
	assert(pTile);
	if(pTile->HasScript()){
		script_window.SetScript(pTile->GetScript());
	}else{
		script_window.CreateScript();
	}
	script_window.exec();
}

void TileEditorWindow::on_floater_changed()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		if(m_floater->is_checked())
			pTile->SetFlag(Tile::TIL_FLOATER);
		else
			pTile->UnsetFlag(Tile::TIL_FLOATER);
	}
	m_grid.request_repaint();
}

void TileEditorWindow::on_hot_changed()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		if(m_hot->is_checked())
			pTile->SetFlag(Tile::TIL_HOT);
		else
			pTile->UnsetFlag(Tile::TIL_HOT);
	}
	m_grid.request_repaint();
}

void TileEditorWindow::on_water_changed()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		if(m_water->is_checked())
			pTile->SetFlag(Tile::TIL_WATER);
		else
			pTile->UnsetFlag(Tile::TIL_WATER);
	}
	m_grid.request_repaint();
}


void TileEditorWindow::on_zorder_change()
{
	Tile * pTile = m_grid.get_selected_tile();
	if(pTile){
		pTile->SetZOffset(m_zorder->get_value());
	}
	m_grid.request_repaint();
}


void TileEditorWindow::add_monster_region( int id )
{
	m_monster_regions.push_back(id);
}

std::list< Tile* > TileEditorWindow::get_tiles() const
{
	std::list<Tile*> tiles;
	std::list<Tile*> grid_tiles;
	m_grid.get_tiles(grid_tiles);
	for(std::list<Tile*>::const_iterator it = grid_tiles.begin(); it != grid_tiles.end(); it++){
		tiles.push_back ( (*it)->clone() );
	}
	return tiles;
}


}