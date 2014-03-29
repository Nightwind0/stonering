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


#include "EditorTestState.h"

#if SR2_EDITOR

namespace StoneRing {

EditorTestState::EditorTestState()
{

}

EditorTestState::~EditorTestState()
{

}

void EditorTestState::Init() 
{
        m_resources = clan::ResourceManager(IApplication::GetInstance()->GetResourcePath() + "Media/Editor/GUIThemeBasic3.0/resources.xml");
        m_theme.set_resources(m_resources);
        m_gui_manager.set_css_document(IApplication::GetInstance()->GetResourcePath() + "Media/Editor/GUIThemeBasic3.0/theme.css");
        // TODO: CSS
        m_gui_manager.set_theme(m_theme);
        //m_gui_manager.add_resources(m_resources);
        //gui_manager.set_css_document(css_document);
        m_gui_manager.set_window_manager(m_window_manager);
        m_gui_manager.set_accelerator_table(m_accelerator_table);
}

void EditorTestState::Start()
{
#if 0 
        clan::DisplayWindowDescription desc_window_2;
        desc_window_2.set_title("MultiWindow Example - Window 2");
        desc_window_2.set_allow_resize(true);
        desc_window_2.set_position(clan::Rect(50 + 350, 50, clan::Size(350, 350)), false);

        m_subwindow = new clan::DisplayWindow(desc_window_2);
#endif
        clan::GUITopLevelDescription desc;
        desc.set_title("ClanLib GUI");
        m_pWindow = new clan::Window(&m_gui_manager, desc);
        m_pWindow->func_close().set(this,&EditorTestState::on_close,m_pWindow);
        m_pButton =  new clan::PushButton(m_pWindow);
        m_pButton->set_geometry(clan::Rect(100, 100, 200, 120));
        m_pButton->set_text("Okay!");
        m_pButton->func_clicked().set(this,&EditorTestState::on_button_clicked, m_pButton);
        m_bDone = false;
}

void EditorTestState::on_button_clicked(clan::PushButton* pButton){
    m_bDone = true;
}

bool EditorTestState::on_close(clan::Window*){
   m_bDone = true;
   return true;
}

void EditorTestState::HandleButtonUp(const IApplication::Button& button){
    if(button == IApplication::BUTTON_CANCEL){
        m_bDone = true;
    }
}
void EditorTestState::HandleButtonDown(const IApplication::Button& button){
}
void EditorTestState::HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos){
}
void EditorTestState::HandleMouseUp(const IApplication::MouseButton& button, const clan::Point& pos, uint key_state ){
}
void EditorTestState::HandleMouseDown(const IApplication::MouseButton& button, const clan::Point& pos, uint key_state ){
}


void EditorTestState::HandleDoubleClick(const IApplication::MouseButton& button, const clan::Point& pos, uint key_state ){
}

void EditorTestState::HandleMouseMove(const clan::Point& pos, uint key_state ){
}

void EditorTestState::Draw(const clan::Rect &screenRect,clan::Canvas& GC){
#if 0 
    static int counter = 0;
    ++counter;
    clan::Canvas& gc = m_subwindow->get_gc();
    gc.clear(clan::Colorf(0.0f,0.0f,(counter%255)/255.0f, 1.0f));
    m_subwindow->flip(1);
#endif

    m_gui_manager.process_messages(10);
    m_gui_manager.render_windows();
}

void EditorTestState::Finish(){
#if 0 
    delete m_subwindow;
#endif
    delete m_pButton;
    delete m_pWindow;
}

bool EditorTestState::LastToDraw() const {
    return false;
}

bool EditorTestState::DisableMappableObjects() const {
    return true;
}

void EditorTestState::Update(){
}


bool EditorTestState::IsDone() const {
    return m_bDone || m_gui_manager.get_exit_flag();
}

}
#endif