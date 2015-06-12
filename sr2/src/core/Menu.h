/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2010  Daniel Palm

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

#ifndef MENU_H
#define MENU_H
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <set>
#include <string>
#include "sr_defines.h"
#include <stack>


namespace StoneRing {

class Menu
{
public:
    Menu();
    ~Menu();

    void Init();
    void Draw(clan::Canvas& gc);
    bool SelectUp();
    bool SelectDown();
    int Choose();
    void PushMenu();
    void PopMenu();

protected:
    virtual clan::Rectf get_rect()=0;
    virtual void draw_option(int option, bool selected, const clan::Rectf& dest_rect, clan::Canvas& gc)=0;
    virtual int height_for_option(clan::Canvas& gc)=0;
    virtual void process_choice(int selection)=0;
    virtual int get_option_count()=0;
    virtual void draw_more_down_indicator(clan::Canvas& gc){}
    virtual void draw_more_up_indicator(clan::Canvas& gc){}
    virtual bool hide_option(int option)const { return false; }
    virtual bool roll_over(){ return true; }
    virtual uint get_columns() const { return 1; }
    virtual bool horizontal_scroll() { return false; }
    void reset_menu();
    int get_current_choice() const { return m_stack.front(); }
    bool is_selected(int index) const { return index == m_stack.front(); }

private:
    std::deque<int> m_stack;
    float m_horiz_scroll;
    long  m_idle_start;
};

};
#endif // MENU_H
