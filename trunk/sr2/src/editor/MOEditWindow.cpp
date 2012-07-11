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


#include "MOEditWindow.h"
#include "IApplication.h"
#include "GraphicsManager.h"

namespace StoneRing { 

MOEditWindow::MOEditWindow(CL_GUIManager* owner, const CL_GUITopLevelDescription &desc)
    :CL_Window(owner,desc)
{
    func_close().set(this,&MOEditWindow::on_window_close);
    set_geometry(CL_Rect(32,32,700,500));
    m_name_field = new CL_LineEdit(this);
    m_name_field->set_geometry(CL_Rect(100,24,380,48));
    m_name_label = new CL_Label(this);
    m_name_label->set_geometry(CL_Rect(12,24,90,48));
    m_name_label->set_text("Name:");
    //m_type_list = new CL_ComboBox(this);
    m_sprite_list = new CL_ListView(this);
    m_sprite_list->set_geometry(CL_Rect(12,50,380,300));
    m_sprite_list->set_multi_select(false);
    m_sprite_list->func_selection_changed().set(this,&MOEditWindow::on_list_selection);
    
    m_sprite_view = new MOView(this);
    m_sprite_view->set_geometry(CL_Rect(400,50,661,460));
    
    m_width_spin = new CL_Spin(this);
    m_width_spin->set_ranges(1,5);
    m_width_spin->set_value(1);
    m_width_spin->set_step_size(1);
    m_width_spin->set_geometry(CL_Rect(12,400,68,448));

    m_height_spin = new CL_Spin(this);
    m_height_spin->set_ranges(1,5);
    m_height_spin->set_value(1);
    m_height_spin->set_step_size(1);
    m_height_spin->set_geometry(CL_Rect(72,400,128,448));    
    //m_sprite_view->set_scale_to_fit();
    populate_sprite_list();
}

MOEditWindow::~MOEditWindow()
{
    delete m_name_field;
    delete m_name_label;
    delete m_sprite_list;
    delete m_sprite_view;
    delete m_width_spin;
}

void MOEditWindow::populate_sprite_list()
{
    CL_ListViewHeader *lv_header = m_sprite_list->get_header();
    lv_header->append(lv_header->create_column("Main", "Name")).set_width(200);
    
    CL_ListViewItem doc = m_sprite_list->get_document_item();
    
    CL_ResourceManager & resources =  IApplication::GetInstance()->GetResources();
    std::vector<CL_String> sprites = resources.get_resource_names_of_type("sprite");
    for(std::vector<CL_String>::const_iterator it = sprites.begin(); it != sprites.end(); it++){
        CL_ListViewItem item = m_sprite_list->create_item();
        item.set_column_text("Main",*it);
  //      item.set_userdata(*it);
        doc.append_child(item);
    }
}

void MOEditWindow::on_list_selection ( CL_ListViewSelection selection )
{
    CL_ListViewItem item = selection.get_first().get_item();
    std::string sprite = item.get_column("Main").get_text();
    try {
        m_sprite_view->SetSprite(GraphicsManager::CreateSprite(sprite,false));
    }catch(...){
    }
}


void MOEditWindow::SetName ( const char* pName )
{
    m_name = pName;
}

void MOEditWindow::SetPoint ( const CL_Point& i_pt )
{
    m_point = i_pt;
}

bool MOEditWindow::on_window_close()
{
    set_visible(false);
}


}