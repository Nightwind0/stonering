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


#include "EditorState.h"
#include "SoundManager.h"

#if SR2_EDITOR

namespace StoneRing {

EditorState::EditorState()
{

}

EditorState::~EditorState()
{

}

void EditorState::Init(CL_DisplayWindow &window) 
{
    // TODO: Call the derived classes to get the name and sizes
    CL_Size size = get_window_size();
    m_display_window = CL_DisplayWindow("Editor",size.width,size.height);
    m_display_window.hide();
    m_resources = CL_ResourceManager("Media/Editor/GUIThemeBasic/resources.xml");
    m_theme.set_resources(m_resources);
    m_gui_manager.set_css_document("Media/Editor/GUIThemeBasic/theme.css");
    
    m_window_manager = CL_GUIWindowManagerTexture(m_display_window);
    // TODO: CSS
    m_gui_manager.set_theme(m_theme);
    //m_gui_manager.add_resources(m_resources);
    //gui_manager.set_css_document(css_document);
    m_gui_manager.set_window_manager(m_window_manager);
    m_gui_manager.set_accelerator_table(m_accelerator_table);     
}

void EditorState::Start()
{
        m_bDone = false;
   m_display_window.show();
   m_latch_vol = SoundManager::GetMusicVolume();
   SoundManager::SetMusicVolume(0.0f);
}

void EditorState::finish(){
    m_bDone = true;
}

bool EditorState::on_close(CL_Window*){
   m_bDone = true;
   return false;
}

void EditorState::HandleButtonUp(const IApplication::Button& button){
    if(button == IApplication::BUTTON_CANCEL){
        m_bDone = true;
    }
}
void EditorState::HandleButtonDown(const IApplication::Button& button){
}
void EditorState::HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos){
}
void EditorState::HandleMouseUp(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}
void EditorState::HandleMouseDown(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}


void EditorState::HandleDoubleClick(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}

void EditorState::HandleMouseMove(const CL_Point& pos, uint key_state ){
}

void EditorState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC){
#if 0 
    static int counter = 0;
    ++counter;
    CL_GraphicContext& gc = m_subwindow->get_gc();
    gc.clear(CL_Colorf(0.0f,0.0f,(counter%255)/255.0f, 1.0f));
    m_subwindow->flip(1);
#endif

    m_gui_manager.process_messages(10);
    //m_gui_manager.render_windows();
}

void EditorState::Finish(){
    m_display_window.hide();
    SoundManager::SetMusicVolume(m_latch_vol);
}

bool EditorState::LastToDraw() const {
    return false;
}

bool EditorState::DisableMappableObjects() const {
    return true;
}

void EditorState::MappableObjectMoveHook(){
}


bool EditorState::IsDone() const {
    return m_bDone || m_gui_manager.get_exit_flag();
}

}
#endif