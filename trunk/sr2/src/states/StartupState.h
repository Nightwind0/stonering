/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

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


#ifndef STARTUPSTATE_H
#define STARTUPSTATE_H
#include "State.h"
#include "Menu.h"

namespace StoneRing { 

class StartupState : public State, public Menu
{
public:
    StartupState();
    virtual ~StartupState();
    virtual bool IsDone() const ;
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);    
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);  
    virtual void HandleQuit() { m_bDone = true; }
    virtual bool Threaded() const { return false; }
    virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void SteelInit      (SteelInterpreter *);
    virtual void SteelCleanup   (SteelInterpreter *);
    virtual void Finish(); // Hook to clean up or whatever after being popped
protected:
    virtual clan::Rectf get_rect();
    virtual void draw_option(int option, bool selected, float x, float y, clan::Canvas& gc);
    virtual int height_for_option(clan::Canvas& gc);
    virtual void process_choice(int selection);
    virtual int get_option_count();
private:
    Font        m_option_font;
    Font        m_selection_font;
    clan::Rectf    m_menu_rect;
    clan::Image    m_overlay;
    bool        m_bDone;
    bool        m_bLoad;
    
};

}

#endif // STARTUPSTATE_H
