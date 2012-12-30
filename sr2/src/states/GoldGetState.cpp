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


#include "GoldGetState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "SoundManager.h"
#include <sstream>

#define TIME_PER_ITEM 600

namespace StoneRing { 

GoldGetState::GoldGetState() {

}

GoldGetState::~GoldGetState() {

}


void GoldGetState::Start() {
    m_rect = GraphicsManager::GetRect(GraphicsManager::GOLD_GET,"main");
    m_gold_font = GraphicsManager::GetFont(GraphicsManager::GOLD_GET,"gold");
	m_icon_offset = GraphicsManager::GetPoint(GraphicsManager::GOLD_GET,"icon_offset");
	m_text_offset = GraphicsManager::GetPoint(GraphicsManager::GOLD_GET,"text_offset");	
	m_sound_timer.func_expired().set(this,&GoldGetState::on_sound_timer);
	m_sound_timer.start(TIME_PER_ITEM,false);
	m_done = false;
	m_done_display = false;
	m_icon = GraphicsManager::GetIcon("gold");
	SoundManager::PlayEffect(SoundManager::EFFECT_GOLD);	
	m_start_time = CL_System::get_time();	
}

void GoldGetState::SetGold(int gold) {
	m_gold = gold;
	std::ostringstream os;
	os << m_gold << ' ' << IApplication::GetInstance()->GetCurrencyName() <<" Recieved.";
	m_text = os.str();
}


void GoldGetState::Draw( const CL_Rect& screenRect, CL_GraphicContext& GC ) {
	MenuBox::Draw(GC,m_rect);
	uint time_passed = CL_System::get_time() - m_start_time;
	float percentage = (float)time_passed / (float)TIME_PER_ITEM;
	m_icon.set_alpha(percentage);
	m_icon.draw(GC,m_icon_offset.x + m_rect.get_top_left().x,m_icon_offset.y + m_rect.get_top_left().y);
	m_icon.set_alpha(1.0f);
	m_gold_font.set_alpha(percentage);
	m_gold_font.draw_text(GC,m_text_offset + m_rect.get_top_left(),m_text, Font::TOP_LEFT);	
	m_gold_font.set_alpha(1.0f);
}

bool GoldGetState::DisableMappableObjects() const {
	return true;
}

void GoldGetState::HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos ) {
	StoneRing::State::HandleAxisMove( axis, dir, pos );
}


void GoldGetState::HandleButtonUp( const IApplication::Button& button ) {
	if(m_done_display)
		m_done = true;
}

void GoldGetState::HandleButtonDown( const IApplication::Button& button ) {
	StoneRing::State::HandleButtonDown( button );
}


bool GoldGetState::IsDone() const {
	return m_done;
}


bool GoldGetState::LastToDraw() const {
	return false;
}


void GoldGetState::MappableObjectMoveHook() {
}

void GoldGetState::SteelInit( SteelInterpreter* ) {
}

void GoldGetState::SteelCleanup( SteelInterpreter* ) {
}

void GoldGetState::Finish() {
	m_sound_timer.stop();
}



void GoldGetState::on_sound_timer() {
	m_sound_timer.stop();
	m_done_display = true;
}



}