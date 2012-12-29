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


#include "BannerState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"

namespace StoneRing { 
    
BannerState::BannerState()
{

}

BannerState::~BannerState()
{

}

bool BannerState::DisableMappableObjects() const
{
    return false;
}

void BannerState::Init ( const std::string& text, int time )
{
    m_text = text;
    m_time = time;
    m_banner_rect = GraphicsManager::GetRect(GraphicsManager::BANNER,"banner");
    m_font = GraphicsManager::GetFont(GraphicsManager::BANNER,"font");
    m_text_inset = GraphicsManager::GetPoint(GraphicsManager::BANNER,"inset");
}

void BannerState::Start()
{
    m_start_time = CL_System::get_time();
    m_done = false;
}

bool BannerState::IsDone() const
{
    return m_done || (m_time> 0 && CL_System::get_time() >= (m_start_time + m_time)); 
}

bool BannerState::AcceptInput() const {
	return m_time <= 0;
}


void BannerState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    MenuBox::Draw(GC,m_banner_rect);
    m_font.draw_text(GC,m_text_inset,m_text, Font::TOP_LEFT);
}

void BannerState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
    StoneRing::State::HandleAxisMove ( axis, dir, pos );
}


void BannerState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
    m_done = true;
}

void BannerState::MappableObjectMoveHook()
{

}


bool BannerState::LastToDraw() const
{
    return false;
}

void BannerState::Finish()
{

}

void BannerState::BringDown() {
	m_done = true;
}







}