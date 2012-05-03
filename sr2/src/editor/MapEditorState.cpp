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


#include "MapEditorState.h"
#include <cmath>
#if SR2_EDITOR

namespace StoneRing {

MapEditorState::MapEditorState()
{

}

MapEditorState::~MapEditorState()
{

}

void MapEditorState::Init(CL_DisplayWindow &window) 
{
    EditorState::Init(window);
}

void MapEditorState::Start()
{
    EditorState::Start();
    CL_GUITopLevelDescription desc;
    desc.set_title("Map Editor");
    m_pWindow = new CL_Window(get_gui(), desc);
    m_pWindow->set_geometry(CL_Rect(0,0,800,600));
    m_pWindow->func_close().set(this,&MapEditorState::on_close,m_pWindow);
    m_pWindow->func_input_pointer_moved().set(this,&MapEditorState::on_mouse_moved);
    m_pWindow->func_input_pressed().set(this,&MapEditorState::on_mouse_pressed);
    m_pWindow->func_input_released().set(this,&MapEditorState::on_mouse_released);
    m_pWindow->func_focus_gained().set(this,&MapEditorState::on_pointer_entered);
    m_pWindow->func_focus_lost().set(this,&MapEditorState::on_pointer_exit);

    m_pZoomSlider = new CL_Slider(m_pWindow);
    m_pZoomSlider->set_geometry(CL_Rect(0,32,10,600));
    m_pZoomSlider->set_vertical(true);
    m_pZoomSlider->set_min(1);
    m_pZoomSlider->set_max(12);
    m_pZoomSlider->set_position(10);
    //m_pZoomSlider->set_tick_count(15-2);
    //m_pZoomSlider->set_lock_to_ticks(true);
    m_pZoomSlider->func_value_changed().set(this,&MapEditorState::on_zoom_changed);
    m_pMenuBar = new CL_MenuBar(m_pWindow);
    m_pMenuBar->set_geometry(CL_Rect(0,0,800,32));
    m_pMap = new MapComponent(m_pWindow);
    m_pMap->set_geometry(CL_Rect(20,32,800,600));
    construct_menu();    
    
    m_mouse_state = MOUSE_IDLE;
}

void MapEditorState::construct_menu()
{
    m_file_menu.clear();
    m_file_menu.insert_item("Open").func_clicked().set(this,&MapEditorState::on_file_open);
    m_file_menu.insert_item("New").func_clicked().set(this,&MapEditorState::on_file_new);
    m_file_menu.insert_item("Save").func_clicked().set(this,&MapEditorState::on_file_save);

    
    m_edit_menu.clear();
    m_grow_submenu.clear();
    CL_PopupMenu add_columns_sub, add_rows_sub;
    add_columns_sub.insert_item("Add 1").func_clicked().set(this,&MapEditorState::on_edit_grow_column);
    add_columns_sub.insert_item("Add 5").func_clicked().set(this,&MapEditorState::on_edit_grow_column5);
    add_rows_sub.insert_item("Add 1").func_clicked().set(this,&MapEditorState::on_edit_grow_row);
    add_rows_sub.insert_item("Add 5").func_clicked().set(this,&MapEditorState::on_edit_grow_row5);    
    CL_PopupMenuItem add_columns = m_grow_submenu.insert_item("Add Columns");
    add_columns.set_submenu(add_columns_sub);
    CL_PopupMenuItem add_rows = m_grow_submenu.insert_item("Add Rows");
    add_rows.set_submenu(add_rows_sub);
    CL_PopupMenuItem grow_item = m_edit_menu.insert_item("Grow");
    m_file_menu.insert_separator();
    m_file_menu.insert_item("Close").func_clicked().set(this,&MapEditorState::on_file_close);
    m_file_menu.insert_separator();
    m_file_menu.insert_item("Quit").func_clicked().set(this,&MapEditorState::on_file_quit);    
    grow_item.set_submenu(m_grow_submenu);
    m_pMenuBar->add_menu("File",m_file_menu);
    m_pMenuBar->add_menu("Edit",m_edit_menu);
}

void MapEditorState::on_file_close()
{
    m_pMap->close_level();
}

void MapEditorState::on_file_new()
{
    // TODO: Check if they want to save
    m_pMap->create_level(5,5);
    m_pMap->request_repaint();
}

void MapEditorState::on_file_open()
{
    CL_OpenFileDialog dialog(m_pWindow);
  //  dialog.set_geometry(CL_Rect(0,0,400,400);
    dialog.set_title("Open Level");
    if(dialog.show()){
        std::string name = dialog.get_filename();
        m_pMap->load_level(name);
    }
}

void MapEditorState::on_edit_grow_column()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth()+1,m_pMap->get_level()->GetHeight());
        m_pMap->request_repaint();
    }
}

void MapEditorState::on_edit_grow_column5()
{    
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth()+5,m_pMap->get_level()->GetHeight());
        m_pMap->request_repaint();    
    }
}

void MapEditorState::on_edit_grow_row()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth(),m_pMap->get_level()->GetHeight()+1);
        m_pMap->request_repaint();    
    }
}

void MapEditorState::on_edit_grow_row5()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth(),m_pMap->get_level()->GetHeight()+5);
        m_pMap->request_repaint();    
    }
}

void MapEditorState::on_zoom_changed()
{
    int pos = m_pZoomSlider->get_position();
    float zoom = float(pos * pos)/100.f;
    //float zoom = float(pos) / 10.0f;
    m_pMap->set_scale(zoom);
    m_pMap->request_repaint();
}

void MapEditorState::on_button_clicked(CL_PushButton* pButton){
    static bool large = true;
    if(large){
        m_pMap->set_scale(2.0f);
    }else{
        m_pMap->set_scale(0.5f);
    }
    large = !large;
    m_pMap->request_repaint();
}

