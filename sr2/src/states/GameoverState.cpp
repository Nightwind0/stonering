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

#include <ClanLib/core.h>
#include "GameoverState.h"
#include "GraphicsManager.h"
#include "SoundManager.h"

namespace StoneRing { 

GameoverState::GameoverState():m_done(false) {

}

GameoverState::~GameoverState() {

}

void GameoverState::Init()
{
}


bool GameoverState::DisableMappableObjects() const
{
	return true;
}

void GameoverState::Draw( const clan::Rect& screenRect, clan::Canvas& GC )
{
	float percentage = std::min(1.0, float(clan::System::get_time() - m_start_time) / 3000.0);
	m_background.set_alpha(percentage);
	m_background.draw(GC,0,0);
}

void GameoverState::HandleButtonUp( const StoneRing::IApplication::Button& button )
{
	if(clan::System::get_time() - m_start_time > 3000)
		m_done = true;
}

void GameoverState::HandleButtonDown( const StoneRing::IApplication::Button& button )
{
	StoneRing::State::HandleButtonDown( button );
}

bool GameoverState::LastToDraw() const
{
	return true;
}

void GameoverState::Update()
{

}

void GameoverState::Start()
{
	m_background = GraphicsManager::GetOverlay(GraphicsManager::GAMEOVER);
	m_start_time = clan::System::get_time();
	
	m_done = false;
	// TODO: Play game over music
}


bool GameoverState::IsDone() const
{
	return m_done;
}

void GameoverState::Finish()
{

}


}