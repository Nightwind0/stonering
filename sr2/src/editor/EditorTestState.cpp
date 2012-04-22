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

void EditorTestState::Start()
{
        CL_DisplayWindowDescription desc_window_2;
        desc_window_2.set_title("MultiWindow Example - Window 2");
        desc_window_2.set_allow_resize(true);
        desc_window_2.set_position(CL_Rect(50 + 350, 50, CL_Size(350, 350)), false);

        m_subwindow = new CL_DisplayWindow(desc_window_2);
        m_bDone = false;
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
void EditorTestState::HandleMouseUp(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}
void EditorTestState::HandleMouseDown(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}


void EditorTestState::HandleDoubleClick(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state ){
}

void EditorTestState::HandleMouseMove(const CL_Point& pos, uint key_state ){
}

void EditorTestState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC){
    static int counter = 0;
    ++counter;
    CL_GraphicContext& gc = m_subwindow->get_gc();
    gc.clear(CL_Colorf(0.0f,0.0f,(counter%255)/255.0f, 1.0f));
    m_subwindow->flip(1);
}

void EditorTestState::Finish(){
    delete m_subwindow;
}

bool EditorTestState::LastToDraw() const {
    return false;
}

bool EditorTestState::DisableMappableObjects() const {
    return true;
}

void EditorTestState::MappableObjectMoveHook(){
}


bool EditorTestState::IsDone() const {
    return m_bDone;
}

}
#endif