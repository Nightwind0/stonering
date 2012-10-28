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


#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include "Operation.h"
#include "MapComponent.h"

namespace StoneRing { 
	
	class MapEditorState;

class MapWindow : public CL_Window
{
public:
    MapWindow(CL_GUIComponent* i_component, const CL_GUITopLevelDescription &description);
    virtual ~MapWindow();
	
	void Init(MapEditorState* state);
	void Open(const std::string& filename);
	void New();
        
	// Sets an operation to be performed by click
	void SetOperation(int mods,Operation*); 
	
	// Directly perfporm this and add it to the undo stack
	void PerformOperation(Operation*);
private:
	
        enum MouseButton {
            MOUSE_LEFT=0,
            MOUSE_RIGHT=Operation::Modifier::RIGHT
        }m_drag_button;
	        
        enum MouseState {
            MOUSE_IDLE,
            MOUSE_DOWN,
            MOUSE_DRAG
        }m_mouse_state;
        
        enum ToolBarItem {
			COPY_TILE,
            ADD_TILE,
            DELETE_TILE,
            ADD_OBJECT,
            DELETE_OBJECT,
            SHOW_OBJECTS,
            SHOW_ALL,
            EDIT_OBJECT,
            EDIT_LEVEL,
            BLOCK_WEST,
            BLOCK_NORTH,
            BLOCK_EAST,
            BLOCK_SOUTH,
            BLOCK_ALL,
            HOT,
            POPS,
            FLOATER,
            WATER,
			ALTER_ZORDER
        };
        
        enum ContextMenu {
            CON_EDIT_TILE,
            CON_ADD_OBJECT,
            CON_EDIT_OBJECT,
            CON_MOVE_OBJECT,
            CON_DELETE_OBJECT
        };
		
        int  mod_value(bool shift,bool ctrl,bool alt)const;
        void on_file_close();
        void on_file_save();
        void on_edit_undo();
        void on_edit_grow_column();
        void on_edit_grow_row();
        void on_edit_grow_column5();
        void on_edit_grow_row5();
        void on_edit_grow_column10();
        void on_edit_grow_row10();
        void on_edit_grow_column25();
        void on_edit_grow_row25();		
        void on_mo_create();
        void on_mo_place();
        void on_zoom_changed();
        bool on_mouse_moved(const CL_InputEvent&);
        bool on_mouse_pressed(const CL_InputEvent&);
        bool on_mouse_released(const CL_InputEvent&);
        bool on_mouse_double_click(const CL_InputEvent&);
        bool on_pointer_entered();
        bool on_pointer_exit();
		void on_resize();
        
		void on_create_monster_region();
		
        // Map context menu
        void on_edit_tile();
        void on_add_mo();
        void on_edit_mo(MappableObject* pObj);
        void on_move_mo(MappableObject* pObj);
        void on_delete_mo(MappableObject* pObj);
		void on_edit_region(MonsterRegion* pRegion);
		void on_delete_region(MonsterRegion* pRegion);
		void on_place_region(MonsterRegion* pRegion);
		void on_clear_regions();
        
		void on_view_unzoom();
        void on_view_recenter();
        
        void on_toolbar_item(CL_ToolBarItem);
		void on_music_clicked(int i);
		void on_allows_running_clicked();
		void sync_from_level();
		
        std::string create_unique_mo_name();
        
        // mouse 
        void start_drag(const CL_Point&, MouseButton button, int mod_state);
        void update_drag(const CL_Point& start,const CL_Point& prev_point,const CL_Point& point,MouseButton button, int mod_state);
        void cancel_drag();
        void end_drag(const CL_Point& start,const CL_Point& prev_point, const CL_Point& point,MouseButton button, int mod_state);
        void click(const CL_Point& point,MouseButton button, int mod_state);
        
        void construct_map_context_menu();
        void construct_accels();
        void construct_menu();
		void construct_region_menu();
		void construct_level_data_menu();
		void construct_level_music_menu(CL_PopupMenuItem menu_parent);
        void construct_toolbar();
        void reset_toolbar_toggles(CL_ToolBarItem exception);     
        bool construct_object_submenu(CL_PopupMenuItem menu_item, const CL_Point & level_pt);
        virtual CL_Size get_preferred_size()const{ return CL_Size(1024,700); }
		

        // GUI stuff      

        CL_PopupMenu            m_file_menu;
        CL_PopupMenu            m_edit_menu;
        CL_PopupMenu            m_grow_submenu;
        CL_PopupMenu            m_mo_menu; 
        CL_MenuBar *            m_pMenuBar;
        CL_PushButton*          m_pButton;
        CL_Slider*              m_pZoomSlider;
        CL_ToolBar*             m_toolbar;
        CL_PopupMenu            m_map_context_menu;
		CL_PopupMenu 			m_monster_region_menu;
		CL_PopupMenu            m_level_music_menu;
		CL_PopupMenuItem        m_level_allow_run_item;
   
        MapComponent*           m_pMap;
        int                     m_mod_state;
        CL_Point                m_drag_start;
        CL_Point                m_last_drag_point;
        CL_Point                m_map_context_point;
        std::map<int,Operation*> m_operations;
        std::deque<Operation*>  m_undo_stack;
		std::vector<CL_String>  m_music;
        CL_AcceleratorTable     m_accels;	
		MapEditorState*         m_state;
};

}

#endif // MAPWINDOW_H
