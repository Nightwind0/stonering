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


#include "StatusState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "SoundManager.h"
#include <sstream>
#include <iomanip>

namespace StoneRing {

StatusState::StatusState()
{
    m_stats.push_back(ICharacter::CA_MAXHP);
    m_stats.push_back(ICharacter::CA_MAXMP);
    m_stats.push_back(ICharacter::CA_STR);
    m_stats.push_back(ICharacter::CA_DEF);
    m_stats.push_back(ICharacter::CA_DEX);
    m_stats.push_back(ICharacter::CA_EVD);
    m_stats.push_back(ICharacter::CA_MAG);
    m_stats.push_back(ICharacter::CA_RST);
    m_stats.push_back(ICharacter::CA_LCK);
    m_stats.push_back(ICharacter::CA_JOY);
    
    m_stats.push_back(ICharacter::CA_PIERCE_DEF);
    m_stats.push_back(ICharacter::CA_SLASH_DEF);
    m_stats.push_back(ICharacter::CA_BASH_DEF);
    m_stats.push_back(ICharacter::CA_FIRE_RST);
    m_stats.push_back(ICharacter::CA_WATER_RST);
    m_stats.push_back(ICharacter::CA_WIND_RST);
    m_stats.push_back(ICharacter::CA_EARTH_RST);
    m_stats.push_back(ICharacter::CA_HOLY_RST);
    m_stats.push_back(ICharacter::CA_DARK_RST);
}

StatusState::~StatusState()
{

}

void StatusState::Start()
{
    m_party_rect = GraphicsManager::GetRect(GraphicsManager::STATUS,"party");
    m_status_rect = GraphicsManager::GetRect(GraphicsManager::STATUS,"status_box");
    m_stat_name_font = GraphicsManager::GetFont(GraphicsManager::STATUS,"stat_name");
    m_header_rect = GraphicsManager::GetRect(GraphicsManager::STATUS,"header");
    m_stat_font = GraphicsManager::GetFont(GraphicsManager::STATUS,"stat");
    m_plus_font = GraphicsManager::GetFont(GraphicsManager::STATUS,"plus");
    m_minus_font = GraphicsManager::GetFont(GraphicsManager::STATUS,"minus");
    m_header_font = GraphicsManager::GetFont(GraphicsManager::STATUS,"header");
    m_name_x = GraphicsManager::GetPoint(GraphicsManager::STATUS,"name");
    m_stat_x = GraphicsManager::GetPoint(GraphicsManager::STATUS,"stat");
    m_mod_x = GraphicsManager::GetPoint(GraphicsManager::STATUS,"mod");
    m_base_x = GraphicsManager::GetPoint(GraphicsManager::STATUS,"base");
    
    m_current_character = 0;
    m_bDone = false;
}

void StatusState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
    switch(dir){
        case IApplication::AXIS_LEFT:
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
            m_current_character++;
            if(m_current_character >= IApplication::GetInstance()->GetParty()->GetCharacterCount())
                m_current_character = 0;
            break;
        case IApplication::AXIS_RIGHT:
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
            if(m_current_character == 0){
                m_current_character = IApplication::GetInstance()->GetParty()->GetCharacterCount() - 1;
            } else {
                m_current_character--;
            }
            break;           
    }
}


void StatusState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{

}


void StatusState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
    switch(button){
        case IApplication::BUTTON_CANCEL:
            m_bDone = true;
            break;
    }
}


