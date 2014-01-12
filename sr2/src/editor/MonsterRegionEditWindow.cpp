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


#include "MonsterRegionEditWindow.h"
#include <Monster.h>
#include "MonsterRegion.h"
#include "IApplication.h"
#include "MonsterRef.h"
#include "MonsterGroupEditWindow.h"

#ifdef SR2_EDITOR

namespace StoneRing { 

MonsterRegionEditWindow::MonsterRegionEditWindow(clan::GUIComponent* component, const clan::GUITopLevelDescription& desc):clan::Window(component,desc) {

	m_monster_groups = new clan::ListView(this);
	m_monster_groups->set_geometry(clan::Rect(clan::Point(12,22),clan::Size(400,180)));
	m_monster_groups->set_select_whole_row(true);
	m_monster_groups->set_multi_select(false);
	m_monster_groups->func_selection_changed().set(this,&MonsterRegionEditWindow::on_select_group);
	
	m_edit_group = new clan::PushButton(this);
	m_edit_group->set_geometry(clan::Rect(clan::Point(12,218),clan::Size(100,24)));
	m_edit_group->set_text("Edit Group");
	m_edit_group->func_clicked().set(this,&MonsterRegionEditWindow::on_edit_group);
	
	m_delete_group = new clan::PushButton(this);
	m_delete_group->set_geometry(clan::Rect(clan::Point(100+12+12,218),clan::Size(100,24)));
	m_delete_group->set_text("Delete Group");
	m_delete_group->func_clicked().set(this,&MonsterRegionEditWindow::on_delete_group);
	
	m_add_group = new clan::PushButton(this);
	m_add_group->set_geometry(clan::Rect(clan::Point(200+12+12+12,218),clan::Size(100,24)));
	m_add_group->set_text("Add Group");
	m_add_group->func_clicked().set(this,&MonsterRegionEditWindow::on_add_group);
	
	m_backdrops = new clan::ComboBox(this);
	m_backdrops->set_geometry(clan::Rect(clan::Point(12,260),clan::Size(128,24)));
	
	m_encounter_rate = new clan::Spin(this);
	m_encounter_rate->set_geometry(clan::Rect(clan::Point(128+12+12,260),clan::Size(128,24)));
	m_encounter_rate->set_floating_point_mode(true);
	m_encounter_rate->set_step_size_float(0.002);
	m_encounter_rate->set_ranges_float(0.0,1.0);
	m_encounter_rate->set_number_of_decimal_places(3);
	m_encounter_rate->set_value_float(0.076);
	
	m_group_weight = new clan::Spin(this);
	m_group_weight->set_geometry(clan::Rect(clan::Point(300+12+12+12+12,218),clan::Size(75,24)));
	m_group_weight->set_ranges(1,100);
	m_group_weight->set_value(10);
	m_group_weight->set_enabled(false);
	m_group_weight->func_value_changed().set(this,&MonsterRegionEditWindow::on_group_weight_changed);
	
	m_save = new clan::PushButton(this);
	m_save->set_geometry(clan::Rect(clan::Point(12,320),clan::Size(128,24)));
	m_save->set_text("Save");
	m_save->func_clicked().set(this,&MonsterRegionEditWindow::on_save);
	
	m_region = NULL;
	
	clan::ListViewHeader *lv_header = m_monster_groups->get_header();
	lv_header->append(lv_header->create_column("desc", "Monsters")).set_width(200);
	lv_header->append(lv_header->create_column("weight","Enc. Weight")).set_width(80);
	
	func_close().set(this,&MonsterRegionEditWindow::on_close);
	populate_backdrop_list();
	//populate_group_list();
}

MonsterRegionEditWindow::~MonsterRegionEditWindow() {
	delete   m_backdrops;
	delete   m_monster_groups;
	delete   m_add_group;
	delete   m_delete_group;
	delete   m_edit_group;
	delete   m_encounter_rate;
	delete   m_save;
	delete 	 m_group_weight;
}

void MonsterRegionEditWindow::SetRegion( MonsterRegion* pRegion )
{
	m_region = pRegion;
	sync_from_region();
}

void MonsterRegionEditWindow::sync_from_region()
{
	m_encounter_rate->set_value_float(m_region->GetEncounterRate());
	m_backdrops->set_text(m_region->GetBackdrop());
	populate_group_list();
}


void MonsterRegionEditWindow::populate_group_list()
{

	if(m_region){
		clan::ListViewItem doc = m_monster_groups->get_document_item();
		doc.remove_children();
		int id = 0;
		for(std::list<MonsterGroup*>::const_iterator it = m_region->GetMonsterGroupsBegin();
			it != m_region->GetMonsterGroupsEnd(); it++){
			clan::ListViewItem item = m_monster_groups->create_item();
			item.set_column_text("weight",IntToString((*it)->GetEncounterWeight()));
			std::ostringstream os;
			for(size_t m = 0;m<(*it)->GetMonsters().size();m++){
				os << (*it)->GetMonsters()[m]->GetName();
				if((*it)->GetMonsters()[m]->GetCount() > 1){
					os << '(' << 'x' << (*it)->GetMonsters()[m]->GetCount() << ')';
				}
				if(m >= 0 && m < (*it)->GetMonsters().size()-1){
					os << ',';
				}
			}		
			item.set_column_text("desc",os.str());
			item.set_id(id++);
			doc.append_child(item);
		}
	}  
}

void MonsterRegionEditWindow::populate_backdrop_list()
{
	clan::PopupMenu menu;
	std::vector<std::string> backdrops = clan::XMLResourceManager::get_doc(IApplication::GetInstance()->GetResources()).get_resource_names("Backdrops/"); 
	for(std::vector<std::string>::const_iterator it = backdrops.begin(); it != backdrops.end(); it++){
		menu.insert_item(*it);
	}
	m_backdrops->set_popup_menu(menu);
	if(menu.get_item_count())
		m_backdrops->set_selected_item(0);
}


void MonsterRegionEditWindow::CreateRegion()
{
	m_region = new MonsterRegion();
}



void MonsterRegionEditWindow::on_add_group()
{
	clan::GUIManager mgr = get_gui_manager();
	clan::GUITopLevelDescription desc("Monster Group",clan::Size(800,400),false);
	desc.set_dialog_window(true);
	MonsterGroupEditWindow window(&mgr,desc);
	window.set_draggable(true);
	window.CreateGroup();
	window.GetGroup()->SetEncounterWeight(10);
	if(0 == window.exec()){
		m_region->AddMonsterGroup(window.GetGroup());
		populate_group_list();
	}
}

void MonsterRegionEditWindow::on_group_weight_changed()
{
	clan::ListViewItem item = m_monster_groups->get_selected_item();
	MonsterGroup * group = NULL;
	if(!item.is_null()){
		int id = 0;
		for(std::list<MonsterGroup*>::const_iterator it = m_region->GetMonsterGroupsBegin();
			it != m_region->GetMonsterGroupsEnd(); it++){
			if(id++ == item.get_id()){
				group = *it;
				break;
			}
		}
		
		group->SetEncounterWeight(m_group_weight->get_value());
		item.set_column_text("weight",IntToString(group->GetEncounterWeight()));
	}
}


void MonsterRegionEditWindow::on_select_group( clan::ListViewSelection selection )
{
	MonsterGroup* group = NULL;
	clan::ListViewItem item = m_monster_groups->get_selected_item();
	if(!item.is_null()){
		int id = 0;
		for(std::list<MonsterGroup*>::const_iterator it = m_region->GetMonsterGroupsBegin();
			it != m_region->GetMonsterGroupsEnd(); it++){
			if(id++ == item.get_id()){
				group = *it;
				break;
			}
		}	
		m_group_weight->set_value(group->GetEncounterWeight());
	}
	m_group_weight->set_enabled(!item.is_null());
}


void MonsterRegionEditWindow::on_edit_group()
{
	MonsterGroup* group = NULL;
	clan::ListViewItem item = m_monster_groups->get_selected_item();
	if(item.is_null()) return;
	
	int id = 0;
	for(std::list<MonsterGroup*>::const_iterator it = m_region->GetMonsterGroupsBegin();
		it != m_region->GetMonsterGroupsEnd(); it++){
		if(id++ == item.get_id()){
			group = *it;
			break;
		}
	}
	clan::GUIManager mgr = get_gui_manager();
	clan::GUITopLevelDescription desc("Monster Group",clan::Size(800,400),false);
	desc.set_dialog_window(true);
	MonsterGroupEditWindow window(&mgr,desc);
	window.set_draggable(true);
	window.SetGroup(group);
	if(0 == window.exec()){
		populate_group_list();
	}
}

void MonsterRegionEditWindow::on_delete_group()
{

}

void MonsterRegionEditWindow::sync_to_region()
{
	m_region->SetBackdrop(m_backdrops->get_text());
	m_region->SetEncounterRate(m_encounter_rate->get_value_float());
}


void MonsterRegionEditWindow::on_save()
{
	sync_to_region();
	exit_with_code(0);
}

bool MonsterRegionEditWindow::on_close()
{
	exit_with_code(-1);
	return true;
}




	
}

#endif