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


#include "JoystickConfig.h"

namespace StoneRing { 

JoystickConfig::JoystickConfig():m_setup(false) {

}

JoystickConfig::~JoystickConfig() {

}

void JoystickConfig::Reset()  {
	m_setup = false;
}


int JoystickConfig::GetAxis( IApplication::Axis axis ) const {
	std::map<IApplication::Axis,int>::const_iterator it = m_axis_map.find(axis);
	assert(it != m_axis_map.end());
	return it->second;
}

double JoystickConfig::GetValueFor( IApplication::AxisDirection dir ) const {
	std::map<IApplication::AxisDirection,double>::const_iterator it = m_dir_map.find(dir);
	assert(it != m_dir_map.end());
	return it->second;
}

IApplication::Button JoystickConfig::GetButtonForId( int id ) const {
	std::map<int,IApplication::Button>::const_iterator it = m_button_map.find(id);
	if(it == m_button_map.end())
		return IApplication::BUTTON_INVALID;
	return it->second;	
}

void JoystickConfig::MapAxis( IApplication::Axis axis, int id ) {
	m_axis_map[axis] = id;
}

void JoystickConfig::MapAxisValue( IApplication::AxisDirection dir, double value ) {
	m_dir_map[dir] = value;
}

void JoystickConfig::MapButton( IApplication::Button button, int id ) {
	m_button_map[id] = button;
}

void JoystickConfig::Write( std::ostream& out ) {
	int size = m_button_map.size();
	out.write((char*)&size,sizeof(int));
	for(std::map<int,IApplication::Button>::const_iterator it = m_button_map.begin();
		it != m_button_map.end(); it++){
		out.write((char*)&(it->first),sizeof(int));		
		out.write((char*)&(it->second),sizeof(IApplication::Button));
	}
	size = m_axis_map.size();
	out.write((char*)&size,sizeof(int));
	for(std::map<IApplication::Axis,int>::const_iterator it = m_axis_map.begin();
		it != m_axis_map.end(); it++){
		out.write((char*)&(it->first),sizeof(IApplication::Button));
		out.write((char*)&(it->second),sizeof(int));
	}
	size = m_dir_map.size();
	out.write((char*)&size,sizeof(int));
	for(std::map<IApplication::AxisDirection,double>::const_iterator it = m_dir_map.begin();
		it != m_dir_map.end(); it++){
		out.write((char*)&(it->first),sizeof(IApplication::AxisDirection));
		out.write((char*)&(it->second),sizeof(double));
	}
}

void StoneRing::JoystickConfig::Read( std::istream& in ) {
	int size = 0;
	in.read((char*)&size,sizeof(int));
	for(int i = 0; i<size; i++){
		IApplication::Button button; 
		int id;
		in.read((char*)&id,sizeof(id));		
		in.read((char*)&button,sizeof(button));
		m_button_map[id] = button;
	}
	
	in.read((char*)&size,sizeof(int));
	for(int i = 0; i<size; i++){
		IApplication::Axis axis;
		int id;
		in.read((char*)&axis,sizeof(axis));
		in.read((char*)&id,sizeof(id));
		m_axis_map[axis] = id;
	}
	
	in.read((char*)&size,sizeof(int));
	for(int i=0;i<size;i++){
		IApplication::AxisDirection dir;
		double val;
		in.read((char*)&dir,sizeof(dir));
		in.read((char*)&val,sizeof(val));
		m_dir_map[dir] = val;
	}
	FinishedSetup();
}

void JoystickConfig::FinishedSetup() {
	m_setup = true;
}


bool JoystickConfig::IsSetup() const {
	return m_setup;
}



}
