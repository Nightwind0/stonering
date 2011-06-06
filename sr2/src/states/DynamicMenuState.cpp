/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

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


#include "DynamicMenuState.h"
#include "GraphicsManager.h"


using namespace StoneRing;
using StoneRing::DynamicMenuState;

DynamicMenuState::DynamicMenuState()
{

}

DynamicMenuState::~DynamicMenuState()
{

}

bool DynamicMenuState::IsDone() const
{
    return m_bDone;
}

CL_Rect DynamicMenuState::calculate_rect()
{
    int width, height;
    width=height=0;
    m_nOptionHeight = 0;
    
    for(std::vector<std::string>::const_iterator it = m_choices.begin(); it != m_choices.end(); it++){
        CL_Size size = m_option_font.get_text_size(GET_MAIN_GC(),*it);
        height += size.height;
        m_nOptionHeight = cl_max(m_nOptionHeight,size.height);
        width = cl_max(width,size.width);
    }
    
    CL_Rect rect;
    CL_Rect screen  = IApplication::GetInstance()->GetDisplayRect();
    width = cl_min(width,screen.get_width());
    height = cl_min(height,screen.get_height());

    
    rect.top = (screen.get_height() - height) / 2;
    rect.bottom = height + rect.top;
    rect.left = (screen.get_width() - width) / 2;
    rect.right = width + rect.left;
    
    
    return rect;
}


void DynamicMenuState::Init ( const std::vector< std::string >& choices )
{
    m_choices = choices;
    m_option_font = GraphicsManager::GetFont(GraphicsManager::DYNAMIC_MENU,"Option");
    m_selection_font = GraphicsManager::GetFont(GraphicsManager::DYNAMIC_MENU,"Selection");
    m_bgGradient = GraphicsManager::GetGradient(GraphicsManager::DYNAMIC_MENU,"bg");
    m_shadowColor = GraphicsManager::GetColor(GraphicsManager::DYNAMIC_MENU,"shadow");
    m_borderColor = GraphicsManager::GetColor(GraphicsManager::DYNAMIC_MENU,"border");
    m_margins = GraphicsManager::GetPoint(GraphicsManager::DYNAMIC_MENU,"margins");
    m_optionsRect = calculate_rect();
    m_rect = m_optionsRect;
    
    m_rect.top -= m_margins.y;
    m_rect.bottom += m_margins.y; 
    m_rect.left -= m_margins.x;
    m_rect.right += m_margins.x;
}

    // Handle joystick / key events that are processed according to mappings
void DynamicMenuState::HandleButtonUp(const IApplication::Button& button)
{
    if(button == IApplication::BUTTON_CANCEL)
        m_bDone = true;
    else if(button == IApplication::BUTTON_CONFIRM)
        Menu::Choose();
}

void DynamicMenuState::HandleButtonDown(const IApplication::Button& button)
{
}

void DynamicMenuState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
    if(axis == IApplication::AXIS_VERTICAL)
    {
        if(dir == IApplication::AXIS_DOWN)
            Menu::SelectDown();
        else if(dir == IApplication::AXIS_UP)
            Menu::SelectUp();
    }
}

void DynamicMenuState::Start()
{
    Menu::Init();
    m_bDone = false;
}

void DynamicMenuState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    CL_Rect shadowRect = m_rect;
    shadowRect.translate(CL_Vec2f(8,8));
    CL_Draw::fill(GC,shadowRect,m_shadowColor);
    CL_Draw::gradient_fill(GC,m_rect,m_bgGradient);
    //CL_Draw::box(GC,m_optionsRect, CL_Colorf::red);
    CL_Draw::box(GC,m_rect,m_borderColor);
   
    Menu::Draw(GC);
}

bool DynamicMenuState::LastToDraw() const 
{
    return true;
}

bool DynamicMenuState::DisableMappableObjects() const
{
    return false;
}

void DynamicMenuState::MappableObjectMoveHook()
{
}


void DynamicMenuState::SteelInit ( SteelInterpreter* )
{

}


void DynamicMenuState::SteelCleanup ( SteelInterpreter* )
{

}


void DynamicMenuState::Finish()
{

}

int DynamicMenuState::GetSelection() const
{
    return m_nSelection;
}


void DynamicMenuState::draw_more_down_indicator()
{
    StoneRing::Menu::draw_more_down_indicator();
}

void DynamicMenuState::draw_more_up_indicator()
{
    StoneRing::Menu::draw_more_up_indicator();
}

void DynamicMenuState::draw_option ( int option, bool selected, float x, float y, CL_GraphicContext& gc )
{
    
    Font lineFont;
    if(selected)
    {
        lineFont = m_selection_font;
    }
    else
    {
        lineFont = m_option_font;
    }
    
    float font_height_offset = 0 - ((lineFont.get_font_metrics(gc).get_height() - 
                                    lineFont.get_font_metrics(gc).get_descent() -  
                                    lineFont.get_font_metrics(gc).get_internal_leading())/ 2);
    
    lineFont.draw_text(gc,x,y + lineFont.get_font_metrics(gc).get_height() + font_height_offset, m_choices[option]); 
    
}

int DynamicMenuState::get_option_count()
{
    return m_choices.size();
}

CL_Rectf DynamicMenuState::get_rect()
{
    return m_optionsRect;
}

int DynamicMenuState::height_for_option ( CL_GraphicContext& gc )
{
    return m_nOptionHeight;
}

void DynamicMenuState::process_choice ( int selection )
{
    m_nSelection = selection;
    m_bDone = true;
}



