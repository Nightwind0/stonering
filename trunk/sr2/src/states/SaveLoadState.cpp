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


#include "SaveLoadState.h"
#include "MenuBox.h"
#include "GraphicsManager.h"
#include "DynamicMenuState.h"
#include "SoundManager.h"
#include "CharacterManager.h"
#include "Party.h"
#include <sstream>
#include <fstream>
#include <iomanip>



namespace StoneRing {


SaveLoadState::SaveLoadState()
{

}

SaveLoadState::~SaveLoadState()
{

}
void SaveLoadState::Init ( bool bSave, bool cancelable )
{
    m_bSave = bSave;
    m_cancelable = cancelable;
	m_cancelled = false;
    m_num_selected_font = GraphicsManager::GetFont(GraphicsManager::SAVE_LOAD,"Number_Selected");
    m_number_font = GraphicsManager::GetFont(GraphicsManager::SAVE_LOAD,"Number");
    m_empty_font = GraphicsManager::GetFont(GraphicsManager::SAVE_LOAD,"Empty");
    m_empty_selected_font = GraphicsManager::GetFont(GraphicsManager::SAVE_LOAD,"Empty_Selected");
    m_datetime_font = GraphicsManager::GetFont(GraphicsManager::SAVE_LOAD,"DateTime");
    m_number_pt = GraphicsManager::GetPoint(GraphicsManager::SAVE_LOAD,"number");
    m_datetime_pt = GraphicsManager::GetPoint(GraphicsManager::SAVE_LOAD,"datetime");
    m_empty_pt = GraphicsManager::GetPoint(GraphicsManager::SAVE_LOAD,"empty");
    m_portrait_pt = GraphicsManager::GetPoint(GraphicsManager::SAVE_LOAD,"portrait");
    m_hours_font = GraphicsManager::GetFont(GraphicsManager::SAVE_LOAD,"Hours");

	m_portrait_shadow = GraphicsManager::CreateImage("Overlays/MainMenu/portrait_shadow");
}

void SaveLoadState::Start()
{
    m_bDone = false;
    load_file_previews();
    Menu::Init();
}

bool SaveLoadState::DisableMappableObjects() const
{
    return true;
}
bool SaveLoadState::LastToDraw() const
{
    return false;
}

void SaveLoadState::Finish()
{

}


void SaveLoadState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{
    StoneRing::State::HandleButtonDown ( button );
}

void SaveLoadState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
    switch(button){
        case IApplication::BUTTON_CANCEL:
			if(m_cancelable){
				m_cancelled = true;
				m_bDone = true;
			}
            break;
        case IApplication::BUTTON_CONFIRM:
            SoundManager::PlayEffect(SoundManager::EFFECT_SELECT_OPTION);
            Menu::Choose();
            break;
    }
}

void SaveLoadState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
    if(dir == IApplication::AXIS_DOWN){
        Menu::SelectDown();
    }else if(dir == IApplication::AXIS_UP){
        Menu::SelectUp();
    }
}

bool SaveLoadState::IsDone() const
{
    return m_bDone;
}

void SaveLoadState::Update()
{

}


void SaveLoadState::clear_previews()
{
    m_previews.clear();
}




void SaveLoadState::load_file_previews()
{
    clear_previews();
    for(int i=0;i<get_option_count();i++){
        if(AppUtils::SaveExists(i)){
			AppUtils::SaveSummary preview = AppUtils::LoadSaveSummary(i);
			if(preview.m_isValid)
				m_previews[i] = preview;
        }
    }
}



void SaveLoadState::Draw ( const clan::Rect& screenRect, clan::Canvas& GC )
{
    Menu::Draw (GC);
}



// MENU stuff
int SaveLoadState::get_option_count()
{
    return 25;
}

void SaveLoadState::draw_option ( int option, bool selected, float x, float y, clan::Canvas& gc )
{
    // Need #, hours, date last saved, face for each person in the party
    const clan::Rectf screen = get_rect(); 
    clan::Rect rect(x,y,x+screen.get_width(),y+height_for_option(gc));
    MenuBox::Draw(gc,rect,true);
 
    std::ostringstream os;
    os << option;
    
    if(option == Menu::get_current_choice()){
        m_num_selected_font.draw_text(gc,x + m_number_pt.x, y + m_number_pt.y,os.str(), Font::TOP_LEFT);
    }else{
        m_number_font.draw_text(gc,x + m_number_pt.x,y + m_number_pt.y,os.str(), Font::TOP_LEFT);
    }
    
    std::map<uint,AppUtils::SaveSummary>::const_iterator iter = m_previews.find(option);
    if(iter != m_previews.end()){
#if 0 
        std::ostringstream strDate;
        strDate << (int)iter->second.m_datetime.get_hour() << ':' << (int)iter->second.m_datetime.get_minutes() << ' ' 
            << (int)iter->second.m_datetime.get_month() << '/' << (int)iter->second.m_datetime.get_day();
        m_datetime_font.draw_text(gc,x+m_datetime_pt.x,y+m_datetime_pt.y,strDate.str(),Font::TOP_LEFT);
#endif
        std::ostringstream strTime;
        strTime << std::setfill('0') << std::setw(2) << iter->second.m_minutes / 60 << ':'
                << std::setfill('0') << std::setw(2) << iter->second.m_minutes % 60 << 'm';
        m_hours_font.draw_text(gc,x+m_datetime_pt.x,y+m_datetime_pt.y,strTime.str(),Font::TOP_LEFT);
        
        float alpha = selected?1.0f:0.5f;
        uint i = 0;
        for(std::list<AppUtils::SaveSummary::CharInfo>::const_iterator char_iter = iter->second.m_characters.begin();
            char_iter != iter->second.m_characters.end(); char_iter++){
            Character * pChar = CharacterManager::GetCharacter (char_iter->m_name);
            clan::Sprite portrait = pChar->GetPortrait(Character::PORTRAIT_DEFAULT);
            
            clan::Point point(m_portrait_pt.x+x+i*(portrait.get_width()+m_portrait_pt.x),y+m_portrait_pt.y);
			m_portrait_shadow.draw(gc,point.x,point.y);
            portrait.draw (gc,point.x,point.y);
            i++;
        }

    }else{
        if(option == Menu::get_current_choice())
            m_empty_selected_font.draw_text(gc,x + m_empty_pt.x, y + m_empty_pt.y,"No Data");
        else
            m_empty_font.draw_text(gc,x + m_empty_pt.x,y+m_empty_pt.y,"No Data");
    }
    
}

clan::Rectf SaveLoadState::get_rect()
{
    return IApplication::GetInstance()->GetDisplayRect();
}

void SaveLoadState::process_choice ( int selection )
{
    if(m_bSave){
        if(m_previews.find(selection) != m_previews.end()){
            DynamicMenuState menuState;
            std::vector<std::string> options;
            options.push_back("Cancel");
            options.push_back("Overwrite");
            menuState.Init(options);
            IApplication::GetInstance()->RunState(&menuState);
            if(menuState.GetSelection() == 1){
                AppUtils::SaveGame(selection);
                m_bDone = true;
            }
        }else{
            AppUtils::SaveGame(selection);
            m_bDone = true;
        }
    }else{
        if(m_previews.find(selection) != m_previews.end()){
            if(AppUtils::LoadGame(selection))
                m_bDone = true;
            else
                SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
        }else{
            SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
        }
    }
}



int SaveLoadState::height_for_option ( clan::Canvas& gc )
{
    return IApplication::GetInstance()->GetDisplayRect().get_height() / 3; // Hard-code 4 per screen per now
}






}