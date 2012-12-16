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


#include "MapWindow.h"
#include "MOEditWindow.h"
#include "MapEditorState.h"
#include "MonsterRegionEditWindow.h"
#include "MonsterRegion.h"
#include "TileEditor.h"

#if SR2_EDITOR

namespace StoneRing { 
	
	
	class AlterZOffsetOperation : public Operation {
	public:
		AlterZOffsetOperation(){}
		virtual ~AlterZOffsetOperation(){}
		virtual Operation* clone() {
			return new AlterZOffsetOperation();
		}
		virtual bool Execute(shared_ptr<Level> level){
			const std::list<Tile*>& tiles = level->GetTilesAt(m_data.m_level_pt);
			if(!tiles.empty()){
				Tile* pTile = tiles.back(); 
				if(m_data.m_mod_state&Operation::CTRL)
					pTile->SetZOffset(pTile->GetZOffset() -1);
				else
					pTile->SetZOffset(pTile->GetZOffset() + 1);
				return true;
			}
			return false;
		}
		virtual void Undo(shared_ptr<Level> level){
			const std::list<Tile*>& tiles = level->GetTilesAt(m_data.m_level_pt);
			if(!tiles.empty()){
				Tile* pTile = tiles.back(); 
				if(m_data.m_mod_state&Operation::CTRL)
					pTile->SetZOffset(pTile->GetZOffset() + 1);
				else
					pTile->SetZOffset(pTile->GetZOffset() - 1);				
			
			}
		}
	};
	
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
            if(m_data.m_mod_state & Operation::SHIFT){
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
            if(m_data.m_mod_state & Operation::CTRL){
                tiles.back()->UnsetFlag(m_flag);
            }else{
                tiles.back()->SetFlag(m_flag);
            }
            return true;
        }
        virtual void Undo(shared_ptr<Level> level){
            std::list<Tile*> tiles = level->GetTilesAt(m_data.m_level_end_pt);
            if(m_data.m_mod_state & Operation::CTRL){
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
	
	class PlaceMonsterRegionOperation: public Operation {
	public:
		PlaceMonsterRegionOperation(){
		}
		virtual ~PlaceMonsterRegionOperation(){
		}
		void SetRegionId(char id){
			m_id = id;
		}
		virtual bool Execute(shared_ptr<Level> level){
			std::list<Tile*> tiles =  level->GetTilesAt(m_data.m_level_pt);
			if(!tiles.empty()){
				m_prev_id = tiles.back()->GetMonsterRegion();
				tiles.back()->SetMonsterRegion(m_id);
			}
			return true;
		}
		virtual void Undo(shared_ptr<Level> level){
			std::list<Tile*> tiles =  level->GetTilesAt(m_data.m_level_pt);
			if(!tiles.empty()){
				tiles.back()->SetMonsterRegion(m_prev_id);
			}			
		}
		virtual Operation* clone() {
			PlaceMonsterRegionOperation* op =  new PlaceMonsterRegionOperation();
			op->SetRegionId(m_id);
			op->m_prev_id = m_prev_id;
			return op;
		}
	private:
		char m_id;
		char m_prev_id;
	};
	
	class PlaceMonsterRegionOperationGroup : public OperationGroup {
	public:
		PlaceMonsterRegionOperationGroup(){
		}
		virtual ~PlaceMonsterRegionOperationGroup(){
		}
		void SetRegionId(char id){
			m_id = id;
		};
		virtual Operation* clone() {
			PlaceMonsterRegionOperationGroup * pOp = new PlaceMonsterRegionOperationGroup();
			pOp->SetRegionId(m_id);
			return pOp;
		}
	protected:
		virtual Operation* create_suboperation() const { 
            PlaceMonsterRegionOperation * pOp =  new PlaceMonsterRegionOperation();
            pOp->SetRegionId(m_id);
            return pOp;
        }
	private:
		char m_id;
	};
	
	class PasteTilesOperation : public Operation {
	public:
		PasteTilesOperation(){
		}
		virtual ~PasteTilesOperation(){
			// TODO: Delete orig tiles?
		}
		virtual bool Execute(shared_ptr<Level> level){
			// If click (as opposed to drag)
			// then we want to just plop the whole thing down
			if(m_data.m_mod_state & Operation::CLICK){
				m_data.m_level_end_pt = m_data.m_level_pt + CL_Point(m_source_tiles.size()-1,m_source_tiles[0].size()-1);
			}
			CL_Point tile_start = m_data.m_level_pt;
			CL_Point tile_end = m_data.m_level_end_pt;
			
			// Repeat the pattern if they drag a big square area.
			m_orig_tiles.resize(tile_end.x - tile_start.x + 1);		
			for(int x=tile_start.x;x<=tile_end.x;x++){
				for(int y=tile_start.y;y<=tile_end.y;y++){
						m_orig_tiles[x-tile_start.x].resize(tile_end.y - tile_start.y+1); 
						int source_x = (x - tile_start.x) % m_source_tiles.size();
						int source_y = (y - tile_start.y) % m_source_tiles[source_x].size();
						assert(source_x < m_source_tiles.size());
						assert(source_y < m_source_tiles[0].size());
						
						if(x < level->GetWidth() && y < level->GetHeight()){
							std::list<Tile*> orig_tiles = level->GetTilesAt(CL_Point(x,y));
							for(std::list<Tile*>::const_iterator it = orig_tiles.begin(); it != orig_tiles.end(); it++){
								m_orig_tiles[(x-tile_start.x)][(y-tile_start.y)].push_back((*it)->clone());
							}
							level->AddTilesAt(CL_Point(x,y),m_source_tiles[source_x][source_y],!(m_data.m_mod_state & Operation::CTRL));
						}
				}
			}
			return true;
		}
		virtual void Undo(shared_ptr<Level> level){
			CL_Point tile_start = m_data.m_level_pt;
			CL_Point tile_end = m_data.m_level_end_pt;
					
			for(int x=tile_start.x;x<=tile_end.x;x++){
				for(int y=tile_start.y;y<=tile_end.y;y++){
						int source_x = x - tile_start.x;
						int source_y = y - tile_start.y;
						assert(source_x < m_orig_tiles.size());
						assert(source_y < m_orig_tiles[0].size());
						if(x < level->GetWidth() && y < level->GetHeight()){
							level->AddTilesAt(CL_Point(x,y),m_orig_tiles[source_x][source_y],true);
						}
				}
			}			
		}
		virtual Operation* clone(){
			PasteTilesOperation * pOp = new PasteTilesOperation();
			pOp->m_orig_tiles = m_orig_tiles;
			pOp->SetSourceTiles(m_source_tiles);
			return pOp;
		}
		void SetSourceTiles(const std::vector<std::vector<std::list<Tile*> > >& source_tiles){
			m_source_tiles = source_tiles;
		}
	private:
		std::vector<std::vector<std::list<Tile*> > > m_orig_tiles;
		std::vector<std::vector<std::list<Tile*> > > m_source_tiles;
	};
	
	class CopyTileOperation : public Operation {
	public:
		CopyTileOperation(MapEditorState& parent):m_parent(parent){
		}
		virtual ~CopyTileOperation(){
			
		}
		
		virtual bool Execute(shared_ptr<Level> level){
			CL_Point tile_start = m_data.m_level_pt;
			CL_Point tile_end = m_data.m_level_end_pt;
			tile_end.x = min(int(level->GetWidth())-1,tile_end.x);
			tile_end.y = min(int(level->GetHeight())-1,tile_end.y);
			m_source_tiles.resize((tile_end.x-tile_start.x) + 1);
			
			for(int x=tile_start.x;x<=tile_end.x;x++){
				m_source_tiles[x - tile_start.x].resize((tile_end.y-tile_start.y) + 1);
				for(int y=tile_start.y;y<=tile_end.y;y++){
					int source_x = x - tile_start.x;
					int source_y = y - tile_start.y;
					assert(x < level->GetWidth() && y < level->GetHeight());
					std::list<Tile*> tiles = level->GetTilesAt(CL_Point(x,y));
					for(std::list<Tile*>::const_iterator it = tiles.begin(); it != tiles.end(); it++){
						m_source_tiles[source_x][source_y].push_back ( (*it)->clone() );
					}
				}
			}
			PasteTilesOperation * pOp = new PasteTilesOperation();
			pOp->SetSourceTiles(m_source_tiles);
			m_parent.SetOperation(Operation::CLICK,pOp);
			m_parent.SetOperation(Operation::CLICK|Operation::CTRL,pOp);
			m_parent.SetOperation(Operation::DRAG,pOp);
			m_parent.SetOperation(Operation::DRAG|Operation::CTRL,pOp);
	
			//m_parent.SetCopyTiles(m_source_tiles);
			return false; // don't put us on the undo stack
		}
		virtual void Undo(shared_ptr<Level> level){
			
		}
		virtual Operation* clone(){
			CopyTileOperation * pOp = new CopyTileOperation(m_parent);
			pOp->m_source_tiles = m_source_tiles;
			return pOp;
		}
	
	private:
		std::vector<std::vector<std::list<Tile*> > > m_source_tiles;
		MapEditorState& m_parent;
	};
	
	class TileEditOperation : public Operation {
	public:
		TileEditOperation ( ){
		}
		virtual ~TileEditOperation ( ){
		}
		void SetEditedTiles ( const std::list<Tile*> &tiles ){
			m_edited_tiles = tiles;
		}
		virtual bool Execute(shared_ptr<Level> level){
			m_original_tiles.clear();	
			std::list<Tile*> original_tiles = level->GetTilesAt(m_data.m_level_pt);
			for(std::list<Tile*>::const_iterator it = original_tiles.begin(); it != original_tiles.end(); it++){
				m_original_tiles.push_back ( (*it)->clone() );
			}
			level->AddTilesAt(m_data.m_level_pt,m_edited_tiles,true);
			return true;
		}
		virtual void Undo(shared_ptr<Level> level){
			level->AddTilesAt(m_data.m_level_pt,m_original_tiles,true);
		}
		virtual Operation* clone(){
			TileEditOperation * pOp = new TileEditOperation;
			return pOp;
		}
	private:
		std::list<Tile*> m_edited_tiles;	
		std::list<Tile*> m_original_tiles;
	};
	

MapWindow::MapWindow(CL_GUIComponent *parent, const CL_GUITopLevelDescription& desc):CL_Window(parent,desc) {
	set_draggable(true);
	set_clip_children(true);
    func_input_pointer_moved().set(this,&MapWindow::on_mouse_moved);
    func_input_pressed().set(this,&MapWindow::on_mouse_pressed);
    func_input_released().set(this,&MapWindow::on_mouse_released);
    func_focus_gained().set(this,&MapWindow::on_pointer_entered);
    func_focus_lost().set(this,&MapWindow::on_pointer_exit);
    func_input_doubleclick().set(this,&MapWindow::on_mouse_double_click);
	func_resized().set(this,&MapWindow::on_resize);

    m_pZoomSlider = new CL_Slider(this);
    m_pZoomSlider->set_vertical(true);
    m_pZoomSlider->set_min(1);
    m_pZoomSlider->set_max(12);
    m_pZoomSlider->set_position(10);
    //m_pZoomSlider->set_tick_count(15-2);
    //m_pZoomSlider->set_lock_to_ticks(true);
    m_pZoomSlider->func_value_changed().set(this,&MapWindow::on_zoom_changed);

    m_pMap = new MapComponent(this);
   
    m_toolbar = new CL_ToolBar(this); 
    m_toolbar->func_item_clicked().set(this,&MapWindow::on_toolbar_item);

    m_pMenuBar = new CL_MenuBar(this);
  
    construct_map_context_menu();
    construct_menu();
    construct_toolbar();
	
	on_resize();

    //m_pToolWindow->set_geometry(CL_Rect(0,0,300,400));
    m_mouse_state = MOUSE_IDLE;	
}

MapWindow::~MapWindow() {
	delete m_toolbar;
	delete m_pMenuBar;
	delete m_pMap;
	delete m_pZoomSlider;
}

void MapWindow::on_resize()
{	
	CL_Rect client = get_client_area();
	CL_Size size = client.get_size();
	CL_Point top_left = client.get_top_left();	
	m_pMap->set_geometry(CL_Rect(CL_Point(32,64),CL_Size(size.width-32,size.height-64)).translate(client));
	m_toolbar->set_geometry(CL_Rect(CL_Point(0,32),CL_Size(size.width,32)).translate(client));	
    m_pMenuBar->set_geometry(CL_Rect(0,0,size.width,32).translate(client));  
    m_pZoomSlider->set_geometry(CL_Rect(0,64,10,size.height).translate(client));	
}


void MapWindow::Init(MapEditorState * state)
{
	m_state = state;
}

void MapWindow::construct_map_context_menu()
{
    m_map_context_menu.clear();
    CL_PopupMenuItem edit_tile_item = m_map_context_menu.insert_item("Edit Tiles",CON_EDIT_TILE);
    m_map_context_menu.insert_separator();
    CL_PopupMenuItem add_mo_item = m_map_context_menu.insert_item("Add Object",CON_ADD_OBJECT);
    CL_PopupMenuItem edit_mo_item = m_map_context_menu.insert_item("Edit Object",CON_EDIT_OBJECT);
    CL_PopupMenuItem move_mo_item = m_map_context_menu.insert_item("Move Object",CON_MOVE_OBJECT);
    CL_PopupMenuItem delete_mo_item = m_map_context_menu.insert_item("Delete Object",CON_DELETE_OBJECT);
    
    edit_tile_item.func_clicked().set(this,&MapWindow::on_edit_tile);    
    add_mo_item.func_clicked().set(this,&MapWindow::on_add_mo);
}

void MapWindow::construct_menu()
{
    m_file_menu.clear();

    m_file_menu.insert_item("Save").func_clicked().set(this,&MapWindow::on_file_save);

    
    m_edit_menu.clear();
    m_grow_submenu.clear();
    CL_PopupMenu add_columns_sub, add_rows_sub;
    add_columns_sub.insert_item("Add 1").func_clicked().set(this,&MapWindow::on_edit_grow_column);
    add_columns_sub.insert_item("Add 5").func_clicked().set(this,&MapWindow::on_edit_grow_column5);
    add_columns_sub.insert_item("Add 10").func_clicked().set(this,&MapWindow::on_edit_grow_column10);
    add_columns_sub.insert_item("Add 25").func_clicked().set(this,&MapWindow::on_edit_grow_column25);	
    add_rows_sub.insert_item("Add 1").func_clicked().set(this,&MapWindow::on_edit_grow_row);
    add_rows_sub.insert_item("Add 5").func_clicked().set(this,&MapWindow::on_edit_grow_row5);   
    add_rows_sub.insert_item("Add 10").func_clicked().set(this,&MapWindow::on_edit_grow_row10);   
    add_rows_sub.insert_item("Add 25").func_clicked().set(this,&MapWindow::on_edit_grow_row25);   	
    CL_PopupMenuItem add_columns = m_grow_submenu.insert_item("Add Columns");
    add_columns.set_submenu(add_columns_sub);
    CL_PopupMenuItem add_rows = m_grow_submenu.insert_item("Add Rows");
    add_rows.set_submenu(add_rows_sub);
    CL_PopupMenuItem grow_item = m_edit_menu.insert_item("Grow");
    m_edit_menu.insert_separator();
    m_edit_menu.insert_item_accel("Undo", "Ctrl-Z").func_clicked().set(this,&MapWindow::on_edit_undo);
    m_file_menu.insert_separator();
    m_file_menu.insert_item("Close").func_clicked().set(this,&MapWindow::on_file_close);
    grow_item.set_submenu(m_grow_submenu);
    m_pMenuBar->add_menu("File",m_file_menu);
    m_pMenuBar->add_menu("Edit",m_edit_menu);
    
    CL_PopupMenu view;
    view.insert_item("Unzoom").func_clicked().set(this,&MapWindow::on_view_unzoom);
    view.insert_item("Recenter").func_clicked().set(this,&MapWindow::on_view_recenter);
	
    
    m_mo_menu.clear();
    //m_mo_menu.insert_item("Create").func_clicked().set(this,&MapWindow::on_mo_create);
    
    m_pMenuBar->add_menu("View",view);
	m_pMenuBar->add_menu("Monster Regions",m_monster_region_menu);
	
	construct_level_data_menu();
}

void MapWindow::construct_region_menu()
{
	m_monster_region_menu.clear();
	m_monster_region_menu.insert_item("Create Region").func_clicked().set(this,&MapWindow::on_create_monster_region);
	CL_PopupMenu place_menu;
	CL_PopupMenu edit_menu;
	CL_PopupMenu delete_menu;
	const MonsterRegions* regions = m_pMap->get_level()->GetMonsterRegions();
	if(regions && regions->GetRegionsBegin() != regions->GetRegionsEnd()){
		for(std::map<char,MonsterRegion*>::const_iterator it = regions->GetRegionsBegin();
				it != regions->GetRegionsEnd();it++){
			CL_PopupMenuItem edit_item = edit_menu.insert_item(IntToString((it->second)->GetId()));
			CL_PopupMenuItem delete_item = delete_menu.insert_item(IntToString((it->second)->GetId()));
			CL_PopupMenuItem place_item = place_menu.insert_item(IntToString((it->second)->GetId()));
			edit_item.func_clicked().set(this,&MapWindow::on_edit_region,it->second);
			delete_item.func_clicked().set(this,&MapWindow::on_delete_region,it->second);
			place_item.func_clicked().set(this,&MapWindow::on_place_region,it->second);
		}	
		CL_PopupMenuItem place_item = m_monster_region_menu.insert_item("Place Region");
		place_item.set_submenu(place_menu);
		CL_PopupMenuItem item =  m_monster_region_menu.insert_item("Edit Region");
		item.set_submenu(edit_menu);
		CL_PopupMenuItem delete_item = m_monster_region_menu.insert_item("Delete Region");
		delete_item.set_submenu(delete_menu);	
	}
	CL_PopupMenuItem clear_item = m_monster_region_menu.insert_item("Clear Region");
	clear_item.func_clicked().set(this,&MapWindow::on_clear_regions);
}

void MapWindow::construct_toolbar()
{
    struct Tool {
        std::string icon;
        std::string name;
        ToolBarItem item;
        bool toggle;
    };
    Tool tools[] = {
		{"Editor/Images/add.png","Copy Tiles",COPY_TILE,true},
        {"Editor/Images/map_delete.png","Erase Tiles",DELETE_TILE,true},
        {"Editor/Images/view.png","View",SHOW_ALL,true},
        {"Editor/Images/eye.png","Objects",SHOW_OBJECTS,true},
        {"Editor/Images/block_west.png","",BLOCK_WEST,true},
        {"Editor/Images/block_north.png","",BLOCK_NORTH,true},
        {"Editor/Images/block_east.png","",BLOCK_EAST,true},
        {"Editor/Images/block_south.png","",BLOCK_SOUTH,true},
        {"Editor/Images/surround.png","",BLOCK_ALL,true},
        {"Editor/Images/hot.png","",HOT,true},
        {"Editor/Images/pop.png","",POPS,true},
        {"Editor/Images/floater.png","",ALTER_ZORDER,true},
		{"Editor/Images/floater_icon.png","",FLOATER,true}
    };
    
    for(int i=0;i<sizeof(tools)/sizeof(Tool);i++){
        CL_ToolBarItem item = m_toolbar->insert_item(CL_Sprite(this->get_gc(),tools[i].icon),0,tools[i].name);
        item.set_id(tools[i].item);
        item.set_toggling(tools[i].toggle);
    }
}

bool MapWindow::construct_object_submenu(CL_PopupMenuItem item, const CL_Point& level_pt)
{
        // Find the MO at this point
    std::list<MappableObject*> objects = m_pMap->get_mos_at(level_pt);
    if(!objects.empty()){
        CL_PopupMenu submenu;

        for(std::list<MappableObject*>::const_iterator it = objects.begin();
            it != objects.end(); it++){
            CL_PopupMenuItem subitem = submenu.insert_item((*it)->GetName());
            if(item.get_id() == CON_EDIT_OBJECT){
                subitem.func_clicked().set(this,&MapWindow::on_edit_mo,*it);
            }else if(item.get_id() == CON_MOVE_OBJECT){
                subitem.func_clicked().set(this,&MapWindow::on_move_mo,*it);
            }else if(item.get_id() == CON_DELETE_OBJECT){
                subitem.func_clicked().set(this,&MapWindow::on_delete_mo,*it);
            }
        }
        
        item.set_submenu(submenu);  
        return true;
    }
    
    return false;
}

void MapWindow::construct_level_data_menu()
{
	CL_PopupMenu level_data_menu;
	m_level_allow_run_item = level_data_menu.insert_item("Allow Running");
	m_level_allow_run_item.set_checkable(true);
	m_level_allow_run_item.func_clicked().set(this,&MapWindow::on_allows_running_clicked);

	CL_PopupMenuItem level_music_item = level_data_menu.insert_item("Music");
	construct_level_music_menu(level_music_item);
	m_pMenuBar->add_menu("Settings",level_data_menu);
}

void MapWindow::construct_level_music_menu(CL_PopupMenuItem menu_parent)
{
	CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    m_music = resources.get_resource_names_of_type("sample","Music");    
    for(int i=0;i<m_music.size();i++){
        CL_PopupMenuItem item = m_level_music_menu.insert_item(m_music[i]);
        item.func_clicked().set(this,&MapWindow::on_music_clicked,i);
        item.set_checkable(true);
        item.set_checked(false);
    }
    menu_parent.set_submenu(m_level_music_menu);
}

void MapWindow::on_music_clicked(int index)
{
	m_pMap->get_level()->SetMusic(m_music[index]);
	for(int i=0;i<m_level_music_menu.get_item_count();i++){
		CL_PopupMenuItem item = m_level_music_menu.get_item_at(i);
		item.set_checked(i==index);
	}	
}

void MapWindow::on_allows_running_clicked()
{
	m_pMap->get_level()->SetAllowsRunning(m_level_allow_run_item.is_checked());
}

void MapWindow::sync_from_level()
{
	m_level_allow_run_item.set_checked(m_pMap->get_level()->AllowsRunning());
	for(int i=0;i<m_music.size();i++){
		if(std::string(m_music[i]) == m_pMap->get_level()->GetMusic()){
			m_level_music_menu.get_item_at(i).set_checked(true);
		}else{
			m_level_music_menu.get_item_at(i).set_checked(false);
		}
	}
}

void MapWindow::New()
{
	assert(m_pMap);
	m_pMap->create_level(5,5);
	construct_region_menu();
	request_repaint();
	sync_from_level();
}

void MapWindow::Open( const std::string& filename )
{
	assert(m_pMap);
	m_pMap->load_level(filename);
	construct_region_menu();
	request_repaint();
	sync_from_level();
}

void MapWindow::PerformOperation( Operation* pOp )
{
	if(pOp->Execute(m_pMap->get_level())){
		m_undo_stack.push_front(pOp);
		request_repaint();
	}
}




void MapWindow::on_file_save()
{
    CL_SaveFileDialog dialog(this);
  //  dialog.set_geometry(CL_Rect(0,0,400,400);
    dialog.set_title("Save Level");
    if(dialog.show()){
        std::string name = dialog.get_filename();
        m_pMap->writeXML(name);
    }
}


void MapWindow::on_file_close()
{
    m_pMap->close_level();
    for(std::deque<Operation*>::iterator it = m_undo_stack.begin();
        it != m_undo_stack.end(); it++){
        delete *it;
    }
    m_undo_stack.clear();
	set_parent_component(NULL);
	m_state->CloseMapWindow(this);
	delete this;
}


void MapWindow::on_view_unzoom()
{
    m_pZoomSlider->set_position(10);
    on_zoom_changed();
}

void MapWindow::on_view_recenter()
{
    m_pMap->set_origin(CL_Point(0,0));
    m_pMap->request_repaint();
}

void MapWindow::on_edit_grow_column()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth()+1,m_pMap->get_level()->GetHeight());
        m_pMap->request_repaint();
    }
}

