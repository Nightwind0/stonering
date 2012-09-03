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
#include "Operation.h"
#include "Level.h"
#include "MapEditorState.h"
#include "ScriptEditWindow.h"
#include "EventEditWindow.h"

namespace StoneRing { 
    
class EditorMappableObject : public MappableObjectElement {
public:
    EditorMappableObject(){
    }
    virtual ~EditorMappableObject(){
    }
    
    void copy(MappableObject* other){
        SetMovementSpeed(other->GetMovementSpeed());
        SetMovementType(other->GetMovementType());
        SetName(other->GetName());
        SetSize(other->GetSize()); 
        SetSprite(other->GetSprite());
        SetPixelPosition(other->GetPixelPosition());
        SetStartPos(other->GetStartPos());
        m_cFlags = other->GetFlags();
        
        MappableObjectElement* element = dynamic_cast<MappableObjectElement*>(other);
        if(element != NULL){
            for(std::list<Event*>::const_iterator it = element->EventsBegin(); it != element->EventsEnd();
                it++){
                m_events.push_back(*it);
            }
            m_pCondition = element->GetCondition();
            m_start_facing = element->GetFacing(); 
            SetSpriteName(element->GetSpriteName());
        }
        EditorMappableObject * editor = dynamic_cast<EditorMappableObject*>(other);
        if(editor != NULL){
            m_condition_string = editor->m_condition_string;
            m_proto_events = editor->m_proto_events;
        }
    };
    
    void SetName(const std::string& name){
        m_name = name;
    }
    void SetSize(const CL_Size& size){
        m_size = size;
    }
    void SetSprite(CL_Sprite sprite){
        if(!sprite.is_null()){
            m_sprite.clone(sprite);
            m_cFlags |= SPRITE;
        }else{
            m_sprite = CL_Sprite();
            m_cFlags &= ~SPRITE;
        }
    }
    void SetSpriteName(const std::string& name){
        m_sprite_name = name;
    }
    std::string GetSpriteName() const { 
        return m_sprite_name;
    }
    void SetMovementType(MappableObject::eMovementType move_type){
        m_move_type = move_type;
    }
    void SetMovementSpeed(MappableObject::eMovementSpeed move_speed){
        m_speed = move_speed;
    }
    void SetCondition(const std::string& condition){
    }
    void SetCondition(ScriptElement * pCondition){
        m_pCondition = pCondition;
    }
    void SetSolid(bool solid){
        if(solid)
            m_cFlags |= SOLID;
        else
            m_cFlags &= ~SOLID;
    }
    void SetFlying(bool flying){
        if(flying)
            m_cFlags |= FLYING;
        else
            m_cFlags &= ~FLYING;
    }
    void SetFacing(const Direction& dir){
        m_start_facing = dir;
    }


protected:
    // void AddEvent
    virtual void create_dom_element_hook(CL_DomElement& element)const{
        // TODO: Need to write out our events and conditions here
    }
private:
    struct ProtoEvent {
    };
    std::string m_condition_string;
    std::list<ProtoEvent> m_proto_events;
};    
    

class CreateMOOperation : public Operation {
public:
    CreateMOOperation(){
    }
    virtual ~CreateMOOperation(){
    }
    virtual Operation* clone(){
        return new CreateMOOperation();
    }
    virtual bool Execute(shared_ptr<Level> level){
        m_pMo->SetStartPos(m_data.m_level_pt);
        level->AddMappableObject(m_pMo);
        return true;
    }
    virtual void Undo(shared_ptr<Level> level){
        level->RemoveMappableObject(m_pMo);
    }
    void SetMappableObject(EditorMappableObject* mo){
        m_pMo = mo;
    }
private:
    EditorMappableObject*  m_pMo;
};

class EditMOOperation : public Operation { 
public:
    EditMOOperation(){
    }
    virtual ~EditMOOperation(){
    }
    virtual Operation* clone(){
        return new EditMOOperation();
    }
    virtual bool Execute(shared_ptr<Level> level){
        level->RemoveMappableObject(m_original_mo);
        level->AddMappableObject(m_new_mo);
    }
    virtual void Undo(shared_ptr<Level> level){
        level->RemoveMappableObject(m_new_mo);
        level->AddMappableObject(m_original_mo);
    }
    void SetOriginalMappableObject(MappableObject* mo){
        m_original_mo = mo;
    }
    void SetNewMappableObject(MappableObject* mo){
        m_new_mo = mo;
    }
private:
    MappableObject* m_original_mo;
    MappableObject* m_new_mo;
};
    

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
	m_sprite_list->set_select_whole_row(true);
    m_sprite_list->func_selection_changed().set(this,&MOEditWindow::on_list_selection);
    
