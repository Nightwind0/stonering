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


#ifndef JOYSTICKCONFIG_H
#define JOYSTICKCONFIG_H

#include "IApplication.h"
#include <fstream>
#include <map>

namespace StoneRing { 

class JoystickConfig
{
public:
    JoystickConfig();
    virtual ~JoystickConfig();
	
	void Reset();
	bool IsSetup() const; 
	void FinishedSetup();
	
	int GetAxis(IApplication::Axis axis) const;
	double GetValueFor(IApplication::AxisDirection dir)const;
	IApplication::Button GetButtonForId(int id)const;
	
	void MapButton(IApplication::Button button, int id);
	void MapAxis(IApplication::Axis, int id);
	void MapAxisValue(IApplication::AxisDirection, double value);
	
	void Write(std::ostream& out);
	void Read(std::istream& in);
private:
	std::map<int,IApplication::Button> m_button_map;
	std::map<IApplication::AxisDirection, double> m_dir_map;
	std::map<IApplication::Axis, int> m_axis_map;
	bool m_setup;
};

};

#endif // JOYSTICKCONFIG_H
