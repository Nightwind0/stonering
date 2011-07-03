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


#include "MenuBox.h"
#include "GraphicsManager.h"

using namespace StoneRing;

MenuBox::MenuBox()
{

}

MenuBox::~MenuBox()
{

}

void MenuBox::Draw ( CL_GraphicContext& gc, const CL_Rectf& rect, bool inset_shadow, CL_Pointf shadow_point )
{
    const float line_width = 4.0f;
    const float corner = 16.0f;
    const uint shadow_width = 3;
    CL_Sizef rrect_size = rect.get_size();
    CL_Pointf origin = rect.get_top_left();
    CL_RoundedRect rrect_border(rrect_size,corner);
    rrect_border.fill(gc, origin + shadow_point, CL_Colorf(0.0f,0.0f,0.0f,0.4f));
    rrect_border.fill(gc, origin ,CL_Colorf::white);
    CL_Sizef fill_size = rrect_size;
    CL_Pointf fill_origin = origin;
    fill_size-= line_width * 2;
    fill_origin += line_width;
    CL_RoundedRect rrect_fill(fill_size,corner);
    rrect_fill.fill(gc, fill_origin, StoneRing::GraphicsManager::GetMenuGradient());
    if(inset_shadow){
        CL_Sizef shadow_size = fill_size;
        CL_Pointf shadow_origin = fill_origin;
        for(uint i=0;i<shadow_width;i++){
            shadow_size -= 2;
            shadow_origin += 1;
            CL_RoundedRect rrect_shadow(shadow_size,corner);
            rrect_shadow.draw(gc,shadow_origin, CL_Colorf(0.0f,0.0f,0.0f,0.2f));
        }
    }
    
}

