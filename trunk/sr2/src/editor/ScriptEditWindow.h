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


#ifndef SCRIPTEDITWINDOW_H
#define SCRIPTEDITWINDOW_H
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/gui.h>
#include "ScriptElement.h"

namespace StoneRing { 

class ScriptEditWindow : public clan::Window {
public:
	ScriptEditWindow(clan::GUIManager* owner, const clan::GUITopLevelDescription &desc);
	virtual ~ScriptEditWindow();
	void SetIsCondition(bool is_condition) { m_is_condition = is_condition; }
	void SetScript(ScriptElement * pElement);
	std::string GetName()const;
	std::string GetScriptText()const;
	ScriptElement * CreateScript()const;
	virtual clan::Size get_preferred_size() const { return clan::Size(800,750); }
private:
	bool on_close();
	void on_save();
	void on_parse();
	clan::LineEdit*    m_script_name;
	clan::TextEdit*	m_script_text;
	clan::TextEdit*    m_script_errors;
	clan::PushButton*  m_build_button;
	clan::PushButton*  m_save_button;
	bool 			m_is_condition;
};

}

#endif // SCRIPTEDITWINDOW_H
