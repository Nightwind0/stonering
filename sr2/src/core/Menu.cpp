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
    PushMenu();
}

void Menu::PopMenu()
{
    m_stack.pop_front();
}

void Menu::PushMenu()
{
    m_stack.push_front(0);
}


void Menu::Draw(CL_GraphicContext& gc)
{
    CL_Rectf rect = get_rect();
    int cursor = m_stack.front();
    int options_per_page = cl_max(1,rect.get_height() / height_for_option(gc));
    int page = cursor / options_per_page;
   
    
    for(int i = options_per_page * page; i < cl_min(get_option_count(),options_per_page * (page+1)); i++)
    {
	draw_option(i,i==cursor,rect.get_top_left().x,rect.get_top_left().y + (i-options_per_page * page) * height_for_option(gc),gc) ;
    }
}

bool Menu::SelectUp()
{
    int cursor = m_stack.front();
    if(cursor > 0)
	cursor--;
    else if(roll_over())
	cursor = get_option_count() - 1;
    
    m_stack.front() = cursor;
    return true;
}

bool Menu::SelectDown()
{
    int cursor = m_stack.front();
    if(cursor + 1 < get_option_count())
	cursor++;
    else if(roll_over())
	cursor = 0;
    m_stack.front() = cursor;
    return true;
}

int Menu::Choose()
{
    process_choice(m_stack.front());
    return m_stack.front();
}

void Menu::reset_menu()
{
    m_stack.clear();
    m_stack.push_front(0);
}
