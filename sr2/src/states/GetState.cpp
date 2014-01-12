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


#include "GetState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "SoundManager.h"
#include <sstream>

#define TIME_PER_ITEM 600

namespace StoneRing { 

GetState::GetState() {
	m_inverse = false;
}

GetState::~GetState() {

}


void GetState::Start() {
	load();
    m_rect = GraphicsManager::GetRect(get_overlay(),"main");
    m_font = GraphicsManager::GetFont(get_overlay(),"main");
	m_icon_offset = GraphicsManager::GetPoint(GraphicsManager::GOLD_GET,"icon_offset");
	m_text_offset = GraphicsManager::GetPoint(GraphicsManager::GOLD_GET,"text_offset");	
	m_sound_timer.func_expired().set(this,&GetState::on_sound_timer);
	m_sound_timer.start(TIME_PER_ITEM,false);
	m_done = false;
	m_done_display = false;
	SoundManager::PlayEffect(get_sound_effect());	
	m_start_time = clan::System::get_time();	
}

void GetState::SetInverse( bool inverse ) {
	m_inverse = inverse;
}

void GetState::Draw( const clan::Rect& screenRect, clan::Canvas& GC ) {
	MenuBox::Draw(GC,m_rect);
	uint time_passed = clan::System::get_time() - m_start_time;
	float percentage = (float)time_passed / (float)TIME_PER_ITEM;
	clan::Image icon = get_icon();
	icon.set_alpha(m_inverse?(1.0-percentage):percentage);
	icon.draw(GC,m_icon_offset.x + m_rect.get_top_left().x,m_icon_offset.y + m_rect.get_top_left().y);
	icon.set_alpha(1.0f);
	std::string text = get_text();
	m_font.set_alpha(percentage);
	m_font.draw_text(GC,m_text_offset + m_rect.get_top_left(), text, Font::TOP_LEFT);	
	m_font.set_alpha(1.0f);
}

bool GetState::DisableMappableObjects() const {
	return true;
}

void GetState::HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos ) {
	StoneRing::State::HandleAxisMove( axis, dir, pos );
}


void GetState::HandleButtonUp( const IApplication::Button& button ) {
	if(m_done_display)
		m_done = true;
}

void GetState::HandleButtonDown( const IApplication::Button& button ) {
	StoneRing::State::HandleButtonDown( button );
}


bool GetState::IsDone() const {
	return m_done;
}


bool GetState::LastToDraw() const {
	return false;
}


void GetState::MappableObjectMoveHook() {
}

void GetState::SteelInit( SteelInterpreter* ) {
}

void GetState::SteelCleanup( SteelInterpreter* ) {
}

void GetState::Finish() {
	m_sound_timer.stop();
}



void GetState::on_sound_timer() {
	m_sound_timer.stop();
	m_done_display = true;
}



}