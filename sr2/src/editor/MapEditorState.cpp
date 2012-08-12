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
#if SR2_EDITOR

namespace StoneRing {
    
    
    class DeleteTileOperation: public Operation {
    public:
        DeleteTileOperation(){}
        virtual ~DeleteTileOperation(){}
        virtual Operation* clone(){
            return new DeleteTileOperation;
        }
        virtual bool Execute(shared_ptr<Level> level){
            if(m_data.m_level_end_pt.x < 0 || m_data.m_level_end_pt.y < 0 ||
                m_data.m_level_end_pt.x >= level->GetWidth() || m_data.m_level_end_pt.y >= level->GetHeight()){
                return false;
            }
            if(m_data.m_mod_state & MapEditorState::SHIFT){
                if(level->TilesAt(m_data.m_level_end_pt)){
                    m_removed_tiles.push_back(level->PopTileAtPos(m_data.m_level_end_pt));
                }else{
                    return false;
                }
            }else{
                m_removed_tiles = level->GetTilesAt(m_data.m_level_end_pt);
                while(level->PopTileAtPos(m_data.m_level_end_pt));
            }
            return true;
        }
        virtual void Undo(shared_ptr<Level> level){
            for(std::list<Tile*>::const_iterator it = m_removed_tiles.begin();
                it != m_removed_tiles.end(); it++){
                level->AddTile(*it);
            }
        }
    private:
        std::list<Tile*> m_removed_tiles;
    };
    
    
    class BlockOperation : public Operation {
    public:
        BlockOperation(){}
        virtual ~BlockOperation(){}
        virtual Operation* clone(){
            BlockOperation * op = new BlockOperation();
            op->m_flag = m_flag;
            return op;
        }
        virtual bool Execute(shared_ptr<Level> level){
            if(m_data.m_level_end_pt.x < 0 || m_data.m_level_end_pt.y < 0 ||
                m_data.m_level_end_pt.x >= level->GetWidth() || m_data.m_level_end_pt.y >= level->GetHeight()){
                return false;
            }
            std::list<Tile*> tiles = level->GetTilesAt(m_data.m_level_end_pt);
            if(tiles.empty())
                return false;
            if(m_data.m_mod_state & MapEditorState::CTRL){
                tiles.back()->UnsetFlag(m_flag);
            }else{
                tiles.back()->SetFlag(m_flag);
            }
            return true;
        }
        virtual void Undo(shared_ptr<Level> level){
            std::list<Tile*> tiles = level->GetTilesAt(m_data.m_level_end_pt);
            if(m_data.m_mod_state & MapEditorState::CTRL){
                tiles.back()->SetFlag(m_flag);
            }else{
                tiles.back()->UnsetFlag(m_flag);        
            }
        }
        void SetBlock(int flag){
            m_flag = flag;
        }
    private:
        int m_flag;
    };
    
    class BlockOperations : public OperationGroup {
    public:
        BlockOperations(){}
        virtual ~BlockOperations(){}
        void SetBlock(int flag){m_flag = flag;}
        Operation* clone() {
            BlockOperations * pOP = new BlockOperations;
            pOP->SetBlock(m_flag);
            return pOP;
        }
    protected:
        Operation* create_suboperation()const{
            BlockOperation * op = new BlockOperation;
            op->SetBlock(m_flag);
            return op;
        }
    private:
        int m_flag;
    };
    
    class MoveObjectOperation: public Operation {
    public:
        MoveObjectOperation():m_pMo(NULL){
        }
        virtual ~MoveObjectOperation(){
        }
        void SetMappableObject(MappableObject* pMo){
            m_pMo = pMo;
            m_original_pos = pMo->GetStartPos();
        }
        virtual bool Execute(shared_ptr<Level> level){
            level->RemoveMappableObject(m_pMo);            
            m_pMo->SetStartPos(m_data.m_level_pt);
            level->AddMappableObject(m_pMo);
            return true;
        }
        virtual void Undo(shared_ptr<Level> level){
            level->RemoveMappableObject(m_pMo);
            m_pMo->SetStartPos(m_original_pos);
            level->AddMappableObject(m_pMo);
        }
        virtual Operation* clone() {
            MoveObjectOperation* op =  new MoveObjectOperation();
            op->m_pMo = m_pMo;
            op->m_original_pos = m_original_pos;
            return op;
        }
    private:
        MappableObject* m_pMo;
        CL_Point        m_original_pos;
    };
    