    m_sprite_view = new MOView(this);
    m_sprite_view->set_geometry(CL_Rect(400,50,661,460));
    
    m_width_spin = new CL_Spin(this);
    m_width_spin->set_ranges(1,5);
    m_width_spin->set_value(1);
    m_width_spin->set_step_size(1);
    m_width_spin->set_geometry(CL_Rect(12,400,68,428));

    m_height_spin = new CL_Spin(this);
    m_height_spin->set_ranges(1,5);
    m_height_spin->set_value(1);
    m_height_spin->set_step_size(1);
    m_height_spin->set_geometry(CL_Rect(72,400,128,428));    
    
    m_movement = new CL_ComboBox(this);
    m_movement->set_geometry(CL_Rect(12,310,128,334));
    
    m_move_speed = new CL_ComboBox(this);
    m_move_speed->set_geometry(CL_Rect(12,340,128,364));
    
    m_solid = new CL_CheckBox(this);
    m_solid->set_geometry(CL_Rect(12,370,128,394));
    m_solid->set_text("Solid");
    m_solid->set_checked(true);
    
    m_flying = new CL_CheckBox(this);
    m_flying->set_geometry(CL_Rect(136,370,256,394));
    m_flying->set_text("Flying");
    m_flying->set_checked(false);
    
    m_event_list = new CL_ComboBox(this);
    m_event_list->set_geometry(CL_Rect(132,310,264,334));
	
	m_condition_button = new CL_PushButton(this);
	m_condition_button->set_geometry(CL_Rect(132,340,264,364));
	m_condition_button->set_text("Create Condition");
	m_condition_button->func_clicked().set(this,&MOEditWindow::on_condition_edit);
    
    m_open_event_button = new CL_PushButton(this);
    m_open_event_button->set_geometry(CL_Rect(270,310,380,334));
    m_open_event_button->set_text("Open Event");
	m_open_event_button->func_clicked().set(this,&MOEditWindow::on_edit_event);
    
    m_add_event_button = new CL_PushButton(this);
    m_add_event_button->set_geometry(CL_Rect(270,340,380,364));
    m_add_event_button->set_text("Add Event");
	m_add_event_button->func_clicked().set(this,&MOEditWindow::on_add_event);

    
    m_face_dir = new CL_ComboBox(this);
    m_face_dir->set_geometry(CL_Rect(132,400,264,420));
 
    
    m_save_button = new CL_PushButton(this);
    m_save_button->set_geometry(CL_Rect(270,430,380,454));
    m_save_button->set_text("Save");
    m_save_button->func_clicked().set(this,&MOEditWindow::on_save);
    
    populate_movement_combo();
    populate_move_speed_combo();
    populate_facing_combo();
    m_movement->set_selected_item(0);    
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
	delete m_height_spin;
    delete m_movement;
    delete m_move_speed;
    delete m_event_list;
    delete m_open_event_button;
    delete m_add_event_button;
    delete m_save_button;
    delete m_face_dir;
    delete m_flying;
	delete m_solid;
	delete m_condition_button;
}

void MOEditWindow::SetMappableObject ( MappableObject* pObject )
{
    // Create EditorMappableObject that is a copy of pObject
    // TODO: Have to remove this pObject from the level when
    // "save" is hit
    m_pOriginalObject = pObject;
    m_pMo = new EditorMappableObject();
    m_pMo->copy(pObject);
    m_edit_mode = true;
    sync_from_mo();
	populate_event_list();
}

void MOEditWindow::SetCreate()
{
    m_edit_mode = false;
    m_pMo = new EditorMappableObject();
}



void MOEditWindow::populate_event_list()
{
	CL_PopupMenu menu;
	for(std::list<Event*>::const_iterator it = m_pMo->EventsBegin();
		it != m_pMo->EventsEnd(); it++){
		menu.insert_item((*it)->GetName());
	}
	m_event_list->set_popup_menu(menu);
	if(menu.get_item_count() > 0)
		m_event_list->set_selected_item(0);
}

