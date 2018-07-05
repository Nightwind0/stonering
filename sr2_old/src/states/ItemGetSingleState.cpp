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


#include "ItemGetSingleState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "SoundManager.h"
#include <sstream>

#define TIME_PER_ITEM 600

namespace StoneRing { 

ItemGetSingleState::ItemGetSingleState() {

}

ItemGetSingleState::~ItemGetSingleState() {

}



void ItemGetSingleState::SetItem(Item * pItem, int count) {
	m_pItem = pItem;
	m_count = count;
	std::ostringstream os;
	os << pItem->GetName();
	if(count > 1){
		os << " x" << count;
	}
	if(IsInverse())
			os << " Was Taken.";
		else
			os <<" Received.";
	m_text = os.str();
}

GraphicsManager::Overlay ItemGetSingleState::get_overlay() const {
	return GraphicsManager::GOLD_GET;
}

SoundManager::Effect ItemGetSingleState::get_sound_effect() const {
	return SoundManager::EFFECT_REWARD;
}

clan::Image ItemGetSingleState::get_icon() const{
	return m_pItem->GetIcon();
}

std::string ItemGetSingleState::get_text() const {
	return m_text;
}

void ItemGetSingleState::load() {
}


}