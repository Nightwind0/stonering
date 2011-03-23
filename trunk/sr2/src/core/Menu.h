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

namespace StoneRing {

class Menu
{
public:
    Menu();
    ~Menu();

    void Init();
    void Draw(CL_GraphicContext& gc);
    bool SelectUp();
    bool SelectDown();
    int Choose();

protected:
    virtual CL_Rectf get_rect()=0;
    virtual void draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc)=0;
    virtual int height_for_option(CL_GraphicContext& gc)=0;
    virtual void process_choice(int selection)=0;
    virtual int get_option_count()=0;
    virtual void draw_more_down_indicator(){}
    virtual void draw_more_up_indicator(){}
    virtual uint get_columns() const { return 1; }
    int get_current_choice() const { return m_cursor; }
    bool is_selected(int index) { return index == m_cursor; }

private:
    int m_cursor;
};

};
#endif // MENU_H
