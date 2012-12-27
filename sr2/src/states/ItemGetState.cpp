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


#include "ItemGetState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "SoundManager.h"
#include <sstream>

#define TIME_PER_ITEM 600

namespace StoneRing { 

ItemGetState::ItemGetState() {

}

ItemGetState::~ItemGetState() {

}


void ItemGetState::Start() {
    m_rect = GraphicsManager::GetRect(GraphicsManager::ITEM_GET,"main");
    m_item_font = GraphicsManager::GetFont(GraphicsManager::ITEM_GET,"item");
	m_offset = GraphicsManager::GetPoint(GraphicsManager::ITEM_GET,"offset");
	
	m_sound_timer.func_expired().set(this,&ItemGetState::on_sound_timer);
	m_sound_timer.start(TIME_PER_ITEM,true);
	m_item_cursor = 0;
	m_sound_count = 0;
	m_done = false;
	m_done_display = false;
	Menu::Init();
	m_start_time = CL_System::get_time();
}

void ItemGetState::SetItems( const std::vector< Item* >& items, const std::vector<uint>& counts ) {
	m_items = items;
	m_counts = counts;
}


void ItemGetState::Draw( const CL_Rect& screenRect, CL_GraphicContext& GC ) {
	MenuBox::Draw(GC,m_rect);
	
	uint time_passed = CL_System::get_time() - m_start_time;
	m_item_cursor = min((size_t)time_passed / TIME_PER_ITEM, m_items.size());
	Menu::Draw(GC);
}

bool ItemGetState::DisableMappableObjects() const {
	return true;
}

void ItemGetState::HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos ) {
	StoneRing::State::HandleAxisMove( axis, dir, pos );
}


void ItemGetState::HandleButtonUp( const IApplication::Button& button ) {
	if(m_done_display)
		m_done = true;
}

void ItemGetState::HandleButtonDown( const IApplication::Button& button ) {
	StoneRing::State::HandleButtonDown( button );
}


bool ItemGetState::IsDone() const {
	return m_done;
}


bool ItemGetState::LastToDraw() const {
	return false;
}


void ItemGetState::MappableObjectMoveHook() {
}

void ItemGetState::SteelInit( SteelInterpreter* ) {
}

void ItemGetState::SteelCleanup( SteelInterpreter* ) {
}

void ItemGetState::Finish() {
	m_sound_timer.stop();
}

int ItemGetState::get_option_count() {
	return min(m_item_cursor+1, (uint)m_items.size());
}

CL_Rectf ItemGetState::get_rect() {
	return CL_Rectf(m_rect.left+m_offset.x, m_rect.top+12, m_rect.right-m_offset.x, m_rect.bottom-12);
}

int ItemGetState::height_for_option( CL_GraphicContext& gc ) {
	return m_offset.y;
}

void ItemGetState::process_choice( int selection ) {

}

void ItemGetState::draw_option( int option, bool selected, float x, float y, CL_GraphicContext& gc ) {
	float percentage = 1.0f;
	if(option == m_item_cursor) {
		percentage = float((CL_System::get_time() - m_start_time) % TIME_PER_ITEM) / float(TIME_PER_ITEM);
	}
	CL_Image image = m_items[option]->GetIcon();
	image.set_scale(percentage,percentage);
	image.set_alpha(percentage);
	image.draw(gc, x, y);
	image.set_scale(1.0f,1.0f);
	image.set_alpha(1.0f);
	
	std::ostringstream os;
	os << m_items[option]->GetName();
	if(m_counts[option] > 1){
		os << " x" << m_counts[option];
	}
	
	m_item_font.set_alpha(percentage);
	m_item_font.draw_text(gc,x+40,y,os.str(), Font::TOP_LEFT);
	m_item_font.set_alpha(1.0f);
}


void ItemGetState::on_sound_timer() {
	m_sound_count++;
	SoundManager::PlayEffect(SoundManager::EFFECT_REWARD);
	if(m_sound_count == m_items.size()){
		m_sound_timer.stop();
		m_done_display = true;
	}
}



}