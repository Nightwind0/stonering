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

void MenuBox::Draw ( clan::Canvas& gc, const clan::Rectf& rect, bool inset_shadow, clan::Pointf shadow_point )
{
    const float line_width = 4.0f;
    const float corner = 16.0f;
    const uint shadow_width = 3;
    clan::Sizef rrect_size = rect.get_size();
    clan::Pointf origin = rect.get_top_left();
    std::vector<clan::Vec2f> borderTris;
	std::vector<clan::Vec2f> borderShadowTris;
	clan::Shape2D borderShape;
	clan::Shape2D borderShadow;
	borderShadow.add_rounded_rect(origin+shadow_point, rrect_size,corner);
	borderShadow.get_triangles(borderShadowTris);
	borderShape.add_rounded_rect(origin,rrect_size,corner);
	borderShape.get_triangles(borderTris);
	gc.fill_triangles(borderShadowTris,clan::Colorf(0.0f,0.0f,0.0f,0.4f));
	gc.fill_triangles(borderTris,clan::Colorf::white);
    clan::Sizef fill_size = rrect_size;
    clan::Pointf fill_origin = origin;
    fill_size-= line_width * 2;
    fill_origin += line_width;
	clan::Shape2D fillShape;
	std::vector<clan::Vec2f> fillTris;
    fillShape.add_rounded_rect(fill_origin,fill_size,corner);
	fillShape.get_triangles(fillTris);
	gc.fill_triangles(fillTris, StoneRing::GraphicsManager::GetMenuGradient());
    //rrect_fill.fill(g fill_origin, StoneRing::GraphicsManager::GetMenuGradient());
    if(inset_shadow){
        clan::Sizef shadow_size = fill_size;
        clan::Pointf shadow_origin = fill_origin;
        for(uint i=0;i<shadow_width;i++){
            shadow_size -= 2;
            shadow_origin += 1;
			clan::Shape2D rrect;
			std::vector<clan::Vec2f> tris;
			rrect.add_rounded_rect(shadow_origin,shadow_size,corner);
            rrect.get_triangles(fillTris);
			gc.fill_triangles(fillTris,clan::Colorf(0.0f,0.0f,0.0f,0.2f));
        }
    }
    
}

