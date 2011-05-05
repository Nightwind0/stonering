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
    CL_ResourceManager resources = IApplication::GetInstance()->GetResources();
    static std::string resource = "Overlays/Experience/";
    m_characters.clear();
    m_overlay = GraphicsManager::GetOverlay(GraphicsManager::EXPERIENCE);
    m_portraitOffset.x = (float)resources.get_integer_resource(resource + "portrait/x",0);
    m_portraitOffset.y = (float)resources.get_integer_resource(resource + "portrait/y",0);
    m_offset.x = (float)resources.get_integer_resource(resource + "x",0);
    m_offset.y = (float)resources.get_integer_resource(resource + "y",0);
    m_textOffset.x = (float)resources.get_integer_resource(resource + "text/x",0);
    m_textOffset.y = (float)resources.get_integer_resource(resource + "text/y",0);
    
    m_characterFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::EXPERIENCE,"Character"));
    m_xpFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::EXPERIENCE,"XP"));
    m_oldlevelFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::EXPERIENCE,"OldLevel"));
    m_levelFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::EXPERIENCE,"Level"));

    
    CL_Colorf xp_bar_tl(CL_String_load(resource + "xp_bar/gradient_top_left",resources));
    CL_Colorf xp_bar_tr(CL_String_load(resource + "xp_bar/gradient_top_right",resources));
    CL_Colorf xp_bar_bl(CL_String_load(resource + "xp_bar/gradient_bottom_left",resources));
    CL_Colorf xp_bar_br(CL_String_load(resource + "xp_bar/gradient_bottom_right",resources));
    
    m_barGradient = CL_Gradient(xp_bar_tl,xp_bar_tr,xp_bar_bl,xp_bar_br);
    m_barPoint = CL_Pointf(resources.get_integer_resource(resource + "xp_bar/x",0),
			   resources.get_integer_resource(resource + "xp_bar/y",0));
    CL_Sizef barSize = CL_Sizef(resources.get_integer_resource(resource + "xp_bar/width",0),
				resources.get_integer_resource(resource + "xp_bar/height",0));
    m_barRect = CL_Rectf(m_barPoint,barSize);

    m_pTNL = IApplication::GetInstance()->GetUtility(IApplication::XP_FOR_LEVEL);
    m_pLNT = IApplication::GetInstance()->GetUtility(IApplication::LEVEL_FOR_XP);
    
				       
    
}


void ExperienceState::AddCharacter(StoneRing::Character* pCharacter, int xp_gained, int old_level)
{
    Char c;
    c.m_pCharacter = pCharacter;
    c.m_nXP = xp_gained;
    c.m_nOldLevel = old_level;
    m_characters.push_back(c);
}


void ExperienceState::Finish()
{

}

void ExperienceState::Start()
{
    m_bDone = false;
    m_start_time = CL_System::get_time();
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
    const int height_per_char = m_overlay.get_height();
    CL_Pointf offset = m_offset;
    offset.x += (IApplication::GetInstance()->GetDisplayRect().get_width() - m_overlay.get_width()) / 2.0;
    offset.y += (IApplication::GetInstance()->GetDisplayRect().get_width() - (height_per_char * total_characters)) / 2.0;
    for(int i=0;i<m_characters.size();i++)
    {
	CL_Pointf point = offset;
	point.y += height_per_char * i;
	m_overlay.draw(GC,point.x,point.y);
	CL_Pointf portraitPoint = point + m_portraitOffset;  
	Character * pCharacter = m_characters[i].m_pCharacter;
	pCharacter->GetPortrait(Character::PORTRAIT_HAPPY).draw(GC,portraitPoint.x,portraitPoint.y);
	m_characterFont.draw_text(GC,point + m_textOffset,pCharacter->GetName());
	CL_Pointf xp_point = point + m_textOffset;
	std::ostringstream ostream;
	ostream << '+' << m_characters[i].m_nXP << "XP";
	xp_point.y += m_characterFont.get_font_metrics(GC).get_height();
	m_xpFont.draw_text(GC,xp_point,ostream.str());
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


