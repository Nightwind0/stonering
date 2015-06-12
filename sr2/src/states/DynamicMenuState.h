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


#ifndef DYNAMICMENUSTATE_H
#define DYNAMICMENUSTATE_H

#include "State.h"
#include "Menu.h"

namespace StoneRing{

class DynamicMenuState : public State, public Menu
{
public:
    DynamicMenuState();
    virtual ~DynamicMenuState();
    void Init(const std::vector<std::string>& choices);
    virtual bool IsDone() const;
    virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
    virtual bool LastToDraw() const ; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void Update(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void SteelInit      (SteelInterpreter *);
    virtual void SteelCleanup   (SteelInterpreter *);
    virtual void Finish(); // Hook to clean up or whatever after being popped
    int GetSelection() const;
protected:
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);  	
    virtual clan::Rectf get_rect();
    virtual void draw_option(int option, bool selected, const clan::Rectf& dest_rect, clan::Canvas& gc);
    virtual int height_for_option(clan::Canvas& gc);
    virtual void process_choice(int selection);
    virtual int get_option_count();

private:
    clan::Rect calculate_rect();
    std::vector<std::string> m_choices;
    std::vector<clan::Image> m_icons;
    clan::Rectf m_rect;
    clan::Rectf m_optionsRect;
    StoneRing::Font m_option_font;
    StoneRing::Font m_selection_font;
    clan::Gradient m_bgGradient;
    clan::Colorf m_shadowColor;
    clan::Colorf m_borderColor;
    clan::Pointf m_margins;
    int m_nSelection;
    int m_nOptionHeight;
    bool m_bDone;
    bool m_bIcons;
};

}
#endif // DYNAMICMENUSTATE_H
