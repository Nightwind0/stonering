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


#ifndef MOEDITWINDOW_H
#define MOEDITWINDOW_H

#include <ClanLib/gui.h>
#include "MappableObject.h"
#include "MOView.h"
#include "MapWindow.h"

namespace StoneRing { 

class EditorMappableObject;
class MapEditorState;

class MOEditWindow : public clan::Window
{
public:
    MOEditWindow(clan::GUIManager* owner, const clan::GUITopLevelDescription &desc);
    virtual ~MOEditWindow();
    void SetMapWindow(MapWindow * window);
    void SetMappableObject(MappableObject* pObject);
    void SetCreate();
    void SetName(const char* pName);
    void SetPoint(const clan::Point& i_pt);
private:
    bool on_window_close();
    void populate_sprite_list();
    void populate_move_speed_combo();
    void populate_movement_combo();
    void populate_event_list();
    void populate_facing_combo();
    void on_list_selection(clan::ListViewSelection selection);
    void on_save();
    void on_cancel();
	void on_edit_event();
	void on_add_event();
	void on_condition_edit();
    void sync_to_mo();
    void sync_from_mo();
    
    std::string m_name;
    std::string m_sprite_ref;
    clan::ListView *        m_sprite_list;
    MOView *             m_sprite_view;
    std::string          m_sprite_name;
    //clan::ComboBox*         m_type_list;
    clan::Spin *            m_width_spin;
    clan::Spin *            m_height_spin;
    clan::ComboBox*         m_movement;
    clan::ComboBox*         m_move_speed;
    clan::ComboBox*         m_face_dir;
    clan::LineEdit*         m_name_field;
    clan::Label*            m_name_label;
    clan::PushButton*       m_condition_button;
    clan::ComboBox*         m_event_list;
    clan::PushButton*       m_add_event_button;
    clan::PushButton*       m_open_event_button;
    clan::PushButton*       m_save_button;
    clan::ListViewItem      m_no_sprite_item;
    // TODO: Events, opens up event editor
    // TODO: Condition script, goes to script editor
    clan::CheckBox*          m_solid;
    clan::CheckBox*          m_flying;
    clan::Point              m_point;
    EditorMappableObject* m_pMo;
    MappableObject*       m_pOriginalObject;
    bool                  m_edit_mode;
    MapWindow*            m_map_window;
    
};


}
#endif // MOEDITWINDOW_H
