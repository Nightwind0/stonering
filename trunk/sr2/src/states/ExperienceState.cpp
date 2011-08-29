/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "ExperienceState.h"
#include "GraphicsManager.h"
#include <sstream>
#include <iomanip>
#include "MenuBox.h"

using StoneRing::ExperienceState;
using StoneRing::IApplication;

ExperienceState::ExperienceState()
{

}

ExperienceState::~ExperienceState()
{

}

void ExperienceState::Init()
{
    m_characters.clear();
    m_xpbar = GraphicsManager::GetImage(GraphicsManager::EXPERIENCE,"xp_bar");
    m_portraitShadow = GraphicsManager::GetImage(GraphicsManager::EXPERIENCE, "portrait_shadow");
    m_portraitOffset = GraphicsManager::GetPoint(GraphicsManager::EXPERIENCE,"portrait");
    m_offset = GraphicsManager::GetPoint(GraphicsManager::EXPERIENCE,"origin");
    m_textOffset = GraphicsManager::GetPoint(GraphicsManager::EXPERIENCE,"text");

    
    m_characterFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::EXPERIENCE,"Character"));
    m_xpFont = GraphicsManager::GetFont(GraphicsManager::EXPERIENCE,"XP");
    m_oldlevelFont = GraphicsManager::GetFont(GraphicsManager::EXPERIENCE,"OldLevel");
    m_levelFont = GraphicsManager::GetFont(GraphicsManager::EXPERIENCE,"Level");
    m_spFont = GraphicsManager::GetFont(GraphicsManager::EXPERIENCE,"SP");

    m_barPoint = GraphicsManager::GetPoint(GraphicsManager::EXPERIENCE,"xp_bar");
    m_barGradient = GraphicsManager::GetGradient(GraphicsManager::EXPERIENCE,"xp_bar");
    m_barRect = GraphicsManager::GetRect(GraphicsManager::EXPERIENCE,"xp_bar");
    m_charRect = GraphicsManager::GetRect(GraphicsManager::EXPERIENCE,"char");

    m_pTNL = IApplication::GetInstance()->GetUtility(IApplication::XP_FOR_LEVEL); 
    m_pLNT = IApplication::GetInstance()->GetUtility(IApplication::LEVEL_FOR_XP);
}


void ExperienceState::AddCharacter(StoneRing::Character* pCharacter, int xp_gained, int old_level, int sp)
{
    Char c;
    c.m_pCharacter = pCharacter;
    c.m_nXP = xp_gained;
    c.m_nOldLevel = old_level;
    c.m_nSP = sp;
    m_characters.push_back(c);
}


void ExperienceState::Finish()
{

}

void ExperienceState::Start()
{
    m_bDone = false;
    m_start_time = CL_System::get_time();
    // TODO: Play "xp" sound
}

void ExperienceState::MappableObjectMoveHook()
{

}

bool ExperienceState::DisableMappableObjects() const
{
	return true;
}

bool ExperienceState::LastToDraw() const
{
	return false;
}