    class DeleteObjectOperation: public Operation {
    public:
        DeleteObjectOperation(){
        }
        virtual ~DeleteObjectOperation(){
            
        }
        void SetMappableObject(MappableObject* pMo){
            m_pMo = pMo;
        }
        virtual bool Execute(shared_ptr<Level> level){
            level->RemoveMappableObject(m_pMo);
            return true;
        }
        virtual void Undo(shared_ptr<Level> level){
            level->AddMappableObject(m_pMo);
        }
        virtual Operation* clone() {
            return new DeleteObjectOperation();
        }
    private:
        MappableObject*  m_pMo;
    };
    

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
    CL_Size size = get_window_size();  
    desc.set_title("Map Editor");
    desc.set_size(size,true);
    m_pWindow = new CL_MainWindow(get_gui(), desc);
    m_pWindow->set_geometry(CL_Rect(CL_Point(0,0),size));
    m_pWindow->func_close().set(this,&MapEditorState::on_close);
    m_pWindow->func_input_pointer_moved().set(this,&MapEditorState::on_mouse_moved);
    m_pWindow->func_input_pressed().set(this,&MapEditorState::on_mouse_pressed);
    m_pWindow->func_input_released().set(this,&MapEditorState::on_mouse_released);
    m_pWindow->func_focus_gained().set(this,&MapEditorState::on_pointer_entered);
    m_pWindow->func_focus_lost().set(this,&MapEditorState::on_pointer_exit);
    m_pWindow->func_input_doubleclick().set(this,&MapEditorState::on_mouse_double_click);

    m_pZoomSlider = new CL_Slider(m_pWindow);
    m_pZoomSlider->set_geometry(CL_Rect(0,64,10,size.height));
    m_pZoomSlider->set_vertical(true);
    m_pZoomSlider->set_min(1);
    m_pZoomSlider->set_max(12);
    m_pZoomSlider->set_position(10);
    //m_pZoomSlider->set_tick_count(15-2);
    //m_pZoomSlider->set_lock_to_ticks(true);
    m_pZoomSlider->func_value_changed().set(this,&MapEditorState::on_zoom_changed);

    m_pMap = new MapComponent(m_pWindow);
    m_pMap->set_geometry(CL_Rect(CL_Point(20,64),size));

    CL_GUITopLevelDescription tooldesc;
    tooldesc.set_title("Tiles");
    tooldesc.set_size(CL_Size(300,400),true);
    tooldesc.set_position(CL_Rect(CL_Point(size.width-300,64),CL_Size(300,400)),true);
    tooldesc.set_tool_window(true);
    tooldesc.set_decorations(true);
    m_pTileWindow = new TileSelectorWindow(m_pWindow,tooldesc);
    m_pTileWindow->set_draggable(true);
    m_pTileWindow->SetMapEditor(this);
    
    CL_GUITopLevelDescription mo_edit_desc;
    mo_edit_desc.set_title("Edit Mappable Object");
    mo_edit_desc.set_size(CL_Size(400,400),true);
    mo_edit_desc.set_position(CL_Rect(CL_Point(size.width-400,64),CL_Size(400,400)),true);
    mo_edit_desc.set_dialog_window(true);
    mo_edit_desc.set_decorations(true);
    m_pMOEditWindow = new MOEditWindow(get_gui(), mo_edit_desc);
    m_pMOEditWindow->set_draggable(true);
    m_pMOEditWindow->set_visible(false);
    m_pMOEditWindow->SetMapEditorState(this);
    
    
    m_toolbar = new CL_ToolBar(m_pWindow);
    m_toolbar->set_geometry(CL_Rect(CL_Point(0,32),CL_Size(size.width,36)));

    m_toolbar->func_item_clicked().set(this,&MapEditorState::on_toolbar_item);

    m_pMenuBar = m_pWindow->get_menubar();//new CL_MenuBar(m_pWindow);
    m_pMenuBar->set_geometry(CL_Rect(0,0,size.width,32));
  
    construct_map_context_menu();
    construct_menu();
    construct_toolbar();
    construct_accels();
    get_gui()->set_accelerator_table(m_accels);
    //m_pToolWindow->set_geometry(CL_Rect(0,0,300,400));
    m_mouse_state = MOUSE_IDLE;
}

