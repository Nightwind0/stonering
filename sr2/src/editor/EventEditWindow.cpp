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


#include "EventEditWindow.h"
#include "Event.h"
#include "ScriptEditWindow.h"

#ifdef SR2_EDITOR

namespace StoneRing { 

EventEditWindow::EventEditWindow(CL_GUIManager* pManager, const CL_GUITopLevelDescription& desc):CL_Window(pManager,desc) 
{
	m_pEvent = NULL;
	m_event_name = new CL_LineEdit(this);
	m_event_name->set_geometry(CL_Rect(CL_Point(12,34),CL_Size(200,24)));
	
	m_repeatable = new CL_CheckBox(this);
	m_repeatable->set_geometry(CL_Rect(CL_Point(12,74),CL_Size(100,24)));
	m_repeatable->set_text("Repeatable");
	
	m_remember = new CL_CheckBox(this);
	m_remember->set_geometry(CL_Rect(CL_Point(122,74),CL_Size(100,24)));
	m_remember->set_text("Remember");
	
	m_trigger_type = new CL_ComboBox(this);
	m_trigger_type->set_geometry(CL_Rect(CL_Point(12,96),CL_Size(100,24)));
	
	m_edit_script = new CL_PushButton(this);
	m_edit_script->set_geometry(CL_Rect(CL_Point(12,128),CL_Size(100,24)));
	m_edit_script->func_clicked().set(this,&EventEditWindow::on_edit_script);
	
	m_delete_script = new CL_PushButton(this);
	m_delete_script->set_geometry(CL_Rect(CL_Point(128,128),CL_Size(100,24)));
	m_delete_script->func_clicked().set(this,&EventEditWindow::on_delete_script);
	m_delete_script->set_text("Delete Script");
	
	m_edit_condition = new CL_PushButton(this);
	m_edit_condition->set_geometry(CL_Rect(CL_Point(12,160),CL_Size(100,24)));
	m_edit_condition->func_clicked().set(this,&EventEditWindow::on_edit_condition);
	
	m_delete_condition = new CL_PushButton(this);
	m_delete_condition->set_geometry(CL_Rect(CL_Point(128,160),CL_Size(100,24)));
	m_delete_condition->func_clicked().set(this,&EventEditWindow::on_delete_condition);
	m_delete_condition->set_text("Delete Condition");
	
	m_save_button = new CL_PushButton(this);
	m_save_button->set_geometry(CL_Rect(CL_Point(12,200),CL_Size(100,24)));
	m_save_button->func_clicked().set(this,&EventEditWindow::on_save);
	m_save_button->set_text("Save");
	
	populate_trigger_types();
	
	func_close().set(this,&EventEditWindow::on_close);
	
	m_bEdit = false;
}

EventEditWindow::~EventEditWindow() 
{
	delete m_event_name;
	delete m_repeatable;
	delete m_remember;
	delete m_trigger_type;
	delete m_delete_condition;
	delete m_delete_script;
	delete m_edit_condition;
	delete m_edit_script;
}

void EventEditWindow::sync_button_names()
{
	assert(m_pEvent);
	if(m_pEvent->GetCondition()){
		m_edit_condition->set_text("Edit Condition");
		m_delete_condition->set_enabled(true);
	}else{
		m_edit_condition->set_text("Create Condition");
		m_delete_condition->set_enabled(false);
	}
	if(m_pEvent->GetScript()){
		m_edit_script->set_text("Edit Script");
		m_delete_script->set_enabled(true);
	}else{
		m_edit_script->set_text("Create Script");
		m_delete_script->set_enabled(false);
	}
}


void EventEditWindow::populate_trigger_types()
{
	CL_PopupMenu menu;
    const char* triggers[] = {"step", "talk", "act"};
    for(int i=0;i<sizeof(triggers)/sizeof(const char*);i++){
        CL_PopupMenuItem item = menu.insert_item(triggers[i]);
    }
    m_trigger_type->set_popup_menu(menu);
    m_trigger_type->set_selected_item(1);
}


void EventEditWindow::CreateEvent()
{
	m_pEvent = new Event();
	m_bEdit = false;
	sync_button_names();
}

void EventEditWindow::SetEvent( Event* pEvent )
{
	m_pEvent = pEvent;
	m_bEdit = true;
	
	sync_from_event();
}

void EventEditWindow::sync_from_event()
{
	m_event_name->set_text(m_pEvent->GetName());
	m_remember->set_checked(m_pEvent->Remember());
	m_repeatable->set_checked(m_pEvent->Repeatable());
	// hack
	m_trigger_type->set_selected_item(m_pEvent->GetTriggerType());
	sync_button_names();
}

void EventEditWindow::sync_to_event()
{
	m_pEvent->SetRepeatable(m_repeatable->is_checked());
	m_pEvent->SetRemember(m_remember->is_checked());
	m_pEvent->SetTrigger((Event::eTriggerType)m_trigger_type->get_selected_item());
	sync_button_names();
}

bool EventEditWindow::on_close()
{
	exit_with_code(-1);
	return true;
}

void EventEditWindow::on_save()
{
	sync_to_event();
	exit_with_code(0);
}


void EventEditWindow::on_delete_condition()
{
	m_pEvent->SetCondition(NULL);
	sync_button_names();
}

void EventEditWindow::on_delete_script()
{
	m_pEvent->SetScript(NULL);
	sync_button_names();
}

void EventEditWindow::on_edit_condition()
{
	CL_GUIManager mgr = get_gui_manager();
	CL_GUITopLevelDescription desc("Condition",CL_Size(800,650),false);
	desc.set_dialog_window(true);
	ScriptEditWindow window(&mgr,desc);
	window.SetIsCondition(true);
	if(m_pEvent->GetCondition())
		window.SetScript(m_pEvent->GetCondition());
	window.set_draggable(true);
	if(window.exec() == 0){
		m_pEvent->SetCondition(window.CreateScript());
	}
	sync_button_names();
}

void EventEditWindow::on_edit_script()
{
	CL_GUIManager mgr = get_gui_manager();
	CL_GUITopLevelDescription desc("Event Script", CL_Size(800,650),false);
	desc.set_dialog_window(true);
	ScriptEditWindow window(&mgr,desc);
	if(m_pEvent->GetScript())
		window.SetScript(m_pEvent->GetScript());
	window.SetIsCondition(false);
	window.set_draggable(true);
	if(window.exec() == 0){
		m_pEvent->SetScript(window.CreateScript());
	}
	sync_button_names();
}


Event* EventEditWindow::GetEvent() const
{
	return m_pEvent;
}


}

#endif