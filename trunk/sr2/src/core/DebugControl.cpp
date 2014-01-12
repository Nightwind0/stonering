/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "DebugControl.h"

using StoneRing::DebugControl;

bool DebugControl::m_infinite_bp = false;
bool DebugControl::m_infinite_sp = false;
bool DebugControl::m_infinite_gold = false;
bool DebugControl::m_all_skills = false;

DebugControl::DebugControl() {

}

DebugControl::~DebugControl() {

}

void DebugControl::EnableInfiniteBP (bool enabled){
	m_infinite_bp = enabled;
}
void DebugControl::EnableInfiniteSP (bool enabled){
	m_infinite_sp = enabled;
}
void DebugControl::EnableInfiniteGold (bool enabled){
	m_infinite_gold = enabled;
}
void DebugControl::EnableAllSkills( bool enabled ) {
	m_all_skills = enabled;
}


bool DebugControl::InfiniteBP() {
#ifdef NDEBUG
	return false;
#else
	return m_infinite_bp;
#endif
}

bool DebugControl::InfiniteGold() {
#ifdef NDEBUG
	return false;
#else
	return m_infinite_gold;
#endif
}

bool DebugControl::InfiniteSP() {
#ifdef NDEBUG
	return false;
#else
	return m_infinite_sp;
#endif
}

bool DebugControl::AllSkills() {
#ifdef NDEBUG
	return false;
#else
	return m_all_skills;
#endif
}