void MOEditWindow::populate_facing_combo()
{
    CL_PopupMenu menu;
    const char* dirs[] = {"south","north","east","west"};
    for(int i =0;i<sizeof(dirs)/sizeof(const char*); i++){
        menu.insert_item(dirs[i]);
    }
    m_face_dir->set_popup_menu(menu);
    m_face_dir->set_selected_item(0);
}


void MOEditWindow::populate_sprite_list()
{
    CL_ListViewHeader *lv_header = m_sprite_list->get_header();
    lv_header->append(lv_header->create_column("Main", "Name")).set_width(200);
    
    CL_ListViewItem doc = m_sprite_list->get_document_item();
    
    m_no_sprite_item = m_sprite_list->create_item();
    m_no_sprite_item.set_column_text("Main","None");
    doc.append_child(m_no_sprite_item);
    
    CL_ResourceManager & resources =  IApplication::GetInstance()->GetResources();
    const char * SubSections[] = {"Npc","Mob"};
    for(int sub = 0; sub < sizeof(SubSections) / sizeof(const char*); sub++){
        std::string section_name = "Sprites/" + std::string(SubSections[sub]) + '/';
        std::vector<CL_String> sprites = resources.get_resource_names(section_name);
        for(std::vector<CL_String>::const_iterator it = sprites.begin(); it != sprites.end(); it++){
            CL_ListViewItem item = m_sprite_list->create_item();
            std::string name = std::string(SubSections[sub]) + '/' + std::string(*it);
            item.set_column_text("Main",name);
  //      item.set_userdata(*it);
            doc.append_child(item);
        }
    }
}

void MOEditWindow::populate_movement_combo()
{
    CL_PopupMenu menu;
    const char* movements[] = {"none","wander","paceNS","paceEW"};
    for(int i=0;i<sizeof(movements)/sizeof(const char*);i++){
        CL_PopupMenuItem item = menu.insert_item(movements[i]);
    }
    m_movement->set_popup_menu(menu);
    m_movement->set_selected_item(0);
}

void MOEditWindow::populate_move_speed_combo()
{
    CL_PopupMenu menu;
    const char* movements[] = {"slow","medium","fast"};
    for(int i=0;i<sizeof(movements)/sizeof(const char*);i++){
        CL_PopupMenuItem item = menu.insert_item(movements[i]);
    }
    m_move_speed->set_popup_menu(menu);
    m_move_speed->set_selected_item(0);
}




void MOEditWindow::on_list_selection ( CL_ListViewSelection selection )
{
    CL_ListViewItem item = selection.get_first().get_item();
    if(item == m_no_sprite_item){
        m_sprite_view->Clear();
        m_sprite_name.clear();
    }else{
        m_sprite_name = item.get_column("Main").get_text();
        CL_Size sprite_size;
        try {
            CL_Sprite sprite = GraphicsManager::CreateSprite(m_sprite_name,true);
            m_sprite_view->SetSprite(sprite);
            sprite_size = CL_Size(max(sprite.get_width() / 32,1), max(sprite.get_height() / 64,1));
            m_height_spin->set_value(sprite_size.height);
            m_width_spin->set_value(sprite_size.width);
            m_sprite_view->SetSize(sprite_size);
        }catch(...){
        }
    }
}

void MOEditWindow::on_add_event()
{
	CL_GUIManager mgr = get_gui_manager();
	CL_GUITopLevelDescription desc("Create Event",CL_Size(250,250), false);		
	desc.set_dialog_window(true);
	EventEditWindow window(&mgr,desc);
	window.CreateEvent();
	window.set_draggable(true);
	if(0 == window.exec()){
		Event * pEvent = window.GetEvent();
		if(pEvent->GetName().empty()){
			std::string event_name;
			int i = 0;
			do{
				event_name = m_pMo->GetName() + " Event " +IntToString(i++);
			}while(m_pMo->GetEventByName(event_name) != NULL);
			pEvent->SetName(event_name);
		}		
		m_pMo->AddEvent(pEvent);
		populate_event_list();
	}
}

void MOEditWindow::on_edit_event()
{
	Event * pEvent = m_pMo->GetEventByName(m_event_list->get_text());
	if(pEvent){
		CL_GUIManager mgr = get_gui_manager();
		CL_GUITopLevelDescription desc("Edit Event",CL_Size(250,250), false);		
		desc.set_dialog_window(true);
		EventEditWindow window(&mgr,desc);
		window.SetEvent(pEvent);
		window.set_draggable(true);
		if(0 == window.exec()){
			if(pEvent->GetName().empty()){
				std::string event_name;
				int i = 0;
				do{
					event_name = m_pMo->GetName() + " Event " +IntToString(i++);
				}while(m_pMo->GetEventByName(event_name) != NULL);
				pEvent->SetName(event_name);
			}
		}else{
			//m_pMo->AddEvent(window.GetEvent());
			// TODO: Cancel changes somehow
		}
		populate_event_list();
	}
}


