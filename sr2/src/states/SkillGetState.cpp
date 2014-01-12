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


#include "SkillGetState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "SoundManager.h"
#include <sstream>

#define TIME_PER_ITEM 600

namespace StoneRing { 

SkillGetState::SkillGetState() {

}

SkillGetState::~SkillGetState() {

}



void SkillGetState::SetSkill( Skill* pSkill ) {
	m_skill = pSkill;
	std::ostringstream os;
	os << "Learned " << pSkill->GetName() << '!';
	m_text = os.str();
}

GraphicsManager::Overlay SkillGetState::get_overlay() const {
	return GraphicsManager::SKILL_GET;
}

SoundManager::Effect SkillGetState::get_sound_effect() const {
	return SoundManager::EFFECT_LEARNED_SKILL;
}

clan::Image SkillGetState::get_icon() const{
	assert(m_skill);
	return m_skill->GetIcon();
}

std::string SkillGetState::get_text() const {
	return m_text;
}

void SkillGetState::load() {
}


}