void MapWindow::on_edit_grow_column5()
{    
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth()+5,m_pMap->get_level()->GetHeight());
        m_pMap->request_repaint();    
    }
}

void MapWindow::on_edit_grow_column10()
{    
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth()+10,m_pMap->get_level()->GetHeight());
        m_pMap->request_repaint();    
    }
}

void MapWindow::on_edit_grow_column25()
{    
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth()+25,m_pMap->get_level()->GetHeight());
        m_pMap->request_repaint();    
    }
}

void MapWindow::on_edit_grow_row()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth(),m_pMap->get_level()->GetHeight()+1);
        m_pMap->request_repaint();    
    }
}

void MapWindow::on_edit_grow_row5()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth(),m_pMap->get_level()->GetHeight()+5);
        m_pMap->request_repaint();    
    }
}

void MapWindow::on_edit_grow_row10()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth(),m_pMap->get_level()->GetHeight()+10);
        m_pMap->request_repaint();    
    }
}

void MapWindow::on_edit_grow_row25()
{
    if(m_pMap->get_level()){
        m_pMap->get_level()->GrowLevelTo(m_pMap->get_level()->GetWidth(),m_pMap->get_level()->GetHeight()+25);
        m_pMap->request_repaint();    
    }
}


void MapWindow::on_edit_undo()
{
    if(!m_undo_stack.empty()){
        Operation * pOp = m_undo_stack.front();
        m_undo_stack.pop_front();
        pOp->Undo(m_pMap->get_level());
        m_pMap->request_repaint();
        delete pOp;
    }
}

