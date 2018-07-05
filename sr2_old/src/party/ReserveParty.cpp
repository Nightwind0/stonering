/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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


#include "ReserveParty.h"

namespace StoneRing { 

ReserveParty::ReserveParty() {

}

ReserveParty::~ReserveParty() {

}

ICharacter* ReserveParty::GetCharacter( uint index ) const {
	assert(index < m_party.size());
	return m_party[index];
}


uint ReserveParty::GetCharacterCount() const {
	return m_party.size();
}


void ReserveParty::AddCharacter( Character* pChar ) {
	m_party.push_back(pChar);
}

Character* ReserveParty::RemoveCharacter( const std::string& name ) {
	for(int i=0;i<m_party.size();i++){
		if(m_party[i]->GetName() == name){
			Character * c = m_party[i];
			m_party.erase(m_party.begin()+i);
			return c;
		}
	}
	return NULL;
}

}