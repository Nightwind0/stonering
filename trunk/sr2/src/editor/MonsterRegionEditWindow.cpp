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

namespace StoneRing { 

MonsterRegionEditWindow::MonsterRegionEditWindow(CL_GUIComponent* component, const CL_GUITopLevelDescription& desc):CL_Window(component,desc) {

	m_monster_groups = new CL_ListView(this);
	m_monster_groups->set_geometry(CL_Rect(CL_Point(12,22),CL_Size(400,180)));
	
	m_edit_group = new CL_PushButton(this);
	m_edit_group->set_geometry(CL_Rect(CL_Point(12,218),CL_Size(128,24)));
	m_edit_group->set_text("Edit Group");
	m_edit_group->func_clicked().set(this,&MonsterRegionEditWindow::on_edit_group);
	
	m_delete_group = new CL_PushButton(this);
	m_delete_group->set_geometry(CL_Rect(CL_Point(128+12+12,218),CL_Size(128,24)));
	m_delete_group->set_text("Delete Group");
	m_delete_group->func_clicked().set(this,&MonsterRegionEditWindow::on_delete_group);
	
	m_add_group = new CL_PushButton(this);
	m_add_group->set_geometry(CL_Rect(CL_Point(256+12+12+12,218),CL_Size(128,24)));
	m_add_group->set_text("Add Group");
	m_add_group->func_clicked().set(this,&MonsterRegionEditWindow::on_add_group);
	
	m_backdrops = new CL_ComboBox(this);
	m_backdrops->set_geometry(CL_Rect(CL_Point(12,260),CL_Size(128,24)));
	
	m_encounter_rate = new CL_Spin(this);
	m_encounter_rate->set_geometry(CL_Rect(CL_Point(128+12+12,260),CL_Size(128,24)));
	m_encounter_rate->set_floating_point_mode(true);
	m_encounter_rate->set_step_size_float(0.002);
	m_encounter_rate->set_ranges_float(0.0,1.0);
	m_encounter_rate->set_number_of_decimal_places(3);
	m_encounter_rate->set_value_float(0.076);
	
	m_save = new CL_PushButton(this);
	m_save->set_geometry(CL_Rect(CL_Point(12,320),CL_Size(128,24)));
	m_save->set_text("Save");
	m_save->func_clicked().set(this,&MonsterRegionEditWindow::on_save);
	
	populate_backdrop_list();
	populate_group_list();
}

MonsterRegionEditWindow::~MonsterRegionEditWindow() {
	delete   m_backdrops;
	delete   m_monster_groups;
	delete   m_add_group;
	delete   m_delete_group;
	delete   m_edit_group;
	delete   m_encounter_rate;
	delete   m_save;
}

void MonsterRegionEditWindow::populate_group_list()
{
	CL_ListViewHeader *lv_header = m_monster_groups->get_header();
	lv_header->append(lv_header->create_column("weight","Enc. Weight")).set_width(80);
	lv_header->append(lv_header->create_column("desc", "Monsters")).set_width(200);
    
   
	if(m_region){
		CL_ListViewItem doc = m_monster_groups->get_document_item();
		for(std::list<MonsterGroup*>::const_iterator it = m_region->GetMonsterGroupsBegin();
			it != m_region->GetMonsterGroupsEnd(); it++){
			CL_ListViewItem item = m_monster_groups->create_item();
			item.set_column_text("weight",IntToString((*it)->GetEncounterWeight()));
			std::ostringstream os;
			for(size_t m = 0;m<(*it)->GetMonsters().size();m++){
				os << (*it)->GetMonsters()[m]->GetName();
				if((*it)->GetMonsters()[m]->GetCount() > 1){
					os << 'x' << (*it)->GetMonsters()[m]->GetCount();
				}
				if(m > 0 && m < (*it)->GetMonsters().size()-1){
					os << ',';
				}
			}		
			item.set_column_text("desc",os.str());
			doc.append_child(item);
		}
	}  
}

void MonsterRegionEditWindow::populate_backdrop_list()
{
	CL_PopupMenu menu;
	std::vector<CL_String> backdrops = IApplication::GetInstance()->GetResources().get_resource_names("Backdrops/"); 
	for(std::vector<CL_String>::const_iterator it = backdrops.begin(); it != backdrops.end(); it++){
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

}

void MonsterRegionEditWindow::on_edit_group()
{

}

void MonsterRegionEditWindow::on_delete_group()
{

}

void MonsterRegionEditWindow::on_save()
{

}


}