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

#include "Menu.h"

using StoneRing::Menu;

Menu::Menu()
{
}

Menu::~Menu()
{
}

void Menu::Init() 
{
    m_cursor = 0;
}

void Menu::Draw(CL_GraphicContext& gc)
{
    CL_Rectf rect = get_rect();
    int options_per_page = rect.get_height() / height_for_option(gc);
    int page = m_cursor / options_per_page;
   
    
    for(int i = options_per_page * page; i < cl_min(get_option_count(),options_per_page * (page+1)); i++)
    {
	draw_option(i,i==m_cursor,rect.get_top_left().x,rect.get_top_left().y + (i-options_per_page * page) * height_for_option(gc),gc) ;
    }
}

bool Menu::SelectUp()
{
    if(m_cursor > 0)
	m_cursor--;
    else
	m_cursor = get_option_count() - 1;
	return true;
}

bool Menu::SelectDown()
{
    if(m_cursor + 1 < get_option_count())
	m_cursor++;
    else
	m_cursor = 0;
	return true;
}

int Menu::Choose()
{
    process_choice(m_cursor);
    return m_cursor;
}

void Menu::reset_menu()
{
    m_cursor = 0;
}