void MapWindow::on_mo_create()
{
}

void MapWindow::on_create_monster_region()
{
	CL_GUITopLevelDescription desc("Monster Region",CL_Size(424,424),true);
	MonsterRegionEditWindow window(this,desc);
	window.CreateRegion();
	window.set_draggable(true);
	if(0 == window.exec()){
		m_pMap->get_level()->AddMonsterRegion(window.GetRegion());
		construct_region_menu();
	}
}


void MapWindow::on_zoom_changed()
{
    int pos = m_pZoomSlider->get_position();
    float zoom = float(pos * pos)/100.f;
    //float zoom = float(pos) / 10.0f;
    m_pMap->set_scale(zoom);
    m_pMap->request_repaint();
}


void MapWindow::SetOperation(int mods, Operation* pOp)
{
    m_operations[mods] = pOp;
}  


bool MapWindow::on_mouse_moved(const CL_InputEvent& event){
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

bool MapWindow::on_mouse_pressed(const CL_InputEvent& event){ 
    if(event.id == CL_MOUSE_RIGHT || event.id == CL_MOUSE_LEFT && 
		m_pMap->get_geometry().contains(event.mouse_pos)){
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
        bring_to_front();
        return true;
    }
    return false;
}

int MapWindow::mod_value(bool shift, bool ctrl, bool alt)const
{
    int value = 0;
    if(shift)
        value |= Operation::SHIFT;
    if(ctrl)
        value |= Operation::CTRL;
    if(alt)
        value |= Operation::ALT;
    return value;
}

bool MapWindow::on_mouse_released(const CL_InputEvent& event){
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

bool MapWindow::on_mouse_double_click(const CL_InputEvent& event)
{
    if(event.id == CL_MOUSE_RIGHT){
        // Popup menu (but only when clicking on the level itself)
        CL_Point map_offset = m_pMap->get_geometry().get_top_left();     
        if(m_pMap->valid_location(event.mouse_pos-map_offset, m_pMap->get_geometry().get_center()-map_offset)){
            
            CL_Point level_pt = m_pMap->screen_to_level(event.mouse_pos-map_offset,m_pMap->get_geometry().get_center());
            level_pt /= 32;  
            
            for(int id=CON_EDIT_OBJECT;id<=CON_DELETE_OBJECT;id++){
                CL_PopupMenuItem item = m_map_context_menu.get_item(id);
                construct_object_submenu(item,level_pt);                
            }
            
            m_map_context_point = event.mouse_pos;
            m_map_context_menu.start(m_pMap, component_to_screen_coords(event.mouse_pos));    
        }
    }else{
        CL_Point map_offset = m_pMap->get_geometry().get_top_left();
        CL_Point level_pt = m_pMap->screen_to_level(event.mouse_pos-map_offset,m_pMap->get_geometry().get_center()-map_offset);
        level_pt /= 32;  
        std::cerr << "Edit: " << level_pt.x << ',' << level_pt.y << std::endl;        
    }
    return true;
}

bool MapWindow::on_pointer_entered(){
    if(m_mouse_state == MOUSE_DRAG)
        cancel_drag();
    return true;
}

bool MapWindow::on_pointer_exit(){
    if(m_mouse_state == MOUSE_DRAG)
        cancel_drag();
    return true;
}

void MapWindow::on_add_mo()
{	
    CL_Size size = get_size(); 	
    CL_GUITopLevelDescription mo_edit_desc;
    mo_edit_desc.set_title("Create Mappable Object");
    mo_edit_desc.set_size(CL_Size(400,400),true);
    mo_edit_desc.set_position(CL_Rect(CL_Point(size.width-400,64),CL_Size(400,400)),true);
    mo_edit_desc.set_dialog_window(true);
    mo_edit_desc.set_decorations(true);
	CL_GUIManager manager = get_gui_manager();
    MOEditWindow edit_window(&manager, mo_edit_desc);
    edit_window.set_draggable(true);
    edit_window.set_visible(false);
    edit_window.SetMapWindow(this);	
    edit_window.set_visible(true);
    std::string name = create_unique_mo_name();
    edit_window.SetName(name.c_str());
    edit_window.SetCreate();
    CL_Point map_offset = m_pMap->get_geometry().get_top_left();
    CL_Point level_pt = m_pMap->screen_to_level(m_map_context_point-map_offset,m_pMap->get_geometry().get_center());
    level_pt /= 32;  	
	
    edit_window.SetPoint(level_pt);
	edit_window.exec();
} 

void MapWindow::on_edit_mo(MappableObject* pMo)
{
    CL_Size size = get_size(); 	
    CL_GUITopLevelDescription mo_edit_desc;
    mo_edit_desc.set_title("Edit Mappable Object");
    mo_edit_desc.set_size(CL_Size(400,400),true);
    mo_edit_desc.set_position(CL_Rect(CL_Point(size.width-400,64),CL_Size(400,400)),true);
    mo_edit_desc.set_dialog_window(true);
    mo_edit_desc.set_decorations(true);
	CL_GUIManager  manager = get_gui_manager();
    MOEditWindow edit_window(&manager, mo_edit_desc);
    edit_window.set_draggable(true);
    edit_window.set_visible(false);
    edit_window.SetMapWindow(this);	
    edit_window.SetMappableObject(pMo);
    edit_window.set_visible(true);
	edit_window.exec();
}

void MapWindow::on_delete_mo(MappableObject* pMo)
{
    DeleteObjectOperation *op = new DeleteObjectOperation();
    op->SetMappableObject(pMo);
    PerformOperation(op);
    m_pMap->request_repaint();
}

void MapWindow::on_move_mo(MappableObject* pMo)
{
    MoveObjectOperation * op = new MoveObjectOperation();
    op->SetMappableObject(pMo);
    SetOperation(Operation::CLICK,op); 
}

void MapWindow::on_edit_tile()
{
    CL_Point map_offset = m_pMap->get_geometry().get_top_left();	
	CL_Point level_pt = m_pMap->screen_to_level(m_map_context_point-map_offset,m_pMap->get_geometry().get_center());
    level_pt /= 32;  
	CL_GUITopLevelDescription desc("Edit Tile",CL_Size(424,424),true);	
	TileEditorWindow window(this,desc);
	window.set_tiles(m_pMap->get_level()->GetTilesAt(level_pt));
	window.set_draggable(true);
	const MonsterRegions * regions = m_pMap->get_level()->GetMonsterRegions();
	if(regions){
		for(std::map<char,MonsterRegion*>::const_iterator it = regions->GetRegionsBegin();
			it != regions->GetRegionsEnd(); it++){
			window.add_monster_region((it->second)->GetId());
		}
	}
	window.populate_monster_region_list();
	
	if(0 == window.exec()){
		TileEditOperation * pOp = new TileEditOperation();
		Operation::Data data;
		data.m_level_pt = data.m_level_end_pt = level_pt;
		data.m_mod_state = 0;
		pOp->SetEditedTiles(window.get_tiles());
		pOp->SetData(data);
		PerformOperation(pOp);
	}
}

void MapWindow::on_edit_region(MonsterRegion* pRegion)
{
	CL_GUITopLevelDescription desc("Monster Region",CL_Size(424,424),true);
	MonsterRegionEditWindow window(this,desc);
	window.SetRegion(pRegion);
	window.set_draggable(true);
	if(0 == window.exec()){
		
	}
}

void MapWindow::on_delete_region(MonsterRegion* pRegion)
{
	m_pMap->get_level()->RemoveMonsterRegion(pRegion);
	construct_region_menu();
}

void MapWindow::on_place_region(MonsterRegion* pRegion)
{
	PlaceMonsterRegionOperation * op = new PlaceMonsterRegionOperation();
	op->SetRegionId(pRegion->GetId());
	SetOperation(Operation::CLICK, op);

	PlaceMonsterRegionOperationGroup * group_op = new PlaceMonsterRegionOperationGroup();
	group_op->SetRegionId(pRegion->GetId());
    SetOperation(Operation::DRAG,group_op);
  
	m_pMap->show_monster_region(true);
}

void MapWindow::on_clear_regions()
{
	PlaceMonsterRegionOperation * op = new PlaceMonsterRegionOperation();
	op->SetRegionId(-1);
	SetOperation(Operation::CLICK, op);

	PlaceMonsterRegionOperationGroup * group_op = new PlaceMonsterRegionOperationGroup();
	group_op->SetRegionId(-1);
    SetOperation(Operation::DRAG,group_op);
  
	m_pMap->show_monster_region(true);	
}

std::string MapWindow::create_unique_mo_name()
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


void MapWindow::start_drag(const CL_Point& point, MouseButton button, int mod){
    if(mod == 0 && button == MOUSE_RIGHT){
        // Pan
    }else{
        // invoke on current tool
        m_mod_state = mod;
    }
}
void MapWindow::update_drag(const CL_Point& start,const CL_Point& prev, const CL_Point& point, MouseButton button, int mod){
    if(mod == 0 && button == MOUSE_RIGHT){
        // Pan
        CL_Pointf delta = CL_Pointf(point.x,point.y) - CL_Pointf(prev.x,prev.y);
        //delta *= m_pMap->get_scale();
        CL_Point map_offset = m_pMap->get_geometry().get_top_left();
        CL_Point level = m_pMap->screen_to_level(point-map_offset,m_pMap->get_geometry().get_center()-map_offset);
        m_pMap->set_origin(m_pMap->get_origin() + CL_Point(delta.x,delta.y));
        this->request_repaint();
    }else{
        // invoke on current tool
        m_pMap->set_rubber_band(CL_Rect(start.x,start.y,point.x,point.y));     
        this->request_repaint();        
    }
}

void MapWindow::cancel_drag(){
    m_mouse_state = MOUSE_IDLE;
    std::cout << "Cancel drag" << std::endl;
    if(m_mod_state == Operation::RIGHT){
        // pan
    }else{
        // current tool
        m_pMap->cancel_rubber_band();
    }
}

void MapWindow::end_drag(const CL_Point& start,const CL_Point& prev, const CL_Point& point, MouseButton button, int mod){
    m_mouse_state = MOUSE_IDLE;
    if(mod == 0 && button == MOUSE_RIGHT){

    }else{
        // current tool  
        if(m_pMap->get_level()){
            Operation::Data data;			
            CL_Point map_offset = m_pMap->get_geometry().get_top_left();   			
            data.m_level_pt = m_pMap->screen_to_level(start-map_offset,m_pMap->get_geometry().get_center()-map_offset) / 32;
            data.m_level_end_pt = m_pMap->screen_to_level(point-map_offset,m_pMap->get_geometry().get_center()-map_offset) / 32;			
            int tool = mod;
			if(data.m_level_pt != data.m_level_end_pt){
				tool |= Operation::DRAG;
			}else{
				tool |= Operation::CLICK;
			}
            if(button == MOUSE_RIGHT)
                tool |= Operation::RIGHT;
			Operation * orig_op = NULL;
            std::map<int,Operation*>::iterator it = m_operations.find(tool);
			if(it != m_operations.end()){
				orig_op = it->second;
			}
            if(orig_op != NULL){

                data.m_mod_state = tool;         
				CL_Point min_pt, max_pt;
				
				min_pt = CL_Point ( min(data.m_level_pt.x,data.m_level_end_pt.x), min(data.m_level_pt.y, data.m_level_end_pt.y ) );
				max_pt = CL_Point ( max(data.m_level_pt.x,data.m_level_end_pt.x), max(data.m_level_pt.y, data.m_level_end_pt.y ) );
				
				data.m_level_pt = min_pt;
				data.m_level_end_pt = max_pt;
				
			
				
                Operation * op = orig_op->clone();
                op->SetData(data);
                if(op->Execute(m_pMap->get_level())){
                    m_undo_stack.push_front(op);
                    this->request_repaint();
					m_pMap->request_repaint();
                }else{
                    delete op;
                }
            }
        }
        m_pMap->cancel_rubber_band();
    }  
}

void MapWindow::click(const CL_Point& point,MouseButton button, int mod)
{
    if(m_pMap->get_level()){
        int tool = mod | Operation::CLICK;
        if(button == MOUSE_RIGHT)
            tool |= Operation::RIGHT;
		Operation * orig_op = NULL;
        std::map<int,Operation*>::iterator it = m_operations.find(tool);
		if(it != m_operations.end()){
			orig_op = it->second;
		}
        if(orig_op != NULL){
            Operation::Data data;
            data.m_mod_state = tool;
            CL_Point map_offset = m_pMap->get_geometry().get_top_left();             
            data.m_level_pt = m_pMap->screen_to_level(point-map_offset,m_pMap->get_geometry().get_center()-map_offset) / 32;
            data.m_level_end_pt = data.m_level_pt;
            Operation * op = orig_op->clone();
            op->SetData(data);
            if(op->Execute(m_pMap->get_level())){
                m_undo_stack.push_front(op);
                this->request_repaint();
				m_pMap->request_repaint();
            }else{
                delete op;
            }
        }
    }
}

void MapWindow::reset_toolbar_toggles(CL_ToolBarItem exception)
{
    for(int i=0;i<m_toolbar->get_item_count();i++){
        CL_ToolBarItem item = m_toolbar->get_item(i);
        if(item.get_id() != exception.get_id()){
            item.set_pressed(false);
        }
    }
}

void MapWindow::on_toolbar_item(CL_ToolBarItem item)
{
    static BlockOperation block_op;
    static BlockOperations block_ops;
    
    reset_toolbar_toggles(item);
    
    switch(item.get_id()){
		case ALTER_ZORDER:{
			static AlterZOffsetOperation op;
			static BasicOperationGroup<AlterZOffsetOperation>  group_op;
			SetOperation(Operation::CLICK,&op);
			SetOperation(Operation::DRAG,&group_op);
			SetOperation(Operation::CLICK|Operation::CTRL,&op);
			SetOperation(Operation::DRAG|Operation::CTRL,&group_op);
			m_pMap->show_zorder(true);
			break;
		}
        case COPY_TILE:{
			CopyTileOperation * op = new CopyTileOperation(*m_state);
			SetOperation(Operation::CLICK,op);
			SetOperation(Operation::DRAG,op);
            break;
        }
        case DELETE_TILE:{
            static DeleteTileOperation op;
            static BasicOperationGroup<DeleteTileOperation> group_op;
            SetOperation(Operation::CLICK,&op);
            SetOperation(Operation::CLICK|Operation::SHIFT,&op);
            SetOperation(Operation::DRAG,&group_op);
            SetOperation(Operation::DRAG|Operation::SHIFT,&group_op);
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
			m_pMap->show_monster_region(toggled);
			m_pMap->show_zorder(toggled);
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
            SetOperation(Operation::CLICK,&block_op);
            SetOperation(Operation::CLICK|Operation::CTRL,&block_op);
            SetOperation(Operation::DRAG,&block_ops);
            SetOperation(Operation::DRAG|Operation::CTRL,&block_ops);
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


}

#endif