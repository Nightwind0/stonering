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

#ifndef UTILITY_SCRIPTS_H
#define UTILITY_SCRIPTS_H

#include "sr_defines.h"
#include <map>

namespace Steel { 
	class AstScript;
}

using Steel::AstScript;

namespace StoneRing {
    class UtilityScripts
    {
    public:
	UtilityScripts();
	~UtilityScripts();
	
	void Load(const std::string& filename);
	bool ScriptExists(const std::string& filename)const;
	AstScript* GetScript(const std::string& filename)const;

    private:
	std::map<std::string,AstScript*> m_scripts;
    };
}
#endif // BATTLECONFIG_H
