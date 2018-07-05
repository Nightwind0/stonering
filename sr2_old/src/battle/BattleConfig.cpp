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

#include "BattleConfig.h"
#include "IApplication.h"

using StoneRing::BattleConfig;
using namespace Steel;


BattleConfig::BattleConfig():m_setupScript(NULL),m_teardownScript(NULL),m_lostScript(NULL),m_wonScript(NULL)
{
}

BattleConfig::~BattleConfig()
{

}


void BattleConfig::Load(const std::string& filename)
{
#ifndef NDEBUG
	std::cout << "Loading battle config..." << std::endl;
#endif
    clan::IODevice file = IApplication::GetInstance()->OpenResource(filename);
    clan::DomDocument document;
    document.load(file);
    
    clan::DomNode child = document.get_first_child();
    clan::DomElement childElement = child.to_element();
    
    for(;!childElement.is_null(); childElement = childElement.get_next_sibling().to_element())
    {
	clan::DomNode grandchild = childElement.get_first_child();
	AstScript ** ppScript = NULL;
	if(childElement.get_node_name() == "setup")
	{
	    ppScript = &m_setupScript;
	}
	else if(childElement.get_node_name() == "teardown")
	{
	    ppScript = &m_teardownScript;
	}
	else if(childElement.get_node_name() == "onWon")
	{
	    ppScript = &m_wonScript;
	}
	else if(childElement.get_node_name() == "onLost")
	{
	    ppScript = &m_lostScript;
	}
	else
	{
	    continue;
	}
	
	if(!grandchild.is_text() && !grandchild.is_cdata_section())
	    throw XMLException("Child of battle configuration element wasn't text.");
	
	std::string script_text;
	if(grandchild.is_cdata_section())
	    script_text = grandchild.to_cdata_section().get_node_value();
	else if(grandchild.is_text())
	    script_text = grandchild.to_text().get_node_value();
	else
	    std::cout << "What is Grandchild?" << std::endl;
	
	
	*ppScript = IApplication::GetInstance()->LoadScript(childElement.get_node_name(), script_text);
	 
    }
    
#ifndef NDEBUG
	std::cout << "Finished loading battle config." << std::endl;
#endif

}
// Run battle script
void BattleConfig::SetupForBattle()
{
     IApplication *pApp = IApplication::GetInstance();
     if(m_setupScript)
	pApp->RunScript ( m_setupScript );
}
void BattleConfig::OnTurn(ParameterList& params)
{
    
}
void BattleConfig::OnBattleLost(ParameterList& params)
{
    if(m_lostScript){
	IApplication *pApp = IApplication::GetInstance();
	pApp->RunScript(m_lostScript,params);
    }
}

void BattleConfig::OnBattleWon(ParameterList& params)
{
    if(m_wonScript){
	IApplication *pApp = IApplication::GetInstance();
	pApp->RunScript(m_wonScript,params);
    }
}

void BattleConfig::TeardownForBattle()
{
    if(m_teardownScript){
	IApplication *pApp = IApplication::GetInstance();
	pApp->RunScript(m_teardownScript);
    }
}
