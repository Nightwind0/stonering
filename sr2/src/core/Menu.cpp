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
#include "SoundManager.h"
#include <algorithm>

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
	int start = 0;
	while(hide_option(start)){
		if(start + 1 < get_option_count()){
			start++;
		}else{
			// Couldn't find any non-hidden options... 
			start = 0;
			break;
		}
	}
    m_stack.push_front(start);
}


void Menu::Draw(clan::Canvas& gc)
{
    clan::Rectf rect = get_rect();
    int cursor = m_stack.front();
    int options_per_page = std::max(1,int(rect.get_height() / height_for_option(gc)));
    int page = cursor / options_per_page;
    
    int skip_count = 0;
    int options = 0;	
    int option = 0;
    for(int i = options_per_page * page;  i  < get_option_count() && option < options_per_page; i++)
    {
            if(hide_option(i))
                ++skip_count;
			else ++options;
    }   
    
	option = 0;
    
    for(int i = options_per_page * page; i < get_option_count() && option < options_per_page; i++)
    {
        if(!hide_option(i)){
	  const float height = height_for_option(gc);
	  clan::Rectf clipRect ( rect.get_top_left().x, rect.get_top_left().y + option * height, rect.get_top_right().x, rect.get_top_left().y + (option+1) *  height);
	  
	  gc.push_cliprect(clipRect);
            draw_option(i,i==cursor,clipRect,gc);
			++option;
	  gc.pop_cliprect();		
	}
    }
}

bool Menu::SelectUp()
{
    SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
    int cursor = m_stack.front();
	const int original = cursor;
	do{
	if(cursor > 0)
		cursor--;
	else if(roll_over())
		cursor = get_option_count() - 1;
	}while(cursor != original && hide_option(cursor));
	m_stack.front() = cursor;	
    return true;
}

bool Menu::SelectDown()
{
    SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
    int cursor = m_stack.front();
	const int original = cursor;
	do{
		if(cursor + 1 < get_option_count())
			cursor++;
		else if(roll_over())
			cursor = 0;
	} while(cursor != original && hide_option(cursor));
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
    PushMenu();
}
