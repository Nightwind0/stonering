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
#include <vector>
#include <string>

class MenuDelegate {
public:
    void DrawMenuItem(CL_GraphicContext& gc, CL_Rect& rect, int option);
    void ChooseMenuItem(int option);
};

class Menu
{
public:
    Menu(MenuDelegate& delegate);
    ~Menu();
    
    void SetRect(const CL_Rect& rect);
    void SetRowHeight(int height);
    
    void Draw(CL_GraphicContext& gc);
    bool SelectUp();
    bool SelectDown();
    
protected:
private:
    MenuDelegate& m_delegate;
    CL_Rect m_rect;
    int m_height;
    int m_nSelection;
};

#endif // MENU_H