void MOEditWindow::on_condition_edit()
{
	CL_GUIManager mgr = get_gui_manager();
	CL_GUITopLevelDescription desc("Condition",CL_Size(800,650),false);
	desc.set_dialog_window(true);
	ScriptEditWindow script_window(&mgr,desc);
	script_window.SetIsCondition(true);
	script_window.set_draggable(true);
	if(0 == script_window.exec()){
		// Create and associate a condition
		m_pMo->SetCondition(script_window.CreateScript());
		if(m_pMo->GetCondition()->GetScriptId().empty()){
			std::string id = m_pMo->GetName() + "Condition";
			m_pMo->GetCondition()->SetId(id);
		}
	}
}


void MOEditWindow::on_cancel()
{

}

void MOEditWindow::on_save()
{
    sync_to_mo();
    
    Operation::Data data;
    data.m_level_pt = data.m_level_end_pt = m_point;
    data.m_mod_state = 0;    
    
    if(m_edit_mode){
        EditMOOperation *pOp = new EditMOOperation();
        pOp->SetNewMappableObject(m_pMo);
        pOp->SetOriginalMappableObject(m_pOriginalObject);

        pOp->SetData(data);
        
        m_map_window->PerformOperation(pOp);
    }else{    
        CreateMOOperation* pOp = new CreateMOOperation();
        pOp->SetMappableObject(m_pMo);
        pOp->SetData(data);
    
        m_map_window->PerformOperation(pOp);    
    }
    exit_with_code(0);
}



void MOEditWindow::SetName ( const char* pName )
{
    m_name = pName;
    m_name_field->set_text(m_name);
}

void MOEditWindow::SetPoint ( const CL_Point& i_pt )
{
    m_point = i_pt;
}

void MOEditWindow::SetMapWindow ( MapWindow* pWindow )
{
    m_map_window = pWindow;
}


bool MOEditWindow::on_window_close()
{
    exit_with_code(1);

	return true;
}

void MOEditWindow::sync_from_mo()
{
    m_name_field->set_text(m_pMo->GetName());
    m_height_spin->set_value(m_pMo->GetSize().height);
    m_width_spin->set_value(m_pMo->GetSize().width);
    m_movement->set_selected_item(m_pMo->GetMovementType());
    m_move_speed->set_selected_item(m_pMo->GetMovementSpeed());
    
    m_flying->set_checked(m_pMo->IsFlying());
    m_solid->set_checked(m_pMo->IsSolid());
    
    std::string face_dir = m_pMo->GetFacing();
    
    if(face_dir == "south") 
        m_face_dir->set_selected_item(0);
    else if(face_dir == "north")
        m_face_dir->set_selected_item(1);
    else if(face_dir == "west")
        m_face_dir->set_selected_item(2);
    else if(face_dir == "east")
        m_face_dir->set_selected_item(3);
    
     CL_ListViewItem item = m_sprite_list->find("Main",m_pMo->GetSpriteName());
     if(!item.is_null())
        m_sprite_list->set_selected(item,true);
     else
         m_sprite_list->set_selected(m_no_sprite_item);

    // TODO: Load conditions and events
    // Gets tricky when you have Event* and ScriptElement*
	// TODO: If there is a condition, change the button to "Edit Condition",
	// if not, change to "Create Condition"
}

void MOEditWindow::sync_to_mo()
{
    m_pMo->SetName(m_name_field->get_text());
    m_pMo->SetSize(CL_Size(m_width_spin->get_value(),m_height_spin->get_value()));
    m_pMo->SetMovementType((MappableObject::eMovementType)m_movement->get_selected_item());
    m_pMo->SetMovementSpeed((MappableObject::eMovementSpeed)m_move_speed->get_selected_item());
    m_pMo->SetSprite(m_sprite_view->GetSprite());
    m_pMo->SetSolid(m_solid->is_checked());
    m_pMo->SetFlying(m_flying->is_checked());
    
    m_pMo->SetSpriteName(m_sprite_name);
    m_pMo->SetFacing((std::string)m_face_dir->get_text());
    // Add conditions and events
}



}