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
#include <sstream>
#include <fstream>
#include <iomanip>



namespace StoneRing {
    
    const int kVersion = 2;

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
            m_bDone = true;
            break;
        case IApplication::BUTTON_CONFIRM:
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

void SaveLoadState::MappableObjectMoveHook()
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
        std::ifstream in_file;
        in_file.open(filename_for_slot(i), std::ios::in | std::ios::binary);
        if(in_file.is_open()){
            if(verify_file(in_file)){
                FilePreview preview = load_file_header(in_file);
                m_previews[i] = preview;
            }
            in_file.close();
        }
    }
}

bool SaveLoadState::verify_file ( std::istream& in )
{
    char magic[5];
    magic[4] = '\0';
    in.read(magic,4);
    std::string magic_str(magic);
    if(magic_str != "SR2S"){
        return false;
    }
    
    int version;
    in.read((char*)&version,sizeof(version));
    if(version != kVersion)
        return false;
    return true;
}


SaveLoadState::FilePreview SaveLoadState::load_file_header ( std::istream& in )
{
    FilePreview preview;
    uint gold;
    uint minutes;
    cl_byte64 ticks;
    in.read((char*)&gold,sizeof(preview.m_gold));
    in.read((char*)&minutes,sizeof(preview.m_minutes));
    in.read((char*)&ticks,sizeof(ticks));
    preview.m_gold = gold;
    preview.m_minutes = minutes;
    preview.m_datetime = CL_DateTime::get_utc_time_from_ticks(ticks).to_local();
    uint party_size;
    in.read((char*)&party_size,sizeof(uint));
    for(uint i=0;i<party_size;i++){
        FilePreview::CharInfo info;
        info.m_name = ReadString(in);
        in.read((char*)&info.m_level,sizeof(info.m_level));
        preview.m_characters.push_back(info);
    }
    
    return preview;
}




void SaveLoadState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    Menu::Draw (GC);
}



// MENU stuff
int SaveLoadState::get_option_count()
{
    return 25;
}

void SaveLoadState::draw_option ( int option, bool selected, float x, float y, CL_GraphicContext& gc )
{
    // Need #, hours, date last saved, face for each person in the party
    const CL_Rectf screen = get_rect(); 
    CL_Rect rect(x,y,x+screen.get_width(),y+height_for_option(gc));
    MenuBox::Draw(gc,rect,true);
 
    std::ostringstream os;
    os << option;
    
    if(option == Menu::get_current_choice()){
        m_num_selected_font.draw_text(gc,x + m_number_pt.x, y + m_number_pt.y,os.str(), Font::TOP_LEFT);
    }else{
        m_number_font.draw_text(gc,x + m_number_pt.x,y + m_number_pt.y,os.str(), Font::TOP_LEFT);
    }
    
    std::map<uint,FilePreview>::const_iterator iter = m_previews.find(option);
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
        for(std::list<FilePreview::CharInfo>::const_iterator char_iter = iter->second.m_characters.begin();
            char_iter != iter->second.m_characters.end(); char_iter++){
            Character * pChar = IApplication::GetInstance()->GetCharacterManager()->GetCharacter (char_iter->m_name);
            CL_Sprite portrait = pChar->GetPortrait(Character::PORTRAIT_DEFAULT);
            
            CL_Point point(m_portrait_pt.x+x+i*(portrait.get_width()+m_portrait_pt.x),y+m_portrait_pt.y);
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

CL_Rectf SaveLoadState::get_rect()
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
                save(selection);
                m_bDone = true;
            }
        }else{
            save(selection);
            m_bDone = true;
        }
    }else{
        if(m_previews.find(selection) != m_previews.end()){
            if(load(selection))
                m_bDone = true;
            else
                SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
        }else{
            SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
        }
    }
}

std::string SaveLoadState::filename_for_slot ( uint slot )
{
    std::ostringstream os;
    os << "SaveSlot" << slot << ".sr2s";
    return os.str();
}


int SaveLoadState::height_for_option ( CL_GraphicContext& gc )
{
    return IApplication::GetInstance()->GetDisplayRect().get_height() / 3; // Hard-code 4 per screen per now
}

void SaveLoadState::save ( uint slot )
{
    char sig[] = "SR2S";
    std::ofstream out_file(filename_for_slot(slot),std::ios::out|std::ios::binary);
    out_file.write(sig,4);
    out_file.write((char*)&kVersion,sizeof(kVersion));
    IParty * party = IApplication::GetInstance()->GetParty();
    int gold = party->GetGold();
    int minutes = party->GetMinutesPlayed();
    cl_byte64 ticks = CL_DateTime::get_current_utc_time().to_ticks();
    uint num_chars = party->GetCharacterCount();
    out_file.write((char*)&gold,sizeof(gold));
    out_file.write((char*)&minutes,sizeof(minutes));
    out_file.write((char*)&ticks,sizeof(ticks));
    out_file.write((char*)&num_chars,sizeof(num_chars));
    for(uint c = 0; c < num_chars; c++){
        uint level = party->GetCharacter(c)->GetLevel();
        WriteString(out_file,party->GetCharacter(c)->GetName());
        out_file.write((char*)&level,sizeof(uint));
    }
    IApplication::GetInstance()->Serialize(out_file);
    out_file.close();
}

bool SaveLoadState::load ( uint slot )
{
    std::ifstream in_file(filename_for_slot(slot),std::ios::in|std::ios::binary);
    if(!verify_file(in_file))
        return false;
    load_file_header(in_file); // load and drop on the floor. don't care now
    bool success = IApplication::GetInstance()->Deserialize(in_file);
    in_file.close();
    return success;
}




}