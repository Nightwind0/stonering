/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "UtilityScripts.h"
#include "IApplication.h"

using StoneRing::UtilityScripts;


UtilityScripts::UtilityScripts()
{
}

UtilityScripts::~UtilityScripts()
{

}


void UtilityScripts::Load(const std::string& filename)
{
    CL_File file(filename);
    CL_DomDocument document;
    document.load(file);
    
    CL_DomNode child = document.get_first_child();
    CL_DomElement childElement = child.to_element();
    
    for(;!childElement.is_null(); childElement = childElement.get_next_sibling().to_element())
    {
	CL_DomNode grandchild = childElement.get_first_child();
	std::string childname = childElement.get_node_name();
	std::string scriptname;
	if(childElement.has_attributes())
	{
	    CL_DomNamedNodeMap attributes = childElement.get_attributes();
	    scriptname = attributes.get_named_item("id").to_attr().get_value();
	}
	
	if(!grandchild.is_text() && !grandchild.is_cdata_section())
	    throw CL_Exception("Child of battle configuration element wasn't text.");
	
	std::string script_text;
	if(grandchild.is_cdata_section())
	    script_text = grandchild.to_cdata_section().get_node_value();
	else if(grandchild.is_text())
	    script_text = grandchild.to_text().get_node_value();
	else
	    std::cout << "What is Grandchild?" << std::endl;
	
	
	AstScript* script = IApplication::GetInstance()->LoadScript(scriptname, script_text);	
	m_scripts[scriptname] = script;
    }
       
}


bool UtilityScripts::ScriptExists(const std::string& filename)const
{
    return m_scripts.find(filename) != m_scripts.end();
}

AstScript* UtilityScripts::GetScript(const std::string& filename)const
{
    return m_scripts.find(filename)->second;
}

