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


#include "StartupState.h"
#include "GraphicsManager.h"
#include "DynamicMenuState.h"
#include "SoundManager.h"

namespace StoneRing { 

StartupState::StartupState()
{

}

StartupState::~StartupState()
{

}

bool StartupState::DisableMappableObjects() const
{
    return false;
}

bool StartupState::LastToDraw() const
{
    return false;
}

bool StartupState::IsDone() const
{
    return m_bDone;
}

void StartupState::Start()
{
    m_bDone = false;
    Menu::Init();
    m_overlay = GraphicsManager::GetOverlay(GraphicsManager::STARTUP);
    m_menu_rect = GraphicsManager::GetRect(GraphicsManager::STARTUP,"menu");
    m_option_font = GraphicsManager::GetFont(GraphicsManager::STARTUP,"option");
    m_selection_font = GraphicsManager::GetFont(GraphicsManager::STARTUP,"selection");
}

void StartupState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
	if(button == IApplication::BUTTON_CANCEL)
		m_bDone = true;
	
	if(button == IApplication::BUTTON_CONFIRM){
		SoundManager::PlayEffect(SoundManager::EFFECT_SELECT_OPTION);
		Menu::Choose();
	}
}


void StartupState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{
    StoneRing::State::HandleButtonDown ( button );
}

void StartupState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
    if(dir == IApplication::AXIS_DOWN){
        SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
        Menu::SelectDown();
    }
    else if(dir == IApplication::AXIS_UP){
        SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);        
        Menu::SelectUp();
    }
}


void StartupState::SteelCleanup ( SteelInterpreter* )
{
}

void StartupState::SteelInit ( SteelInterpreter* )
{
}

void StartupState::Draw ( const clan::Rect& screenRect, clan::Canvas& GC )
{
    m_overlay.draw(GC,0,0);   
    Menu::Draw(GC);
}


void StartupState::Update()
{

}

void StartupState::Finish()
{

}

clan::Rectf StartupState::get_rect()
{
    return m_menu_rect;
}


void StartupState::draw_option ( int option, bool selected, const clan::Rectf& rect, clan::Canvas& gc )
{
    std::string option_str;
    const float x = rect.get_top_left().x;
    const float y = rect.get_top_right().y;
    if(option == 0)
        option_str = "Continue";
    else if(option == 1)
        option_str = "New Game";
    else if(option == 2)
        option_str = "Quit";
#ifdef SR2_EDITOR
	else if(option == 3)
		option_str = "Edit Maps";
#endif
    
    Font font = selected?m_selection_font:m_option_font;
    font.draw_text(gc,x,y,option_str,Font::TOP_LEFT);
}

int StartupState::get_option_count()
{
#ifdef SR2_EDITOR
	return 4;
#else
    return 3;
#endif
}

int StartupState::height_for_option ( clan::Canvas& gc )
{
    return m_option_font.get_font_metrics(gc).get_height();
}

void StartupState::process_choice ( int selection )
{
    switch(selection){
        case 0:
            IApplication::GetInstance()->StartGame(true);
            break;
        case 1:
            IApplication::GetInstance()->StartGame(false);
            break;
        case 2:
            m_bDone = true;
            break;
#ifdef SR2_EDITOR
		case 3:
			IApplication::GetInstance()->EditMaps();
			break;
#endif
        default:
            break;
    }
}



}