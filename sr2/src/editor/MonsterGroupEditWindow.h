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


#ifndef MONSTERGROUPEDITWINDOW_H
#define MONSTERGROUPEDITWINDOW_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/gui.h>
#include "MonsterGroup.h"


namespace StoneRing { 

class MonsterGroupEditWindow : public clan::Window
{
public:
    MonsterGroupEditWindow(clan::GUIManager* manager, const clan::GUITopLevelDescription& desc);
    virtual ~MonsterGroupEditWindow();
	void CreateGroup();
	void SetGroup(MonsterGroup* pGroup);
	MonsterGroup* GetGroup() const{ return m_group; }
public:
	class BattleWindow : public clan::GUIComponent {
	public:
		BattleWindow ( clan::GUIComponent* parent ) : clan::GUIComponent(parent) {
			func_render().set(this,&BattleWindow::on_render);
			m_selected_item = -1;
		}
		void SetMonsterGroup(MonsterGroup* group);
		void SetScale(float scale);
		void SetSelectedItem(int item){ m_selected_item = item; request_repaint(); }
	private:
		void on_render(clan::Canvas &gc, const clan::Rect &clip_rect); 
		int m_cols;
		int m_rows;
		float m_scale;
		int m_selected_item;
		MonsterGroup* m_group;
	}*m_battle_window;
	void on_close();
	void populate_monster_list();
	void populate_monster_ref_list();
	void sync_move_buttons();
	void on_add_monsters();
	void on_delete_monsters();
	void on_done();
	void sync_from_group();
	void on_monster_count_changed();
	void on_group_grid_changed();
	void on_monsters_selected(int);
	void on_move_down();
	void on_move_up();
	void on_move_left();
	void on_move_right();
	void on_monster_list_selection(clan::ListViewSelection);
	bool can_place(const clan::Rect& rect, MonsterRef* ignore=NULL);
	
	
	clan::ListView* m_monster_ref_list;
	clan::ComboBox* m_monsters;
	clan::Label* m_monster_count_label;
	clan::Label* m_col_count_label;
	clan::Label* m_row_count_label;
	clan::Spin* m_monster_count;
	clan::Spin* m_col_count;
	clan::Spin* m_row_count;
	clan::Spin* m_group_col_count;
	clan::Spin* m_group_row_count;
	clan::PushButton* m_add_monsters;
	clan::PushButton* m_delete_monsters;
	clan::PushButton* m_move_up;
	clan::PushButton* m_move_down;
	clan::PushButton* m_move_left;
	clan::PushButton* m_move_right;
	clan::PushButton* m_done_button;	
	MonsterGroup* m_group;
};


}
#endif // MONSTERGROUPEDITWINDOW_H