void MapEditorState::construct_map_context_menu()
{
    m_map_context_menu.clear();
    CL_PopupMenuItem edit_tile_item = m_map_context_menu.insert_item("Edit Tiles",CON_EDIT_TILE);
    m_map_context_menu.insert_separator();
    CL_PopupMenuItem add_mo_item = m_map_context_menu.insert_item("Add Object",CON_ADD_OBJECT);
    CL_PopupMenuItem edit_mo_item = m_map_context_menu.insert_item("Edit Object",CON_EDIT_OBJECT);
    CL_PopupMenuItem move_mo_item = m_map_context_menu.insert_item("Move Object",CON_MOVE_OBJECT);
    CL_PopupMenuItem delete_mo_item = m_map_context_menu.insert_item("Delete Object",CON_DELETE_OBJECT);
    
    edit_tile_item.func_clicked().set(this,&MapEditorState::on_edit_tile);    
    add_mo_item.func_clicked().set(this,&MapEditorState::on_add_mo);
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
    m_edit_menu.insert_separator();
    m_edit_menu.insert_item_accel("Undo", "Ctrl-Z").func_clicked().set(this,&MapEditorState::on_edit_undo);
    m_file_menu.insert_separator();
    m_file_menu.insert_item("Close").func_clicked().set(this,&MapEditorState::on_file_close);
    m_file_menu.insert_separator();
    m_file_menu.insert_item("Quit").func_clicked().set(this,&MapEditorState::on_file_quit);    
    grow_item.set_submenu(m_grow_submenu);
    m_pMenuBar->add_menu("File",m_file_menu);
    m_pMenuBar->add_menu("Edit",m_edit_menu);
    
    CL_PopupMenu view;
    view.insert_item("Unzoom").func_clicked().set(this,&MapEditorState::on_view_unzoom);
    view.insert_item("Recenter").func_clicked().set(this,&MapEditorState::on_view_recenter);
    
    m_mo_menu.clear();
    //m_mo_menu.insert_item("Create").func_clicked().set(this,&MapEditorState::on_mo_create);
    
    m_pMenuBar->add_menu("View",view);
}

void MapEditorState::construct_toolbar()
{
    struct Tool {
        std::string icon;
        std::string name;
        ToolBarItem item;
        bool toggle;
    };
    Tool tools[] = {
        {"Media/Editor/Images/map_add.png","Add Tiles",ADD_TILE,false},
        {"Media/Editor/Images/map_delete.png","Erase Tiles",DELETE_TILE,true},
        {"Media/Editor/Images/view.png","Show Data",SHOW_ALL,true},
        {"Media/Editor/Images/eye.png","Show Objects",SHOW_OBJECTS,true},
        {"Media/Editor/Images/world_edit.png","Level Data",EDIT_LEVEL,false},
        {"Media/Editor/Images/block_west.png","",BLOCK_WEST,true},
        {"Media/Editor/Images/block_north.png","",BLOCK_NORTH,true},
        {"Media/Editor/Images/block_east.png","",BLOCK_EAST,true},
        {"Media/Editor/Images/block_south.png","",BLOCK_SOUTH,true},
        {"Media/Editor/Images/surround.png","",BLOCK_ALL,true},
        {"Media/Editor/Images/hot.png","",HOT,true},
        {"Media/Editor/Images/pop.png","",POPS,true},
        {"Media/Editor/Images/floater.png","",FLOATER,true}
    };
    
    for(int i=0;i<sizeof(tools)/sizeof(Tool);i++){
        CL_ToolBarItem item = m_toolbar->insert_item(CL_Sprite(m_pWindow->get_gc(),tools[i].icon),0,tools[i].name);
        item.set_id(tools[i].item);
        item.set_toggling(tools[i].toggle);
    }
}

bool MapEditorState::construct_object_submenu(CL_PopupMenuItem item, const CL_Point& level_pt)
{
        // Find the MO at this point
    std::list<MappableObject*> objects = m_pMap->get_mos_at(level_pt);
    if(!objects.empty()){
        CL_PopupMenu submenu;

        for(std::list<MappableObject*>::const_iterator it = objects.begin();
            it != objects.end(); it++){
            CL_PopupMenuItem subitem = submenu.insert_item((*it)->GetName());
            if(item.get_id() == CON_EDIT_OBJECT){
                subitem.func_clicked().set(this,&MapEditorState::on_edit_mo,*it);
            }else if(item.get_id() == CON_MOVE_OBJECT){
                subitem.func_clicked().set(this,&MapEditorState::on_move_mo,*it);
            }else if(item.get_id() == CON_DELETE_OBJECT){
                subitem.func_clicked().set(this,&MapEditorState::on_delete_mo,*it);
            }
        }
        
        item.set_submenu(submenu);  
        return true;
    }
    return false;
}

