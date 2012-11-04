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


#ifndef TILEEDITOR_H
#define TILEEDITOR_H

#include <ClanLib/gui.h>
#include <list>
#include "Level.h"
#include "DebugTileVisitors.h"


namespace StoneRing { 

class TileEditorWindow : public CL_Window
{
public:
	TileEditorWindow(CL_GUIComponent* owner, const CL_GUITopLevelDescription &desc);   
    virtual ~TileEditorWindow();
	class TileSelectGrid : public CL_GUIComponent {
	public:
		TileSelectGrid(CL_GUIComponent* parent);
		virtual ~TileSelectGrid();
		void set_edit_window(TileEditorWindow* pWindow);
		void set_tiles(const std::list<Tile*>& tiles);
		void free_tiles();
		void delete_selected();
		void move_up();
		void move_down();
		Tile* get_selected_tile() const;
		void get_tiles(std::list<Tile*>& tiles)const;
	private:
		void on_render(CL_GraphicContext& gc, const CL_Rect& rect);	
		bool on_click(const CL_InputEvent& event);
		TileMonsterRegionDrawer m_region_drawer;
		TileHotDrawer m_hot_drawer;
		TileSideBlockDrawer m_block_drawer;
		TileZOrderDrawer m_zorder_drawer;
		int m_selected_num;
		TileEditorWindow* m_pWindow;
		std::vector<Tile*> m_tiles;
	};
	void set_tiles(const std::list<Tile*>& tiles);
	void add_monster_region(int id);
	void selection_changed();
	void populate_monster_region_list();
	std::list<Tile*> get_tiles()const;
private:
	bool on_close();
	void on_save();
	void sync_from_selected();
	void on_block_north_changed();
	void on_block_west_changed();
	void on_block_east_changed();
	void on_block_south_changed();
	void on_hot_changed();
	void on_water_changed();
	void on_floater_changed();
	void on_edit_condition();
	void on_edit_script();
	void on_delete_tile();
	void on_move_up();
	void on_move_down();
	void on_zorder_change();
	
	void on_select_monster_region(int selection);
	

	TileSelectGrid m_grid;
	CL_PushButton* m_save;
	CL_PushButton* m_delete_selected;
	CL_PushButton* m_edit_condition;
	CL_PushButton* m_edit_script;
	CL_PushButton* m_move_up;
	CL_PushButton* m_move_down;
	CL_CheckBox*   m_block_north;
	CL_CheckBox*   m_block_west;
	CL_CheckBox*   m_block_south;
	CL_CheckBox*   m_block_east;
	CL_CheckBox*   m_hot;
	CL_CheckBox*   m_water;
	CL_CheckBox*   m_floater;
	CL_ComboBox*   m_monster_region;
	CL_Label*      m_zorder_label;
	CL_Spin*       m_zorder;
	std::list<int> m_monster_regions;
};


}
#endif // TILEEDITOR_H