void MapEditorState::on_file_save()
{
    CL_SaveFileDialog dialog(m_pWindow);
  //  dialog.set_geometry(CL_Rect(0,0,400,400);
    dialog.set_title("Save Level");
    if(dialog.show()){
        std::string name = dialog.get_filename();
        m_pMap->writeXML(name);
    }
}

void MapEditorState::on_file_quit()
{
    on_close(m_pWindow);
}

bool MapEditorState::on_close(CL_Window* pWindow){  
   return EditorState::on_close(pWindow);
}

bool MapEditorState::on_mouse_moved(const CL_InputEvent& event){
    int mod = mod_value(event.shift,event.ctrl,event.alt);
    if(m_mouse_state == MOUSE_DRAG){
        if(mod != m_mod_state){
            cancel_drag();
        }else{
            update_drag(m_drag_start,m_last_drag_point,event.mouse_pos,m_drag_button,mod);
            m_last_drag_point = event.mouse_pos;
        }
    }
}

bool MapEditorState::on_mouse_pressed(const CL_InputEvent& event){ 
    if(event.id == CL_MOUSE_RIGHT || event.id == CL_MOUSE_LEFT){
        switch(m_mouse_state){
            case MOUSE_IDLE:
                m_mouse_state = MOUSE_DRAG;
                m_drag_start = event.mouse_pos;
                m_drag_button = (event.id == CL_MOUSE_LEFT)?MOUSE_LEFT:MOUSE_RIGHT;
                m_last_drag_point = m_drag_start;
                m_mod_state = mod_value(event.shift,event.ctrl,event.alt);
                start_drag(event.mouse_pos,m_drag_button,m_mod_state);
                break;
            case MOUSE_DRAG:
                cancel_drag();
                break;
        }
    }
}

int MapEditorState::mod_value(bool shift, bool ctrl, bool alt)const
{
    int value = 0;
    if(shift)
        value |= SHIFT;
    if(ctrl)
        value |= CTRL;
    if(alt)
        value |= ALT;
    return value;
}

bool MapEditorState::on_mouse_released(const CL_InputEvent& event){
    if(event.id == CL_MOUSE_RIGHT || event.id == CL_MOUSE_LEFT){
        if(m_mouse_state == MOUSE_DRAG){
            int mod = mod_value(event.shift,event.ctrl,event.alt);
            if(mod == m_mod_state){
                end_drag(m_drag_start,m_last_drag_point,event.mouse_pos,(event.id==CL_MOUSE_LEFT)?MOUSE_LEFT:MOUSE_RIGHT,mod);
            }else{
                cancel_drag();
            }
            m_mouse_state = MOUSE_IDLE;
        }
    }
    return true;
}

void MapEditorState::on_mouse_double_click(const CL_InputEvent& event){
}

bool MapEditorState::on_pointer_entered(){
    if(m_mouse_state == MOUSE_DRAG)
        cancel_drag();
    return true;
}

bool MapEditorState::on_pointer_exit(){
    if(m_mouse_state == MOUSE_DRAG)
        cancel_drag();
    return true;
}

void MapEditorState::start_drag(const CL_Point& point, MouseButton button, int mod){
    std::cout << "Start drag " << point.x << ',' << point.y << std::endl;
    if(mod == 0 && button == MOUSE_LEFT){
        // Pan
    }else{
        // invoke on current tool
    }
}
void MapEditorState::update_drag(const CL_Point& start,const CL_Point& prev, const CL_Point& point, MouseButton button, int mod){
    if(mod == 0 &&  button == MOUSE_LEFT){
        // Pan
        CL_Pointf delta = CL_Pointf(point.x,point.y) - CL_Pointf(prev.x,prev.y);
        //delta *= m_pMap->get_scale();
        CL_Point map_offset = m_pMap->get_geometry().get_top_left();
        CL_Point level = m_pMap->screen_to_level(point-map_offset,m_pMap->get_geometry().get_center()-map_offset);
        m_pMap->set_origin(m_pMap->get_origin() + CL_Point(delta.x,delta.y));
        m_pMap->request_repaint();
    }else{
        // invoke on current tool
    }
}

void MapEditorState::cancel_drag(){
    m_mouse_state = MOUSE_IDLE;
    std::cout << "Cancel drag" << std::endl;
    if(m_mod_state == 0){
        // pan
    }else{
        // current tool
    }
}

void MapEditorState::end_drag(const CL_Point& start,const CL_Point& prev, const CL_Point& point, MouseButton button, int mod){
    m_mouse_state = MOUSE_IDLE;
    std::cout << "End drag " << std::endl;
    if(mod == 0 && MOUSE_LEFT){
    }else{
        // current tool
    }  
}
        

void MapEditorState::HandleButtonUp(const IApplication::Button& button){
}
void MapEditorState::HandleButtonDown(const IApplication::Button& button){
}
void MapEditorState::HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos){
}
void MapEditorState::HandleMouseUp(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}
void MapEditorState::HandleMouseDown(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}


void MapEditorState::HandleDoubleClick(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}

void MapEditorState::HandleMouseMove(const CL_Point& pos, uint key_state ){
}

void MapEditorState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC){
    EditorState::Draw(screenRect,GC);
    
    
}

void MapEditorState::Finish(){
    delete m_pMenuBar;
    delete m_pZoomSlider;
    delete m_pMap;
    delete m_pWindow;
 
    EditorState::Finish();
}

bool MapEditorState::LastToDraw() const {
    return false;
}

bool MapEditorState::DisableMappableObjects() const {
    return true;
}

void MapEditorState::MappableObjectMoveHook(){
}


}
#endif