void MapEditorState::construct_accels()
{

}

void MapEditorState::on_file_close()
{
    m_pMap->close_level();
    for(std::deque<Operation*>::iterator it = m_undo_stack.begin();
        it != m_undo_stack.end(); it++){
        delete *it;
    }
    m_undo_stack.clear();
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

void MapEditorState::on_view_unzoom()
{
    m_pZoomSlider->set_position(10);
    on_zoom_changed();
}

void MapEditorState::on_view_recenter()
{
    m_pMap->set_origin(CL_Point(0,0));
    m_pMap->request_repaint();
    m_pWindow->request_repaint();
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

void MapEditorState::on_edit_undo()
{
    if(!m_undo_stack.empty()){
        Operation * pOp = m_undo_stack.front();
        m_undo_stack.pop_front();
        pOp->Undo(m_pMap->get_level());
        m_pMap->request_repaint();
        delete pOp;
    }
}

void MapEditorState::on_mo_create()
{
    m_pMOEditWindow->set_visible(true);
    m_pMOEditWindow->bring_to_front();
    m_pMap->show_mos(true);
}

void MapEditorState::on_zoom_changed()
{
    int pos = m_pZoomSlider->get_position();
    float zoom = float(pos * pos)/100.f;
    //float zoom = float(pos) / 10.0f;
    m_pMap->set_scale(zoom);
    m_pMap->request_repaint();
    m_pWindow->request_repaint();
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
    on_close();
}

void MapEditorState::on_close(){  
   EditorState::on_close(NULL);
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
    }else if(m_mouse_state == MOUSE_DOWN){
        if(mod == m_mod_state){
            m_mouse_state = MOUSE_DRAG;
        }
    }
	return true;
}

bool MapEditorState::on_mouse_pressed(const CL_InputEvent& event){ 
    if(event.id == CL_MOUSE_RIGHT || event.id == CL_MOUSE_LEFT){
        switch(m_mouse_state){
            case MOUSE_IDLE:
                m_mouse_state = MOUSE_DOWN;
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
	return true;
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
        }else if(m_mouse_state == MOUSE_DOWN){
            if(event.type != CL_InputEvent::Type::doubleclick)
                click(event.mouse_pos,event.id==CL_MOUSE_LEFT?MOUSE_LEFT:MOUSE_RIGHT, mod_value(event.shift,event.ctrl,event.alt));
            m_mouse_state = MOUSE_IDLE;
        }
    }else if(event.id == CL_KEY_Z && event.ctrl){
        on_edit_undo();
    }
    return true;
}

bool MapEditorState::on_mouse_double_click(const CL_InputEvent& event)
{
    if(event.id == CL_MOUSE_RIGHT){
        // Popup menu (but only when clicking on the level itself)
        CL_Point map_offset = m_pMap->get_geometry().get_top_left();     
        if(m_pMap->valid_location(event.mouse_pos, m_pMap->get_geometry().get_center()-map_offset)){
            
            CL_Point map_offset = m_pMap->get_geometry().get_top_left();
            CL_Point level_pt = m_pMap->screen_to_level(event.mouse_pos-map_offset,m_pMap->get_geometry().get_center());
            level_pt /= 32;  
            
            for(int id=CON_EDIT_OBJECT;id<=CON_DELETE_OBJECT;id++){
                CL_PopupMenuItem item = m_map_context_menu.get_item(id);
                construct_object_submenu(item,level_pt);                
            }
            
            m_map_context_point = event.mouse_pos;
            m_map_context_menu.start(m_pMap,event.mouse_pos);    
        }
    }else{
        CL_Point map_offset = m_pMap->get_geometry().get_top_left();
        CL_Point level_pt = m_pMap->screen_to_level(event.mouse_pos-map_offset,m_pMap->get_geometry().get_center()-map_offset);
        level_pt /= 32;  
        std::cerr << "Edit: " << level_pt.x << ',' << level_pt.y << std::endl;        
    }
    return true;
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

void MapEditorState::on_add_mo()
{
    CL_Point map_offset = m_pMap->get_geometry().get_top_left();
    CL_Point level_pt = m_pMap->screen_to_level(m_map_context_point-map_offset,m_pMap->get_geometry().get_center()-map_offset);
    level_pt /= 32;
    
    std::string name = create_unique_mo_name();
    m_pMOEditWindow->SetName(name.c_str());
    m_pMOEditWindow->SetCreate();
    m_pMOEditWindow->SetPoint(level_pt);
    m_pMOEditWindow->set_visible(true);
    m_pMOEditWindow->bring_to_front();    
    
    m_pMap->show_mos(true);
} 

void MapEditorState::on_edit_mo(MappableObject* pMo)
{
    m_pMOEditWindow->SetMappableObject(pMo);
    m_pMOEditWindow->set_visible(true);
    m_pMOEditWindow->bring_to_front();
    m_pMap->show_mos(true);        
}

void MapEditorState::on_delete_mo(MappableObject* pMo)
{
    DeleteObjectOperation *op = new DeleteObjectOperation();
    op->SetMappableObject(pMo);
    PerformOperation(op);
    m_pMap->request_repaint();
}

void MapEditorState::on_move_mo(MappableObject* pMo)
{
    MoveObjectOperation * op = new MoveObjectOperation();
    op->SetMappableObject(pMo);
    SetOperation(CLICK,op); 
}

void MapEditorState::on_edit_tile()
{
}

std::string MapEditorState::create_unique_mo_name()
{
    std::ostringstream stream;
    int num = 0;
    do{
        stream.clear();
        stream.str("");
        stream << "Object " << num;
        if(NULL == m_pMap->get_mo_named(stream.str())){
            return stream.str();
        }else{
            ++num;
        }
    }while(true);
}


void MapEditorState::start_drag(const CL_Point& point, MouseButton button, int mod){
    if(mod == 0 && button == MOUSE_RIGHT){
        // Pan
    }else{
        // invoke on current tool
        m_mod_state = mod;
    }
}
void MapEditorState::update_drag(const CL_Point& start,const CL_Point& prev, const CL_Point& point, MouseButton button, int mod){
    if(mod == 0 && button == MOUSE_RIGHT){
        // Pan
        CL_Pointf delta = CL_Pointf(point.x,point.y) - CL_Pointf(prev.x,prev.y);
        //delta *= m_pMap->get_scale();
        CL_Point map_offset = m_pMap->get_geometry().get_top_left();
        CL_Point level = m_pMap->screen_to_level(point-map_offset,m_pMap->get_geometry().get_center()-map_offset);
        m_pMap->set_origin(m_pMap->get_origin() + CL_Point(delta.x,delta.y));
        m_pWindow->request_repaint();
    }else{
        // invoke on current tool
        m_pMap->set_rubber_band(CL_Rect(start.x,start.y,point.x,point.y));     
        m_pWindow->request_repaint();        
    }
}

void MapEditorState::cancel_drag(){
    m_mouse_state = MOUSE_IDLE;
    std::cout << "Cancel drag" << std::endl;
    if(m_mod_state == RIGHT){
        // pan
    }else{
        // current tool
        m_pMap->cancel_rubber_band();
    }
}

void MapEditorState::end_drag(const CL_Point& start,const CL_Point& prev, const CL_Point& point, MouseButton button, int mod){
    m_mouse_state = MOUSE_IDLE;
    if(mod == 0 && button == MOUSE_RIGHT){

    }else{
        // current tool
        if(m_pMap->get_level()){
            int tool = mod | DRAG;
            if(button == MOUSE_RIGHT)
                tool |= RIGHT;
            std::map<int,Operation*>::iterator it = m_operations.find(tool);
            if(it != m_operations.end()){
                Operation::Data data;
                data.m_mod_state = tool;
                CL_Point map_offset = m_pMap->get_geometry().get_top_left();                
                data.m_level_pt = m_pMap->screen_to_level(start-map_offset,m_pMap->get_geometry().get_center()-map_offset) / 32;
                data.m_level_end_pt = m_pMap->screen_to_level(point-map_offset,m_pMap->get_geometry().get_center()-map_offset) / 32;
                Operation * op = it->second->clone();
                op->SetData(data);
                if(op->Execute(m_pMap->get_level())){
                    m_undo_stack.push_front(op);
                    m_pWindow->request_repaint();
                }else{
                    delete op;
                }
            }
        }
        m_pMap->cancel_rubber_band();
    }  
}

void MapEditorState::click(const CL_Point& point,MouseButton button, int mod)
{
    if(m_pMap->get_level()){
        int tool = mod | CLICK;
        if(button == MOUSE_RIGHT)
            tool |= RIGHT;
        std::map<int,Operation*>::iterator it = m_operations.find(tool);
        if(it != m_operations.end()){
            Operation::Data data;
            data.m_mod_state = tool;
            CL_Point map_offset = m_pMap->get_geometry().get_top_left();             
            data.m_level_pt = m_pMap->screen_to_level(point-map_offset,m_pMap->get_geometry().get_center()-map_offset) / 32;
            data.m_level_end_pt = data.m_level_pt;
            Operation * op = it->second->clone();
            op->SetData(data);
            if(op->Execute(m_pMap->get_level())){
                m_undo_stack.push_front(op);
                m_pWindow->request_repaint();
            }else{
                delete op;
            }
        }
    }
}

void MapEditorState::reset_toolbar_toggles(CL_ToolBarItem exception)
{
    for(int i=0;i<m_toolbar->get_item_count();i++){
        CL_ToolBarItem item = m_toolbar->get_item(i);
        if(item.get_id() != exception.get_id()){
            item.set_pressed(false);
        }
    }
}

void MapEditorState::on_toolbar_item(CL_ToolBarItem item)
{
    static BlockOperation block_op;
    static BlockOperations block_ops;
    
    reset_toolbar_toggles(item);
    
    switch(item.get_id()){
        case ADD_TILE:{
            CL_Rect rect = m_pTileWindow->get_geometry();
            m_pTileWindow->bring_to_front();
            
            m_pTileWindow->set_geometry(CL_Rect(CL_Point(800-rect.get_width(),64),rect.get_size()));
            m_pWindow->request_repaint();
            break;
        }
        case DELETE_TILE:{
            static DeleteTileOperation op;
            static BasicOperationGroup<DeleteTileOperation> group_op;
            SetOperation(CLICK,&op);
            SetOperation(CLICK|SHIFT,&op);
            SetOperation(DRAG,&group_op);
            SetOperation(DRAG|SHIFT,&group_op);
            break;
        }
        case SHOW_OBJECTS:{
            static bool toggled = false;
            toggled = !toggled;
            m_pMap->show_mos(toggled);
            m_pMap->request_repaint();
            break;
        }
        case SHOW_ALL:{
            static bool toggled = false;
            toggled = !toggled;
            m_pMap->show_hot(toggled);
            m_pMap->show_pop(toggled);
            m_pMap->show_direction_blocks(toggled);
            m_pMap->show_floaters(toggled);
            m_pMap->request_repaint();
            
            break;
        }case BLOCK_WEST:{
        case BLOCK_SOUTH:
        case BLOCK_NORTH:
        case BLOCK_EAST:
        case BLOCK_ALL:
        case HOT:
        case POPS:
        case FLOATER:
            int flag = Tile::TIL_HOT;
            if(item.get_id() == BLOCK_WEST)
                flag = Tile::TIL_BLK_WEST;
            else if(item.get_id() == BLOCK_EAST)
                flag = Tile::TIL_BLK_EAST;
            else if(item.get_id() == BLOCK_SOUTH)
                flag = Tile::TIL_BLK_SOUTH;
            else if(item.get_id() == BLOCK_NORTH)
                flag = Tile::TIL_BLK_NORTH;
            else if(item.get_id() == BLOCK_ALL)
                flag = Tile::TIL_BLK_NORTH | Tile::TIL_BLK_SOUTH | Tile::TIL_BLK_WEST | Tile::TIL_BLK_EAST;
            else if(item.get_id() == POPS)
                flag = Tile::TIL_POPS;
            else if(item.get_id() == FLOATER)
                flag = Tile::TIL_FLOATER;
          
      
            block_op.SetBlock( flag );
            block_ops.SetBlock( flag );
            SetOperation(CLICK,&block_op);
            SetOperation(CLICK|CTRL,&block_op);
            SetOperation(DRAG,&block_ops);
            SetOperation(DRAG|CTRL,&block_ops);
            if(flag == Tile::TIL_POPS){
                m_pMap->show_pop(true);
            }else if(flag == Tile::TIL_HOT){
                m_pMap->show_hot(true);
            }else if(flag == Tile::TIL_FLOATER){
                m_pMap->show_floaters(true);
            }else if(flag != Tile::TIL_HOT){
                m_pMap->show_direction_blocks(true);
            }
            m_pMap->request_repaint();
            break;
        }
        
    }
}

void MapEditorState::SetOperation(int mods, Operation* pOp)
{
    m_operations[mods] = pOp;
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
    delete m_pZoomSlider;
    delete m_pMap;
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

void MapEditorState::PerformOperation(Operation* pOp){
    pOp->Execute(m_pMap->get_level());
    m_undo_stack.push_front(pOp);
    m_pMap->request_repaint();
}


}
#endif