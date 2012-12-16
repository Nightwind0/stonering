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
#include "IApplication.h"

using namespace Steel;

#ifdef SR2_EDITOR

namespace StoneRing { 

ScriptEditWindow::ScriptEditWindow(CL_GUIManager* parent,const CL_GUITopLevelDescription &desc):CL_Window(parent,desc) {
	m_script_name = new CL_LineEdit(this);	
	m_script_name->set_geometry(CL_Rect(12,24,200,44));
	m_script_text = new CL_TextEdit(this);
	m_script_text->set_geometry(CL_Rect(CL_Point(12,50),CL_Size(750,400)));
	m_script_errors = new CL_TextEdit(this);
	m_script_errors->set_geometry(CL_Rect(CL_Point(12,460),CL_Size(750,100)));
	m_script_errors->set_read_only(true);
	m_script_errors->set_cursor_drawing_enabled(false);
	m_build_button = new CL_PushButton(this);
	m_build_button->set_geometry(CL_Rect(CL_Point(12,570),CL_Size(128,32)));
	m_build_button->set_text("Parse");
	m_build_button->func_clicked().set(this,&ScriptEditWindow::on_parse);
	m_save_button = new CL_PushButton(this);
	m_save_button->set_geometry(CL_Rect(CL_Point(750-128,570),CL_Size(128,32)));
	m_save_button->set_text("Save");
	m_save_button->func_clicked().set(this,&ScriptEditWindow::on_save);
	func_close().set(this,&ScriptEditWindow::on_close);
	
	m_is_condition = false;
}

ScriptEditWindow::~ScriptEditWindow() {
	delete m_script_name;
	delete m_script_text;
	delete m_script_errors;
	delete m_build_button;
	delete m_save_button;
}

ScriptElement* ScriptEditWindow::CreateScript() const
{
	ScriptElement * script = new ScriptElement(m_is_condition);
	script->SetId(m_script_name->get_text());
	script->SetScript(m_script_text->get_text());
	return script;
}

void ScriptEditWindow::SetScript( ScriptElement* pElement )
{
	m_script_name->set_text(pElement->GetScriptId());
	m_script_text->set_text(pElement->GetScriptText());
}



void ScriptEditWindow::on_parse()
{
	m_script_errors->set_text("");
	try {
		std::auto_ptr<AstScript> ptr(IApplication::GetInstance()->LoadScript(m_script_name->get_text(),m_script_text->get_text()));
	}catch(SteelException ex){
		m_script_errors->set_text(ex.getMessage());
	}
}

void ScriptEditWindow::on_save()
{
	exit_with_code(0);
}


bool ScriptEditWindow::on_close()
{
	exit_with_code(1);
	return true;
}

std::string ScriptEditWindow::GetName() const
{
	return m_script_name->get_text();
}

std::string ScriptEditWindow::GetScriptText() const
{
	return m_script_text->get_text();
}



}

#endif