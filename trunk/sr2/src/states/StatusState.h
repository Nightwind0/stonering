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


#ifndef STATUSSTATE_H
#define STATUSSTATE_H

#include "State.h"


namespace StoneRing {

class StatusState : public State
{

public:
    StatusState();
    virtual ~StatusState();
    virtual bool IsDone() const;

    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
    
    virtual bool Threaded() const { return false; }
    virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void Finish(); // Hook to clean up or whatever after being popped    
private:
    void        draw_party(CL_GraphicContext& gc);
    void        draw_stats(CL_GraphicContext& gc, Character * pChar);
    std::vector<ICharacter::eCharacterAttribute> m_stats;
    CL_Rectf    m_party_rect;
    CL_Rectf    m_status_rect;
    Font        m_stat_name_font;
    Font        m_stat_font;
    Font        m_plus_font;
    Font        m_minus_font;
    CL_Pointf   m_name_x;
    CL_Pointf   m_stat_x;
    CL_Pointf   m_mod_x;
    int         m_current_character;
    bool        m_bDone;
};

}

#endif // STATUSSTATE_H