void ExperienceState::Draw(const CL_Rect& screenRect, CL_GraphicContext& GC)
{
    const uint current_time = CL_System::get_time();
    float draw_percentage = (float)(current_time - m_start_time) / (float)1000.0f;
    if(draw_percentage > 1.0f) draw_percentage = 1.0f;
    const int total_characters = m_characters.size();
    const int height_per_char = m_charRect.get_height();
    CL_Pointf offset = m_offset;
    offset.x += (screenRect.get_width() - m_charRect.get_width()) / 2.0;
    offset.y += (screenRect.get_height() - (height_per_char * total_characters)) / 2.0;
    for(int i=0;i<m_characters.size();i++)
    {
        CL_Pointf point = offset;
        point.y += height_per_char * i;
        CL_Rectf rect = m_charRect;
        rect.translate(point);
        MenuBox::Draw(GC,rect,false);
        
        CL_Pointf portraitPoint = point + m_portraitOffset;  
        Character * pCharacter = m_characters[i].m_pCharacter;
      //  m_portraitShadow.draw(GC,portraitPoint.x + m_portraitOffset.x, portraitPoint.y + m_portraitOffset.y);
        m_xpbar.draw(GC,point.x + m_barPoint.x,point.y + m_barPoint.y);
        pCharacter->GetPortrait(Character::PORTRAIT_HAPPY).draw(GC,portraitPoint.x,portraitPoint.y);
        m_characterFont.draw_text(GC,point + m_textOffset,pCharacter->GetName(), StoneRing::Font::TOP_LEFT);
        CL_Pointf xp_point = point + m_textOffset;
        std::ostringstream ostream;
        ostream << '+' << std::setw(6) << m_characters[i].m_nXP << " XP";
        xp_point.y += m_characterFont.get_font_metrics(GC).get_height();
        m_xpFont.draw_text(GC,xp_point,ostream.str(), StoneRing::Font::TOP_LEFT);
        ostream.str("");
        ostream << '+' << std::setw(6) << m_characters[i].m_nSP << " SP";
        CL_Pointf sp_point = xp_point;
        sp_point.y = xp_point.y + m_xpFont.get_font_metrics(GC).get_height();
        m_spFont.draw_text(GC,sp_point,ostream.str(),StoneRing::Font::TOP_LEFT);
        int levelsGained = pCharacter->GetLevel() - m_characters[i].m_nOldLevel;
        if(levelsGained)
        {
            CL_Pointf levelPoint = xp_point;
            CL_Sizef xpSize = m_xpFont.get_text_size(GC,ostream.str());
            levelPoint.x += xpSize.width;
            std::ostringstream ostream;
            ostream << " Level Up! ";
            if(levelsGained > 1)
                ostream << 'X' << levelsGained;
            m_levelFont.draw_text(GC,levelPoint,ostream.str());
        }	
        //int original_level = getLNT(pCharacter->GetXP() - m_characters[i].m_nXP);
        int next_level = getTNL(m_characters[i].m_nOldLevel+1);
        int to_start_level = getTNL(m_characters[i].m_nOldLevel);
        int to_current_level = getTNL(pCharacter->GetLevel());
        int tnl =  next_level - to_start_level;
        float start_percent = (float)(pCharacter->GetXP() - m_characters[i].m_nXP - to_start_level) / (float)tnl;
        if(start_percent < 0.0f || start_percent > 1.0f) start_percent = 0.0f;
        
        float total_percent_to_draw = (pCharacter->GetXP() - to_current_level ) / (float)tnl - start_percent;
        //if(m_characters[i].m_nOldLevel != pCharacter->GetLevel()){
        total_percent_to_draw += levelsGained;
        float percent = start_percent + draw_percentage * total_percent_to_draw;

        percent = percent - (int)percent;

        float barWidth = m_barRect.get_width() * percent;
        CL_Pointf barPoint = m_barRect.get_top_left();
        barPoint.y += point.y;
        barPoint.x += point.x;
        CL_Sizef barSize(barWidth,m_barRect.get_height());
        CL_Rectf barRect(barPoint,barSize);
        CL_Draw::gradient_fill(GC,barRect,m_barGradient);
    }
    }

void ExperienceState::HandleButtonUp(const IApplication::Button& button)
{
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	case IApplication::BUTTON_CANCEL:
	    m_bDone = true;
	    break;
    }
}

bool ExperienceState::IsDone() const
{
    return m_bDone;
}


void ExperienceState::SteelInit(SteelInterpreter* )
{    
}

void ExperienceState::SteelCleanup(SteelInterpreter* )
{

}

double ExperienceState::getTNL(int level)
{
    ParameterList params;
    params.push_back(ParameterListItem("$_LEVEL",level));
    return IApplication::GetInstance()->RunScript(m_pTNL,params);    
}
double ExperienceState::getLNT(int xp)
{
    ParameterList params;
    params.push_back(ParameterListItem("$_XP",xp));
    return IApplication::GetInstance()->RunScript(m_pLNT,params);    
}


