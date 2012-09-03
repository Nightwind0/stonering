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
#include "Operation.h"
#include <cmath>
#include <vector>
#include <sstream>
#include "TileSelectorWindow.h"
#include "MapWindow.h"
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
    CL_GUITopLevelDescription desc;
    CL_Size size = get_window_size();  
    desc.set_title("Map Editor");
    desc.set_size(size,true);
	desc.set_allow_resize(true);
    m_pWindow = new CL_MainWindow(get_gui(), desc);

    m_pWindow->func_close().set(this,&MapEditorState::on_close);


    CL_GUITopLevelDescription tooldesc;
    tooldesc.set_title("Tiles");
    tooldesc.set_size(CL_Size(300,400),true);
    tooldesc.set_position(CL_Rect(CL_Point(size.width-300,64),CL_Size(300,400)),true);
    tooldesc.set_tool_window(true);
    tooldesc.set_decorations(true);
    m_pTileWindow = new TileSelectorWindow(m_pWindow,tooldesc);
    m_pTileWindow->set_draggable(true);
    m_pTileWindow->SetMapEditor(this);

    m_pMenuBar = m_pWindow->get_menubar();//new CL_MenuBar(m_pWindow);
	construct_menu();	
}

void MapEditorState::Start()
{
    EditorState::Start();

}

void MapEditorState::on_display_resize(CL_Rect& rect)
{
	CL_Rect client = m_pWindow->get_geometry();
	CL_Size size = client.get_size();
    m_pWindow->set_geometry(CL_Rect(CL_Point(0,0),size));	
    m_pMenuBar->set_geometry(CL_Rect(0,0,size.width,32));	
}

MapWindow* MapEditorState::new_map_window(const std::string& title)
{
	CL_GUITopLevelDescription desc;
	desc.set_title(title);
	desc.set_allow_resize(true);
	desc.show_border(true);
	desc.set_size(CL_Size(600,600),true);
	desc.set_position(CL_Rect(CL_Point(60,60),CL_Size(600,600)),true);
	desc.set_tool_window(false);
	desc.show_sysmenu(true);
	desc.show_maximize_button(true);
	desc.show_minimize_button(true);
	desc.set_decorations(true);			
	MapWindow * window = new MapWindow(m_pWindow,desc);
	window->set_draggable(true);
	return window;
}


void MapEditorState::SetOperation(int mods, Operation* pOp)
{
    m_operations[mods] = pOp;
}  

Operation * MapEditorState::GetOperation(int mods)
{
	std::map<int,Operation*>::iterator it = m_operations.find(mods);
	if(it!=m_operations.end()){
		return it->second;
	}
	return NULL;
}



void MapEditorState::on_file_new()
{
	MapWindow * window = new_map_window("New Level");
	window->Init(this);
	window->New();
}



void MapEditorState::on_file_open()
{
    CL_OpenFileDialog dialog(m_pWindow);
  //  dialog.set_geometry(CL_Rect(0,0,400,400);
    dialog.set_title("Open Level");
    if(dialog.show()){
        std::string name = dialog.get_filename();
		MapWindow * window = new_map_window(name);
		window->Init(this);
		window->Open(name);
		m_pWindow->request_repaint();
    }
}


void MapEditorState::on_file_quit()
{
    on_close();
}

void MapEditorState::on_close(){  
   EditorState::on_close(NULL);
}

void MapEditorState::construct_menu()
{
    m_file_menu.clear();
    m_file_menu.insert_item("Open").func_clicked().set(this,&MapEditorState::on_file_open);
    m_file_menu.insert_item("New").func_clicked().set(this,&MapEditorState::on_file_new);
	m_file_menu.insert_separator();
	m_file_menu.insert_item("Quit").func_clicked().set(this,&MapEditorState::on_file_quit);

    m_pMenuBar->add_menu("File",m_file_menu);
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
    //delete m_pMenuBar;
    delete m_pTileWindow;    
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