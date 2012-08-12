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


#include "ScriptEditWindow.h"

namespace StoneRing { 

ScriptEditWindow::ScriptEditWindow(CL_GUIManager* parent,const CL_GUITopLevelDescription &desc):CL_Window(parent,desc) {
	m_script_name = new CL_LineEdit(this);	
	m_script_name->set_geometry(CL_Rect(12,22,200,42));
	m_script_text = new CL_TextEdit(this);
	m_script_text->set_geometry(CL_Rect(12,46,400,400));
	func_close().set(this,&ScriptEditWindow::on_close);
/*	CL_TextEdit*	m_script_text;
	CL_TextEdit*    m_script_errors;
	CL_PushButton*  m_build_button;
	CL_PushButton*  m_save_button;
	*/
}

ScriptEditWindow::~ScriptEditWindow() {
	delete m_script_name;
	delete m_script_text;
	/*delete m_script_errors;
	delete m_build_button;
	delete m_save_button;*/
}

bool ScriptEditWindow::on_close()
{
	exit_with_code(1);
	return true;
}


}