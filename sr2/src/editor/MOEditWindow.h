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
#include "MOView.h"

namespace StoneRing { 

class MOEditWindow : public CL_Window
{
public:
    MOEditWindow(CL_GUIManager* owner, const CL_GUITopLevelDescription &desc);
    virtual ~MOEditWindow();
    void SetName(const char* pName);
    void SetPoint(const CL_Point& i_pt);
private:
    bool on_window_close();
    void populate_sprite_list();
    void on_list_selection(CL_ListViewSelection selection);
    std::string m_name;
    std::string m_sprite_ref;
    CL_ListView *        m_sprite_list;
    MOView *             m_sprite_view;
    //CL_ComboBox*         m_type_list;
    CL_Spin *            m_width_spin;
    CL_Spin *            m_height_spin;
    CL_ComboBox*         m_movement;
    CL_LineEdit*         m_name_field;
    CL_Label*            m_name_label;
    // TODO: Events, opens up event editor
    // TODO: Condition script, goes to script editor
    CL_CheckBox*         m_solid;
    CL_Point             m_point;
};


}
#endif // MOEDITWINDOW_H