void StatusState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    MenuBox::Draw ( GC, m_party_rect, false );
    MenuBox::Draw ( GC, m_status_rect, false );
    draw_party(GC);
    draw_header(GC);
    draw_stats(GC, dynamic_cast<Character*>(IApplication::GetInstance()->GetParty()->GetCharacter(m_current_character)));
}
void StatusState::draw_party( CL_GraphicContext& gc ){
    CL_Rectf rect = m_party_rect;
    rect.shrink(GraphicsManager::GetMenuInset().x, GraphicsManager::GetMenuInset().y);
    //rect.translate(GraphicsManager::GetMenuInset());
    //CL_Draw::fill(gc,rect,CL_Colorf(1.0f,1.0f,1.0f,0.5f));
    IParty * party = IApplication::GetInstance()->GetParty();
    for(uint i =0;i<party->GetCharacterCount(); i++)
    {
        CL_Sprite pSprite = dynamic_cast<Character*>(party->GetCharacter(i))
                        ->GetPortrait(Character::PORTRAIT_DEFAULT);        
        CL_Pointf point;
        point.y = rect.get_top_left().y +  (rect.get_height() - pSprite.get_height()) / 2.0f;
        float x_space =(rect.get_width() / party->GetCharacterCount());
        float center =  (x_space*i) + (x_space - pSprite.get_width()) / 2.0f; 
        point.x =  center;
        if(i != m_current_character)
            pSprite.set_alpha(0.5f);
        else
            pSprite.set_alpha(1.0f);
        pSprite.draw(gc,point.x,point.y);
    } 
}

void StatusState::draw_header ( CL_GraphicContext& gc )
{
    int y = m_header_rect.get_center().y;
    m_header_font.draw_text(gc,m_name_x.x,y,"Stat", Font::TOP_LEFT);
    m_header_font.draw_text(gc,m_stat_x.x,y,"Total", Font::TOP_LEFT);
    m_header_font.draw_text(gc,m_base_x.x,y,"Base", Font::TOP_LEFT);
    m_header_font.draw_text(gc,m_mod_x.x,y,"Modifier", Font::TOP_LEFT);
}


void StatusState::draw_stats ( CL_GraphicContext& gc, Character* pChar )
{
    const uint height_each = (m_status_rect.get_height()-m_header_rect.get_height()) / m_stats.size();
    CL_Pointf tl = m_status_rect.get_top_left();
    tl.y += m_header_rect.get_height();
    for(uint i=0;i<m_stats.size();i++){
            int base = 0;
            int bonus = 0;
            // Fade every other line
            float mult  = (i%2 == 1)?0.75f:1.0;
            
            if(ICharacter::IsInteger(m_stats[i])){
                base = int(pChar->GetBaseAttribute(m_stats[i]));
                bonus = int(pChar->GetAttribute(m_stats[i]) - pChar->GetBaseAttribute(m_stats[i]));
            }else {
                base = int(100.0 * pChar->GetBaseAttribute(m_stats[i]));
                bonus = int(100.0 * (pChar->GetAttribute(m_stats[i]) - pChar->GetBaseAttribute(m_stats[i])));
            }
            m_stat_name_font.draw_text(gc,tl.x + m_name_x.x,tl.y + m_name_x.y + i*height_each,ICharacter::CAToLabel(m_stats[i]), Font::TOP_LEFT,mult);
            std::ostringstream os;
            os << std::setw(6) << base + bonus;
            
            m_stat_font.draw_text(gc,tl.x + m_stat_x.x , tl.y + m_stat_x.y + i*height_each,os.str(), Font::TOP_LEFT,mult);
            os.str("");
            os << std::setw(6) << base;
            m_stat_font.draw_text(gc,tl.x + m_base_x.x, tl.y + m_base_x.y + i*height_each,os.str(),Font::TOP_LEFT,mult);
            os.str("");
            os << std::setw(6) << std::showpos <<  bonus;
            if(bonus > 0){
                m_plus_font.draw_text(gc,tl.x + m_mod_x.x,tl.y + m_mod_x.y + i*height_each,os.str(), Font::TOP_LEFT,mult);
            }else if(bonus <0){
                m_minus_font.draw_text(gc,tl.x + m_mod_x.x,tl.y + m_mod_x.y + i*height_each,os.str(), Font::TOP_LEFT,mult);
            }
    }
}


void StatusState::Finish()
{

}

bool StatusState::IsDone() const
{
    return m_bDone;
}



bool StatusState::LastToDraw() const
{
    return false;
}


bool StatusState::DisableMappableObjects() const
{
    return true;
}


void StatusState::MappableObjectMoveHook()
{

}


}