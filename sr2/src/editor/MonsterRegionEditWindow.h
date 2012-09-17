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
	
class MonsterRegionEditWindow : public CL_Window
{

public:
    MonsterRegionEditWindow(CL_GUIComponent* parent, const CL_GUITopLevelDescription& desc);
    virtual ~MonsterRegionEditWindow();
	void CreateRegion();
	MonsterRegion* GetRegion()const{ return m_region; }
private:
	void populate_group_list();
	void populate_backdrop_list();
	void on_add_group();
	void on_delete_group();
	void on_edit_group();
	void on_save();
	CL_ComboBox*   m_backdrops;
	CL_ListView*   m_monster_groups;
	CL_PushButton* m_add_group;
	CL_PushButton* m_delete_group;
	CL_PushButton* m_edit_group;
	CL_Spin*       m_encounter_rate;
	CL_PushButton* m_save;
	
	MonsterRegion* m_region;
};

}

#endif // MONSTERREGIONEDITWINDOW_H
