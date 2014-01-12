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

#ifndef DEBUGCONTROL_H
#define DEBUGCONTROL_H

namespace StoneRing {

class DebugControl
{
public:
	static void EnableInfiniteBP (bool enabled);
	static void EnableInfiniteSP (bool enabled);
	static void EnableInfiniteGold (bool enabled);
	static void EnableAllSkills (bool enabled);
	static bool InfiniteBP();
	static bool InfiniteSP();
	static bool InfiniteGold();
	static bool AllSkills();
private:
    DebugControl();
    ~DebugControl();
	static bool m_infinite_bp;
	static bool m_infinite_sp;
	static bool m_infinite_gold;
	static bool m_all_skills;
};

}

#endif // DEBUGCONTROL_H
