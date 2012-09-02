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


#ifndef EVENTEDITWINDOW_H
#define EVENTEDITWINDOW_H
#include <ClanLib/core.h>
#include <ClanLib/gui.h>

namespace StoneRing  { 
	
class Event;

class EventEditWindow : public CL_Window
{
public:
    EventEditWindow(CL_GUIManager* manager, const CL_GUITopLevelDescription &desc);
    virtual ~EventEditWindow();
	void SetEvent(Event* pEvent);
	void CreateEvent();
	Event* GetEvent() const;
private:
	void populate_trigger_types();
	void sync_to_event();
	void sync_from_event();
	bool on_close();
	void on_edit_script();
	void on_edit_condition();
	void on_delete_script();
	void on_delete_condition();
	void sync_button_names();
	void on_save();
	
	CL_PushButton * m_edit_condition; // edit or create, change text based on mode
	CL_PushButton * m_delete_condition; // disabled if there is no current condition
	CL_PushButton * m_edit_script;
	CL_PushButton * m_delete_script;
	CL_PushButton * m_save_button;
	CL_LineEdit   * m_event_name;
	CL_CheckBox   * m_repeatable;
	CL_CheckBox   * m_remember;
	CL_ComboBox   * m_trigger_type;
	
	Event * 		m_pEvent;
	bool            m_bEdit;
};

}
#endif // EVENTEDITWINDOW_H
