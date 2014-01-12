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

class TileEditorWindow : public clan::Window
{
public:
	TileEditorWindow(clan::GUIComponent* owner, const clan::GUITopLevelDescription &desc);   
    virtual ~TileEditorWindow();
	class TileSelectGrid : public clan::GUIComponent {
	public:
		TileSelectGrid(clan::GUIComponent* parent);
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
		void on_render(clan::Canvas& gc, const clan::Rect& rect);	
		bool on_click(const clan::InputEvent& event);
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
	clan::PushButton* m_save;
	clan::PushButton* m_delete_selected;
	clan::PushButton* m_edit_condition;
	clan::PushButton* m_edit_script;
	clan::PushButton* m_move_up;
	clan::PushButton* m_move_down;
	clan::CheckBox*   m_block_north;
	clan::CheckBox*   m_block_west;
	clan::CheckBox*   m_block_south;
	clan::CheckBox*   m_block_east;
	clan::CheckBox*   m_hot;
	clan::CheckBox*   m_water;
	clan::CheckBox*   m_floater;
	clan::ComboBox*   m_monster_region;
	clan::Label*      m_zorder_label;
	clan::Spin*       m_zorder;
	std::list<int> m_monster_regions;
};


}
#endif // TILEEDITOR_H
