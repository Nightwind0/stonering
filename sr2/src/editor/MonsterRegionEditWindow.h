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


#ifndef MONSTERREGIONEDITWINDOW_H
#define MONSTERREGIONEDITWINDOW_H

#include <ClanLib/core.h>
#include <ClanLib/gui.h>

namespace StoneRing {

class MonsterRegion;
	
class MonsterRegionEditWindow : public clan::Window
{

public:
    MonsterRegionEditWindow(clan::GUIComponent* parent, const clan::GUITopLevelDescription& desc);
    virtual ~MonsterRegionEditWindow();
	void CreateRegion();
	void SetRegion(MonsterRegion* pRegion);
	MonsterRegion* GetRegion()const{ return m_region; }
private:
	void populate_group_list();
	void populate_backdrop_list();
	void on_add_group();
	void on_delete_group();
	void on_edit_group();
	void on_save();
	void on_group_weight_changed();
	void on_select_group(clan::ListViewSelection);
	void sync_to_region();
	void sync_from_region();
	bool on_close();
	clan::ComboBox*   m_backdrops;
	clan::ListView*   m_monster_groups;
	clan::PushButton* m_add_group;
	clan::PushButton* m_delete_group;
	clan::PushButton* m_edit_group;
	clan::Spin*       m_encounter_rate;
	clan::PushButton* m_save;
	clan::Spin*       m_group_weight;
	
	MonsterRegion* m_region;
};

}

#endif // MONSTERREGIONEDITWINDOW_H
