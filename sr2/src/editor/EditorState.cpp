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

#if SR2_EDITOR

namespace StoneRing {

EditorState::EditorState()
{

}

EditorState::~EditorState()
{

}

void EditorState::Init() 
{
        m_resources = CL_ResourceManager("Media/Editor/GUIThemeBasic/resources.xml");
        m_theme.set_resources(m_resources);
        m_gui_manager.set_css_document("Media/Editor/GUIThemeBasic/theme.css");
        // TODO: CSS
        m_gui_manager.set_theme(m_theme);
        //m_gui_manager.add_resources(m_resources);
        //gui_manager.set_css_document(css_document);
        m_gui_manager.set_window_manager(m_window_manager);
        m_gui_manager.set_accelerator_table(m_accelerator_table);
}

void EditorState::Start()
{
#if 0 
        CL_DisplayWindowDescription desc_window_2;
        desc_window_2.set_title("MultiWindow Example - Window 2");
        desc_window_2.set_allow_resize(true);
        desc_window_2.set_position(CL_Rect(50 + 350, 50, CL_Size(350, 350)), false);

        m_subwindow = new CL_DisplayWindow(desc_window_2);
#endif
        CL_GUITopLevelDescription desc;
        desc.set_title("ClanLib GUI");
        m_pWindow = new CL_Window(&m_gui_manager, desc);
        m_pWindow->func_close().set(this,&EditorState::on_close,m_pWindow);
        m_pButton =  new CL_PushButton(m_pWindow);
        m_pButton->set_geometry(CL_Rect(100, 100, 200, 120));
        m_pButton->set_text("Okay!");
        m_pButton->func_clicked().set(this,&EditorState::on_button_clicked, m_pButton);
        m_bDone = false;
}

void EditorState::on_button_clicked(CL_PushButton* pButton){
    m_bDone = true;
}

bool EditorState::on_close(CL_Window*){
   m_bDone = true;
   return true;
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
    m_gui_manager.render_windows();
}

void EditorState::Finish(){
#if 0 
    delete m_subwindow;
#endif
    delete m_pButton;
    delete m_pWindow;
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