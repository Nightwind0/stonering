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


#include "MonsterGroupEditWindow.h"
#include "MonsterRef.h"
#include "GraphicsManager.h"
#include "CharacterManager.h"
#include "MonsterElement.h"
#include <Monster.h>

namespace StoneRing  {
	
void MonsterGroupEditWindow::BattleWindow::SetMonsterGroup( MonsterGroup* group )
{
	m_group = group;
}

void MonsterGroupEditWindow::BattleWindow::SetScale( float scale )
{
	m_scale = scale;
}

void MonsterGroupEditWindow::BattleWindow::on_render( CL_GraphicContext& gc, const CL_Rect& clip_rect )
{
	int cols = m_group->GetCellColumns();
	int rows = m_group->GetCellRows();
	
	int cell_width = get_geometry().get_width() / cols;
	int cell_height = get_geometry().get_height() / rows;
	
	
	for(int x=0;x<cols; x++){
		for(int y=0;y<rows; y++){
			CL_Colorf color;
			if( (y%2) == (x%2) )
				color = CL_Colorf(0.2f,0.2f,0.2f);
			else
				color = CL_Colorf(0.6f,0.6f,0.6f);
			CL_Draw::fill(gc,CL_Rect(CL_Point(x*cell_width,y*cell_height),CL_Size(cell_width,cell_height)),color);
		}
	}
	int id = 0;
	const std::vector<MonsterRef*> & monsters = m_group->GetMonsters();
	
	for ( std::vector<MonsterRef*>::const_iterator it = monsters.begin();
							it != monsters.end(); it++ ) {
		MonsterRef *pRef = *it;
		uint count = pRef->GetCount();
		if( m_selected_item == id++ ){
			CL_Rect rect(CL_Point(pRef->GetCellX()*cell_width,pRef->GetCellY()*cell_height),
						 CL_Size(cell_width*pRef->GetColumns(),cell_height*pRef->GetRows()));
			CL_Draw::box(gc,rect,CL_Colorf(1.0f,1.0f,1.0f));
		}

		for ( int y = 0;y < pRef->GetRows();y++ ) {
			for ( int x = 0;x < pRef->GetColumns();x++ ) {
				if ( count > 0 ) {
					CL_Sprite sprite = GraphicsManager::CreateMonsterSprite(pRef->GetName(),"idle");
					int cell_x = pRef->GetCellX() + x;
					int cell_y = pRef->GetCellY() + y;
					
				    const uint cellWidth = get_geometry().get_width() / m_group->GetCellColumns();
					const uint cellHeight = get_geometry().get_height() / m_group->GetCellRows();

					CL_Point point;
					point.x = cell_x * cellWidth + ( cellWidth  / 2 );
					point.y = cell_y * cellHeight + ( cellHeight / 2 );
					sprite.set_scale(m_scale,m_scale);
					sprite.draw(gc,point.x,point.y);
				}

				if ( --count == 0 ) break;
			}
			if ( count == 0 ) break;
		}

	}	
	
}

	
MonsterGroupEditWindow::MonsterGroupEditWindow(CL_GUIManager* manager, const CL_GUITopLevelDescription& desc):CL_Window(manager,desc){
	m_battle_window = new BattleWindow(this);
	m_battle_window->set_geometry(CL_Rect(CL_Point(24,24),CL_Size(256,256)));
	m_battle_window->SetScale(1.0f/2.0f);
	m_monster_ref_list = new CL_ListView(this);
	m_monster_ref_list->set_geometry(CL_Rect(CL_Point(256+24+60,24),CL_Size(300,256)));
	m_monster_ref_list->func_selection_changed().set(this,&MonsterGroupEditWindow::on_monster_list_selection);
	m_monster_ref_list->set_select_whole_row(true);
	m_monster_ref_list->set_multi_select(false);

	m_monsters = new CL_ComboBox(this);
	m_monsters->set_geometry(CL_Rect(CL_Point(256+24,256+24+12),CL_Size(160,24)));
	m_monsters->func_item_selected().set(this,&MonsterGroupEditWindow::on_monsters_selected);
	
	m_group_col_count = new CL_Spin(this);
	m_group_col_count->set_geometry(CL_Rect(CL_Point(12+128,256+24+12),CL_Size(60,18)));
	m_group_col_count->func_value_changed().set(this,&MonsterGroupEditWindow::on_group_grid_changed);
	m_group_col_count->set_ranges(1,16);
	m_group_col_count->set_value(1);
	
	
	m_group_row_count = new CL_Spin(this);
	m_group_row_count->set_geometry(CL_Rect(CL_Point(256+24,128+12),CL_Size(60,18)));
	m_group_row_count->func_value_changed().set(this,&MonsterGroupEditWindow::on_group_grid_changed);
	m_group_row_count->set_ranges(1,16);
	m_group_row_count->set_value(1);	
	
	m_monster_count_label = new CL_Label(this);
	m_monster_count_label->set_geometry(CL_Rect(CL_Point(256+24+160+12,256+24+12+32),CL_Size(64,18)));
	m_monster_count_label->set_text("Count");
	m_monster_count = new CL_Spin(this);
	m_monster_count->set_geometry(CL_Rect(CL_Point(256+24+160+12,256+24+12),CL_Size(64,24)));
	m_monster_count->func_value_changed().set(this,&MonsterGroupEditWindow::on_monster_count_changed);
	m_monster_count->set_ranges(1,24);
	m_monster_count->set_value(1);
	m_col_count_label = new CL_Label(this);
	m_col_count_label->set_geometry(CL_Rect(CL_Point(256+24+160+12+64+12,256+24+12+32),CL_Size(60,18)));
	m_col_count_label->set_text("Columns");
	m_col_count = new CL_Spin(this);
	m_col_count->set_geometry(CL_Rect(CL_Point(256+24+160+12+64+12,256+24+12),CL_Size(60,24)));
	m_col_count->set_ranges(1,10);
	m_col_count->set_value(1);
	m_row_count_label = new CL_Label(this);
	m_row_count_label->set_geometry(CL_Rect(CL_Point(256+24+160+12+64+12+32+12+22,256+24+12+32),CL_Size(60,18)));
	m_row_count_label->set_text("Rows");
	m_row_count = new CL_Spin(this);
	m_row_count->set_geometry(CL_Rect(CL_Point(256+24+160+12+64+12+32+12+22,256+24+12),CL_Size(60,24)));
	m_row_count->set_ranges(1,10);
	m_row_count->set_value(1);
	m_add_monsters = new CL_PushButton(this);
	m_add_monsters->set_geometry(CL_Rect(CL_Point(800-12-120,256+24+12),CL_Size(120,24)));
	m_add_monsters->set_text("Add");
	m_add_monsters->func_clicked().set(this,&MonsterGroupEditWindow::on_add_monsters);
	m_add_monsters->set_enabled(false);
	
	m_delete_monsters = new CL_PushButton(this);
	m_delete_monsters->set_geometry(CL_Rect(CL_Point(256+24+60+300,24+24+24+60),CL_Size(64,32)));
	m_delete_monsters->set_text("Delete");
	m_delete_monsters->func_clicked().set(this,&MonsterGroupEditWindow::on_delete_monsters);
	m_delete_monsters->set_enabled(false);
	
	m_move_up = new CL_PushButton(this);
	m_move_up->set_geometry(CL_Rect(CL_Point(256+24+60+300+32,24),CL_Size(32,24)));
	m_move_up->set_text("Up");
	m_move_up->set_enabled(false);
	m_move_up->func_clicked().set(this,&MonsterGroupEditWindow::on_move_up);
	
	m_move_down = new CL_PushButton(this);
	m_move_down->set_geometry(CL_Rect(CL_Point(256+24+60+300+32,24+24+24),CL_Size(32,24)));
	m_move_down->set_text("Down");
	m_move_down->set_enabled(false);
	m_move_down->func_clicked().set(this,&MonsterGroupEditWindow::on_move_down);
	
	m_move_left = new CL_PushButton(this);
	m_move_left->set_geometry(CL_Rect(CL_Point(256+24+60+300,24+24),CL_Size(32,24)));
	m_move_left->set_text("Left");
	m_move_left->set_enabled(false);
	m_move_left->func_clicked().set(this,&MonsterGroupEditWindow::on_move_left);
	
	m_move_right = new CL_PushButton(this);
	m_move_right->set_geometry(CL_Rect(CL_Point(256+24+60+300+64,24+24),CL_Size(32,24)));
	m_move_right->set_text("Right");
	m_move_right->set_enabled(false);
	m_move_right->func_clicked().set(this,&MonsterGroupEditWindow::on_move_right);
	
	m_done_button = new CL_PushButton(this);
	m_done_button->set_geometry(CL_Rect(CL_Point(24,256+24+12+64),CL_Size(128,32)));
	m_done_button->set_text("Done");
	m_done_button->func_clicked().set(this,&MonsterGroupEditWindow::on_done);	
	m_group = NULL;
	
	CL_ListViewHeader *lv_header = m_monster_ref_list->get_header();
	lv_header->append(lv_header->create_column("monster","Monster")).set_width(180);
	lv_header->append(lv_header->create_column("count", "Count")).set_width(32);
	lv_header->append(lv_header->create_column("cols", "Cols")).set_width(32);
	lv_header->append(lv_header->create_column("rows", "Rows")).set_width(32);
	
	
	populate_monster_list();
}

MonsterGroupEditWindow::~MonsterGroupEditWindow() {
	delete m_battle_window;
	delete m_monster_ref_list;
	delete m_done_button;
	delete m_monsters;
	delete m_monster_count;
	delete m_col_count_label;
	delete m_col_count;
	delete m_monster_count_label;
	delete m_row_count_label;
	delete m_row_count;
	delete m_move_down;
	delete m_move_up;
	delete m_move_left;
	delete m_move_right;
}

void MonsterGroupEditWindow::populate_monster_list()
{
	std::list<MonsterElement*> monsters;
	CharacterManager::GetMonsterList(monsters);
	CL_PopupMenu menu;
	for(std::list<MonsterElement*>::const_iterator it = monsters.begin();
		it != monsters.end(); it++){
	    CL_PopupMenuItem item = menu.insert_item((*it)->GetName());
	}
	m_monsters->set_popup_menu(menu);
}

void MonsterGroupEditWindow::populate_monster_ref_list()
{
	CL_ListViewItem doc = m_monster_ref_list->get_document_item();
	doc.remove_children();
	int id = 0;
	for(std::vector<MonsterRef*>::const_iterator it = m_group->GetMonsters().begin();
		it != m_group->GetMonsters().end(); it++){
		CL_ListViewItem item = m_monster_ref_list->create_item();
		item.set_column_text("monster",(*it)->GetName());
		item.set_column_text("count",IntToString((*it)->GetCount()));
		item.set_column_text("cols",IntToString((*it)->GetColumns()));
		item.set_column_text("rows",IntToString((*it)->GetRows()));
		item.set_id(id++);
		doc.append_child(item);
	}
}



void MonsterGroupEditWindow::CreateGroup()
{
	SetGroup(new MonsterGroup());
}

void MonsterGroupEditWindow::SetGroup( MonsterGroup* pGroup )
{
	m_group = pGroup;
	m_battle_window->SetMonsterGroup(m_group);
	sync_from_group();
}

void MonsterGroupEditWindow::on_monster_list_selection(CL_ListViewSelection selection)
{
	CL_ListViewItem item = m_monster_ref_list->get_selected_item();
	if(!item.is_null()){
		m_battle_window->SetSelectedItem(item.get_id());
		m_delete_monsters->set_enabled(true);
	}else{
		m_battle_window->SetSelectedItem(-1);
		m_delete_monsters->set_enabled(false);
	}
	sync_move_buttons();
}

void MonsterGroupEditWindow::on_monsters_selected(int selection)
{
	m_add_monsters->set_enabled(selection >= 0);
}

void MonsterGroupEditWindow::on_move_up()
{
	CL_ListViewItem item = m_monster_ref_list->get_selected_item();
	if(!item.is_null()){
		MonsterRef* ref = (MonsterRef*)m_group->GetMonsters()[item.get_id()];
		ref->SetCellY(ref->GetCellY()-1);
	}	
	sync_move_buttons();
	m_battle_window->request_repaint();
}

void MonsterGroupEditWindow::on_move_down()
{
	CL_ListViewItem item = m_monster_ref_list->get_selected_item();
	if(!item.is_null()){
		MonsterRef* ref = (MonsterRef*)m_group->GetMonsters()[item.get_id()];
		ref->SetCellY(ref->GetCellY()+1);
	}		
	sync_move_buttons();
	m_battle_window->request_repaint();
}

void MonsterGroupEditWindow::on_move_left()
{
	CL_ListViewItem item = m_monster_ref_list->get_selected_item();
	if(!item.is_null()){
		MonsterRef* ref = (MonsterRef*)m_group->GetMonsters()[item.get_id()];
		ref->SetCellX(ref->GetCellY()-1);
	}		
	sync_move_buttons();
	m_battle_window->request_repaint();
}

void MonsterGroupEditWindow::on_move_right()
{
	CL_ListViewItem item = m_monster_ref_list->get_selected_item();
	if(!item.is_null()){
		MonsterRef* ref = (MonsterRef*)m_group->GetMonsters()[item.get_id()];
		ref->SetCellX(ref->GetCellY()+1);
	}			
	sync_move_buttons();
	m_battle_window->request_repaint();
}


void MonsterGroupEditWindow::on_monster_count_changed()
{
	// Calc square root and set cells and rows
	float sqr = sqrt(m_monster_count->get_value());
	int cols = ceil(sqr);
	m_col_count->set_value(cols);
	m_row_count->set_value(ceil(float(m_monster_count->get_value()) / float(cols)));
}

void MonsterGroupEditWindow::on_group_grid_changed()
{
	m_group->SetCellColumns(m_group_col_count->get_value());
	m_group->SetCellRows(m_group_row_count->get_value());
	m_battle_window->request_repaint();
}

void MonsterGroupEditWindow::on_delete_monsters()
{
	CL_ListViewItem item = m_monster_ref_list->get_selected_item();
	if(!item.is_null()){
		m_group->RemoveMonsters(item.get_id());
		m_battle_window->request_repaint();
		item.remove();
	}
}


void MonsterGroupEditWindow::on_add_monsters()
{
	if(m_monsters->get_selected_item() >= 0){
		std::string monster_name = m_monsters->get_text();
		DynamicMonsterRef * pRef = new DynamicMonsterRef();
		pRef->SetName(monster_name);
		pRef->SetColumns(m_col_count->get_value());
		pRef->SetRows(m_row_count->get_value());
		pRef->SetCount(m_monster_count->get_value());
		// Find a location where this ref can fit, or TODO: expand the grid to fit it
		bool found_spot = false;
		for(int x=m_group->GetCellColumns()-1;x>=0 && !found_spot;x--){
			for(int y=m_group->GetCellRows()-1;y>=0 && !found_spot;y--){
				CL_Rect candidate(CL_Point(x,y),CL_Size(pRef->GetColumns(),pRef->GetRows()));
				if(can_place(candidate)){
					found_spot = true;
					pRef->SetCellX(x);
					pRef->SetCellY(y);
				}
			}
		}
		
		if(found_spot){
			
			// Change spinners minimum, so you can't shrink the group too small to hold what you've got
			int max_x = 0;
			int max_y = 0;
			m_group->AddMonster(pRef);
			for(std::vector<MonsterRef*>::const_iterator it = m_group->GetMonsters().begin();
						it != m_group->GetMonsters().end();it++){
				CL_Rect rect(CL_Point((*it)->GetCellX(),(*it)->GetCellY()),CL_Size((*it)->GetColumns(),(*it)->GetRows()));
				max_x = cl_max(max_x,rect.right);
				max_y = cl_max(max_y,rect.bottom);
			}
			m_group_col_count->set_ranges(max_x,m_group_col_count->get_max());
			m_group_row_count->set_ranges(max_y,m_group_row_count->get_max());
			populate_monster_ref_list();
			m_battle_window->request_repaint();
		}
	}
}

bool MonsterGroupEditWindow::can_place( const CL_Rect& candidate, MonsterRef* ignore )
{
	bool blocked = false;
	if(candidate.left <0 ||  candidate.top < 0 || candidate.right  > m_group->GetCellColumns() || 
		candidate.bottom > m_group->GetCellRows()){
		blocked = true;
	}else{
		for(std::vector<MonsterRef*>::const_iterator it = m_group->GetMonsters().begin();
			it != m_group->GetMonsters().end();it++){
			if(*it == ignore) continue;
			
			CL_Rect rect(CL_Point((*it)->GetCellX(),(*it)->GetCellY()),CL_Size((*it)->GetColumns(),(*it)->GetRows()));
			if(candidate.is_overlapped(rect)){
				blocked=true;
				break;
			}
		}
	}

	return !blocked;
}


void MonsterGroupEditWindow::sync_move_buttons()
{
	m_move_down->set_enabled(false);
	m_move_up->set_enabled(false);
	m_move_left->set_enabled(false);
	m_move_right->set_enabled(false);
	
	const CL_Rect group_rect(CL_Point(0,0),CL_Size(m_group->GetCellColumns(),m_group->GetCellRows()));
	//m_delete_monsters->set_enabled(false);
	CL_ListViewItem item = m_monster_ref_list->get_selected_item();
	if(!item.is_null()){
		MonsterRef* ref = (MonsterRef*)m_group->GetMonsters()[item.get_id()];
		CL_Rect rect(CL_Point(ref->GetCellX(),ref->GetCellY()),CL_Size(ref->GetColumns(),ref->GetRows()));
		CL_Rect right,left,up,down;
		right=left=up=down=rect;
		right.translate(1,0);
		left.translate(-1,0);
		up.translate(0,-1);
		down.translate(0,1);
		
		if(can_place(right,ref))
			m_move_right->set_enabled(true);
		if(can_place(left,ref))
			m_move_left->set_enabled(true);
		if(can_place(up,ref))
			m_move_up->set_enabled(true);
		if(can_place(down,ref))
			m_move_down->set_enabled(true);
		
		//m_delete_monsters->set_enabled(true);
	}
}


void MonsterGroupEditWindow::sync_from_group()
{
	populate_monster_ref_list();
}


void MonsterGroupEditWindow::on_done()
{
	exit_with_code(0);
}


}